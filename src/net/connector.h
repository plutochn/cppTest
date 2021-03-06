#ifndef _connector_h_
#define _connector_h_

#include "base/zz_compile_opt.h"
#include "net/netbase.h"
#include "concurrency/task_queue_i.h"
#include "net/msg_handler_i.h"

namespace zz {

class   socket_client_t;

class LIBRARY_API connector_t
{
public:
	connector_t(task_queue_pool_t* tqp_, io_demultiplexer_i* iocp_poll_, msg_handler_i* handler_):
		m_tqp(tqp_),
		m_iocp_poll(iocp_poll_),
		m_socket_client(NULL),
		m_msg_handler(handler_)
	{
	}
    
	~connector_t()
	{
		close();
	}
	/**
	 *	连接指定主机.
	 * @param host_ {std::string} 连接地址, 格式:127.0.0.1:8080.
	 * @return {int} 0:success; other:fail.
	 */
	virtual int connect(string& host_) ;

	/**
	 *	关闭连接.
	 */
	virtual void close();

	/**
	 * 创建一个客户端.
	 */
	virtual socket_client_t* create(int new_fd);

	/**
	 * 连接成功.
	 */
	virtual	int on_connect_complete();

	socket_client_t* get_sock()
	{
		return m_socket_client;
	}

protected:
	io_demultiplexer_i*		m_iocp_poll;
	task_queue_pool_t*		m_tqp;
	msg_handler_i*			m_msg_handler;

private:
	socket_client_t*		m_socket_client;
	fd_socket_t				m_sock_fd;
};

}

#endif // _connector_h_
