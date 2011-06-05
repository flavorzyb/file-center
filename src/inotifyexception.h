/*
 * InotifyException.h
 *
 *  Created on: 2011-3-15
 *      Author: yanbin
 */
/*
 * InotifyException.h
 * Inotify的异常类
 *
 * @version 1.0.0
 * @author Yanbin Zhu <starwind2004@hotmail.com>
 * @copyright http://www.shiwan.com
 */

#ifndef INOTIFYEXCEPTION_H_
#define INOTIFYEXCEPTION_H_
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>

/**
 * Inotify的异常类
 */
class InotifyException
{
	public:
	  /// Constructor
	  /**
	   * \param[in] rMsg message
	   * \param[in] iErr error number (see errno.h)
	   * \param[in] pSrc source
	   */
	  InotifyException(const std::string& rMsg = "", int iErr = 0, void* pSrc = NULL)
	  : m_msg(rMsg),
		m_err(iErr)
	  {
		m_pSrc = pSrc;
	  }

	  /// Returns the exception message.
	  /**
	   * \return message
	   */
	  inline const std::string& GetMessage() const
	  {
		return m_msg;
	  }

	  /// Returns the exception error number.
	  /**
	   * If not applicable this value is 0 (zero).
	   *
	   * \return error number (standardized; see errno.h)
	   */
	  inline int GetErrorNumber() const
	  {
		return m_err;
	  }

	  /// Returns the exception source.
	  /**
	   * \return source
	   */
	  inline void* GetSource() const
	  {
		return m_pSrc;
	  }

	protected:
	  std::string m_msg;      ///< message
	  int m_err;              ///< error number
	  mutable void* m_pSrc;   ///< source
};

#endif /* INOTIFYEXCEPTION_H_ */
