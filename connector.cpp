#include "io_channel_impl.h"
#include "connector.h"
#include "socket_ops.h"
#include "socket_client.h"
#include "net_factory.h"
#include "strtool.h"
#include "common_socket_controller.h"

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

	m_tq = m_tqp->random_alloc(m_sock_fd);

	//m_tq->produce(task_binder::bind(test));

	ret = m_iocp_poll->register_fd(this);
	if (0 != ret)
	{
		return ret;
	}

	cout<<"<connector>连接主机:"<<addr.c_str()<<":"<<port<<endl;

	return ret;
}

void acceptor_t::close()
{
	 m_socket_client.close();
}

socket_client_t* acceptor_t::create(int new_fd)
{
	return new socket_client_t(m_tqp->random_alloc(new_fd), new_fd, new common_socket_controller_t(m_msg_handler));
}

int acceptor_t::accept_impl(fd_socket_t client_fd)
{
	m_io_channel.dec_pending_req_num();

	int ret = 0;

	const char * zz =  (char*)&m_listen_sock;
	ret = setsockopt(client_fd, 
		SOL_SOCKET, 
		SO_UPDATE_ACCEPT_CONTEXT, 
		(char*)&m_listen_sock, sizeof(m_listen_sock));
	
	if (0 != ret )
	{
		ret = WSAGetLastError();
		return ret;
	}

	sockaddr_in sa_local = {0};
	sockaddr_in sa_remote = {0};
	int len = sizeof(sockaddr_in);

	ret = getsockname(client_fd, (sockaddr*)&sa_local, &len);
	if (0 != ret)
	{
		ret = WSAGetLastError();
		return ret;
	}

	ret = getpeername(client_fd, (sockaddr*)&sa_remote, &len);
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

	socket_client_t* client = create(client_fd);

	client->on_open(m_iocp_poll,addr_);

	m_io_channel.post_accept_req(this);

	return ret;
}


}// namespace zz