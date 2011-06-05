/*
 * InotifyEvent.cpp
 *
 *  Created on: 2011-3-16
 *      Author: yanbin
 */
#include "inotifyevent.h"

/**
 * 构造函数
 */
InotifyEvent::InotifyEvent(uint32_t mask, uint32_t cookie, int wd)
:m_uMask(mask),
 m_uCookie(cookie),
 m_wd(wd)
{
}


/**
 * 设置监控事件mask
 *
 * @param uint32_t mask
 * @return InotifyEvent&
 */
InotifyEvent& InotifyEvent::setMask(uint32_t mask)
{
	m_uMask = mask;
	return *this;
}

/**
 * 设置监控事件cookie
 *
 * @param uint32_t cookie
 * @return InotifyEvent&
 */
InotifyEvent& InotifyEvent::setCookie(uint32_t cookie)
{
	m_uCookie = cookie;
	return *this;
}

/**
 * 设置watch句柄
 *
 * @return InotifyEvent &
 */
InotifyEvent& InotifyEvent::setDescriptor(int wd)
{
	m_wd = wd;
	return *this;
}

/**
 * set null-terminated name
 *
 * @param std::string
 * @return InotifyEvent&
 */
InotifyEvent& InotifyEvent::setName(std::string name)
{
	m_sName = name;
	return *this;
}

/**
 * 析构函数
 */
InotifyEvent::~InotifyEvent()
{
}
