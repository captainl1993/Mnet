#include "mnet.h"
namespace mnet{
int WorkerThread::thread_count_ = 1;
char WorkerThread::msgbuf[MAX_BUF];
MsgHandler* WorkerThread::msghandler ;
//新建工作线程，初始化参数
WorkerThread::WorkerThread() {
	pthread_event_base_ = NULL;
	pnotify_event_ = NULL;
	notfiy_recv_fd_ = -1;
	notfiy_send_fd_ = -1;
	thread_id_ = thread_count_++;
}
//释放资源
WorkerThread::~WorkerThread() {
	if (notfiy_recv_fd_ != -1) {
		close(notfiy_recv_fd_);
		close(notfiy_send_fd_);
	}
	if (pthread_event_base_ != NULL)
		event_base_free(pthread_event_base_);
	if (pnotify_event_ != NULL)
		event_free(pnotify_event_);
}
//启动工作线程
bool WorkerThread::Run() {
	do {
		//初始化管道
		if (!CreateNotifyFds())
			break;
		//初始化事件处理回调
		if (!InitEventHandler())
			break;
		try {
			shared_ptr_thread_.reset(new std::thread([this]
			{	event_base_loop(pthread_event_base_, 0);}));
		} catch (...) {
			break;
		}

		return true;
	} while (0);

	return false;
}
bool WorkerThread::CreateNotifyFds() {
	int fds[2];
	bool ret = false;
	if (!pipe(fds/*, O_NONBLOCK*/)) //maomao 学习管道相关的应用
			{
		notfiy_recv_fd_ = fds[0];
		notfiy_send_fd_ = fds[1];
		ret = true;
	}
	return ret;
}
bool WorkerThread::AddConnItem(ConnItem& conn_item) {
	try {
		std::lock_guard<std::mutex> lock(conn_mutex_);
		list_conn_item_.push_back(conn_item);
	} catch (...) {

		return false;
	}
	return true;
}
bool WorkerThread::DeleteConnItem(ConnItem& conn_item) {
	try { //list_conn_item_是按session_id从小到大排序的
		std::lock_guard<std::mutex> lock(conn_mutex_);
		auto pos = std::lower_bound(list_conn_item_.begin(),
				list_conn_item_.end(), conn_item);
		if ((pos != list_conn_item_.end())
				&& (conn_item.session_id == pos->session_id))
			list_conn_item_.erase(pos);
	} catch (...) {
		return false;
	}
	return true;
}
bool WorkerThread::InitEventHandler() {
	do {
		pthread_event_base_ = event_base_new();
		if (pthread_event_base_ == NULL)
			break;
		pnotify_event_ = event_new(pthread_event_base_, notfiy_recv_fd_,
		EV_READ | EV_PERSIST, HandleConn, (void *) this);
		if (pnotify_event_ == NULL)
			break;
		if (event_add(pnotify_event_, 0))
			break;
		return true;
	} while (0);
	return false;
}
 MsgHandler* WorkerThread::GetMsgHandler()
{
	 return msghandler;
}
//设置消息处理类
void WorkerThread::SetHandler(MsgHandler& _handler){
	msghandler=&_handler;
}
//连接进入后的调用
void WorkerThread::HandleConn(evutil_socket_t fd, short what, void * arg) {
	//当连接请求到来时
	WorkerThread * pwt = static_cast<WorkerThread *>(arg);
	char buf[1];
	read(fd, buf, 1);//从sockpair的另一端读数据
	std::lock_guard<std::mutex> lock(pwt->conn_mutex_);
	ConnItem *pitem = &pwt->list_conn_item_.back(); //取出在容器中的位置
	//pwt->queue_conn_item_.pop();
	struct bufferevent * bev = bufferevent_socket_new(pwt->pthread_event_base_,pitem->conn_fd, BEV_OPT_CLOSE_ON_FREE);
	pitem->bev=bev;
	if (bev == NULL)
		return;

	//注册读取事件
	bufferevent_setcb(bev, ConnReadCb, NULL/*ConnWriteCb*/, ConnEventCB,
			pitem);

	bufferevent_enable(bev, EV_READ | EV_WRITE);
	msghandler->sessionOpened(pitem);
}
void WorkerThread::ConnReadCb(bufferevent * bev, void *ctx) {
	ConnItem * pitem = static_cast<ConnItem *>(ctx);
	//WorkerThread * pwt = static_cast<WorkerThread *>(pitem->pthis);
	//std::cout<<"thread :"<<pwt->thread_id_<<" recieveddata "<<std::endl;
	memset(msgbuf, 0, sizeof(msgbuf));
	//读取数据并返回长度
	size_t len= bufferevent_read(bev, msgbuf, MAX_BUF);
	msghandler->messageReceived(pitem,msgbuf,len);
}
void WorkerThread::ConnWriteCb(bufferevent *bev, void * ctx) {
	//TODO
	ConnItem * pitem = static_cast<ConnItem *>(ctx);
	msghandler->messageSent(pitem);
}
void WorkerThread::ConnEventCB(bufferevent *bev, short int events, void * ctx) {
	ConnItem * pitem = static_cast<ConnItem *>(ctx);
	WorkerThread * pwt = static_cast<WorkerThread*>(pitem->pthis);
	if (pitem->log_fd != -1)	//连接意外关闭，关闭写日志文件描述符
		close(pitem->log_fd);
	pwt->DeleteConnItem(*pitem);	//从线程队列中删除连接对象
	bufferevent_free(bev);
	msghandler->sessionClosed(pitem);
}
}
