#ifndef _zzrpc_base_h_
#define _zzrpc_base_h_

#include "codec.h"
#include "msg_handler_i.h"
#include "zzrpc_ops.h"

#include <iostream>
using namespace std;

namespace zz {

enum node_id_reserve
{
	master_broker_node_id	 = 0,
	invalid_node_id			 = -1
};

enum rpc_node_type
{
	k_rpc_node_bridge_broker,
	k_rpc_node_master_broker,
	k_rpc_node_slave_broker,
	k_rpc_node_delegator_server, /*< 委托人,代表一个功能节点. */
	k_rpc_node_delegator_client,
	k_rpc_node_invalid
};

enum rpc_cmd_e
{
	k_rpc_reg_delegator_server =			10,
	k_rpc_reg_delegator_client,			/*  12 */
	k_rpc_reg_master_broker,			/*  13 */				
	k_rpc_reg_slave_broker,				/*  14 */
	k_rpc_reg_interface					/*  15 */
};

//------------------------------------
// msg define begin.
//------------------------------------

class msg_reg_client_to_mb : public msg_i
{
public:
	msg_reg_client_to_mb(string client_desc = ""):
	  m_client_desc(client_desc)
	  {}

	  string		m_client_desc;

protected:
	virtual void _encode()
	{
		m_encoder<<m_client_desc;
	}

	virtual void _decode()
	{
		m_decoder>>m_client_desc;
	}
};

/**
 * 注册服务到主代理 消息.
 */
class msg_reg_svr_to_mb : public msg_i
{
public:
	msg_reg_svr_to_mb(string svr_name = ""):
		m_service_name(svr_name)
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

/**
 * 注册服务到主代理 返回消息.
 */
class msg_reg_svr_to_mb_ret : public msg_i
{
public:
	msg_reg_svr_to_mb_ret()
	{}

	vector<string>		m_slave_broker_host_vec;

	uint32_t			m_svr_node_id_allcated;

	uint32_t			m_bind_slave_broker_idx;

protected:
	virtual void _encode()
	{
		m_encoder<<m_bind_slave_broker_idx<<m_svr_node_id_allcated<<m_slave_broker_host_vec;
	}

	virtual void _decode()
	{
		m_decoder>>m_bind_slave_broker_idx>>m_svr_node_id_allcated>>m_slave_broker_host_vec;
	}
};

/**
 * 注册从代理到主代理 消息.
 */
class msg_reg_sb_to_mb : public msg_i
{
public:
	msg_reg_sb_to_mb(string host_addr = ""):
		m_host_addr(host_addr)
	{}

	/**
	 * 从代理服务的监听地址.e.g. 127.0.0.0.1:7415 
	 */
	string		m_host_addr;

protected:
	virtual void _encode()
	{
		m_encoder<<m_host_addr;
	}

	virtual void _decode()
	{
		m_decoder>>m_host_addr;
	}
};

/**
 * 注册从代理到主代理 返回消息.
 */
class msg_reg_sb_to_mb_ret : public msg_i
{
public:
	msg_reg_sb_to_mb_ret()
	{}

	map<uint32_t, string>		m_svr_node_id_to_name;

	uint32_t					m_sb_node_id;

protected:
	virtual void _encode()
	{
		m_encoder<<m_sb_node_id<<m_svr_node_id_to_name;
	}

	virtual void _decode()
	{
		m_decoder>>m_sb_node_id>>m_svr_node_id_to_name;
	}
};

class msg_reg_svr_to_sb : public msg_i
{
public:
	msg_reg_svr_to_sb(string service_name = ""):
		m_service_name(service_name)
	{}

	string		m_service_name;
	uint32_t	m_svr_node_id;

protected:
	virtual void _encode()
	{
		m_encoder<<m_svr_node_id<<m_service_name;
	}

	virtual void _decode()
	{
		m_decoder>>m_svr_node_id>>m_service_name;
	}
};

//------------------------------------
// msg define end.
//------------------------------------

class zzrpc_base_t : public msg_handler_i
{
public:
	zzrpc_base_t(msg_handler_i* hook_handler_=NULL):
		m_hook_handler(hook_handler_)
	{}

	virtual int handle_msg(msg_t& msg_, socket_ptr_t sock_)
	{
		if (NULL != m_hook_handler)
		{
			m_hook_handler->handle_msg(msg_, sock_);
		}

		uint32_t cmd_id = msg_.cmd_id();

		zzslot_callback_t* callback = m_slot_interface.get(cmd_id);

		if (NULL == callback)
		{
			cout<<"not found callback func with cmd_id = "<<cmd_id<<endl;
			return -1;
		}

		else
		{
			zzslot_msg_arg_t args(sock_, msg_.body);
			callback->exe(args);

		}
		return 0;
	}

	virtual int handle_broken(socket_ptr_t sock_)
	{
		if (NULL != m_hook_handler)
		{
			m_hook_handler->handle_broken(sock_);
		}

		return 0;
	}

	virtual int handle_open(socket_ptr_t sock_)
	{
		if (NULL != m_hook_handler)
		{
			m_hook_handler->handle_open(sock_);
		}

		return 0;
	}

protected:
	zzslot_t			m_slot_interface;
	msg_handler_i*		m_hook_handler;

};

}

#endif // _zzrpc_base_h_