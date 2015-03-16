#ifndef io_channel_impl_h_
#define io_channel_impl_h_

#include "netbase.h"
#include "iocp_event.h"
#include <vector>

namespace zz {

class rw_channel_t : public io_channel_i
{
public:
	rw_channel_t():
	  m_num_pending_req(0)
	{}

	virtual int post_accept_req(socket_i*)
	{
		return -1; 
	} 

	virtual bool has_pending_req()
	{
		return m_num_pending_req > 0 ;
	}

	virtual int post_read_req(socket_i*) ;
	virtual int post_write_req(socket_i*) ;

	/**
	 * @param sock_ { socket_i* } 套接字接口.
	 * @param out_  { string& } 接受缓冲区.
	 * @return {int} >=0:读取数据的长度; <0:错误码.
	 */
	virtual int read(socket_i* sock_, std::string& out_) ;

	/**
	 * @param sock_ { socket_i* } 套接字接口.
	 * @param send_buff {vector<string>&} 发送缓冲区.
	 * @return {int} >=0:发送数据的长度; <0:错误码.
	 */
	virtual int write(socket_i* sock_, std::vector<std::string>& send_buff) ;

	void inc_pending_req_num()
	{
		m_num_pending_req++;
	}

	void dec_pending_req_num()
	{
		m_num_pending_req--;
	}

private:
	int				m_num_pending_req;		
	iocp_event_t	m_read_event;
	iocp_event_t	m_write_event;
};

class accept_channel_t : public io_channel_i
{
public:
	accept_channel_t():
	  m_num_pending_req(0)
	{}  
	
	virtual int post_accept_req(socket_i*);

	virtual int post_read_req(socket_i*) 
	{
		return -1;
	}

	virtual int post_write_req(socket_i*) 
	{
		return -1;
	}

	virtual bool has_pending_req()
	{
		return m_num_pending_req > 0 ;
	}

	void inc_pending_req_num()
	{
		m_num_pending_req++;
	}

	void dec_pending_req_num()
	{
		m_num_pending_req--;
	}

private:
	char			m_out_buff[200];

	int				m_num_pending_req;
	iocp_event_t	m_accept_event;
};

} // namespace zz
#endif