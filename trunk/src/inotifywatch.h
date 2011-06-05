/*
 * inotifywatch.h
 *
 *  Created on: 2011-3-15
 *      Author: yanbin
 */

#ifndef INOTIFYWATCH_H_
#define INOTIFYWATCH_H_
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <sys/inotify.h>

/**
 * 监控路径的相关信息
 *
 */
class InotifyWatch
{
public:
	/**
	 * 构造函数
	 */
	InotifyWatch(const std::string& rPath, int32_t uMask);

	/**
	 * 设置路径
	 *
	 * @param const std::string& rPath
	 * @return InotifyWatch &
	 */
	InotifyWatch & setPath(const std::string& rPath);

	/**
	 * 获取监控的路径
	 *
	 * @return std::string
	 */
	inline const std::string getPath() const
	{
		return m_sPath;
	}

	/**
	 * 设置mask
	 *
	 * @param uint32_t mask
	 * @return InotifyWatch &
	 */
	InotifyWatch & setMask(uint32_t mask);

	/**
	 * 获取监控掩码
	 *
	 * @return uint32_t
	 */
	inline uint32_t getMask() const
	{
		return m_uMask;
	}

	/**
	 * 设置Descriptor
	 *
	 * @param int32_t wd
	 * @return InotifyWatch &
	 */
	InotifyWatch & setDescriptor(int32_t wd);

	/**
	 * 获取描述符
	 *
	 * @return int32_t
	 */
	inline int32_t getDescriptor() const
	{
	    return m_wd;
	}

	/**
	 * 析构函数
	 */
	~InotifyWatch();
private:
	/**
	 * 监控的路径
	 */
	std::string m_sPath;
	/**
	 * 监控掩码
	 */
	uint32_t m_uMask;
	/**
	 * 监控的描述符
	 */
	int m_wd;
};
#endif /* INOTIFYWATCH_H_ */
