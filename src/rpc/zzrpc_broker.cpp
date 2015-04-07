#include "rpc/zzrpc_broker.h"
#include "net/net_factory.h"
#include "rpc/zzrpc_ops.h"

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

//int test_handle_rpc_client_reg(msg_reg_svr_to_mb& msg_, socket_ptr_t sock_)
//{
//	cout<<"test_handle_rpc_client_reg begin"<<endl;
//	cout<<msg_.m_service_name<<endl;
//	cout<<"test_handle_rpc_client_reg end"<<endl;
//
//	return 0;
//}

int	zzrpc_broker_t::start()
{
	
	m_acceptor = net_factory_t::listen(m_listen_host, this);

	if (m_acceptor == NULL)
	{
		return -1 ;
	}

	bind_callback_with_cmd();

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

int zzrpc_master_broker_t::handle_rpc_svr_reg(msg_reg_svr_to_mb& msg_, socket_ptr_t sock_)
{

#ifdef _DEBUG
	cout<<"zzrpc_master_broker_t::handle_rpc_svr_reg called!"<<endl;
	cout<<msg_.m_service_name<<endl;
#endif

	m_tq.produce(task_binder::bind(&zzrpc_master_broker_t::handle_rpc_svr_reg_impl,msg_,sock_,this));
	return 0;
}

int zzrpc_master_broker_t::handle_rpc_svr_reg_impl(msg_reg_svr_to_mb& msg_, socket_ptr_t sock_)
{
	uint32_t new_node_id = m_id_alloctor.alloc_id();
	m_svr_name_to_node_id[msg_.m_service_name] = new_node_id;

	msg_reg_svr_to_mb_ret msg_ret_;
	msg_ret_.m_slave_broker_host_vec = m_slave_broker_host;
	msg_ret_.m_svr_node_id_allcated = new_node_id;

	msg_ret_.m_bind_slave_broker_idx = -1;

	if (false == m_slave_broker_host.empty() )
	{
		msg_ret_.m_bind_slave_broker_idx = new_node_id%m_slave_broker_host.size();
	}

	msg_send_tool_t::send(msg_ret_, k_rpc_reg_delegator_service_ret, sock_);

	return 0;
}

int zzrpc_master_broker_t::handle_rpc_client_reg(msg_reg_client_to_mb& msg_, socket_ptr_t sock_)
{

#ifdef _DEBUG
	cout<<"zzrpc_master_broker_t::handle_rpc_client_reg called!"<<endl;
	cout<<msg_.m_client_desc<<endl;
#endif

	return 0;
}

int zzrpc_master_broker_t::handle_broken(socket_ptr_t sock_)
{
	session_ctx_t* ctx = sock_->get_ctx<session_ctx_t>();

	delete ctx;

	zzrpc_broker_t::handle_broken(sock_);

	return 0;
}

int zzrpc_master_broker_t::handle_open(socket_ptr_t sock_)
{
	zzrpc_broker_t::handle_open(sock_);

	session_ctx_t* ctx = new session_ctx_t;

	sock_->set_ctx(ctx);

	return 0;
}

int zzrpc_master_broker_t::bind_callback_with_cmd()
{
	m_slot_interface.add(k_rpc_reg_delegator_service,zzrpc_ops_t::gen_callback(&zzrpc_master_broker_t::handle_rpc_svr_reg, this));
	m_slot_interface.add(k_rpc_reg_delegator_client,zzrpc_ops_t::gen_callback(&zzrpc_master_broker_t::handle_rpc_client_reg, this));

	return 0;
}

}// namespace zz end