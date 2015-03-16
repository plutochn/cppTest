#ifndef _zz_msg_h_
#define _zz_msg_h_

namespace zz {

struct msg_head_t
{
	msg_head_t(uint16_t cmd_id_)

	uint16_t	cmd_id;
	uint16_t	flag;
	uint32_t	msg_body_szie;
};

}

#endif