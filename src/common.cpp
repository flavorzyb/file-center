/*
 * common.cpp
 *
 *  Created on: 2011-3-22
 *      Author: yanbin
 */
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include "common.h"
/**
 * 打印帮助信息
 *
 * @return void
 */
void print_help()
{
	std::cout<<"Options"<<std::endl;
	std::cout<<"-d,\t run as a daemon"<<std::endl;
	std::cout<<"-c,\t read configuration from FILE"<<std::endl;
	std::cout<<"-h,\t display this help"<<std::endl;
	exit(0);
}
/**
 * 读取配置文件
 */
INOTIFYCONFIG_MAP readConfigFile(const char * configFile, char* errorLog, int errorLen,
								 char *logDirPath, int logDirPathLen,
								 char *redoLog, int redoLen,
								 char *userName, int userNameLen)
{
	INOTIFYCONFIG_MAP configMap;
	XMLNode xMainNode = XMLNode::openFileHelper(configFile, "head");
	int numProject = xMainNode.nChildNode("project");
	for (int i = 0; i < numProject; i++)
	{
		XMLNode xProjectNode = xMainNode.getChildNode("project", i);

		std::string sProjectName = xProjectNode.getAttribute("name");

		if (!sProjectName.empty())
		{
			InotifyConfig oInotifyConfig;
			oInotifyConfig.setName(sProjectName);
			//解析localpath
			if (xProjectNode.nChildNode("localpath") < 1)
			{
				std::cout<<"missing localpath config."<<std::endl;
				exit(255);
			}

			XMLCSTR sWatch = xProjectNode.getChildNode("localpath").getAttribute("watch");
			if (NULL == sWatch)
			{
				std::cout<<"error on local path."<<std::endl;
				exit(255);
			}

			if (!isdir(sWatch))
			{
				printf("local path(%s) is not a directory\n", sWatch);
				exit(255);
			}

			oInotifyConfig.setLocalPath(sWatch);

			//解析localpath->remote
			int numRemote = xProjectNode.getChildNode("localpath").nChildNode("remote");
			for(int j = 0; j < numRemote; j++)
			{
				XMLCSTR sIp = xProjectNode.getChildNode("localpath").getChildNode("remote", j).getAttribute("ip");
				XMLCSTR sModule = xProjectNode.getChildNode("localpath").getChildNode("remote", j).getAttribute("module");
				//std::cout<< "ip:"<< sIp <<",module:"<< sModule <<std::endl;
				if ((NULL == sIp) || (NULL == sModule))
				{
					std::cout<<"error on remote."<<std::endl;
					exit(255);
				}

				oInotifyConfig.addRemote(sIp, sModule);
			}

			//解析rsync
			if (xProjectNode.nChildNode("rsync") < 1)
			{
				std::cout<<"missing rsync config."<<std::endl;
				exit(255);
			}

			// 默认的rsync执行路径
			XMLCSTR sRsyncBinPath = "/usr/bin/rsync";
			// 默认的rsync执行参数
			XMLCSTR sRsyncParams = "-rauvz";
			// 默认的rsync用户
			XMLCSTR sRsyncUser = "";
			// 默认的rsync密码文件
			XMLCSTR sRsyncPasswordFile = "";
			// 默认的rsync端口
			int sRsyncPort = 0;
			// 默认的rsync超时时间
			int sRsyncTimeOut = 0;
			// 默认的rsync SSH
			bool sRsyncSSH = false;
			// 默认的rsync exclude
			INOTIFY_CONFIG_RSYNC_EXCLUDE_DEQUE sRsyncExclude;

			// 解析rsync
			if (xProjectNode.nChildNode("rsync") > 0)
			{
				// 解析rsync->bin
				if (xProjectNode.getChildNode("rsync").nChildNode("bin") > 0)
				{
					XMLCSTR path = xProjectNode.getChildNode("rsync").getChildNode("bin").getAttribute("path");
					if (NULL != path)
					{
						sRsyncBinPath = path;
					}
				}

				// 解析rsync->commonParams
				if (xProjectNode.getChildNode("rsync").nChildNode("commonParams") > 0)
				{
					XMLCSTR params = xProjectNode.getChildNode("rsync").getChildNode("commonParams").getAttribute("params");
					if (NULL != params)
					{
						sRsyncParams = params;
					}
				}

				// 解析rsync->auth
				if (xProjectNode.getChildNode("rsync").nChildNode("auth") > 0)
				{
					XMLCSTR users = xProjectNode.getChildNode("rsync").getChildNode("auth").getAttribute("user");
					if (NULL != users)
					{
						sRsyncUser = users;
					}

					XMLCSTR passwordFile = xProjectNode.getChildNode("rsync").getChildNode("auth").getAttribute("passwordfile");
					if (NULL != passwordFile)
					{
						sRsyncPasswordFile = passwordFile;
					}
				}

				// 解析rsync->userDefinedPort
				if (xProjectNode.getChildNode("rsync").nChildNode("userDefinedPort") > 0)
				{
					XMLCSTR port = xProjectNode.getChildNode("rsync").getChildNode("userDefinedPort").getAttribute("port");
					if (NULL != port)
					{
						sRsyncPort = atoi(port);
					}
				}

				// 解析rsync->timeout
				if (xProjectNode.getChildNode("rsync").nChildNode("timeout") > 0)
				{
					XMLCSTR timeout = xProjectNode.getChildNode("rsync").getChildNode("timeout").getAttribute("time");
					if (NULL != timeout)
					{
						sRsyncTimeOut = atoi(timeout);
					}
				}

				// 解析rsync->ssh
				if (xProjectNode.getChildNode("rsync").nChildNode("ssh") > 0)
				{
					XMLCSTR ssh = xProjectNode.getChildNode("rsync").getChildNode("ssh").getAttribute("start");
					if (NULL != ssh)
					{
						if (strcmp(ssh, "true") == 0)
						{
							sRsyncSSH = true;
						}
					}
				}

				// 解析rsync->exclude
				int numExclude = xProjectNode.getChildNode("rsync").nChildNode("exclude");
				for (int j = 0; j < numExclude; j++)
				{
					XMLCSTR exclude = xProjectNode.getChildNode("rsync").getChildNode("exclude", j).getAttribute("expression");
					if (NULL != exclude)
					{
						sRsyncExclude.push_back(exclude);
					}
				}
			}

			InotifyRsync iInotifyRsync;
			iInotifyRsync.binPath = sRsyncBinPath;
			iInotifyRsync.params = sRsyncParams;
			iInotifyRsync.users = sRsyncUser;

			iInotifyRsync.passwordfile = sRsyncPasswordFile;
			iInotifyRsync.port = sRsyncPort;
			iInotifyRsync.timeout = sRsyncTimeOut;

			iInotifyRsync.ssh = sRsyncSSH;
			iInotifyRsync.excludes = sRsyncExclude;

			oInotifyConfig.setInotifyRsync(iInotifyRsync);

			uint32_t uMask = 0;
			// 解析inotify
			if (xProjectNode.nChildNode("inotify") > 0)
			{
				// 解析inotify的事件监控
				uMask = initEventMaskFromXml(xProjectNode.getChildNode("inotify"), uMask, &oInotifyConfig);
			}

			oInotifyConfig.setMask(uMask);
			configMap.insert(INOTIFYCONFIG_MAP::value_type(sProjectName, oInotifyConfig));
		}
	}

	memset(errorLog, 0, errorLen);
	if (xMainNode.nChildNode("errorlog") > 0)
	{
		XMLCSTR logfile=xMainNode.getChildNode("errorlog").getAttribute("path");
		if ((NULL != logfile) && (strlen(logfile) > 0) && (strlen(logfile) <= errorLen))
		{
			memcpy(errorLog, logfile, strlen(logfile));
		}
	}

	memset(logDirPath, 0, logDirPathLen);
	if (xMainNode.nChildNode("logpath") > 0)
	{
		XMLCSTR logDir=xMainNode.getChildNode("logpath").getAttribute("path");
		if ((NULL != logDir) && (strlen(logDir) > 0) && (strlen(logDir) <= logDirPathLen))
		{
			memcpy(logDirPath, logDir, strlen(logDir));
		}
	}

	memset(redoLog, 0, redoLen);
	if (xMainNode.nChildNode("redolog") > 0)
	{
		XMLCSTR redopath=xMainNode.getChildNode("redolog").getAttribute("path");
		if ((NULL != redopath) && (strlen(redopath) > 0) && (strlen(redopath) <= redoLen))
		{
			memcpy(redoLog, redopath, strlen(redopath));
		}
	}

	memset(userName, 0, userNameLen);
	if (xMainNode.nChildNode("user") > 0)
	{
		XMLCSTR ptrUserName=xMainNode.getChildNode("user").getAttribute("name");
		if ((NULL != ptrUserName) && (strlen(ptrUserName) > 0) && (strlen(ptrUserName) <= userNameLen))
		{
			memcpy(userName, ptrUserName, strlen(ptrUserName));
		}
	}

	return configMap;
}

/**
 * 初始化event mask
 *
 * @param const XMLNode & xProjectNode
 * @param const char * event
 * @param uint32_t uMask
 * @return uint32_t
 */
uint32_t initEventMaskFromXml(const XMLNode & xProjectNode, uint32_t uMask, InotifyConfig *ptrInotifyConfig)
{
	static const char *eventStr[11] = {
							"MODIFY",
							"DELETE",
							"CREATE",

							"MOVED_FROM",
							"MOVED_TO",

							"DELETE_SELF",
							"MOVE_SELF",

							"DIR_CREATE",
							"DIR_DELETE",
							"DIR_MOVED",

							"ATTRIB",
							};

	static uint32_t eventMask[] = {
									IN_MODIFY,
									IN_DELETE,
									IN_CREATE,

									IN_MOVED_FROM,
									IN_MOVED_TO,

									IN_DELETE_SELF,
									IN_MOVE_SELF,

									IN_ISDIR | IN_CREATE,
									IN_ISDIR | IN_DELETE,
									IN_ISDIR | IN_MOVE,
									IN_ATTRIB,
									};

	for(int i = 0; i < 11; i++)
	{
		const char *event = eventStr[i];
		if (xProjectNode.nChildNode(event) > 0)
		{
			XMLCSTR flag = xProjectNode.getChildNode(event).getAttribute("start");
			if (NULL != flag)
			{
				if (strcmp(flag, "true") == 0)
				{
					uMask |= eventMask[i];
					ptrInotifyConfig->addMask(eventMask[i]);
				}
			}
		}
	}

	return uMask;
}

/**
 * 检测是否是目录
 *
 * @param const char * path
 * @return bool
 */
bool isdir(const char * path )
{
	static struct stat64 my_stat;

	if ( -1 == lstat64( path, &my_stat ) )
	{
		if (errno == ENOENT) return false;
		return false;
	}

	return S_ISDIR(my_stat.st_mode ) && !S_ISLNK( my_stat.st_mode );
}
/**
 * 获取监控事件
 *
 * @param void * ptrInotify
 */
void * waitForEvent(void * argv)
{
	Inotify *pInotify = ((Inotify **) argv)[0];
	InotifyConfig *ptrInotifyConfig = ((InotifyConfig **) argv)[1];
	char *ptrLogDirPath = ((char **) argv)[2];
	char *redoLog		= ((char **) argv)[3];

	std::string sLogDirPath;
	sLogDirPath.append(ptrLogDirPath);
	std::string sRedoLog(redoLog);

	InotifyConfig oInotifyConfig = (*ptrInotifyConfig);
	//delete ptrInotifyConfig;
	delete ptrInotifyConfig;
	delete []argv;

	INOTIFY_CONFIG_REMOTE_MAP mapRemote = oInotifyConfig.getRemote();
	INOTIFY_CONFIG_REMOTE_MAP::iterator mapRemoteIt;

	InotifyEvent oInotifyEvent;

	std::string sLogPath;
	time_t rawtime;
	struct tm *timeInfo = NULL;
	char this_time[16];
	FILE *fp = NULL;


	std::string rsyncPath;
	std::string rootPath = pInotify->getRootPath();
	char logFormat[32];
	printf("start filecenter service,waiting for event(%s)......\n", rootPath.c_str());
	try
	{
		for(;;)
		{
			time(&rawtime);
			timeInfo = localtime(&rawtime);
			memset(this_time, 0, sizeof(this_time));
			sprintf(this_time, "%d_%02d_%02d.log", timeInfo->tm_year+1900, timeInfo->tm_mon+1, timeInfo->tm_mday);

			sLogPath = sLogDirPath + "/" + oInotifyConfig.getName();
			sLogPath.append(this_time);

			// 打开日志文件
			fp = fopen(sLogPath.c_str(), "ab+");

			pInotify->waitForEvents();

			while (pInotify->getEventCount() > 0)
			{
				if (pInotify->getEvent(&oInotifyEvent))
				{
					mapRemoteIt = mapRemote.begin();
					while(mapRemoteIt != mapRemote.end())
					{
						std::string command = "cd " + rootPath + " && ";

						if (rootPath.at(rootPath.length() -1) == '/')
						{
							rsyncPath = oInotifyEvent.getName().substr(rootPath.length());
						}
						else
						{
							rsyncPath = oInotifyEvent.getName().substr(rootPath.length() + 1);
						}

						command +=oInotifyConfig.getCommand(rsyncPath, mapRemoteIt->first.c_str(), mapRemoteIt->second.c_str());
						if (fp)
						{
							time(&rawtime);
							timeInfo = localtime(&rawtime);
							memset(logFormat, 0, sizeof(logFormat));
							sprintf(logFormat, "date:%d-%02d-%02d %02d:%02d:%02d | ",
									timeInfo->tm_year+1900,
									timeInfo->tm_mon+1,
									timeInfo->tm_mday,
									timeInfo->tm_hour,
									timeInfo->tm_min,
									timeInfo->tm_sec);
							std::string logMsg(logFormat);
							logMsg += command + "\n";
							fwrite(logMsg.c_str(), sizeof(char), logMsg.length(), fp);
						}

						// 如果执行同步失败 则写入重做日志
						runCommand(command.c_str(), command.length(), sRedoLog.c_str());

						mapRemoteIt++;
					}
				}
			}

			//关闭日志文件
			if (NULL != fp)
			{
				fclose(fp);
				fp = NULL;
			}
		}
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
}

void daemonStop(int nSignal)
{
    pid_t _pid_t;
    int nState;
    while ((_pid_t = waitpid(-1, &nState, WNOHANG)) > 0);
    signal(SIGCLD, daemonStop);
}

int daemonStart(char * errorLog)
{
	int fd;
	pid_t pid = fork();
	if (pid < 0)
	{
		fprintf(stderr, "Failed to fork1 whilst daemonizing!\n");
		return EXIT_FAILURE;
	}
	else if (pid > 0)
	{
		_exit(0);
	}

	if (setsid() < 0)
	{
		fprintf(stderr, "Failed to setsid whilst daemonizing!\n");
		return EXIT_FAILURE;
	}

	if (chdir("/") < 0)
	{
		fprintf(stderr, "Failed to chdir whilst daemonizing!\n");
		return EXIT_FAILURE;
	}

	// Redirect stdin from /dev/null
	fd = open("/dev/null", O_RDONLY);
	if (fd != fileno(stdin))
	{
		dup2(fd, fileno(stdin));
		close(fd);
	}

	if (strlen(errorLog) == 0)
	{
		memcpy(errorLog, "/dev/null", strlen("/dev/null"));
	}

	// Redirect stdout to a file
	/*
	fd = open(errorLog, O_WRONLY | O_CREAT | O_APPEND, 0600);
	if (fd < 0)
	{
		fprintf( stderr, "Failed to open output file %s\n", errorLog );
		return EXIT_FAILURE;
	}

	if (fd != fileno(stdout)) {
		dup2(fd, fileno(stdout));
		close(fd);
	}
	*/

	// Redirect stderr to /dev/null
	fd = open(errorLog, O_WRONLY);
	if (fd != fileno(stderr)) {
		dup2(fd, fileno(stderr));
		close(fd);
	}

    umask(0);
    signal(SIGINT, SIG_IGN);
    signal(SIGCLD, daemonStop);
    signal(SIGCHLD, SIG_IGN);
    return 0;
}

/**
 * 重做失败的事件
 *
 * @param void * redoLog
 */
void * runRedoLog(void * redoLog)
{
	char *ptrRedoLog = ((char *) redoLog);
	std::string sRedoLog(ptrRedoLog);
	std::string sRedoLogBak = sRedoLog + ".bak";
	FILE *fp = NULL;
	char str[2048];

	for(;;)
	{
		if (access(sRedoLog.c_str(), F_OK) == 0)
		{
			if (rename(sRedoLog.c_str(), sRedoLogBak.c_str()) == 0)
			{
				fp = fopen(sRedoLogBak.c_str(), "rb");
				if (NULL != fp)
				{
					while(0 == feof(fp))
					{
						memset(str, 0, sizeof(str));
						fgets(str, sizeof(str), fp);
						runCommand(str, strlen(str), sRedoLog.c_str());
					}
					fclose(fp);
				}
			}
		}

		sleep(10);
	}
}

/**
 * 执行同步 如果同步失败则写入重做日志
 *
 * @param const char * command
 * @param int commandLen
 * @param const char* logFile
 * @return void
 */
void runCommand(const char * command, int commandLen, const char* logFile)
{
	int ret = system(command);
	if (!((0 == ret) || (-1 == ret)))
	{
		FILE *fp = fopen(logFile, "ab+");
		if (NULL != fp)
		{
			std::string cmd(command);
			cmd +="\n";
			flockfile(fp);
			fwrite(cmd.c_str(), sizeof(char), cmd.length(), fp);
			funlockfile(fp);
			fclose(fp);
		}
	}
}

/**
 * 查找用户uid
 *
 * @param const char *
 * @param uid_t &
 * @return bool
 */
bool getUserId(const char *username, uid_t &uid, gid_t &gid)
{
	struct passwd *user = getpwnam(username);

	if (NULL == user)
	{
		return false;
	}

	uid = user->pw_uid;
	gid = user->pw_gid;
	return true;
}
