#include "zzrpc_broker.h"
#include "net_factory.h"
#include "zzrpc_ops.h"

#include <iostream>
using namespace std;

namespace zz {

zzrpc_broker_t::zzrpc_broker_t(string& host_, msg_handler_i* hook_handler_):
	m_host(host_),
	m_hook_handler(hook_handler_),
	m_acceptor(NULL)
{}

int zzrpc_broker_t::handle_rpc_client_reg(rpc_reg_client_msg_t& msg_, socket_ptr_t sock_)
{
	return 0;
}

int zzrpc_broker_t::handle_msg(msg_t& msg_, socket_ptr_t sock_)
{
	if (NULL != m_hook_handler)
	{
		m_hook_handler->handle_msg(msg_, sock_);
	}

	uint32_t cmd_id = msg_.cmd_id();

	zzslot_callback_t* callback = m_slot_interface.get(cmd_id);

	if (NULL == callback)
	{
		cout<<"not found callback func with cmd_id = %u"<<cmd_id<<endl;
		return -1;
	}

	else
	{
		zzslot_msg_arg_t args(sock_, msg_.body);
		callback->exe(args);

	}
	return 0;
}

int zzrpc_broker_t::handle_broken(socket_ptr_t sock_)
{
	if (NULL != m_hook_handler)
	{
		m_hook_handler->handle_broken(sock_);
	}
	return 0;
}

int zzrpc_broker_t::handle_open(socket_ptr_t sock_)
{
	if (NULL != m_hook_handler)
	{
		m_hook_handler->handle_open(sock_);
	}
	return 0;
}

int test_handle_rpc_client_reg(rpc_reg_client_msg_t& msg_, socket_ptr_t sock_)
{
	cout<<"test_handle_rpc_client_reg begin"<<endl;
	cout<<msg_.m_service_name<<endl;
	cout<<"test_handle_rpc_client_reg end"<<endl;

	return 0;
}

int	zzrpc_broker_t::start()
{
	
	m_acceptor = net_factory_t::listen(m_host, this);

	if (m_acceptor == NULL)
	{
		return -1 ;
	}

	m_slot_interface.add(k_rpc_reg_client,zzrpc_ops_t::gen_callback(test_handle_rpc_client_reg));

	m_thread.create(task_binder::bind(&task_queue_t::consume,&m_tq),1);
	return 0;
}

void zzrpc_broker_t::stop()
{
	if (NULL != m_acceptor)
	{
		m_acceptor->close();
	}

	m_tq.stop();
	m_thread.join_all();
}

}