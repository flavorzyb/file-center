/*
 * fileCenter.cpp
 *
 *  Created on: 2011-3-22
 *      Author: yanbin
 */
#include <iostream>
#include <cstring>
#include "common.h"

/**
 * 创建监控项目处理线程
 *
 * @param const Inotify *
 * @param const INOTIFY_THREAD_MAP *
 * @param const InotifyConfig *
 * @param const char *logDirPath
 * @return pthread_t
 */
pthread_t createProjectPthread(	Inotify * ptrInotify,
								InotifyConfig * ptrInotifyConfig,
								char *logDirPath,
								char *redoLog);
int main(int argc, char *argv[])
{
	bool bHasConfigFile = false;
	char errorLog[1024];
	char logDirPath[1024];
	char redoLog[1024];
	char userName[64];
	pthread_t *pthreadArr = NULL;
	pthread_t thread;
	bool bRunAsDaemon = false;
	INOTIFYCONFIG_MAP oInotifyConfigMap;

	for(int i = 0; i< argc; i++)
	{
		if (strcmp("-h", argv[i]) == 0)
		{
			print_help();
		}
		else if (strcmp("-c", argv[i]) == 0)
		{
			if ((i + 1) > argc)
			{
				std::cout << "need config file path" << std::endl;
				exit(-1);
			}

			// 读取配置文件
			oInotifyConfigMap = readConfigFile(argv[i+1], errorLog, sizeof(errorLog),
																logDirPath, sizeof(logDirPath),
																redoLog, sizeof(redoLog), userName, sizeof(userName));
			bHasConfigFile = true;
		}
		else if (strcmp("-d", argv[i]) == 0)
		{
			bRunAsDaemon = true;
		}
	}

	try
	{
		if (false == bHasConfigFile)
		{
			std::cout << "missing config file!"<<std::endl;
			exit(-1);
		}

		// 初始化监控事件
		INOTIFYCONFIG_MAP::iterator configIt = oInotifyConfigMap.begin();
		INOTIFY_MAP oInotifyMap;
		INOTIFY_MAP::iterator inotifyMapIt;
		Inotify *oInotify = NULL;

		if (oInotifyConfigMap.size() > 0)
		{
			pthreadArr = new pthread_t[oInotifyConfigMap.size()];
			for (int i =0; i< oInotifyConfigMap.size(); i++)
			{
				pthreadArr[i] = 0;
			}
		}

		while(configIt != oInotifyConfigMap.end())
		{
			InotifyConfig oInotifyConfig = configIt->second;
			oInotify = new Inotify(oInotifyConfig.getLocalPath());
			oInotify->addMask(oInotifyConfig.getMaskMap());
			oInotify->addWatch(oInotifyConfig.getLocalPath(), oInotifyConfig.getMask());
			inotifyMapIt = oInotifyMap.find(configIt->first.c_str());
			if (inotifyMapIt != oInotifyMap.end())
			{
				Inotify *ptrInotify = inotifyMapIt->second;
				delete ptrInotify;
			}

			oInotifyMap.insert(INOTIFY_MAP::value_type(configIt->first.c_str(), oInotify));
			configIt++;
		}

		if (bRunAsDaemon)
		{
			daemonStart(errorLog);

			//切换用户
			if (strlen(userName) == 0)
			{
				printf("need username for running...\n");
				exit(255);
			}

			uid_t uid = 0;
			gid_t gid = 0;
			if (!getUserId(userName, uid, gid))
			{
				printf("can not find user(%s)\n", userName);
				exit(255);
			}

			if ((setuid(uid) != 0) && (setgid(gid) != 0))
			{
				printf("setting daemon uid fail.\n");
				exit(255);
			}
		}



		inotifyMapIt = oInotifyMap.begin();
		int pthreadNum = 0;
		while (inotifyMapIt != oInotifyMap.end())
		{
			configIt = oInotifyConfigMap.find(inotifyMapIt->first);
			InotifyConfig *pInotifyConfig = new InotifyConfig;
			(*pInotifyConfig) = configIt->second;
			// 启动监控线程 定时的读取监控事件
			pthreadArr[pthreadNum] = createProjectPthread(inotifyMapIt->second, pInotifyConfig, logDirPath, redoLog);
			inotifyMapIt++;
			pthreadNum++;
		}

		//启动重做守护线程
		pthread_create(&thread, NULL, runRedoLog, redoLog);

		//主守护进程
		for(int i =0; i< sizeof(pthreadArr); i++)
		{
			pthread_join(pthreadArr[i], NULL);
		}

		pthread_join(thread, NULL);
	}
	catch(InotifyException ie)
	{
		std::cout << "main error:"<<ie.GetMessage() <<",\terrorno:"<<ie.GetErrorNumber()<<std::endl;
		exit(255);
	}
	catch(...)
	{
		std::cout << "main  unkown error"<<std::endl;
		exit(255);
	}

	return 0;
}
/**
 * 创建监控项目处理线程
 *
 * @param const Inotify *
 * @param const INOTIFY_THREAD_MAP *
 * @param const InotifyConfig *
 * @param const char *logDirPath
 * @return pthread_t
 */
pthread_t createProjectPthread(	Inotify * ptrInotify,
								InotifyConfig * ptrInotifyConfig,
								char *logDirPath,
								char *redoLog)
{
	pthread_t thread;
	void **argv = new void *[4];
	argv[0] = ptrInotify;
	argv[1] = ptrInotifyConfig;
	argv[2] = logDirPath;
	argv[3] = redoLog;

	pthread_create(&thread, NULL, waitForEvent, argv);

	return thread;
}
