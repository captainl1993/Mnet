/************************************************************
 * FileName: mmaster.h
 * Author:   毛自豪
 * Version : v0.1
 * Date:	    2015-08-24
 * Description: 多线程网络分配的基础，分配基本链接信息给worker线程处理，以及基本conitem的构造。
 * Version:  v0.1
 * Function List:Run(),WaitForListenThread(),AcceptConn(evconnlistener *, int, sockaddr *, int, void *)
 ***********************************************************/
#ifndef LIBEVENT_SERVER_H_
#define LIBEVENT_SERVER_H_
namespace mnet
{
class MMaster
{
public:
	MMaster(int port, int num_of_threads);
	~MMaster();
	bool Run(); //开启服务
	void WaitForListenThread();
	static void AcceptConn(evconnlistener *, int, sockaddr *, int, void *);
	static void AcceptError(evconnlistener *, void *);
	static int micsec;
private:
	static void timeout_cb(evutil_socket_t fd, short event, void *arg);
	bool StartListen();
	bool InitWorkerThreads();
	std::vector<std::shared_ptr<WorkerThread> > vec_worker_thread_;
	std::shared_ptr<std::thread> main_listen_thread_;
	event_base * server_base_;
	evconnlistener *server_conn_listenner_;
	int listen_port_; //监听端口号
	int last_thread_index_; //上一次请求分配的线程索引
	int num_of_workerthreads_; //工作线程总数
	static int current_max_session_id_; //当前最大session_id
	//主循环参数
	static timeval lasttime;
};
}
#endif

