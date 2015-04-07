#include "net/net_factory.h"
#include "net/io_channel_impl.h"
#include "net/acceptor_impl.h"

#define TASK_QUEUE_NUM 5

namespace zz {

net_factory_t::global_data_t net_factory_t::global_data(TASK_QUEUE_NUM);

acceptor_i* net_factory_t::listen(string& host_, msg_handler_i* handler_)
{
	global_data.start();

	acceptor_t* acceptor_ = new acceptor_t(&global_data.tqp, &global_data.poll, handler_);

	if (0 != acceptor_->open(host_) )
	{
		acceptor_->do_cleanup();
		return NULL;
	}

	global_data.socket_mgr.add_accptor(acceptor_);

	return acceptor_;
}

connector_t* net_factory_t::connect(string& host_, msg_handler_i* handler_)
{
	global_data.start();

	connector_t* connector_ = new connector_t(&global_data.tqp, &global_data.poll, handler_);

	if ( 0 != connector_->connect(host_) )
	{
		connector_->close();
		delete connector_;

		connector_ = NULL;
	}

	return connector_;
}

} // namespace zz