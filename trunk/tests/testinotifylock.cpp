/*
 * testinotifylock.cpp
 *
 *  Created on: 2011-3-17
 *      Author: yanbin
 */
#include "../src/inotifylock.h"
int count = 0;
InotifyLock iLock;
void * addCount(void *arg)
{
	iLock.startWriteLock();
	count++;
	std::cout << "pthread_id==" << pthread_self() << ",count == "<< count <<std::endl;
	iLock.endWriteLock();
	return ((void *)0);
}

void * descCount(void *arg)
{
	iLock.startWriteLock();
	count--;
	std::cout << "pthread_id==" << pthread_self() << ",count == "<< count <<std::endl;
	iLock.endWriteLock();
	return ((void *)0);
}
class testLock
{
public:
	InotifyLock m_oInotifyLock;
};
/*
int main()
{
	 pthread_t thread;
	 for(int i = 0; i < 20; i++)
	 {
		 pthread_create(&thread, NULL, addCount, NULL);
		 //pthread_join(thread, NULL);
		 pthread_create(&thread, NULL, descCount, NULL);
		 //pthread_join(thread, NULL);
	 }

	 sleep(2);
	return 0;
}
*/