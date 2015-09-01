/************************************************************
 * FileName: msghandler.h
 * Author:   毛自豪
 * Version : v0.1
 * Date:	    2015-08-24
 * Description: 消息处理handler的基本实现，实现基本的消息处理方法，使用时继承这个类
 * Version:  v0.1
 * Function List:
 ***********************************************************/
#pragma once
namespace mnet
{
	class ConnItem;
	class MsgHandler
	{
		public:
			MsgHandler();
			virtual ~MsgHandler();
			virtual void Process(timeval newtime, timeval difference);
			virtual void sessionOpened(ConnItem *session);
			virtual void sessionClosed(ConnItem *session);
			virtual void messageReceived(ConnItem *session, char* datas,
			        int length);
			virtual void messageSent(ConnItem *session);
			virtual void errorCaught(ConnItem *session);
			bool SendData(ConnItem *session, std::string datas);
	};
}
