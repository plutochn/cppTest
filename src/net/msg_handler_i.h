#ifndef _msg_handler_i_h_
#define _msg_handler_i_h_

#include "net/msg.h"
#include "net/netbase.h"

namespace zz {

class LIBRARY_API msg_handler_i
{
public:
	virtual int handle_msg(msg_t& msg_, socket_ptr_t sock_) = 0;
	virtual int handle_broken(socket_ptr_t sock_) = 0;
	virtual int handle_open(socket_ptr_t sock_) = 0;
};

}
#endif// _msg_handler_i_h_