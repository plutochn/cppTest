#ifndef _zzrpc_service_
#define _zzrpc_service_

#include "net/connector.h"
#include "rpc/zzrpc_base.h"
#include "concurrency/thread.h"
#include "concurrency/task_queue_i.h"

namespace zz {

class LIBRARY_API zzrpc_service_t : public zzrpc_base_t
{
public:
	struct session_ctx_t
	{
		session_ctx_t():
			node_id(invalid_node_id),
			node_type(k_rpc_node_invalid)
		{}
		uint32_t	node_id;
		int			node_type;
	};

public:
	zzrpc_service_t(string& service_name, string& mb_host_, msg_handler_i* hook_handler_=NULL);

	virtual int handle_msg(msg_t& msg_, socket_ptr_t sock_);
	virtual int handle_broken(socket_ptr_t sock_);
	virtual int handle_open(socket_ptr_t sock_);

	int		start();
	void	stop();

	virtual int bind_callback_with_cmd();

	int	handle_reg_svr_to_mb_ret(msg_reg_svr_to_mb_ret& msg_, socket_ptr_t sock_);

	int connect_master_broker();
	int connect_slave_broker();

protected:
	connector_t*			m_mb_connector;
	vector<connector_t*>	m_sb_connector;
	uint32_t				m_sb_idx;

	thread_t				m_thread;
	task_queue_t			m_tq;
	string					m_mb_host;

private:
	string					m_service_name;
	uint32_t				m_node_id;
};

}// namespace zz end

#endif // _zzrpc_service_