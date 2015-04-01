#ifndef _zzrpc_base_h_
#define _zzrpc_base_h_

#include "msg_handler_i.h"
#include "zzrpc_ops.h"

#include <iostream>
using namespace std;

namespace zz {

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