#include "net/iocp_impl.h"
#include "net/iocp_event.h"
#include "net/io_channel_impl.h"

namespace zz {

#define CPU_CORE_NUM 4

int	iocp_poll::event_loop()
{
	int ret = 0;

	while(1)
	{
		DWORD				dwBytes;
		ULONG				ulCompletionKey;
		LPOVERLAPPED		ovl = NULL ;

		BOOL ret = GetQueuedCompletionStatus(m_fd_iocp, &dwBytes, &ulCompletionKey, &ovl, INFINITE);
		if (ret)
		{
			if(ulCompletionKey == eKeyExit)
			{
				break;
			}
		
			dec_pending_req_num();

			iocp_event_t* ovl_ctx = (iocp_event_t*)ovl;
			if (ovl_ctx->type == IOCP_ACCEPT)
			{
				ovl_ctx->handler->handle_accept(ovl_ctx->client_sock);
			}
			else if (ovl_ctx->type == IOCP_READ)
			{
				ovl_ctx->handler->handle_read();
			}
			else if (ovl_ctx->type == IOCP_WRITE)
			{
				ovl_ctx->handler->handle_write();
			}
		}

		else
		{
			ret = GetLastError();

			if (NULL != ovl)
			{
				dec_pending_req_num();

				iocp_event_t* ovl_ctx = (iocp_event_t*)ovl;
				ovl_ctx->handler->handle_error();
			}
			break;
		}
	}

	return ret;
}

int	iocp_poll::register_fd(socket_i* sock_)
{
	int ret = 0;

	if (NULL == CreateIoCompletionPort((HANDLE)sock_->socket(), m_fd_iocp, eKeyNormal, 0) )
	{
		ret =WSAGetLastError();
	}

	return ret;
}

int iocp_poll::unregister_fd(socket_i*)
{
	int ret = 0;
	return ret;
}

int	 iocp_poll::start()
{
	int ret = 0;

	m_fd_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (NULL == m_fd_iocp)
	{
		ret = GetLastError();
		return ret;
	}

	m_thread_event_poll.create(task_binder::bind(&iocp_poll::event_loop,this), CPU_CORE_NUM);

	m_exit_flag = false;
	return ret;
}

void iocp_poll::stop()
{
	if (NULL != m_fd_iocp)
	{
		/**
		 * µÈ´ýio²Ù×÷·µ»Ø.
		 */
		while (1)
		{
			if (has_no_pending_req())
				break;
			Sleep(200);
		}

		for (int i =0; i< CPU_CORE_NUM; i++)
		{
			assert(PostQueuedCompletionStatus(m_fd_iocp, 0, eKeyExit, NULL));
		}
	
		m_thread_event_poll.join_all();

		CloseHandle(m_fd_iocp);
		m_fd_iocp = NULL;
	}
}

}// namespace zz