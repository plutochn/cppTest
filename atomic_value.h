#ifndef _zz_atomic_value_h_
#define _zz_atomic_value_h_

#include "mutex.h"
 

namespace zz {

template<typename T>
class atomic_value
{
public:
	typedef T object_t;

public:
	atomic_value(object_t object_):
	  m_object(object_)
	{

	}
    
    void set_value(object_t object_)
	{
		lock_guard_t lock_guard_(m_lock); 
		m_object = object_;
		 
	}

	object_t  get_value()
	{
	    lock_guard_t lock_guard_(m_lock); 
        object_t object_ = m_object;  
	 
		return object_;
	}

	object_t  get_value_and_set(object_t object_)
	{
		 
		lock_guard_t lock_guard_(m_lock); 
		object_t object_ret_ = m_object;
		m_object = object_;
		
		 
		return object_ret_;
	}

	object_t  add_value_and_get(object_t object_)
	{
		 
		 
		lock_guard_t lock_guard_(m_lock); 
		m_object += object_;

		 
		return m_object;
	}

private:
	mutex_t m_lock;
	object_t m_object;
};

}

#endif