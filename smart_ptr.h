#ifndef _zz_smart_ptr_h_
#define _zz_smart_ptr_h_

#include <assert.h>
#include "atomic_value.h"

namespace zz {

/**
 * 引用计数类
 */
struct ref_count_t
{
	ref_count_t()
		:ref_num(0)
	{
	} 

	unsigned int inc_and_fetch()
	{
		return  ref_num.add_value_and_get(1);
	}

	bool dec_and_check_zero()
	{
		return 0 == ref_num.add_value_and_get(-1);
	}

	unsigned int value()
	{
		return ref_num.get_value();
	}
	atomic_value<unsigned int> ref_num;
};

struct ref_data_t
{ 
	ref_count_t  ref_num;
	mutex_t       rw_lock;
};

template<typename T>
class shared_ptr_t
{
public:
	typedef T object_t;
	typedef shared_ptr_t<T> self_type_t;

public:
	shared_ptr_t()
	{
		m_ptr_object = NULL ;
		m_ptr_ref_data = NULL ;
	}

	//@ warning shared_ptr_ should not from different thread
	shared_ptr_t(const self_type_t& shared_ptr_):
		m_ptr_object(shared_ptr_.get()),
		m_ptr_ref_data(shared_ptr_.get_ref_data())
	{
		if (NULL != m_ptr_ref_data)
		{
			m_ptr_ref_data->ref_num.inc_and_fetch();
		}
	}

	/**
	 * 构造函数
	 * @param ptr_object_ {object_t*} 对象指针
	 */
	shared_ptr_t(object_t* ptr_object_)
	{
		m_ptr_object = ptr_object_;
		m_ptr_ref_data = new ref_data_t;
		m_ptr_ref_data->ref_num.inc_and_fetch();
	}

	~shared_ptr_t()
	{
		reset();
	}

	//@ warning shared_ptr_ should not from different thread
	self_type_t& operator = (self_type_t& shared_ptr_)
	{
		if(&shared_ptr_ == this)
		{
			return *this;
		}

		reset();

		m_ptr_ref_data = shared_ptr_.get_ref_data();
		 
		if (NULL != m_ptr_ref_data)
		{
			m_ptr_ref_data->ref_num->inc_and_fetch();
		}
		return *this;
	}

	self_type_t& operator = (object_t obj_)
	{
		lock_guard_t lock_guard_( m_ptr_ref_data->rw_lock);

		if (NULL != m_ptr_object)
		{
			*m_ptr_object = obj_;
		}
		return *this;
	}

	operator object_t ()
	{
		lock_guard_t lock_guard_(m_ptr_ref_data->rw_lock);
		return *m_ptr_object;
	}

	self_type_t& operator + (object_t obj_)
	{
		lock_guard_t lock_guard_( m_ptr_ref_data->rw_lock);

		if (NULL != m_ptr_object)
		{
			(*m_ptr_object) += obj_;
		}
		return *this;
	}

	self_type_t& operator - (object_t obj_)
	{
		lock_guard_t lock_guard_( m_ptr_ref_data->rw_lock);

		if (NULL != m_ptr_object)
		{
			(*m_ptr_object) -= obj_;
		}
		return *this;
	}

	bool operator == (self_type_t& shared_ptr_)
	{
		 if(shared_ptr_.get() == m_ptr_object)
		 {
			 return true;
		 }
		 return false;
	}

	bool operator == (object_t* ptr_object_)
	{
		if(ptr_object_ == m_ptr_object)
		{
			return true;
		}
		return false;
	}

	object_t& operator *()
	{
		assert(NULL != m_ptr_object);
		return *get();
	}

	object_t* operator ->()
	{
		return get();
	}

private:
	object_t* get() const
	{
		lock_guard_t lock_guard_( m_ptr_ref_data->rw_lock);
		return m_ptr_object;
	}

	ref_data_t* get_ref_data() const
	{
		return m_ptr_ref_data;
	}

	void reset()
	{
		if (NULL != m_ptr_object)
		{
		
			if (m_ptr_ref_data->ref_num.dec_and_check_zero())
			{
				delete m_ptr_object;
				m_ptr_object = NULL;

				delete m_ptr_ref_data;
				m_ptr_ref_data = NULL ;
			}
		}
	}

private:
	object_t*		m_ptr_object;
	ref_data_t*		m_ptr_ref_data;
};

}

#endif