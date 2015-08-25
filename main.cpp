#include "All.h"
using namespace mnet;

class handler:public MsgHandler
{
public:
	 void Process(timeval newtime,timeval difference);
	 void sessionOpened(ConnItem *session);
	 void sessionClosed(ConnItem *session);
	 void messageReceived(ConnItem *session,char* datas,int length);
	 void messageSent(ConnItem *session);
	 void errorCaught(ConnItem *session);
};
void handler::Process(timeval newtime,timeval difference)
{
	double elapsed;
	elapsed = difference.tv_sec + (difference.tv_usec / 1.0e6);

	printf("timeout_cb called at %d: %.3f seconds elapsed.\n",(int)newtime.tv_sec, elapsed);
}
void handler::sessionOpened(ConnItem *session)
{
	std::cout<<"sessionOpened session"<<session->conn_fd<<std::endl;
}
void handler::sessionClosed(ConnItem *session)
{
	std::cout<<"sessionClosed session"<<session->conn_fd<<std::endl;
}
void handler::messageReceived(ConnItem *session,char* datas,int length)
{
	std::cout<<"messageReceived session"<<session->conn_fd<<std::endl;
	std::cout<<"messageReceived data "<<datas<<std::endl;
	std::cout<<"messageReceived data len "<<length<<std::endl;
	SendData(session,"nihaoa");
}
void handler::messageSent(ConnItem *session)
{
	std::cout<<"messageSent session"<<session->conn_fd<<std::endl;
}
void handler::errorCaught(ConnItem *session)
{
	std::cout<<"errorCaught session"<<session->conn_fd<<std::endl;
}
int main(int argc, char* argv[]) {
	handler h;
	MServer::Sethandler(h);
	MServer::Start(8989, 10,10000);
}
