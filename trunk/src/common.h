/*
 * common.h
 *
 *  Created on: 2011-3-22
 *      Author: yanbin
 */

#ifndef INOTIFY_COMMON_H_
#define INOTIFY_COMMON_H_
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <map>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include "inotify.h"
#include "inotifyconfig.h"
#include "xmlParser.h"

// key 项目名 value项目的配置
typedef std::map<std::string, InotifyConfig> INOTIFYCONFIG_MAP;

// key 项目名 value监控对象
typedef std::map<std::string, Inotify *> INOTIFY_MAP;

typedef struct _structPthread
{
	Inotify *oInotify;
	InotifyConfig oInotifyConfig;
	time_t time;
} structPthread;

// key 项目名 value线程状态
typedef std::map<pthread_t, structPthread> INOTIFY_THREAD_MAP;

/**
 * 打印帮助信息
 *
 * @return void
 */
void print_help();
/**
 * 检测是否是目录
 *
 * @param const char * path
 * @return bool
 */
bool isdir(const char * path );

/**
 * 初始化event mask
 *
 * @param const XMLNode & xProjectNode
 * @param uint32_t uMask
 * @return uint32_t
 */
uint32_t initEventMaskFromXml(const XMLNode & xProjectNode, uint32_t uMask, InotifyConfig *ptrInotifyConfig);

/**
 * 读取配置文件
 */
INOTIFYCONFIG_MAP readConfigFile(const char * configFile, char* errorlog, int errorLen,
								 char *logDirPath, int logDirPathLen,
								 char *redoLog, int redoLen,
								 char *userName, int userNameLen);

/**
 * 获取监控事件
 *
 * @param Inotify * ptrInotify
 */
void * waitForEvent(void * argv);

void daemonStop(int nSignal);

int daemonStart(char * errorLog);

/**
 * 执行同步 如果同步失败则写入重做日志
 *
 * @param const char * command
 * @param int commandLen
 * @param const char* logFile
 * @return void
 */
void runCommand(const char * command, int commandLen, const char* logFile);

/**
 * 重做失败的事件
 *
 * @param void * redoLog
 */
void * runRedoLog(void * redoLog);

/**
 * 查找用户uid
 *
 * @param const char *
 * @param uid_t &
 * @return bool
 */
bool getUserId(const char *username, uid_t &uid, gid_t &gid);

#endif /* COMMON_H_ */
