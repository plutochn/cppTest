#include "concurrency/thread.h"
#include <windows.h>

using namespace zz;

namespace zz {

void zz_pthread_once(zz_pthread_once_t* once_control, void (*func_)(void) )
{
	int old_value = InterlockedCompareExchange((LONG*)once_control, 1, 0);

	if (0 == old_value)
	{
		func_();
		InterlockedCompareExchange((LONG*)once_control, 2, 1);
	}
	else if (1 == old_value)
	{
		do 
		{
			Sleep(10);
			old_value = InterlockedCompareExchange((LONG*)once_control, 2, 2);

		} while (old_value == 1);
	}
	else if (2 == old_value)
	{

	}
	else
	{
		assert(0);
	}
}

unsigned int __stdcall thread_t::run_func(void* p)
{
	task_t* ptr_task = (task_t*)p;

	ptr_task->exe();
	delete ptr_task;

	return 0;
}

int	thread_t::create(IN task_t& task, OUT tid_t& tid)
{
	task_t* ptr_task = new task_t(task);

	tid.hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)run_func,ptr_task,0,&(tid.dwThreadId) );

	m_tid_list.push_back(tid);

	return 0;
}

int	thread_t::create(IN task_t& task, int thread_num)
{
	for (int i =0; i<thread_num; i++)
	{
		task_t* ptr_task = new task_t(task);

		tid_t tid;
		tid.hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)run_func,ptr_task,0,&(tid.dwThreadId) );

		m_tid_list.push_back(tid);
	}

	return 0;
}

void thread_t::join(tid_t& tid)
{
	WaitForSingleObject(tid.hThread, INFINITE);
}

void thread_t::join_all()
{
	tid_list_t::iterator it = m_tid_list.begin();
	for(; it != m_tid_list.end(); it++)
	{
		tid_t tid = *it;
		WaitForSingleObject(tid.hThread, INFINITE);
	}
}

}// namespace zz
