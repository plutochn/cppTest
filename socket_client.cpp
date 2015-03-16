#include "socket_client.h"
#include "net_factory.h"

#include <iostream>
using namespace std;

namespace zz {

void socket_client_t::on_open(io_demultiplexer_i* poll_, sock_connection_info& addr_)
{
	net_factory_t::global_data.socket_mgr.add_client(this);

	poll_->register_fd(this);
	m_addr = addr_;
	m_io_channel.post_read_req(this);

	string hello = "Welcome to echo server !\r\n";
	async_send(hello);
}

int	socket_client_t::do_cleanup()
{
	int ret = 0;
	net_factory_t::global_data.socket_mgr.remove_client(this);
	delete this;
	return ret;
}

int	socket_client_t::handle_read()
{
	int ret = 0;
	m_tq->produce(task_binder::bind(&socket_client_t::read_impl,this));

	return ret;
}

int	socket_client_t::handle_write()
{
	int ret = 0;
	m_tq->produce(task_binder::bind(&socket_client_t::write_impl,this));

	return ret;
}

int	 socket_client_t::handle_error()
{
	int ret = 0;
	m_tq->produce(task_binder::bind(&socket_client_t::error_impl,this));
	return ret;
}

void socket_client_t::close()
{
	net_factory_t::global_data.socket_mgr.remove_client(this);
	m_tq->produce(task_binder::bind(&socket_client_t::close_impl,this));
}

void socket_client_t::close_impl()
{
	if (m_flag_sock_closed == false) 
	{
		closesocket(m_fd_sock);
		m_flag_sock_closed = true ;
	}

	if (m_io_channel.has_pending_req() == false)
	{
		do_cleanup();
	}
}

void socket_client_t::error_impl()
{
	m_io_channel.dec_pending_req_num();
	close_impl();
}

void socket_client_t::read_impl()
{
	m_io_channel.dec_pending_req_num();

	string recv_data;

	int ret = m_io_channel.read(this,recv_data);

	if (ret > 0)
	{
		std::cout<<"recv data len:"<<ret;
		std::cout<<"{recv data}"<<recv_data.c_str();
	}

	m_io_channel.post_read_req(this);
}

void socket_client_t::write_impl()
{

}

}
