/*
 * inotifywatch.cpp
 *
 *  Created on: 2011-3-16
 *      Author: yanbin
 */
#include "inotifywatch.h"

/**
 * 构造函数
 */
InotifyWatch::InotifyWatch(const std::string& rPath, int32_t uMask)
{
	setDescriptor(-1);
	setMask(uMask);
	setPath(rPath);
}

/**
 * 设置路径
 *
 * @param const std::string& rPath
 * @return InotifyWatch *
 */
InotifyWatch & InotifyWatch::setPath(const std::string& rPath)
{
	m_sPath = rPath;
	return *this;
}

/**
 * 设置mask
 *
 * @param uint32_t mask
 * @return InotifyWatch &
 */
InotifyWatch & InotifyWatch::setMask(uint32_t mask)
{
	m_uMask = mask;
	return *this;
}

/**
 * 设置Descriptor
 *
 * @param int32_t wd
 * @return InotifyWatch &
 */
InotifyWatch & InotifyWatch::setDescriptor(int32_t wd)
{
	m_wd = wd;
	return *this;
}

/**
 * 析构函数
 */
InotifyWatch::~InotifyWatch()
{
}
