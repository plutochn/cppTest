#ifndef _zz_singleton_h_
#define _zz_singleton_h_

#include <stdlib.h>

#include "thread.h"

namespace zz {
template<typename T>
class singleton_t
{
public:
	static T& instance()
	{
		zz_pthread_once(&m_inited,init );
		return *m_instance_ptr;
	}

	static  T* instance_ptr()
	{
		zz_pthread_once(&m_inited,init );
		return m_instance_ptr;
	}

private:
	static void init()
	{
		m_instance_ptr = new T();
		atexit(destroy);
	}

	static void destroy()
	{
		if(NULL != m_instance_ptr)
		{
			delete m_instance_ptr;
			m_instance_ptr = NULL;
		}
	}

private:
	static T*					m_instance_ptr;
	static zz_pthread_once_t	m_inited;
};

template<class T>
zz_pthread_once_t singleton_t<T>::m_inited(0);

template<class T>
T* singleton_t<T>::m_instance_ptr(NULL);

}

#endif