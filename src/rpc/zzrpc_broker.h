#ifndef _zzrpc_broker_
#define _zzrpc_broker_

#include "base/zz_compile_opt.h"
#include "rpc/zzrpc_base.h"
#include "concurrency/thread.h"
#include "concurrency/task_queue_i.h"
#include "net/acceptor_i.h"
#include "base/zzslot.h"

#include <set>
using namespace std;

namespace zz {

class LIBRARY_API zzrpc_broker_t : public zzrpc_base_t
{
public:
	zzrpc_broker_t(string& host_, msg_handler_i* hook_handler_=NULL);

	virtual int handle_msg(msg_t& msg_, socket_ptr_t sock_);
	virtual int handle_broken(socket_ptr_t sock_);
	virtual int handle_open(socket_ptr_t sock_);

	int		start();
	void	stop();

	virtual int bind_callback_with_cmd() = 0;

protected:
	acceptor_i*			m_acceptor;
	thread_t			m_thread;
	task_queue_t		m_tq;
	string				m_listen_host;
	 
};

class rpc_node_id_alloctor_t
{
public:
	rpc_node_id_alloctor_t()
	{
		m_alloc_id = 1;
	}

	uint32_t alloc_id()
	{
		if ( m_free_id_set.empty() )
		{
			m_alloc_id++;

			if (invalid_node_id == m_alloc_id)
			{
				m_alloc_id = 1;
			}
			return m_alloc_id;
		}
		else
		{
			set<uint32_t>::iterator it = m_free_id_set.begin();
			uint32_t id_ = *it;

			m_free_id_set.erase(it);
			return id_;
		}
	}

	void recycle_id(uint32_t id_)
	{
		m_free_id_set.insert(id_);
	}

private:
	/**
	 *	分配节点的起始id.
	 */
	uint32_t			m_alloc_id;
	set<uint32_t>		m_free_id_set;
};

/*
 * zzrpc_master_broker
 */
class LIBRARY_API zzrpc_master_broker_t : public zzrpc_broker_t
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
	zzrpc_master_broker_t(string& host_, msg_handler_i* hook_handler_=NULL);

	int handle_rpc_svr_reg(msg_reg_svr_to_mb&, socket_ptr_t);
	int handle_rpc_client_reg(msg_reg_client_to_mb&, socket_ptr_t);

	virtual int handle_broken(socket_ptr_t sock_);
	virtual int handle_open(socket_ptr_t sock_);

	virtual int bind_callback_with_cmd() ;

protected:
	int handle_rpc_svr_reg_impl(msg_reg_svr_to_mb&, socket_ptr_t);

private:
	rpc_node_id_alloctor_t	m_id_alloctor; 

	map<string, uint32_t>	m_svr_name_to_node_id;

	vector<string>			m_slave_broker_host;

	string					m_bridge_broker_host;
};

}

#endif //_zzrpc_broker_