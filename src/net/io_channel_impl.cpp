#include "net/io_channel_impl.h"

#include "net/socket_ops.h"
#include "net/net_factory.h"

namespace zz {

int rw_channel_t::post_read_req(socket_i* sock_) 
{
	int ret = 0;

    memset(&m_read_event,0,sizeof(m_read_event));

	m_read_event.handler = sock_;
	m_read_event.type = IOCP_READ;
	m_read_event.wsa_buff.buf = NULL;
	m_read_event.wsa_buff.len = 0;

	DWORD dwBytes;
	DWORD dwFlags = 0 ;
	ret = WSARecv(sock_->socket(),&(m_read_event.wsa_buff),1,&dwBytes,&dwFlags,&(m_read_event.ovl),NULL);
	if (ret != 0)
	{
		ret = WSAGetLastError();
		if (ret == WSA_IO_PENDING)
		{
			ret = 0;
		}
	}

	if (ret == 0)
	{
		net_factory_t::global_data.poll.inc_pending_req_num();
		m_num_pending_req++;
	}

	return ret;
}

int rw_channel_t::post_write_req(socket_i* sock_) 
{
	int ret = 0;

	memset(&m_write_event,0,sizeof(m_write_event));

	m_write_event.handler = sock_;
	m_write_event.type = IOCP_WRITE;
	m_write_event.wsa_buff.buf = NULL;
	m_write_event.wsa_buff.len = 0;

	DWORD dwBytes;
	ret = WSASend(sock_->socket(),&(m_write_event.wsa_buff),1,&dwBytes,0,&(m_write_event.ovl),NULL);
	if (ret != 0)
	{
		ret = WSAGetLastError();
		if (ret == WSA_IO_PENDING)
		{
			ret = 0;
		}
	}

	if (ret == 0)
	{	
		net_factory_t::global_data.poll.inc_pending_req_num();
		m_num_pending_req++;
	}

	return ret;
}

int rw_channel_t::read(socket_i* sock_, std::string& out_) 
{
	int ret = 0;

	unsigned long recv_data_len = 0;

	if (0 != ioctlsocket(sock_->socket(), FIONREAD , &recv_data_len))
	{
		ret = WSAGetLastError();
		return ret;
	}
    
	if (recv_data_len > 0)
	{
		char* buf = new char[recv_data_len];

		ret = recv(sock_->socket(),buf, recv_data_len,0);
		assert(ret == recv_data_len);

		out_.append(buf, ret);
		delete [] buf;
	}
	return ret;
}

int rw_channel_t::write(socket_i* sock_, std::vector<std::string>& send_buff) 
{
	int ret = 0;

	while(1)
	{
		if ( send_buff.empty() )
		{
			break;
		}

		std::string data_ = send_buff[0];
		int len = data_.length();

		int sended = send(sock_->socket(), data_.c_str(), len, 0);

		if (sended == SOCKET_ERROR)
		{
			DWORD dwLastError = WSAGetLastError();

			if (WSA_IO_PENDING != dwLastError)
			{
				ret = dwLastError;
			}
			break;
		}
		
		ret += sended;

		if (sended>0)
		{
			std::string new_data_ = data_.substr(sended, len-sended);
			if(new_data_.empty() == false)
			{
				send_buff[0] = new_data_;
			}
			else
			{
				send_buff.erase(send_buff.begin());
			}
		}
		
	}
	
	return ret;
}

int accept_channel_t::post_accept_req(socket_i* sock_)
{
	int ret = 0;
	
	fd_socket_t client_sock = socket_ops_t::create_nonblock_socket();

	if (INVALID_SOCKET == client_sock)
	{
		ret = WSAGetLastError();
		return ret;
	}

	memset(&m_accept_event,0,sizeof(m_accept_event));

	m_accept_event.handler = sock_;
	m_accept_event.type = IOCP_ACCEPT;
	m_accept_event.client_sock = client_sock;

	DWORD dwBytes;
	BOOL bRet = socket_ops_t::AcceptEx(sock_->socket(),client_sock,m_out_buff,0,16+sizeof(sockaddr_in),16+sizeof(sockaddr_in),&dwBytes,&(m_accept_event.ovl));
	if (FALSE == bRet)
	{
		ret = WSAGetLastError();
		if (ret == WSA_IO_PENDING)
		{
			ret = 0;
		}
	}

	if (ret == 0)
	{
		net_factory_t::global_data.poll.inc_pending_req_num();
		m_num_pending_req++;
	}
	return ret;
}

}
