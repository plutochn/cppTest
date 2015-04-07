#ifndef _iocp_impl_h_
#define	_iocp_impl_h_

#include "base/zz_compile_opt.h"
#include "net/netbase.h"
#include "concurrency/thread.h"

namespace zz {

class LIBRARY_API iocp_poll : public io_demultiplexer_i
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
	 * 同步m_num_pending_io_req 的访问.
	 */
	mutex_t		m_lock;
	int			m_num_pending_io_req;

	bool		m_exit_flag;

	fd_iocp_t	m_fd_iocp;
	thread_t	m_thread_event_poll;
};

}
#endif	// _iocp_impl_h_