#ifndef _socket_client_h_
#define _socket_client_h_

#include "net/netbase.h"
#include "concurrency/task_queue_i.h"
#include "net/io_channel_impl.h"
#include "net/msg_handler_i.h"
#include  "net/socket_controller_i.h"

namespace zz {

class socket_client_t : public socket_client_i
{
public:
	socket_client_t(task_queue_t* tq_, fd_socket_t fd_sock_, socket_controller_i* sock_controller_):
	  m_tq(tq_),
	  m_fd_sock(fd_sock_),
	  m_flag_sock_closed(false),
	  m_sock_controller(sock_controller_)
	  {
	  }

	 ~socket_client_t()
	 {
		delete m_sock_controller;
	 }

	virtual void on_open(io_demultiplexer_i* poll_, sock_connection_info& addr_);

	int do_cleanup();
	int handle_read();
	int handle_write();
	int handle_error();

	virtual void close();

	int handle_accept(fd_socket_t client_fd)
	{
		return -1;
	}

	fd_socket_t socket()
	{
		return m_fd_sock;
	}

	virtual void async_send(std::string& data_)
	{
		if (false == data_.empty() )
		{
			m_send_buff.push_back(data_);
			m_io_channel.write(this, m_send_buff);
		}
	}

	virtual sock_connection_info& connection_info() 
	{
		return m_addr;
	}

protected:
	void close_impl();
	void error_impl();
	void read_impl();
	void write_impl();

protected:
	fd_socket_t				m_fd_sock;
	task_queue_t*			m_tq;
	rw_channel_t			m_io_channel;
	sock_connection_info	m_addr;
	socket_controller_i*    m_sock_controller;

private:
	/**
	 * 套接字关闭标志. true:主动退出; false: IO错误,被动关闭.
	 */
	bool						m_flag_sock_closed;

	std::vector<std::string>	m_send_buff;
};

}// namespace zz
#endif