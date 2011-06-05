/**
 * inotifyevent.h
 * 监控事件的类
 *
 *  Created on: 2011-3-15
 *      Author: yanbin
 */

#ifndef INOTIFYEVENT_H_
#define INOTIFYEVENT_H_
#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/inotify.h>

class InotifyEvent
{
public:

	/**
	 * 构造函数
	 */
	InotifyEvent(uint32_t mask = 0, uint32_t cookie = 0, int wd = -1);

	/**
	 * 设置监控事件mask
	 *
	 * @param uint32_t mask
	 * @return InotifyEvent&
	 */
	InotifyEvent& setMask(uint32_t mask);

	/**
	 * 获取监控事件mask
	 *
	 * @return uint32_t
	 */
	inline uint32_t getMask() const
	{
		return m_uMask;
	}

	/**
	 * 设置监控事件cookie
	 *
	 * @param uint32_t cookie
	 * @return InotifyEvent&
	 */
	InotifyEvent& setCookie(uint32_t cookie);


	/**
	 * 获取监控事件cookie
	 *
	 * @return uint32_t
	 */
	inline uint32_t getCookie() const
	{
		return m_uCookie;
	}

	/**
	 * 设置watch句柄
	 *
	 * @return InotifyEvent &
	 */
	InotifyEvent& setDescriptor(int wd);

	/**
	 * 获取watch句柄
	 *
	 * @return int
	 */
	inline int getDescriptor()
	{
		return m_wd;
	}

	/**
	 * set null-terminated name
	 *
	 * @param std::string
	 * @return InotifyEvent&
	 */
	InotifyEvent& setName(std::string name);

	/**
	 * get null-terminated name
	 *
	 * @return std::string
	 */
	inline const std::string getName() const
	{
		return m_sName;
	}

	/**
	 * 析构函数
	 */
	~InotifyEvent();
private:
	/**
	 * 监控事件mask
	 */
	uint32_t m_uMask;
	/**
	 * 监控的唯一cookie
	 */
	uint32_t m_uCookie;
	/**
	 * 监控描述符
	 */
	int m_wd;
	/**
	 * Optional null-terminated name
	 */
	std::string m_sName;
};

#endif /* INOTIFYEVENT_H_ */
