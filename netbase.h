#ifndef _zz_net_base_
#define _zz_net_base_

# ifdef WIN32
  #include <winsock2.h>
  #include <mswsock.h>
  #pragma comment(lib,"ws2_32.lib")
  #pragma comment(lib,"mswsock.lib")
  typedef SOCKET fd_socket_t;
  typedef HANDLE fd_iocp_t;
# endif

#include <string>

namespace zz {

struct sock_connection_info
{
	sock_connection_info():
		local_addr(""),
		local_port(0),
		remote_addr(""),
		remote_port(0)
	{

	}

	/**
	 * 本地地址.
	 */
	std::string			local_addr;
	unsigned short		local_port;

	/**
	 * 远端地址.
	 */
	std::string			remote_addr;
	unsigned short		remote_port;
};

class socket_i
{
public:
	/**
	 * socket 关闭完成, 清理资源.
	 */
	virtual int do_cleanup() = 0;
	/**
	 * socket 可以读.
	 */
	virtual int handle_read() = 0; 

	/**
	 * socket 可以写.
	 */
	virtual int handle_write() = 0; 

	/**
	 * 一个新的客户端已被接受.
	 * @param client_fd {fd_socket_t} 客户端套接字.
	 */
	virtual int handle_accept(fd_socket_t client_fd) = 0;

	virtual int handle_error() = 0;

	virtual fd_socket_t socket() = 0;

	
};

class socket_client_i : public socket_i
{
public:
	virtual sock_connection_info& connection_info() = 0;

	template<typename T>
	T* get_ctx() const
	{
		return (T*)m_ctx;
	}

	void set_ctx(void* ctx)
	{
		m_ctx = ctx;
	}

	virtual void async_send(std::string& data_) {}

private:
	void*		m_ctx;
};

typedef socket_client_i* socket_ptr_t;

class io_demultiplexer_i
{
public:
	virtual int event_loop() = 0;
	virtual int register_fd(socket_i*) = 0;
	virtual int unregister_fd(socket_i*) = 0;
};

class io_channel_i
{
public:
	virtual int post_accept_req(socket_i*) = 0;
	virtual int post_read_req(socket_i*) = 0;
	virtual int post_write_req(socket_i*) = 0;

	/**
	 * 是否有挂起的IO请求未完成.
	 */
	virtual bool has_pending_req() = 0;
};

}
#endif	// _zz_net_base_