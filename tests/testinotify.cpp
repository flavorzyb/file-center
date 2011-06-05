/*
 * 测试Inotify类库的脚本
 *    用于测试Inotify的脚本是否正确
 *
 *  Created on: 2011-3-14
 *      Author: yanbin
 * @version 1.0.0
 */

#include "../src/inotify.h"
/*
int main()
{
	std::string path="/home/yanbin/log/";
	uint32_t mask = IN_ALL_EVENTS;

	try
	{
		Inotify myInotify;
		myInotify.addWatch(path, mask);
		InotifyEvent event;
		while(1)
		{
			myInotify.waitForEvents();
			while(myInotify.getEvent(&event))
			{

				std::cout << "mask: " << event.getMask()
						  << ",cookie: " << event.getCookie()
						  << ",name: " << event.getName() << std::endl;
				printf("mask===%x\n",event.getMask());

			}

			//std::cout << "max watch==="<<myInotify.getMaxWatchCount()<<std::endl;
		}
	}
	catch(InotifyException e)
	{
		std::cout<< e.GetMessage() << std::endl;
	}

	return 0;
}
*/
