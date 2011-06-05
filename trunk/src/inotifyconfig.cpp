/*
 * inotifyconfig.cpp
 *
 *  Created on: 2011-3-22
 *      Author: yanbin
 */
#include "inotifyconfig.h"
/**
 * 构造函数
 */
InotifyConfig::InotifyConfig()
{
}

/**
 * 设置项目名
 *
 * @param const std::string & sName
 * @return InotifyConfig &
 */
InotifyConfig & InotifyConfig::setName(const std::string & sName)
{
	m_sName = sName;
	return *this;
}

/**
 * 设置监控的本地路径
 *
 * @param const std::string & sLocalPath
 * @return InotifyConfig &
 */
InotifyConfig & InotifyConfig::setLocalPath(const std::string & sLocalPath)
{
	m_sLocalPath = sLocalPath;
	return *this;
}

/**
 * 添加同步的ip和module
 *
 * @param const std::string & ip
 * @param const std::string & module
 * @return InotifyConfig &
 */
InotifyConfig & InotifyConfig::addRemote(const std::string & ip, const std::string & module)
{
	m_mRemote.insert(INOTIFY_CONFIG_REMOTE_MAP::value_type(ip, module));

	return *this;
}

/**
 * 删除同步的ip和module
 *
 * @param const std::string & ip
 * @return void
 */
void InotifyConfig::removeRemote(const std::string & ip)
{
	m_mRemote.erase(ip);
}

/**
 * 设置监控的事件
 *
 * @param uint32_t uMask
 * @return InotifyConfig &
 */
InotifyConfig & InotifyConfig::setMask(uint32_t uMask)
{
	m_uMask = uMask;
	return *this;
}

/**
 * 设置rsync命令配置
 *
 * @param const InotifyRsync& strInotifyRsync
 * @return InotifyConfig &
 */
InotifyConfig & InotifyConfig::setInotifyRsync(const InotifyRsync& strInotifyRsync)
{
	m_InotifyRsync.binPath = strInotifyRsync.binPath;
	m_InotifyRsync.params = strInotifyRsync.params;
	m_InotifyRsync.users = strInotifyRsync.users;
	m_InotifyRsync.passwordfile = strInotifyRsync.passwordfile;
	m_InotifyRsync.port = strInotifyRsync.port;
	m_InotifyRsync.timeout = strInotifyRsync.timeout;
	m_InotifyRsync.ssh = strInotifyRsync.ssh;
	m_InotifyRsync.excludes = strInotifyRsync.excludes;
	return *this;
}

/**
 * 获取执行命令
 *
 * @param const std::string & sPath
 * @return const std::string &
 */
const std::string InotifyConfig::getCommand(const std::string & sPath, const std::string & ip, const std::string & module) const
{
	std::string cmd = m_InotifyRsync.binPath + " " + m_InotifyRsync.params;
	if (m_InotifyRsync.passwordfile.length() > 0)
	{
		cmd += " --password-file=" + m_InotifyRsync.passwordfile;
	}

	if (m_InotifyRsync.port > 0)
	{
		cmd +=" --port=";
		char port[10];
		sprintf(port, "%d", m_InotifyRsync.port);
		cmd.append(port);
	}

	if (m_InotifyRsync.timeout > 0)
	{
		cmd +=" --timeout=";
		char timeout[10];
		sprintf(timeout, "%d", m_InotifyRsync.timeout);
		cmd.append(timeout);
	}

	if (m_InotifyRsync.ssh)
	{
		cmd +=" -e ssh ";
	}

	if (m_InotifyRsync.excludes.size() > 0)
	{
		INOTIFY_CONFIG_RSYNC_EXCLUDE_DEQUE dqExclude = m_InotifyRsync.excludes;
		INOTIFY_CONFIG_RSYNC_EXCLUDE_DEQUE::iterator it = dqExclude.begin();

		while(it != dqExclude.end())
		{
			cmd += " --exclude " + (*it) + " ";
			it++;
		}
	}

	cmd += " " + sPath + " ";

	if (m_InotifyRsync.users.length() > 0)
	{
		cmd += " " + m_InotifyRsync.users+"@";
	}

	cmd += ip +"::" + module + " > /dev/null 2> /dev/null";

	return cmd;
}

/**
 * 添加mask类型
 *
 * @param uint32_t
 * @return void
 */
void InotifyConfig::addMask(uint32_t mask)
{
	m_mMaskMap.insert(INOTIFY_MASK_MAP::value_type(mask, mask));
}
/**
 * 移除mask
 *
 * @param uint32_t
 * @return void
 */
void InotifyConfig::removeMask(uint32_t mask)
{
	INOTIFY_MASK_MAP::iterator it = m_mMaskMap.find(mask);
	if (it != m_mMaskMap.end())
	{
		m_mMaskMap.erase(mask);
		it++;
	}
}

/**
 * 析构函数
 */
InotifyConfig::~InotifyConfig()
{
	m_mRemote.clear();
}
