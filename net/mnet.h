/************************************************************
 * FileName: mnet.h
 * Author:   毛自豪
 * Version : v0.1
 * Date:	    2015-08-24
 * Description: 作为多线程libevent封装库的基本调用头文件
 * Version:  v0.1
 * Function List: 主要MServer::Sethandler() 设置服务器消息的处理Handler，MServer::Start()启动服务器监听
 ***********************************************************/
#pragma once

#define MaxThreads 100		//最大的线程数
#define DefualtPort 89899	//默认的服务器监听端口
#define MAX_BUF 1073741824	//接收缓冲区的最大限制

#include<set>
#include<event2/util.h>
#include<event2/event.h>
#include<event2/buffer.h>
#include<event2/listener.h>
#include<event2/bufferevent.h>
#include<event2/event-config.h>
#include<event2/event.h>
#include<event2/event_struct.h>
#include<event2/util.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/stat.h>
#include<vector>
#include<thread>
#include<memory>
#include<list>
#include<mutex>
#include<iostream>
#include<sys/types.h>
#include<time.h>
#ifdef _EVENT_HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include <errno.h>

#include "Conitem.h"
#include "MsgHandler.h"
#include "WorkerThread.h"
#include "MMaster.h"
namespace mnet
{
//服务器类接口
	class MServer
	{
		public:
			static void Sethandler(MsgHandler &msgHadler);
			static void Start(int port, int thread, int micsec);
	};
}

