/*
 * inotifyconfig.h
 *
 *  Created on: 2011-3-22
 *      Author: yanbin
 */

#ifndef INOTIFYCONFIG_H_
#define INOTIFYCONFIG_H_
#include <iostream>
#include <stdio.h>
#include <cstring>
#include <stdint.h>
#include <map>
#include <deque>

// 同步远程配置 key是ip value是模块名
typedef std::map<std::string, std::string> INOTIFY_CONFIG_REMOTE_MAP;
typedef std::deque<std::string> INOTIFY_CONFIG_RSYNC_EXCLUDE_DEQUE;

// mapping for mask
typedef std::map <uint32_t, uint32_t> INOTIFY_MASK_MAP;

/**
 * rsync配置
 */
typedef struct _InotifyRsync
{
	std::string binPath;
	std::string params;
	std::string users;
	std::string passwordfile;
	INOTIFY_CONFIG_RSYNC_EXCLUDE_DEQUE excludes;
	int port;
	int timeout;
	bool ssh;
} InotifyRsync;

class InotifyConfig
{
public:
	/**
	 * 构造函数
	 */
	InotifyConfig();

	/**
	 * 设置项目名
	 *
	 * @param const std::string & sName
	 * @return InotifyConfig &
	 */
	InotifyConfig & setName(const std::string & sName);

	/**
	 * 获取项目名
	 *
	 * @return const std::string &
	 */
	inline const std::string & getName() const
	{
		return m_sName;
	}

	/**
	 * 设置监控的本地路径
	 *
	 * @param const std::string & sLocalPath
	 * @return InotifyConfig &
	 */
	InotifyConfig & setLocalPath(const std::string & sLocalPath);

	/**
	 * 获取监控的本地路径
	 *
	 * @return const std::string &
	 */
	inline const std::string & getLocalPath() const
	{
		return m_sLocalPath;
	}

	/**
	 * 添加同步的ip和module
	 *
	 * @param const std::string & ip
	 * @param const std::string & module
	 * @return InotifyConfig &
	 */
	InotifyConfig & addRemote(const std::string & ip, const std::string & module);

	/**
	 * 删除同步的ip和module
	 *
	 * @param const std::string & ip
	 * @return void
	 */
	void removeRemote(const std::string & ip);

	/**
	 * 设置监控的事件
	 *
	 * @param uint32_t uMask
	 * @return InotifyConfig &
	 */
	InotifyConfig & setMask(uint32_t uMask);

	/**
	 * 获取监控的事件
	 *
	 * @return uint32_t
	 */
	inline uint32_t getMask() const
	{
		return m_uMask;
	}

	/**
	 * 设置rsync命令配置
	 *
	 * @param const InotifyRsync& strInotifyRsync
	 * @return InotifyConfig &
	 */
	InotifyConfig & setInotifyRsync(const InotifyRsync& strInotifyRsync);

	/**
	 * 获取rsync命令配置
	 *
	 * @return const InotifyRsync&
	 */
	inline const InotifyRsync& getInotifyRsync() const
	{
		return m_InotifyRsync;
	}

	/**
	 * 获取远程配置map
	 *
	 * @return const INOTIFY_CONFIG_REMOTE_MAP &
	 */
	inline const INOTIFY_CONFIG_REMOTE_MAP & getRemote() const
	{
		return m_mRemote;
	}

	/**
	 * 获取执行命令
	 *
	 * @param const std::string & sPath
	 * @return const std::string &
	 */
	const std::string getCommand(const std::string & sPath, const std::string & ip, const std::string & module) const;

	/**
	 * 添加mask类型
	 *
	 * @param uint32_t
	 * @return void
	 */
	void addMask(uint32_t mask);

	/**
	 * 移除mask
	 *
	 * @param uint32_t
	 * @return void
	 */
	void removeMask(uint32_t mask);

	/**
	 * 获取mask map
	 */
	inline const INOTIFY_MASK_MAP & getMaskMap() const
	{
		return m_mMaskMap;
	}

	/**
	 * 析构函数
	 */
	~InotifyConfig();
private:
	std::string m_sName;
	std::string m_sLocalPath;
	INOTIFY_CONFIG_REMOTE_MAP m_mRemote;
	uint32_t m_uMask;
	InotifyRsync m_InotifyRsync;
	/**
	 * 监控事件mask map
	 */
	INOTIFY_MASK_MAP m_mMaskMap;
};

#endif /* INOTIFYCONFIG_H_ */
