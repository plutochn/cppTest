#ifndef _zzrpc_broker_
#define _zzrpc_broker_

#include "zzrpc_base.h"
#include "thread.h"
#include "task_queue_i.h"
#include "acceptor_i.h"
#include "codec.h"
#include "base/zzslot.h"

#include <set>
using namespace std;

namespace zz {

enum rpc_node_type
{
	k_rpc_node_bridge_broker,
	k_rpc_node_master_broker,
	k_rpc_node_slave_broker,
	k_rpc_node_server,
};

enum rpc_cmd_e
{
	k_rpc_reg_client = 10,
	k_rpc_reg_interface
};

class rpc_reg_client_msg_t : public msg_i
{
public:
	rpc_reg_client_msg_t():
		m_service_name("")
	{}

	string		m_service_name;

protected:
	virtual void _encode()
	{
		m_encoder<<m_service_name;
	}

	virtual void _decode()
	{
		m_decoder>>m_service_name;
	}
};

class zzrpc_broker_t : public zzrpc_base_t
{
public:
	zzrpc_broker_t(string& host_, msg_handler_i* hook_handler_=NULL);

	virtual int handle_msg(msg_t& msg_, socket_ptr_t sock_);
	virtual int handle_broken(socket_ptr_t sock_);
	virtual int handle_open(socket_ptr_t sock_);

	int		start();
	void	stop();

private:
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
		m_alloc_id = 0;
	}

	uint32_t alloc_id()
	{
		if ( m_free_id_set.empty() )
		{
			m_alloc_id++;

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
class zzrpc_master_broker_t : public zzrpc_broker_t
{
public:
	zzrpc_master_broker_t(string& host_, msg_handler_i* hook_handler_=NULL);

	int handle_rpc_client_reg(rpc_reg_client_msg_t&, socket_ptr_t);

	virtual int handle_broken(socket_ptr_t sock_);
	virtual int handle_open(socket_ptr_t sock_);

private:
	rpc_node_id_alloctor_t	m_id_alloctor; 
};

}

#endif //_zzrpc_broker_