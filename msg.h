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
	 * �����Ϣͷ.
	 * @param buf {const char*} ���뻺��.
	 * @param off {const char*} �Ѿ���ӵ���Ϣͷ�ֽ���.
	 * @param len {size_t} �������:���뻺�����ĳ���. �������:ʵ��д������ݳ���.
	 * @param {bool} �Ƿ�����д����Ϣͷ.
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
	 * �����Ϣ��.
	 * @param buf {const char*} ���뻺��.
	 * @param off {const char*} �Ѿ���ӵ���Ϣ���ֽ���.
	 * @param len {size_t} �������:���뻺�����ĳ���. �������:ʵ��д������ݳ���.
	 * @param {bool} �Ƿ�����д����Ϣ��.
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