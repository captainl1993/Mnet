#include "mnet.h"
namespace mnet {
int MMaster::current_max_session_id_ = 1;
timeval MMaster::lasttime;
int MMaster::micsec=1000;
//构造函数，初始化
MMaster::MMaster(int port, int num_of_threads) {
	last_thread_index_ = -1;
	listen_port_ = port;
	num_of_workerthreads_ = num_of_threads;
	server_base_ = NULL;
	server_conn_listenner_ = NULL;
}
MMaster::~MMaster() {
	if (server_base_ != NULL)
		event_base_free(server_base_);
	if (server_conn_listenner_ != NULL)
		evconnlistener_free(server_conn_listenner_);
}
//启动服务器聚合‘event timeout’类型不完全，无法被定义
bool MMaster::Run() {
	do {
		if (!InitWorkerThreads())
			break;
		if (!StartListen())
			break;
		return true;
	} while (0);
	return false;
}
void MMaster::WaitForListenThread() {
	main_listen_thread_->join();
}
//接收错误的调用
void MMaster::AcceptError(evconnlistener *listener, void *ptr) {
	//TODO
	//LibeventServer * pls=static_cast<LibeventServer *>(ptr);
}
//接收连接事件
void MMaster::AcceptConn(evconnlistener * listener, int sock, sockaddr * addr,
		int len, void *ptr) {
	MMaster * pls = static_cast<MMaster *>(ptr);
	//循环调用线程进行处理
	int cur_thread_index = (pls->last_thread_index_ + 1)
			% pls->num_of_workerthreads_; // 轮循选择工作线程
	pls->last_thread_index_ = cur_thread_index;
	//构建ConnItem对象
	ConnItem item;
	memset(&item, 0, sizeof(ConnItem));
	item.session_id = current_max_session_id_++; //会话ID
	item.conn_fd = sock;
	item.pthis =
			static_cast<void *>((pls->vec_worker_thread_[cur_thread_index]).get()); //将线程对象的指针加入ConnItem中
	pls->vec_worker_thread_[cur_thread_index]->AddConnItem(item);
	if (write(pls->vec_worker_thread_[cur_thread_index]->notfiy_send_fd_, "c",
			1) != 1) { //通知失败
		pls->vec_worker_thread_[cur_thread_index]->DeleteConnItem(item);
	}
}
bool MMaster::InitWorkerThreads() {
	//创建服务线程并将线程push到worker数组中
	bool ret = true;
	try {
		for (int i = 0; i < num_of_workerthreads_; ++i) {
			std::shared_ptr<WorkerThread> pwt(new WorkerThread);
			if (!pwt->Run()) {
				ret = false;
				break;
			}
			vec_worker_thread_.push_back(pwt);
		}
	} catch (...) {
		ret = false;
	}
	return ret;
}

void MMaster::timeout_cb(evutil_socket_t fd, short ev, void *arg) {
	timeval newtime, difference;
	event* timeout = static_cast<event*>(arg);
	evutil_gettimeofday(&newtime, NULL);
	evutil_timersub(&newtime, &lasttime, &difference);
	lasttime = newtime;
	WorkerThread::GetMsgHandler()->Process(newtime,difference);
	struct timeval tv;
	evutil_timerclear(&tv);
	tv.tv_sec = 0;
	tv.tv_usec = micsec;
	event_add(timeout, &tv);
}

//启动监听
bool MMaster::StartListen() {
	do {
		struct sockaddr_in sin;
		server_base_ = event_base_new();
		if (!server_base_)
			break;
		memset(&sin, 0, sizeof(sin));
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = htonl(0);
		sin.sin_port = htons(listen_port_);

		//设置AcceptConn事件 监听连接信息
		server_conn_listenner_ = evconnlistener_new_bind(server_base_,
				AcceptConn, this,
				LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, -1,
				(struct sockaddr*) &sin, sizeof(sin));
		if (!server_conn_listenner_)
			break;
		evconnlistener_set_error_cb(server_conn_listenner_, AcceptError);
		//每个线程都在跑server_base_的事件循环
		try {
			main_listen_thread_.reset(new std::thread([this]
			{
				//注册定时事件，定时调用
				timeval tv;
				int flags = 0;
			   event timeout;
				event_assign(&timeout, server_base_, -1, flags, timeout_cb, (void*) &timeout);
				evutil_timerclear(&tv);
				tv.tv_sec = 0;
				tv.tv_usec = micsec;
				event_add(&timeout, &tv);
				evutil_gettimeofday(&lasttime, NULL);
				event_base_dispatch(server_base_);
			}));
		} catch (...) {
			break;
		}
		return true;
	} while (0);
	return false;
}
}
