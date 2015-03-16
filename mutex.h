#ifndef _zz_mutex_h_
#define _zz_mutex_h_

#include <assert.h>
#include <windows.h>

namespace zz {
	
class mutex_t
{
public:
	mutex_t()
	{
		InitializeCriticalSection(&m_cs);
	}

	~mutex_t()
	{
		DeleteCriticalSection(&m_cs);
	}

	void lock()
	{
		EnterCriticalSection(&m_cs);
	}

	void unlock()
	{
		LeaveCriticalSection(&m_cs);
	}

private:
	CRITICAL_SECTION m_cs;
};

class lock_guard_t
{
public:
	lock_guard_t(mutex_t& mutex_):
	  m_mutex(mutex_)
	{
		m_mutex.lock();
	}
    
	~lock_guard_t()
	{
		m_mutex.unlock();
	}

private:
	mutex_t& m_mutex;
};

class condition_var_t
{
public:
	condition_var_t()
	{
		m_hEvent = CreateEvent(NULL, false, false,NULL);
		assert(NULL != m_hEvent);
	}

	~condition_var_t()
	{
		CloseHandle(m_hEvent);
	}

	int wait()
	{
		WaitForSingleObject(m_hEvent,INFINITE);
		return 0 ;
	}

	void trigger()
	{
		SetEvent(m_hEvent);
	}

private:
	HANDLE m_hEvent;
};

}//	namespace zz

#endif