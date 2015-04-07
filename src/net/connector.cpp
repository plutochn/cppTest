#include "net/io_channel_impl.h"
#include "net/socket_ops.h"
#include "net/socket_client.h"
#include "net/net_factory.h"
#include "base/strtool.h"
#include "net/common_socket_controller.h"
#include "net/connector.h"

#include <iostream>
#include <string>
using namespace std;

namespace zz {

int connector_t::connect(string& host_)
{
	int ret = 0;

	vector<string> vec_strs;
	strtool_t::split(host_,"//",vec_strs);
	if ( vec_strs.size() != 2 )
	{
		return -1;
	}

	string str_ip_port = vec_strs[1];
	strtool_t::split(str_ip_port,":",vec_strs);
	if ( vec_strs.size() != 2 )
	{
		return -1;
	}

	string			addr			 =  vec_strs[0];
	unsigned int	port			 =  0;

	sscanf_s(vec_strs[1].c_str(), "%d", &port);

	m_sock_fd = socket_ops_t::connect(addr.c_str(), port);

	if (m_sock_fd == INVALID_SOCKET)
	{
		ret = -1;
		return  ret;
	}

	cout<<"<connector>连接主机:"<<addr.c_str()<<":"<<port<<endl;

	on_connect_complete();

	return ret;
}

void connector_t::close()
{
	 closesocket(m_sock_fd);
}

socket_client_t* connector_t::create(int new_fd)
{
	return new socket_client_t(m_tqp->random_alloc(new_fd), new_fd, new common_socket_controller_t(m_msg_handler));
}

int connector_t::on_connect_complete()
{
	m_socket_client = create(m_sock_fd);

	sockaddr_in sa_local = {0};
	sockaddr_in sa_remote = {0};
	int len = sizeof(sockaddr_in);

	int ret = getsockname(m_sock_fd, (sockaddr*)&sa_local, &len);
	if (0 != ret)
	{
		ret = WSAGetLastError();
		return ret;
	}

	ret = getpeername(m_sock_fd, (sockaddr*)&sa_remote, &len);
	if (0 != ret)
	{
		ret = WSAGetLastError();
		return ret;
	}

	sock_connection_info addr_;
	addr_.local_addr = inet_ntoa(sa_local.sin_addr);
	addr_.local_port = ntohs(sa_local.sin_port);

	addr_.remote_addr = inet_ntoa(sa_remote.sin_addr);
	addr_.remote_port = ntohs(sa_remote.sin_port);

	socket_client_t* client = create(m_sock_fd);

	client->on_open(m_iocp_poll,addr_);

	return ret;
}

}// namespace zz