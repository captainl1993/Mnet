/*
 * mnet.cpp
 *
 *  Created on: 2015年8月24日
 *      Author: 毛自豪
 */
#include "mnet.h"

namespace mnet
{
void MServer::Sethandler(MsgHandler &msgHadler)
{
	mnet::WorkerThread::SetHandler(msgHadler);
}
void MServer::Start(int port, int thread, int micsec)
{
	MMaster::micsec = micsec;
	if (thread > MaxThreads)
		thread = MaxThreads;

	if (port < 0)
		port = DefualtPort;

	MMaster ls(port, thread); //创建服务

	if (!ls.Run())
	{
		return;
	}

	ls.WaitForListenThread();
}
}
