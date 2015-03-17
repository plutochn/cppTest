#ifndef _common_socket_controller_h_
#define _common_socket_controller_h_

#include "socket_controller_i.h"
#include "msg_handler_i.h"

namespace zz {

class common_socket_controller_t : public socket_controller_i
{
public:
	common_socket_controller_t(msg_handler_i* handler_):
		m_msg_handler(handler_),
		m_have_recv_size(0)
	{}
	
	virtual int handle_read(const char* buf, size_t len, socket_ptr_t sock_);

	virtual int handle_broken(socket_ptr_t sock_);

	virtual int handle_open(socket_ptr_t sock_);

public:
	msg_handler_i* m_msg_handler;

protected:
	msg_t	m_msg;

	/**
	 * 一个完整消息已经接收的大小(包含消息头).
	 */
	size_t  m_have_recv_size;
};

}

#endif // _common_socket_controller_h_