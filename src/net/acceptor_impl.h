#ifndef _acceptor_impl_h_
#define _acceptor_impl_h_

#include "base/zz_compile_opt.h"
#include "net/acceptor_i.h"
#include "net/msg_handler_i.h"

namespace zz {

class LIBRARY_API acceptor_t : public acceptor_i
{
public:
	acceptor_t(task_queue_pool_t* tqp_, io_demultiplexer_i* iocp_poll_, msg_handler_i* handler_);

	virtual int do_cleanup() ;

	virtual int handle_accept(fd_socket_t client_fd);

	virtual int handle_error();

	virtual int open(std::string& host_);

	virtual void close();

	virtual fd_socket_t socket()
	{
		return m_listen_sock;
	}

	socket_client_t* create(int new_fd);

protected:
	int accept_impl(fd_socket_t client_fd);
	void close_impl();
	void error_impl();

private:
	/**
	 * 套接字关闭标志. true:主动退出; false: IO错误,被动关闭.
	 */
	bool				m_flag_sock_closed;
	
	fd_socket_t			m_listen_sock;

	accept_channel_t	m_io_channel;

	msg_handler_i*      m_msg_handler;
};

}
#endif // _acceptor_impl_h_