#include "mnet.h"
namespace mnet
{
	
	MsgHandler::MsgHandler()
	{
	}
	
	MsgHandler::~MsgHandler()
	{
	}
	void MsgHandler::Process(timeval newtime, timeval difference)
	{
		
	}
	void MsgHandler::sessionOpened(ConnItem *session)
	{
	}
	
	void MsgHandler::sessionClosed(ConnItem *session)
	{
	}
	
	void MsgHandler::messageReceived(ConnItem *session, char* datas, int length)
	{
	}
	
	void MsgHandler::messageSent(ConnItem *session)
	{
	}
	
	void MsgHandler::errorCaught(ConnItem *session)
	{
	}
	bool MsgHandler::SendData(ConnItem *session, std::string datas)
	{
		bufferevent_write(session->bev, datas.c_str(), datas.length() + 1);
		return true;
	}

}
