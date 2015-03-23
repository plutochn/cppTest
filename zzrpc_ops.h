#ifndef _zzrpc_ops_
#define _zzrpc_ops_

#include "netbase.h"
#include "base/zzslot.h"

namespace zz {

class zzslot_msg_arg_t : public zzslot_callback_arg_t
{
public:
	zzslot_msg_arg_t(socket_ptr_t sock, std::string& body_=string("") ):
		m_sock(sock),
		m_msg_body(body_)
	{}

	virtual std::string type() const
	{
		return "zzslot_msg_arg_t";
	}

	virtual std::string body() const
	{
		return m_msg_body;
	}

	virtual socket_ptr_t sock() const
	{
		return m_sock;
	}

private:
	std::string		m_msg_body;
	socket_ptr_t	m_sock;
};

class zzrpc_ops_t
{
public:
	template<typename R, typename MSG_>
	static zzslot_callback_t* gen_callback(R (*func)(MSG_&, socket_ptr_t))
	{
		class lambad_t : public zzslot_callback_t
		{
		public:
			lambad_t(R (*func_)(MSG_&, socket_ptr_t)):
				m_func(func_)
			{}

			virtual void exe(zzslot_callback_arg_t& args_)
			{
				if (args_.type() != "zzslot_msg_arg_t" )
				{
					return;
				}

				zzslot_msg_arg_t* pargs = (zzslot_msg_arg_t*)(&args_);

				MSG_ msg_;
				msg_.decode(pargs->body());
				m_func(msg_, pargs->sock());
			}

		private:
			R (*m_func)(MSG_&, socket_ptr_t);
		};

		return new lambad_t(func);
	}

};

}
#endif // _zzrpc_ops_