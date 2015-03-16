#include "io_channel_impl.h"
#include "acceptor_impl.h"
#include "socket_ops.h"
#include "socket_client.h"
#include "net_factory.h"
#include "strtool.h"

#include <iostream>

namespace zz {

acceptor_t::acceptor_t(task_queue_pool_t* tqp_, io_demultiplexer_i* iocp_poll_):
	acceptor_i(tqp_, iocp_poll_),
	m_listen_sock(INVALID_SOCKET),
	m_flag_sock_closed(false)
{}

int acceptor_t::do_cleanup()
{
	int ret = 0;

	net_factory_t::global_data.socket_mgr.remove_accptor(this);
	delete this;

	return ret;
}

int acceptor_t::handle_accept(fd_socket_t client_fd)
{
	int ret = 0;
	m_tq->produce(task_binder::bind(&acceptor_t::accept_impl,client_fd, this));

	return ret;
}

int acceptor_t::handle_error()
{
	int ret = 0;
	m_tq->produce(task_binder::bind(&acceptor_t::error_impl,this));

	return ret;
}

void test()
{
	std::cout<<"This s a test"<<endl;
}

int acceptor_t::open(std::string& host_)
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

	m_listen_sock = socket_ops_t::bind_and_listen(addr.c_str(), port);

	if (m_listen_sock == INVALID_SOCKET)
	{
		ret = -1;
		return  ret;
	}

	m_tq = m_tqp->random_alloc(m_listen_sock);

	m_tq->produce(task_binder::bind(test));

	ret = m_iocp_poll->register_fd(this);
	if (0 != ret)
	{
		return ret;
	}

	ret = m_io_channel.post_accept_req(this);

	cout<<"<acceptor>监听主机:"<<addr.c_str()<<":"<<port<<endl;
	return ret;
}

void acceptor_t::close()
{
	net_factory_t::global_data.socket_mgr.remove_accptor(this);
	m_tq->produce(task_binder::bind(&acceptor_t::close_impl,this));
}

socket_client_t* acceptor_t::create(int new_fd)
{
	return new socket_client_t(m_tqp->random_alloc(new_fd), new_fd);
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

void acceptor_t::close_impl()
{
	if (m_flag_sock_closed == false) 
	{
		closesocket(m_listen_sock);
		m_flag_sock_closed = true ;
	}

	if (m_io_channel.has_pending_req() == false)
	{
		do_cleanup();
	}
}

/**
 *	发生错误直接调用close_impl函数关闭套接字.
 */
void acceptor_t::error_impl()
{
	m_io_channel.dec_pending_req_num();
   close_impl();
}

}// namespace zz