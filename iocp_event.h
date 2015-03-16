#ifndef _iocp_event_h_
#define _iocp_event_h_

#include "netbase.h"

namespace zz {

enum iocp_event_type_t
{
	IOCP_ACCEPT = 0,
	IOCP_READ,
	IOCP_WRITE
};

struct iocp_event_t
{
	OVERLAPPED			ovl;
	iocp_event_type_t	type;
	socket_i*			handler;
	WSABUF				wsa_buff;
	fd_socket_t			client_sock;
};

}

#endif // _iocp_event_h_