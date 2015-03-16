#ifndef _socket_ops_h_
#define _socket_ops_h_

#include "netbase.h"

namespace zz {

#ifndef __in_out
#	define __in_out
#endif

struct  socket_ops_t
{
public:
	/**
	 * ≥ı ºªØ WinSock Lib.
	 * return {int} 0 success, other fail.
	 */
	static int		init_socket_lib(void);
	/**
	 *  Õ∑≈ WinSock Lib.
	 */
	static void		release_socket_lib(void);
	
	static BOOL AcceptEx(
		__in          SOCKET sListenSocket,
		__in          SOCKET sAcceptSocket,
		__in          PVOID lpOutputBuffer,
		__in          DWORD dwReceiveDataLength,
		__in          DWORD dwLocalAddressLength,
		__in          DWORD dwRemoteAddressLength,
		__out         LPDWORD lpdwBytesReceived,
		__in          LPOVERLAPPED lpOverlapped
		);

	static SOCKET create_nonblock_socket(void);

	static	SOCKET bind_and_listen(const char* addr, unsigned short port);
private:
	static int load_mswsock_fn();

private:
	static LPFN_ACCEPTEX lpfnAcceptEx;
	 
};

}
#endif // _socket_ops_h_