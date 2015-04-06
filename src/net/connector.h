#ifndef _connector_h_
#define _connector_h_

#include "net/netbase.h"
#include "concurrency/task_queue_i.h"
#include "net/msg_handler_i.h"

namespace zz {

class   socket_client_t;

class connector_t
{
public:
	connector_t(task_queue_pool_t* tqp_, io_demultiplexer_i* iocp_poll_, msg_handler_i* handler_):
		m_tqp(tqp_),
		m_iocp_poll(iocp_poll_),
		m_tq(NULL),
		m_socket_client(NULL)
	{
	}
    
	/**
	 *	����ָ������.
	 * @param host_ {std::string} ���ӵ�ַ, ��ʽ:127.0.0.1:8080.
	 * @return {int} 0:success; other:fail.
	 */
	virtual int connect(string& host_) ;

	/**
	 *	�ر�����.
	 */
	virtual void close() = 0;

	/**
	 * ����һ���ͻ���.
	 */
	virtual socket_client_t* create(int new_fd);

protected:
	io_demultiplexer_i*		m_iocp_poll;
	task_queue_pool_t*		m_tqp;
	task_queue_t*			m_tq;
	msg_handler_i*			m_msg_handler;

private:
	socket_client_t*		m_socket_client;
	fd_socket_t				m_sock_fd;
};

}

#endif // _connector_h_
