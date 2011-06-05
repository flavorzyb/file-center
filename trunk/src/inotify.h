/*
 * Inotify.h
 * 用于监控文件系统的活动
 *    Inotify类的定义模块
 *
 * @version 1.0.0
 * @author Yanbin Zhu <starwind2004@hotmail.com>
 * @copyright http://www.shiwan.com
 */

#ifndef INOTIFY_H_
#define INOTIFY_H_
#include <iostream>
#include <cstring>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <map>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <deque>

#include "inotifyconfig.h"
#include "inotifyexception.h"
#include "inotifywatch.h"
#include "inotifyevent.h"
#include "inotifylock.h"

//定义路径的最大长度
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

/// Event struct size
#define INOTIFY_EVENT_SIZE (sizeof(struct inotify_event))

/// Event buffer length
#define INOTIFY_EVENT_BUFLEN (1024 * (INOTIFY_EVENT_SIZE + 16))

/// Mapping from watch descriptors to watch objects.
typedef std::map<int32_t, InotifyWatch> INOTIFY_WATCH_MAP;

/// Mapping from paths to watch objects.
typedef std::map<std::string, InotifyWatch> INOTIFY_WATCH_PATH_MAP;

/**
 * 路径的队列 用于保存某目录下的子目录列表
 */
typedef std::deque<std::string> INOTIFY_PATH_DEQUE;
/**
 * 路径映射表
 */
typedef std::map<std::string, INOTIFY_PATH_DEQUE> INOTIFY_PATH_MAP;

class Inotify
{
public:
	/**
	 * 构造函数
	 *
	 * @param const std::string &rootPath
	 * @throw InotifyException
	 */
	Inotify(const std::string &rootPath) throw (InotifyException);

	/**
	 * 添加监控的路径
	 *
	 * @param const std::string sPath 需要监控的路径
	 * @param uint32_t uMask
	 * @return bool
	 * @throw InotifyException
	 */
	void addWatch(std::string sPath, uint32_t uMask) throw (InotifyException);

	/**
	 * 查找监控事件是否已监控
	 *
	 * @param int iDescriptor
	 * @return InotifyWatch *
	 */
	const InotifyWatch * findWatch(int iDescriptor) throw (InotifyException);

	/**
	 * 查找监控事件是否已监控
	 *
	 * @param const std::string& sPath
	 * @return InotifyWatch *
	 */
	const InotifyWatch * findWatch(const std::string& sPath) throw (InotifyException);

	/**
	 * 移除监控路径
	 *
	 * @param const std::string& sPath
	 * @return void
	 */
	void removeWatch(const std::string& sPath) throw (InotifyException);

	/**
	 * 移除所有监控事件
	 *
	 * @param void
	 * @return void
	 */
	void removeAllWatch() throw (InotifyException);


	/**
	 * 等待监听事件的触发
	 * @return bool
	 */
	bool waitForEvents() throw (InotifyException);

	/**
	 * 获取事件
	 *
	 * @return InotifyEvent
	 */
	bool getEvent(InotifyEvent *event);

	/**
	 * 获取监听到的事件数
	 *
	 * @return int
	 */
	int getEventCount();

	/**
	 * 返回最大的事件监控数量
	 *
	 * @return int
	 */
	inline int getMaxEventCount()
	{
		return MAX_EVENTS;
	}

	/**
	 * 获取能监控的最大数
	 *
	 * @return int
	 */
	int getMaxWatchCount();

	/**
	 * 获取root路径地址
	 */
	inline const std::string & getRootPath() const
	{
		return m_sRootPath;
	}

	/**
	 * 获取描述符
	 */
	inline const int getFD()const
	{
		return m_fd;
	}

	/**
	 * 添加mask类型
	 *
	 * @param uint32_t
	 * @return void
	 */
	void addMask(uint32_t mask);

	/**
	 * 添加mask类型
	 *
	 * @param INOTIFY_MASK_MAP
	 * @return void
	 */
	void addMask(INOTIFY_MASK_MAP maskMap);

	/**
	 * 移除mask
	 *
	 * @param uint32_t
	 * @return void
	 */
	void removeMask(uint32_t mask);

	/**
	 * 移除mask
	 *
	 * @param uint32_t
	 * @return void
	 */
	void removeMask(INOTIFY_MASK_MAP maskMap);

	/**
	 * 获取mask map
	 */
	inline const INOTIFY_MASK_MAP getMaskMap() const
	{
		return m_mMask;
	}
	/**
	 * 析构函数
	 */
	~Inotify();

private:
	/**
	 * 将路径加入到监控中
	 *
	 * @param const std::string &sPath
	 * @param uint32_t uMask
	 * @return void
	 */
	void addPath2Watch(const std::string &sPath, uint32_t uMask) throw (InotifyException);

	/**
	 * 将事件添加进已监控列表里
	 *
	 * @return void
	 */
	void addEvent(const struct inotify_event *event)  throw (InotifyException);

	/**
	 * 监控的事件map
	 * key: wd
	 * value: InotifyWatch
	 */
	INOTIFY_WATCH_MAP m_watches;
	/**
	 * 监控的事件map
	 * key: path
	 * value: InotifyWatch
	 */
	INOTIFY_WATCH_PATH_MAP m_paths;
	/**
	 * inotify描述符
	 */
	int m_fd;
	/**
	 * 读取事件的BUFFER
	 */
	char m_buffer[INOTIFY_EVENT_BUFLEN];
	/**
	 * 锁对象
	 */
	InotifyLock m_oInotifyLock;

	/**
	 * root路径
	 */
	std::string m_sRootPath;
	/**
	 * 读取到的事件
	 */
	std::deque<InotifyEvent> m_oInotifyEvents;
	/**
	 * 最大的监控数
	 */
	static const int MAX_WATCHES;

	/**
	 * 最大的事件缓存数
	 */
	static const int MAX_EVENTS;

	/**
	 * 目前监控到的最大监控数
	 */
	static int m_iMaxWatch;

	/**
	 * 当事件队列满时 休眠时间
	 * 单位：秒
	 */
	static const unsigned int SLEEP_TIME = 5;
	/**
	 * 路径的映射表
	 */
	INOTIFY_PATH_MAP m_pathsMap;

	/**
	 * 监控事件mask map
	 */
	INOTIFY_MASK_MAP m_mMask;
};
#endif /* INOTIFY_H_ */
