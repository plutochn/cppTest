#ifndef _iocp_impl_h_
#define	_iocp_impl_h_

#include "netbase.h"
#include "thread.h"

namespace zz {

class iocp_poll : public io_demultiplexer_i
{
public:
	enum completion_key
	{
		eKeyNormal,
		eKeyExit
	};

public:
	iocp_poll():
	  m_fd_iocp(NULL),
	  m_num_pending_io_req(0)
	{}
	
	virtual int event_loop();
	virtual int register_fd(socket_i*);
	virtual int unregister_fd(socket_i*);

	int		start();
	void	stop();

	void	inc_pending_req_num()
	{
		lock_guard_t guard(m_lock);
		m_num_pending_io_req++;
	}

	void	dec_pending_req_num()
	{
		lock_guard_t guard(m_lock);
		m_num_pending_io_req--;
	}

	bool	has_no_pending_req()
	{
		lock_guard_t guard(m_lock);
		return m_num_pending_io_req == 0;
	}

private:
	/**
	 * ͬ��m_num_pending_io_req �ķ���.
	 */
	mutex_t		m_lock;
	int			m_num_pending_io_req;

	bool		m_exit_flag;

	fd_iocp_t	m_fd_iocp;
	thread_t	m_thread_event_poll;
};

}
#endif	// _iocp_impl_h_