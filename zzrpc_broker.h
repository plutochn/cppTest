#ifndef _zzrpc_broker_
#define _zzrpc_broker_

#include "msg_handler_i.h"
#include "zzrpc_ops.h"
#include "zzrpc_broker.h"
#include "thread.h"
#include "task_queue_i.h"
#include "acceptor_i.h"
#include "codec.h"
#include "base/zzslot.h"

namespace zz {

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

class zzrpc_broker_t : public msg_handler_i
{
public:
	zzrpc_broker_t(string& host_, msg_handler_i* hook_handler_=NULL);

	int handle_rpc_client_reg(rpc_reg_client_msg_t&, socket_ptr_t);

	virtual int handle_msg(msg_t& msg_, socket_ptr_t sock_);
	virtual int handle_broken(socket_ptr_t sock_);
	virtual int handle_open(socket_ptr_t sock_);

	int		start();
	void	stop();

private:
	zzslot_t			m_slot_interface;

	acceptor_i*			m_acceptor;
	thread_t			m_thread;
	task_queue_t		m_tq;
	string				m_host;
	msg_handler_i*		m_hook_handler;
};

}

#endif //_zzrpc_broker_