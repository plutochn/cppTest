#include "net/common_socket_controller.h"

namespace zz {

int common_socket_controller_t::handle_read(const char* buf, size_t len, socket_ptr_t sock_)
{
	int ret = 0 ;

	while(len > 0)
	{
		size_t tmp_len = len;
		
		if ( m_msg.have_recv_complete_head(m_have_recv_size) )
		{
			/**
			 * 已经接受到完整的消息头.
			 */
			if (m_msg.add_body(buf,m_have_recv_size-sizeof(msg_head_t),tmp_len))
			{
				/**
				 * 已经接受到完整的消息.
			     */
				m_msg_handler->handle_msg(m_msg, sock_);

				m_msg.clear();
				m_have_recv_size = 0;
			}
			else
			{
				m_have_recv_size += tmp_len;
			}
		}
		else
		{
			m_msg.add_head(buf,m_have_recv_size,tmp_len);
			m_have_recv_size += tmp_len;
		}

		len-=tmp_len;
		
		buf += tmp_len;
	}

	return ret;
}

int common_socket_controller_t::handle_broken(socket_ptr_t sock_)
{
	int ret = 0 ;

	m_msg_handler->handle_broken(sock_);

	return ret;
}

int common_socket_controller_t::handle_open(socket_ptr_t sock_)
{
	int ret = 0 ;

	m_msg_handler->handle_open(sock_);

	return ret;
}

}