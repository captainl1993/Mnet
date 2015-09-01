#include "All.h"

int main(int argc, char* argv[])
{
	MM::DataHandler h;
	mnet::MServer::Sethandler(h);
	mnet::MServer::Start(8989, 10, 10000);
}
