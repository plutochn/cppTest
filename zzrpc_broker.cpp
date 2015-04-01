#include "zzrpc_broker.h"
#include "net_factory.h"
#include "zzrpc_ops.h"

#include <iostream>
using namespace std;

namespace zz {

zzrpc_broker_t::zzrpc_broker_t(string& host_, msg_handler_i* hook_handler_):
	zzrpc_base_t(hook_handler_),
	m_listen_host(host_),
	m_acceptor(NULL)
{}

int zzrpc_broker_t::handle_msg(msg_t& msg_, socket_ptr_t sock_)
{
	return zzrpc_base_t::handle_msg(msg_, sock_);
}

int zzrpc_broker_t::handle_broken(socket_ptr_t sock_)
{
	zzrpc_base_t::handle_broken(sock_);

	return 0;
}

int zzrpc_broker_t::handle_open(socket_ptr_t sock_)
{
	zzrpc_base_t::handle_open(sock_);

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
	
	m_acceptor = net_factory_t::listen(m_listen_host, this);

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

/*
 * zzrpc_master_broker
 */
zzrpc_master_broker_t::zzrpc_master_broker_t(string& host_, msg_handler_i* hook_handler_):
	zzrpc_broker_t(host_, hook_handler_)
{

}

int zzrpc_master_broker_t::handle_rpc_client_reg(rpc_reg_client_msg_t& msg_, socket_ptr_t sock_)
{
	return 0;
}

int zzrpc_master_broker_t::handle_broken(socket_ptr_t sock_)
{
	zzrpc_broker_t::handle_broken(sock_);

	return 0;
}

int zzrpc_master_broker_t::handle_open(socket_ptr_t sock_)
{
	zzrpc_broker_t::handle_open(sock_);

	return 0;
}

}