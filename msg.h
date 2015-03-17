#ifndef _zz_msg_h_
#define _zz_msg_h_

#include <string>

#include "base/zztype.h"

namespace zz {

struct msg_head_t
{
	uint16_t	cmd_id;
	uint16_t	flag;
	uint32_t	msg_body_size;
};

struct msg_t
{
public:
	/**
	 * 添加消息头.
	 * @param buf {const char*} 输入缓冲.
	 * @param off {const char*} 已经添加的消息头字节数.
	 * @param len {size_t} 输入参数:输入缓冲区的长度. 输出参数:实际写入的数据长度.
	 * @param {bool} 是否完整写入消息头.
	 */
	bool add_head(const char* buf, size_t off, IN OUT size_t& len)
	{
		 bool		ret;
		 size_t		copy_size;
		 
		 if ( (off+len)>=sizeof(msg_head_t) )
		 {
			 copy_size = sizeof(msg_head_t)-off;
			 ret = true;
		 }
		 else
		 {
			 copy_size = len;
			 ret = false ;
		 }

		 len = copy_size;
		 memcpy((char*)&head+off, buf, copy_size);

		 return ret;
	}
	/**
	 * 添加消息体.
	 * @param buf {const char*} 输入缓冲.
	 * @param off {const char*} 已经添加的消息体字节数.
	 * @param len {size_t} 输入参数:输入缓冲区的长度. 输出参数:实际写入的数据长度.
	 * @param {bool} 是否完整写入消息体.
	 */
	bool add_body(const char* buf, size_t off, IN OUT size_t& len)
	{
		bool		ret;
		size_t		copy_size;

		if ( (off+len)>=head.msg_body_size)
		{
			copy_size = head.msg_body_size-off;
			ret = true;
		}
		else
		{
			copy_size = len;
			ret = false ;
		}

		len = copy_size;
		body.append(buf, copy_size);

		return ret;
	}
	
	void clear()
	{
		memset(&head, 0, sizeof(head));
		body.clear();
	}

	bool have_recv_complete_head(size_t have_recv_size) const
	{
		return have_recv_size>= sizeof(head);
	}

	uint16_t cmd_id()
	{
		return head.cmd_id;
	}

	msg_head_t	head;
	std::string	body;
};

}

#endif