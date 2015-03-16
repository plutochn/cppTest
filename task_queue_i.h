#ifndef _zz_task_queue_i_
#define _zz_task_queue_i_

#include <list>
using namespace std;

#include "mutex.h"

namespace zz {

class	thread_t;

class task_impl_i
{
public:
	
	virtual void exe() = 0;
	virtual task_impl_i* fork() = 0;
};

class task_t
{
public:
	task_t():
	  m_task_impl(NULL)
	{

	}

	task_t(task_impl_i* task_impl_):
		m_task_impl(task_impl_)
	{

	}

	task_t(const task_t& task_)	 
	{
		m_task_impl = task_.m_task_impl->fork();
	}

	task_t & operator = (const task_t& task_)
	{
		reset();
		m_task_impl = task_.m_task_impl->fork();

		return *this;
	}

	virtual ~task_t()
	{
		reset();
	}

    virtual void exe()
	{
		if(NULL != m_task_impl)
		{
			m_task_impl->exe();
		}
	}

protected:
	void reset()
	{
		if(NULL != m_task_impl)
		{
			delete m_task_impl;
			m_task_impl = NULL ;
		}
	}

private:
	task_impl_i* m_task_impl;
};

struct task_binder
{
	template<typename F>
	static task_t bind(F f)
	{
		struct lambada_t : public task_impl_i 
		{
			lambada_t(F f_):
				_f(f_)
			{

			}

			virtual void exe()
			{
				_f();
			}

			virtual task_impl_i* fork()
			{
				return new lambada_t(_f);
			}

			F _f;
		};

		return task_t(new lambada_t(f));
	}

	template<typename F, typename ARG0>
	static task_t bind(F f, ARG0 arg0)
	{
		struct lambada_t : public task_impl_i 
		{
				lambada_t(F f_,ARG0 arg0_):
			_f(f_),_arg0(arg0_)
			{

			}

			virtual void exe()
			{
				_f(_arg0);
			}

			virtual task_impl_i* fork()
			{
				return new lambada_t(_f,_arg0);
			}

			F _f;
			ARG0 _arg0;

		};

		return task_t(new lambada_t(f,arg0));
	}

	template<typename F, typename ARG0, typename ARG1>
	static task_t bind(F f,ARG0 arg0,ARG1 arg1)
	{
		struct lambada_t : public task_impl_i 
		{
			lambada_t(F f_,ARG0 arg0_,ARG1 arg1_):
		_f(f_),_arg0(arg0_),_arg1(arg1_)
		{

		}

		virtual void exe()
		{
			_f(_arg0,_arg1);
		}

		virtual task_impl_i* fork()
		{
			return new lambada_t(_f,_arg0,_arg1);
		}

		F _f;
		ARG0 _arg0;
		ARG1 _arg1;
		};

		return task_t(new lambada_t(f,arg0,arg1));
	}

	//! class member function bind
	template<typename RET, typename T>
	static task_t bind(RET (T::*f)(void), T * pobj)
	{
		struct lambada_t : public task_impl_i 
		{
			lambada_t(RET (T::*f_)(void),T * pobj_):
		_f(f_),_pobj(pobj_)
		{

		}

		virtual void exe()
		{
			(_pobj->*_f)();
		}

		virtual task_impl_i* fork()
		{
			return new lambada_t(_f, _pobj);
		}

		RET (T::*_f)(void);
		T* _pobj;

		};

		return task_t(new lambada_t(f,pobj));
	}

	template<typename RET, typename A0, typename B0 ,typename T>
	static task_t bind(RET (T::*f)(A0), B0 arg0, T * pobj)
	{
		 
		struct lambada_t : public task_impl_i 
		{
			lambada_t(RET (T::*f_)(A0 ),B0 arg0_,T * pobj_):
		_f(f_),_pobj(pobj_),_arg0(arg0_)
		{

		}

		virtual void exe()
		{
			(_pobj->*_f)(_arg0);
		}

		virtual task_impl_i* fork()
		{
			return new lambada_t(_f, _arg0, _pobj);
		}

		RET (T::*_f)(A0);
		B0 _arg0;
		T* _pobj;

		
		};

		return task_t(new lambada_t(f,arg0,pobj));
	}

};

class task_queue_t
{
public:
	typedef list<task_t> task_list_t ;

public:
	task_queue_t()
	{
		m_exit_flag = false;
	}
 
	void stop()
	{
		lock_guard_t guard(m_mutex);
		m_exit_flag = true;
		m_condition_var.trigger();
	}

	void produce(task_t& task)
	{
		lock_guard_t guard(m_mutex);

		if(false == m_exit_flag)
		{
			m_task_list.push_back(task);
			if ( 1 == m_task_list.size() )
			{
				m_condition_var.trigger();
			}
		}
	}

	/**
	 * 从任务队列中取出任务并执行
	 */
	void consume()
	{
		while(true)
		{
			task_list_t task_list;
			fetch_all_task(task_list);

			m_mutex.lock();
			bool exit_flag = m_exit_flag;
			m_mutex.unlock();

			if (0 == task_list.size())
			{
				if (m_exit_flag)
					break;
				m_condition_var.wait();
			}
			else
			{
				task_list_t::iterator it = task_list.begin();

				for(; it!= task_list.end(); it++)
				{
					task_t task = *it;
					task.exe();
				}

				task_list.clear();
			}
		}
	}

private:
	void fetch_all_task(task_list_t& task_list)
	{
		lock_guard_t guard(m_mutex);

		task_list = m_task_list;
		m_task_list.clear();
	}

private:
	bool				m_exit_flag;
	mutex_t				m_mutex;
	condition_var_t		m_condition_var;
	task_list_t			m_task_list;
};

/**
 * 任务队列池
 */
class task_queue_pool_t
{
public:
	/**
	 * @param tq_num_ {int} 任务队列池拥有的任务队列的数量
	 */
	task_queue_pool_t(int tq_num_);

	~task_queue_pool_t();
	/**
	 * @param idx {int} 任务队列的索引
	 * @return {task_queue_t*} 从任务队列池中分配一个任务队列
	 */
	task_queue_t* alloc(int idx) const;

	/**
	 * @param key {int} 任务队列的索引
	 * @return {task_queue_t*} 从任务队列池中分配一个任务队列
	 */
	task_queue_t* random_alloc(int key) const;

	/**
	 * 停止任务队列池中的任务队列的运行
	 */
	void stop();

	/**
	 * 任务队列池的启动函数
	 * @param thread_ {thread_t} 线程容器, 创建线程执行任务队列池中的任务, 一个任务队列池对应一个线程
	 * @return {int} 0:成功 其他值:错误码
	 */
	int start(thread_t& thread_);
private:
	int				m_task_queue_num;
	task_queue_t*	m_array_task_queue;
};

}// namespace zz

#endif	// _zz_task_queue_i_