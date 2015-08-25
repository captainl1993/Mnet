/************************************************************
 * FileName: mconitem.h
 * Author:   毛自豪
 * Version : v0.1
 * Date:	    2015-08-24
 * Description: 连接对象的基本上下文，保存了连接的基本信息和bufferevent
 * Version:  v0.1
 * Function List:
 ***********************************************************/
#ifndef DATA_HANDLE_H_
#define DATA_HANDLE_H_
namespace mnet
{
struct ConnItem
{
	int session_id; 											//会话ID
	evutil_socket_t conn_fd;
	evutil_socket_t log_fd;
	void * pthis; 												//指向线程对象的指针
	int data_remain_length; 								//剩余未处理完数据
	bool operator<(const ConnItem &ci) const
	{
		return (session_id < ci.session_id);
	}
	bufferevent * bev; 										//指向连接对应的bufferevent
};
}
#endif

