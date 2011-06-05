/*
 * Inotify.cpp
 *
 *  Created on: 2011-3-14
 *      Author: yanbin
 */
#include <unistd.h>
#include "inotify.h"

#define INOTIFY_PROCDIR "/proc/sys/fs/inotify/"
#define INOTIFY_WATCHES_SIZE_PATH INOTIFY_PROCDIR "max_user_watches"

/**
 * 最大的监控数
 */
const int Inotify::MAX_WATCHES = 4096;

/**
 * 最大的事件缓存数
 */
const int Inotify::MAX_EVENTS = 4096;

/**
 * 最大的监控数
 */
int Inotify::m_iMaxWatch = -1;

/**
 * 构造函数
 *
 * @param const std::string &rootPath
 */
Inotify::Inotify(const std::string &rootPath) throw (InotifyException)
{
	m_fd = inotify_init();
	if (-1 == m_fd)
	{
		throw InotifyException("inotify_init()", errno);
	}

	char ptrPath[PATH_MAX]="\0";
	char *p = realpath(rootPath.c_str(), ptrPath);
	if (NULL == p)
	{
		throw InotifyException("error root path", errno);
	}

	m_sRootPath = ptrPath;
}

/**
 * 添加监控的路径
 *
 * @param const std::string sPath 需要监控的路径
 * @param uint32_t uMask
 * @return bool
 * @throw InotifyException
 */
void Inotify::addWatch(std::string sPath, uint32_t uMask) throw (InotifyException)
{
	char ptrPath[PATH_MAX]="\0";
	char *p = realpath(sPath.c_str(), ptrPath);
	if (NULL == p)
	{
		throw InotifyException("error path", errno);
	}

	if (strlen(ptrPath) < m_sRootPath.length())
	{
		throw InotifyException("addWatch path must be sub Directory of root path", EBUSY);
	}


	if (strlen(ptrPath) == m_sRootPath.length())
	{
		if (strcmp(ptrPath, m_sRootPath.c_str()) != 0)
		{
			throw InotifyException("addWatch path must be sub Directory of root path", EBUSY);
		}
	}
	else
	{
		std::string tmpPath = m_sRootPath;
		if (tmpPath.at(tmpPath.length() -1) != '/')
		{
			tmpPath += "/";
		}

		std::string pPath = ptrPath;
		pPath = pPath.substr(0, tmpPath.length());

		if (strcmp(tmpPath.c_str(), pPath.c_str()) != 0)
		{
			throw InotifyException("addWatch path must be sub Directory of root path", EBUSY);
		}
	}

	DIR * dir = opendir(ptrPath);
	if (NULL == dir)
	{
		closedir(dir);
		return ;
	}

	std::string sFullPath = ptrPath;

	addPath2Watch(sFullPath, uMask);

	INOTIFY_PATH_DEQUE pathDeque;

	if (sFullPath.at(sFullPath.length() -1) != '/')
	{
		sFullPath += "/";
	}

	std::string nextFile;

	struct dirent * ent = readdir(dir);
	struct stat nextFileStat;

	while(ent)
	{
		if ((0 != strcmp( ent->d_name, "." )) &&
			(0 != strcmp( ent->d_name, ".." )))
		{
			nextFile = sFullPath + ent->d_name;
			if (-1 == lstat(nextFile.c_str(), &nextFileStat))
			{
				if ( errno != EACCES )
				{
					break;
				}
			}

			if (S_ISDIR(nextFileStat.st_mode) && !S_ISLNK(nextFileStat.st_mode))
			{
				addWatch(nextFile, uMask);
				pathDeque.push_back(nextFile);
			}
		}

		ent = readdir(dir);
	}

	closedir(dir);
	m_pathsMap.insert(INOTIFY_PATH_MAP::value_type(ptrPath, pathDeque));
}

/**
 * 查找监控事件是否已监控
 *
 * @param int iDescriptor
 * @return const InotifyWatch *
 */
const InotifyWatch * Inotify::findWatch(int iDescriptor) throw (InotifyException)
{
	m_oInotifyLock.startReadLock();
	INOTIFY_WATCH_MAP::iterator it = m_watches.find(iDescriptor);

	if (it == m_watches.end())
	{
		m_oInotifyLock.endReadLock();
		return NULL;
	}

	InotifyWatch *pW = &((*it).second);
	m_oInotifyLock.endReadLock();
	return pW;
}

/**
 * 查找监控事件是否已监控
 *
 * @param const std::string& sPath
 * @return const InotifyWatch *
 */
const InotifyWatch * Inotify::findWatch(const std::string& sPath) throw (InotifyException)
{
	m_oInotifyLock.startReadLock();
	INOTIFY_WATCH_PATH_MAP::iterator it = m_paths.find(sPath);
	if (it == m_paths.end())
	{
		m_oInotifyLock.endReadLock();
		return NULL;
	}

	InotifyWatch *pW = &(*it).second;
	m_oInotifyLock.endReadLock();
	return pW;
}

/**
 * 移除监控路径
 *
 * @param const std::string& sPath
 * @return void
 * @throw InotifyException
 */
void Inotify::removeWatch(const std::string& sPath) throw (InotifyException)
{
	if (-1 == m_fd)
	{
		throw InotifyException("invalid file descriptor", EBUSY, this);
	}

	const InotifyWatch *pW = findWatch(sPath);
	if (NULL != pW)
	{
		INOTIFY_PATH_MAP::iterator it = m_pathsMap.find(pW->getPath());

		if(it != m_pathsMap.end())
		{
			INOTIFY_PATH_DEQUE deque = it->second;
			INOTIFY_PATH_DEQUE::iterator dit = deque.begin();
			while (dit != deque.end())
			{
				removeWatch(dit->c_str());
				dit++;
			}
			m_oInotifyLock.startWriteLock();
			m_pathsMap.erase(it->first);
			m_oInotifyLock.endWriteLock();
		}

		int wd = pW->getDescriptor();
		m_oInotifyLock.startWriteLock();
		// 从内核中移除监控
		inotify_rm_watch(m_fd, wd);

		m_watches.erase(wd);
		m_paths.erase(pW->getPath());
		m_oInotifyLock.endWriteLock();

	}
}

/**
 * 移除所有监控事件
 *
 * @param void
 * @return void
 * @throw InotifyException
 */
void Inotify::removeAllWatch() throw (InotifyException)
{
	if (-1 == m_fd)
	{
		throw InotifyException("invalid file descriptor", EBUSY, this);
	}

	m_oInotifyLock.startWriteLock();
	INOTIFY_WATCH_MAP::iterator it = m_watches.begin();
	while (it != m_watches.end())
	{
		InotifyWatch pW = (*it).second;
		uint32_t wd = pW.getDescriptor();
		if (-1 != wd)
		{
			inotify_rm_watch(m_fd, wd);
		}

		it++;
	}

	m_watches.clear();
	m_paths.clear();
	m_pathsMap.clear();
	m_oInotifyLock.endWriteLock();
}

/**
 * 获取事件
 *
 * @param InotifyEvent *event
 * @return bool
 */
bool Inotify::getEvent(InotifyEvent *event)
{
	if (getEventCount() < 1)
	{
		return false;
	}

	m_oInotifyLock.startReadLock();
	*event = m_oInotifyEvents.front();
	m_oInotifyEvents.pop_front();
	m_oInotifyLock.endReadLock();
	return true;
}
/**
 * 等待监听事件的触发
 * @return bool
 */
bool Inotify::waitForEvents() throw (InotifyException)
{
	if (getEventCount() > MAX_EVENTS)
	{
		sleep(SLEEP_TIME);
		return true;
	}

	static fd_set read_fds;

	static struct timeval read_timeout;
	read_timeout.tv_sec = SLEEP_TIME;
	read_timeout.tv_usec = 0;

	if (-1 == m_fd)
	{
		throw InotifyException("invalid file descriptor", EBUSY, this);
	}

	memset(m_buffer, 0, sizeof(m_buffer) + 1);

	int flag = fcntl(m_fd, F_GETFL);
	fcntl(m_fd, F_SETFL, flag | O_NONBLOCK);

	FD_ZERO(&read_fds);
	FD_SET(m_fd, &read_fds);

	int retval = select(m_fd + 1, &read_fds, NULL, NULL, &read_timeout);
	if (retval)
	{
		ssize_t len = read(m_fd, m_buffer, sizeof(m_buffer));

		if (len > 0)
		{
			char *offset = m_buffer;
			struct inotify_event *iEvent = NULL;

			while((offset - m_buffer) < len)
			{
				iEvent = (struct inotify_event *) offset;
				addEvent(iEvent);
				offset += sizeof(struct inotify_event) + iEvent->len;
			}

			fcntl(m_fd, F_SETFL, flag);
			return true;
		}
	}

	fcntl(m_fd, F_SETFL, flag);
	return false;
}

/**
 * 获取监听到的事件数
 *
 * @return int
 */
int Inotify::getEventCount()
{
	m_oInotifyLock.startReadLock();
	int count = m_oInotifyEvents.size();
	m_oInotifyLock.endReadLock();
	return count;
}

/**
 * 析构函数
 *
 * @param void
 * @return void
 */
Inotify::~Inotify()
{
	if (-1 != m_fd)
	{
		removeAllWatch();
		close(m_fd);
		m_fd = -1;
	}
	m_oInotifyEvents.clear();
}

/**
 * 将路径加入到监控中
 *
 * @param const std::string &sPath
 * @param uint32_t uMask
 * @return void
 * @throw InotifyException
 */
void Inotify::addPath2Watch(const std::string &sPath, uint32_t uMask) throw (InotifyException)
{
	InotifyWatch oWatch(sPath, uMask);

	if (-1 == m_fd)
	{
		throw InotifyException("invalid file descriptor", EBUSY, this);
	}

	// 查找是否已监控此路径
	if (findWatch(oWatch.getPath()) != NULL)
	{
		return ;
	}

	m_oInotifyLock.startReadLock();
	if ((m_watches.size() + 10) > getMaxWatchCount())
	{
		throw InotifyException("adding watch fail, too many watch path.", EBUSY, this);
	}
	m_oInotifyLock.endReadLock();

	// try to add watch to kernel
	int wd = inotify_add_watch(m_fd, oWatch.getPath().c_str(), oWatch.getMask());

	if (-1 == wd)
	{
		throw InotifyException("adding watch fail", EBUSY, this);
	}

	// 查找是否该路径已被监控了
	const InotifyWatch *pw = findWatch(wd);
	if (NULL != pw)
	{
		if (inotify_add_watch(m_fd, pw->getPath().c_str(), pw->getMask()) < 0)
		{
			throw InotifyException("watch collision detected and recovery failed", EBUSY, this);
		}
	}
	else
	{

		oWatch.setDescriptor(wd);
		m_oInotifyLock.startWriteLock();
		if (m_watches.size() >= getMaxWatchCount())
		{
			m_oInotifyLock.endWriteLock();
			throw InotifyException("watch collision detected and recovery failed", EBUSY, this);
		}
		m_watches.insert(INOTIFY_WATCH_MAP::value_type(oWatch.getDescriptor(), oWatch));
		m_paths.insert(INOTIFY_WATCH_PATH_MAP::value_type(oWatch.getPath(), oWatch));
		m_oInotifyLock.endWriteLock();
	}
}

/**
 * 将事件添加进已监控列表里
 *
 * @return void
 */
void Inotify::addEvent(const struct inotify_event *event)  throw (InotifyException)
{
	char name[1024];
	memset(name, 0, sizeof(name));
	memcpy(name, event->name, event->len);

	int wd = event->wd;
	int mask = event->mask;
	const InotifyWatch *pW = findWatch(wd);

	InotifyEvent iEvent;
	iEvent.setCookie(event->cookie)
			 .setMask(mask)
			 .setDescriptor(wd);

	std::string sName = name;

	if (NULL != pW)
	{
		sName = pW->getPath() + "/" + sName;
	}
	else
	{
		sName = m_sRootPath + "/" + sName;
	}

	switch(mask)
	{
		case (IN_ISDIR | IN_CREATE):
			addWatch(sName.c_str(), mask);
			break;
		case (IN_ISDIR | IN_DELETE):
			removeWatch(sName.c_str());
			break;
		case (IN_ISDIR | IN_MOVED_FROM):
			removeWatch(sName.c_str());
			break;
		case (IN_ISDIR | IN_MOVED_TO):
			addWatch(sName.c_str(), mask);
			break;
		case IN_DELETE_SELF:
			removeWatch(sName.c_str());
			break;
		case IN_MOVE_SELF:
			removeWatch(sName.c_str());
			break;
	}

	iEvent.setName(sName);
	INOTIFY_MASK_MAP::iterator it = m_mMask.begin();
	while(it != m_mMask.end())
	{
		if (mask == it->second)
		{
			m_oInotifyLock.startWriteLock();
			m_oInotifyEvents.push_back(iEvent);
			m_oInotifyLock.endWriteLock();
			break;
		}
		it++;
	}
}

/**
 * 获取能监控的最大数
 *
 * @return int
 */
int Inotify::getMaxWatchCount()
{
	if (m_iMaxWatch > 0)
	{
		return m_iMaxWatch;
	}

	FILE * file = fopen(INOTIFY_WATCHES_SIZE_PATH, "r");
	if ( !file )
	{
		return m_iMaxWatch;
	}

	int num = -1;

	if (EOF == fscanf( file, "%d", &num ) )
	{
		return m_iMaxWatch;
	}

	fclose( file );
	m_iMaxWatch = num;
	return m_iMaxWatch;
}

/**
 * 添加mask类型
 *
 * @param uint32_t
 * @return void
 */
void Inotify::addMask(uint32_t mask)
{
	m_mMask.insert(INOTIFY_MASK_MAP::value_type(mask, mask));
}

/**
 * 添加mask类型
 *
 * @param INOTIFY_MASK_MAP
 * @return void
 */
void Inotify::addMask(INOTIFY_MASK_MAP maskMap)
{
	INOTIFY_MASK_MAP::iterator it = maskMap.begin();
	while(it != maskMap.end())
	{
		addMask(it->second);
		it++;
	}
}

/**
 * 移除mask
 *
 * @param uint32_t
 * @return void
 */
void Inotify::removeMask(uint32_t mask)
{
	INOTIFY_MASK_MAP::iterator it = m_mMask.find(mask);
	if (it != m_mMask.end())
	{
		m_mMask.erase(mask);
	}
}

/**
 * 移除mask
 *
 * @param uint32_t
 * @return void
 */
void Inotify::removeMask(INOTIFY_MASK_MAP maskMap)
{
	INOTIFY_MASK_MAP::iterator it = maskMap.begin();
	while(it != maskMap.end())
	{
		removeMask(it->second);
		it++;
	}
}

