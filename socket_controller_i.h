#ifndef _socket_controller_i_h_
#define _socket_controller_i_h_

#include "netbase.h"

namespace zz {

class socket_controller_i
{
public:
	virtual ~socket_controller_i(){}

	virtual int handle_read(const char* buf, size_t len, socket_ptr_t sock_) = 0;

	virtual int handle_broken(socket_ptr_t sock_) = 0;

	virtual int handle_open(socket_ptr_t sock_) = 0;
};

}
#endif // _socket_controller_i_h_