#ifndef _net_factory_h_
#define _net_factory_h_

#include "base/zz_compile_opt.h"
#include "net/socket_ops.h"
#include "net/acceptor_i.h"
#include "net/socket_client.h"
#include "net/connector.h"
#include "concurrency/task_queue_i.h"
#include "net/iocp_impl.h"
#include "concurrency/thread.h"

#include <vector>

namespace zz {

class LIBRARY_API net_factory_t
{
public:
	struct socket_mgr_t
	{
	public:
		int start(void)
		{
			int ret = 0;

			tid_t tid;
			ret = tq_thread.create(task_binder::bind(&(task_queue_t::consume), &tq),tid ); 

			return ret;
		}

		void stop(void)
		{
			tq.stop();
			tq_thread.join_all();
		}

		void close_all_alive_socket()
		{
			tq.produce(task_binder::bind(&socket_mgr_t::do_close_all_alive_socket,this) );
		}

		void remove_accptor(acceptor_i* svr_)
		{
			tq.produce(task_binder::bind(&socket_mgr_t::do_remove_accptor,svr_,this) );
		}

		void add_accptor(acceptor_i* svr_)
		{
			tq.produce(task_binder::bind(&socket_mgr_t::do_add_accptor,svr_, this) );
		}

		void remove_client(socket_client_t* conn_)
		{
			tq.produce(task_binder::bind(&socket_mgr_t::do_remove_client,conn_,this) );
		}

		void add_client(socket_client_t* conn_)
		{
			tq.produce(task_binder::bind(&socket_mgr_t::do_add_client,conn_,this) );
		}

	protected:
		void do_close_all_alive_socket(void)
		{
			for ( size_t i =0; i< vec_acceptor_alive.size(); i++)
			{
				vec_acceptor_alive[i]->close();
			}

			for ( size_t i =0; i< vec_connection_alive.size(); i++)
			{
				vec_connection_alive[i]->close();
			}
		}

		void do_remove_accptor(acceptor_i* svr_)
		{
			std::vector<acceptor_i*>::iterator it_ = vec_acceptor_alive.begin();

			for (; it_!= vec_acceptor_alive.end(); it_++)
			{
				if (*it_ == svr_)
				{
					vec_acceptor_alive.erase(it_);
					break;
				}
			}
		}

		void do_add_accptor(acceptor_i* svr_)
		{
			 
			vec_acceptor_alive.push_back(svr_);
			 
		}

		void do_remove_client(socket_client_t* conn_)
		{
			 
			std::vector<socket_client_t*>::iterator it_ = vec_connection_alive.begin();

			for (; it_!= vec_connection_alive.end(); it_++)
			{
				if (*it_ == conn_)
				{
					vec_connection_alive.erase(it_);
					break;
				}
			}
			 
		}

		void do_add_client(socket_client_t* conn_)
		{
		 
			vec_connection_alive.push_back(conn_);
			 
		}
		
	private:
		task_queue_t						tq;
		thread_t							tq_thread;

		std::vector<acceptor_i*>			vec_acceptor_alive;
		std::vector<socket_client_t*>		vec_connection_alive;
	};

public:
	struct global_data_t
	{
		/**
		 * @param tq_num_ {int} 任务队列池中的任务队列数量.
		 */
		global_data_t(int tq_num_):
			tqp(tq_num_)
		{
			started_flag = false;
		}

		int  start()
		{
			int ret = 0;

			if (started_flag)
			{
				return ret;
			}

			socket_ops_t::init_socket_lib();

 			ret = socket_mgr.start();
			if (0 != ret)
			{
				return ret;
			}

			ret = tqp.start(worker_thread);
			if (0 != ret)
			{
				return ret;
			}

			ret = poll.start();

			started_flag = true;

			return ret;
		}

		void  stop()
		{
			socket_mgr.close_all_alive_socket();
			socket_mgr.stop();

			poll.stop();
			tqp.stop();
			worker_thread.join_all();
			started_flag = false;
			
			socket_ops_t::release_socket_lib();

		}

		socket_mgr_t		socket_mgr;			
		task_queue_pool_t	tqp;
		iocp_poll			poll;
		bool				started_flag;
		thread_t			worker_thread;
	};

public:
	static global_data_t global_data;

	static acceptor_i* listen(string& host_, msg_handler_i* handler_);
	static connector_t* connect(string& host_, msg_handler_i* handler_);
};

}// namespace zz
#endif