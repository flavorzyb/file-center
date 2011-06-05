/*
 * inotifylock.cpp
 *
 *  Created on: 2011-3-17
 *      Author: yanbin
 */
#include "inotifylock.h"
/**
 * 构造函数
 */
InotifyLock::InotifyLock()
{
	pthread_rwlockattr_t attr;
	int res = 0;
	if (0 != (res = pthread_rwlockattr_init(&attr)))
	{
	  throw InotifyException("cannot initialize lock attributes", res);
	}

	if (0 != (res = pthread_rwlockattr_setkind_np(&attr, PTHREAD_RWLOCK_PREFER_WRITER_NP)))
	{
	  throw InotifyException("cannot set lock kind", res);
	}

	if (0 != (res = pthread_rwlock_init(&m_lock, &attr)))
	{
	  throw InotifyException("cannot initialize lock", res);
	}

	pthread_rwlockattr_destroy(&attr);
}
/**
 * 析构函数
 */
InotifyLock::~InotifyLock()
{
	pthread_rwlock_destroy(&m_lock);
}
/**
 * 开启读锁
 *
 * @static
 * @return bool
 * @throw InotifyException
 */
bool InotifyLock::startReadLock() throw (InotifyException)
{
	int res = pthread_rwlock_rdlock(&m_lock);
	if (0 != res)
	{
	      throw InotifyException("locking for reading failed", res);
	}

	return true;
}

/**
 * 解锁读锁
 *
 * @static
 * @return bool
 * @throw InotifyException
 */
bool InotifyLock::endReadLock() throw (InotifyException)
{
	int res = pthread_rwlock_unlock(&m_lock);
	if (res != 0)
	{
		throw InotifyException("unlocking failed", res);
	}

	return true;
}

/**
 * 开启写锁
 *
 * @static
 * @return bool
 * @throw InotifyException
 */
bool InotifyLock::startWriteLock() throw (InotifyException)
{
	int res = pthread_rwlock_wrlock(&m_lock);
	if (res != 0)
	{
	  throw InotifyException("locking for writing failed", res);
	}

	return true;
}

/**
 * 解锁写锁
 *
 * @static
 * @return bool
 * @throw InotifyException
 */
bool InotifyLock::endWriteLock() throw (InotifyException)
{
	return endReadLock();
}
