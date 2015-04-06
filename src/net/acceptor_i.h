#ifndef _acceptor_i_h_
#define _acceptor_i_h_

#include "net/netbase.h"
#include "concurrency/task_queue_i.h"

namespace zz {

class	accept_channel_t;
class   socket_client_t;

class acceptor_i : public socket_i
{
public:
	acceptor_i(task_queue_pool_t* tqp_, io_demultiplexer_i* iocp_poll_):
		m_tqp(tqp_),
		m_iocp_poll(iocp_poll_),
		m_tq(NULL)
	{
	}

	int handle_read()
	{
		return -1;
	}

	int handle_write()
	{
		return -1;
	}
	
	/**
	 *	在指定的地址上等待客户端连接.
	 * @param host_ {std::string} 监听地址, 格式:127.0.0.1:8080.
	 * @return {int} 0:success; other:fail.
	 */
	virtual int open(std::string& host_) = 0 ;

	/**
	 *	停止接受连接.
	 */
	virtual void close() = 0;

	/**
	 * 创建一个客户端.
	 */
	virtual socket_client_t* create(int new_fd) = 0;

protected:
	io_demultiplexer_i*		m_iocp_poll;
	task_queue_pool_t*		m_tqp;
	task_queue_t*			m_tq;
};

}
#endif // _acceptor_i_h_