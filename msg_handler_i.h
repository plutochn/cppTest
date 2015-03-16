#ifndef _msg_handler_i_h_
#define _msg_handler_i_h_

#include "msg.h"

namespace zz {

class msg_handler_i
{
public:
	virtual int handle_msg(msg_t& msg_, socket_ptr_t sock_);
	virtual int handle_broken(socket_ptr_t sock_);
};

}
#endif// _msg_handler_i_h_