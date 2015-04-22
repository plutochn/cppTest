#include "rpc/zzrpc_service.h"
#include "net/net_factory.h"
#include "rpc/zzrpc_ops.h"

#include <iostream>
using namespace std;

namespace zz {

zzrpc_service_t::zzrpc_service_t(string& service_name, string& host_, msg_handler_i* hook_handler_):
	zzrpc_base_t(hook_handler_),
	m_service_name(service_name),
	m_mb_host(host_),
	m_mb_connector(NULL),
	m_status(rpc_service_status_e::stopped)
{
	m_node_id = invalid_node_id;
}

int zzrpc_service_t::handle_msg(msg_t& msg_, socket_ptr_t sock_)
{
	return zzrpc_base_t::handle_msg(msg_, sock_);
}

int	zzrpc_service_t::start()
{
	int ret = 0;

	try
	{
		bind_callback_with_cmd();

		/* 进入正在启动状态 */
		m_status = rpc_service_status_e::starting;

		m_thread.create(task_binder::bind(&task_queue_t::consume,&m_tq),1);

		ret = connect_master_broker();

		if (0 != ret)
		{
			cout<<"[zzrpc_service_t::start] connect to mb fail!"<<endl;
			throw ret;
		}

		while (rpc_service_status_e::starting == m_status)
		{
			Sleep(250);
		}

		if (invalid_node_id == m_node_id)
		{
			ret = -1;
			throw ret;
		} 

	}
	catch(int)
	{
		m_status = rpc_service_status_e::stopped;
	}
	return ret;
}

int zzrpc_service_t::connect_master_broker()
{
	int ret = 0;
	m_mb_connector = net_factory_t::connect(m_mb_host, this);

	if (m_mb_connector == NULL)
	{
		return -1 ;
	}

	msg_reg_svr_to_mb msg_;
	msg_.m_service_name = m_service_name;

	msg_send_tool_t::send(msg_, k_rpc_reg_delegator_service, m_mb_connector->get_sock());

	return ret;
}

connector_t* zzrpc_service_t::__connect_slave_broker(string& sb_host)
{
	connector_t* sb_connect = net_factory_t::connect(sb_host, this);

	if (NULL == sb_connect)
	{
		cout<<"connect sb["<<sb_host.c_str()<<"] fail!"<<endl;
		return NULL;
	}

	msg_reg_svr_to_sb msg_;
	msg_.m_service_name = m_service_name;
	msg_.m_svr_node_id = m_node_id;
	
	msg_send_tool_t::send(msg_, k_rpc_reg_delegator_service, sb_connect->get_sock());

	return sb_connect ;
}

int zzrpc_service_t::connect_slave_brokers()
{
	int ret = 0;

	for (size_t i = 0; i<m_slave_broker_host_vec.size(); i++)
	{
		string sb_host = m_slave_broker_host_vec[i];

		__connect_slave_broker(sb_host);
	}
	return ret;
}

void zzrpc_service_t::stop()
{
	if (NULL != m_mb_connector) 
	{
		m_mb_connector->close();
	}

	m_tq.stop();
	m_thread.join_all();
}

int zzrpc_service_t::handle_reg_svr_to_mb_ret(msg_reg_svr_to_mb_ret& msg_, socket_ptr_t sock_) 
{
	m_tq.produce(task_binder::bind(&zzrpc_service_t::handle_reg_svr_to_mb_ret_impl,msg_,sock_,this));

	return 0;
}

int	zzrpc_service_t::handle_reg_svr_to_mb_ret_impl(msg_reg_svr_to_mb_ret& msg_, socket_ptr_t sock_)
{
#ifdef _DEBUG
	cout<<"zzrpc_service_t::handle_reg_svr_to_mb_ret_impl called!"<<endl;
#endif
	cout<<"sb_idx:"<<msg_.m_bind_slave_broker_idx<<"node_id:"<<msg_.m_svr_node_id_allcated<<endl;

	m_node_id = msg_.m_svr_node_id_allcated;
	m_sb_idx = msg_.m_bind_slave_broker_idx;

	connect_slave_brokers();

	return 0;
}

int zzrpc_service_t::handle_broken(socket_ptr_t sock_)
{
<<<<<<< HEAD
=======
	
>>>>>>> 7f2582917344b36eb407ad273b74a1b249fc23b0

	zzrpc_base_t::handle_broken(sock_);

	return 0;
}

int zzrpc_service_t::handle_open(socket_ptr_t sock_)
{
	zzrpc_base_t::handle_open(sock_);

	return 0;
}

int zzrpc_service_t::bind_callback_with_cmd()
{
	m_slot_interface.add(k_rpc_reg_delegator_service_ret,zzrpc_ops_t::gen_callback(&zzrpc_service_t::handle_reg_svr_to_mb_ret, this));
	return 0;
}

}// namespace zz