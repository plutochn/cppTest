#ifndef _zz_thread_
#define _zz_thread_

#include "concurrency/task_queue_i.h"

namespace zz {

typedef int zz_pthread_once_t;
void zz_pthread_once(zz_pthread_once_t* once_control, void (*func_)(void) );

struct tid_t
{
	HANDLE	hThread;
	DWORD	dwThreadId;

	tid_t():
		hThread(NULL),
		dwThreadId(0)
	{}
};

unsigned int inline current_tid()
{
	return ::GetCurrentThreadId();
}

class thread_t
{
public:
	typedef list<tid_t> tid_list_t;

public:
	int		create(IN task_t& task, OUT tid_t& tid);
	int		create(IN task_t& task, int thread_num);

	void	join(tid_t& tid);	
	void	join_all();

private:
	tid_list_t m_tid_list;

	static unsigned int __stdcall run_func(void* p);
};

} // namespace zz

#endif	// _zz_thread_