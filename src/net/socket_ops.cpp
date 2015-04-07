#include <stdlib.h>
#include <assert.h>

#include "net/socket_ops.h"

namespace zz {

LPFN_ACCEPTEX socket_ops_t::lpfnAcceptEx = NULL;

void socket_ops_t::release_socket_lib(void)
{
	assert(0 == WSACleanup() );
}

int socket_ops_t::init_socket_lib(void)
{
	int ret = 0;

	WSADATA wsa_data;
	 
	if(0 != WSAStartup(MAKEWORD(2,2), &wsa_data))
	{
		ret = WSAGetLastError();
	}
	else
	{
		if (LOBYTE(wsa_data.wVersion)!=2 || HIBYTE(wsa_data.wVersion)!=2)
		{
			ret = -1;
		}
		else
		{
			ret = load_mswsock_fn();
		}
	}

	return ret;
}

int socket_ops_t::load_mswsock_fn()
{
	int ret = 0;
	
	try
	{
		SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (INVALID_SOCKET == sock)
		{
			ret = WSAGetLastError();
			throw ret;
		}

		GUID GuidAcceptEx = WSAID_ACCEPTEX;
		DWORD dwBytes;

		ret =	WSAIoctl(sock, 
				SIO_GET_EXTENSION_FUNCTION_POINTER, 
				&GuidAcceptEx, 
				sizeof(GuidAcceptEx),
				&lpfnAcceptEx, 
				sizeof(lpfnAcceptEx), 
				&dwBytes, 
				NULL, 
				NULL);
		if (0 != ret)
		{
			ret = WSAGetLastError();
		}

		closesocket(sock);

	}
	catch (int)
	{
	}
	return ret;
}

BOOL socket_ops_t::AcceptEx(
			  __in          SOCKET sListenSocket,
			  __in          SOCKET sAcceptSocket,
			  __in          PVOID lpOutputBuffer,
			  __in          DWORD dwReceiveDataLength,
			  __in          DWORD dwLocalAddressLength,
			  __in          DWORD dwRemoteAddressLength,
			  __out         LPDWORD lpdwBytesReceived,
			  __in          LPOVERLAPPED lpOverlapped
			  )
{
	return lpfnAcceptEx(sListenSocket, 
						sAcceptSocket, 
						lpOutputBuffer, 
						dwReceiveDataLength,
						dwLocalAddressLength,
						dwRemoteAddressLength,
						lpdwBytesReceived,
						lpOverlapped);
}

SOCKET socket_ops_t::create_nonblock_socket(void)
{
	return	 WSASocket(AF_INET, 
		SOCK_STREAM, 
		IPPROTO_TCP, 
		NULL, 
		0, 
		WSA_FLAG_OVERLAPPED);
}

SOCKET socket_ops_t::connect(const char* addr, unsigned short port)
{
	SOCKET sock = create_nonblock_socket();

	if (INVALID_SOCKET == sock)
	{
		return sock;
	}

	sockaddr_in sock_addr_ ;
	ZeroMemory(&sock_addr_, sizeof(sockaddr_in));

	sock_addr_.sin_family = AF_INET;
	sock_addr_.sin_port = htons(port);
	sock_addr_.sin_addr.S_un.S_addr = inet_addr(addr);

	if ( 0 != ::connect(sock, (sockaddr*)(&sock_addr_), sizeof(sock_addr_) ) )
	{
		closesocket(sock);
		return INVALID_SOCKET;
	}

	return sock;
}

SOCKET socket_ops_t::bind_and_listen(const char* addr, unsigned short port)
{
	SOCKET sock = create_nonblock_socket();

	if (INVALID_SOCKET == sock)
	{
		return sock;
	}

	sockaddr_in sock_addr_ ;
	ZeroMemory(&sock_addr_, sizeof(sockaddr_in));

	sock_addr_.sin_family = AF_INET;
	sock_addr_.sin_port = htons(port);
	sock_addr_.sin_addr.S_un.S_addr = inet_addr(addr);

	if ( 0 != bind(sock, (sockaddr*)(&sock_addr_), sizeof(sock_addr_) ) )
	{
		closesocket(sock);
		return INVALID_SOCKET;
	}

	if ( 0 != listen(sock, 5))
	{
		closesocket(sock);
		return INVALID_SOCKET;
	}
	return sock;
}

}// namespace zz
