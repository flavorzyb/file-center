/*
 * test.cpp
 *
 *  Created on: 2011-3-15
 *      Author: yanbin
 */
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/inotify.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <map>
#include <deque>
#include <pthread.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <pwd.h>
#include <wait.h>
#include <sys/types.h>

void * pthread_func(void * argv)
{

}

void daemonStop(int nSignal)
{
    pid_t _pid_t;
    int nState;
    printf("dddddddddd\n");
    while ((_pid_t = waitpid(-1, &nState, WNOHANG)) > 0)
    {
    	printf("nState=%d, pid=%d\n", nState, _pid_t);
    }
    signal(SIGCLD, daemonStop);
}

int main()
{
	pthread_t thread;
	//signal(SIGCHLD, SIG_IGN);
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

	signal(SIGINT, SIG_IGN);
	signal(SIGCLD, daemonStop);


	int ret = system("ls -l");
	printf("ret==%d\n",ret);

	//sleep(10);
	return 0;
}
