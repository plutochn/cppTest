#include "net_factory.h"
#include "io_channel_impl.h"
#include "acceptor_impl.h"

#define TASK_QUEUE_NUM 5

namespace zz {

net_factory_t::global_data_t net_factory_t::global_data(TASK_QUEUE_NUM);

acceptor_i* net_factory_t::listen(string& host_)
{
	global_data.start();

	acceptor_i* acceptor_ = new acceptor_t(&global_data.tqp, &global_data.poll);

	acceptor_->open(host_);

	global_data.socket_mgr.add_accptor(acceptor_);

	return acceptor_;
}

}