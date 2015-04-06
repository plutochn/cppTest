#include "concurrency/task_queue_i.h"
#include "concurrency/thread.h"

namespace zz {

task_queue_pool_t::task_queue_pool_t(int tq_num_):
	m_task_queue_num(tq_num_)
{
	assert(tq_num_>=0);

	m_array_task_queue = new task_queue_t[tq_num_];
}

task_queue_pool_t::~task_queue_pool_t()
{
	if(NULL != m_array_task_queue)
		delete [] m_array_task_queue;
}

task_queue_t* task_queue_pool_t::alloc(int idx) const
{
	return &(m_array_task_queue[idx]);
}

task_queue_t* task_queue_pool_t::random_alloc(int key) const
{
	return	alloc(key%m_task_queue_num);
}

void task_queue_pool_t::stop()
{
	for(int i=0 ; i<m_task_queue_num; i++)
	{
		m_array_task_queue[i].stop();
	}
}

int task_queue_pool_t::start(thread_t& thread_)
{
	int nret = 0;
	for (int i = 0; i<m_task_queue_num;i++)
	{
		tid_t tid;
		nret = thread_.create(task_binder::bind(&(task_queue_t::consume), &(m_array_task_queue[i])),tid );

		if(0 != nret)
			break;
	}

	return nret;
}

}// namespace zz