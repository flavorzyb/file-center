/*
 * inotifylock.h
 *
 *  Created on: 2011-3-17
 *      Author: yanbin
 */

#ifndef INOTIFYLOCK_H_
#define INOTIFYLOCK_H_
#include <iostream>
#include <cstring>
#include <errno.h>
#include <pthread.h>
#include "inotifyexception.h"

class InotifyLock
{
public:
	/**
	 * 构造函数
	 */
	InotifyLock();
	/**
	 * 析构函数
	 */
	~InotifyLock();
	/**
	 * 开启读锁
	 *
	 * @static
	 * @return bool
	 * @throw InotifyException
	 */
	bool startReadLock() throw (InotifyException);

	/**
	 * 解锁读锁
	 *
	 * @static
	 * @return bool
	 * @throw InotifyException
	 */
	bool endReadLock() throw (InotifyException);

	/**
	 * 开启写锁
	 *
	 * @static
	 * @return bool
	 * @throw InotifyException
	 */
	bool startWriteLock() throw (InotifyException);

	/**
	 * 解锁写锁
	 *
	 * @static
	 * @return bool
	 * @throw InotifyException
	 */
	bool endWriteLock() throw (InotifyException);
private:
	/**
	 * 线程锁描述符
	 */
	pthread_rwlock_t m_lock;
};

#endif /* INOTIFYLOCK_H_ */
