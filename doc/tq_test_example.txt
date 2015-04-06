1) feature
   1. This is  a c++ socket lib.
   2. --add by pluto. 2015-03-16
   
   // cppTest.cpp : 定义控制台应用程序的入口点。
//
/*
#include "stdafx.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

using namespace zz;

bool exit_flag = false;

void print_char(char ch)
{
	cout<<"["<<current_tid()<<"]"<<ch<<endl;
}

void recv_key_input(task_queue_t* tq_,task_queue_pool_t* tqp_)
{
	do	{

		char ch;
		cin>>ch;

		if('q' == ch)
		{
			exit_flag = true;
			break;
		}

		tq_->produce(task_binder::bind(print_char,ch));
	}while(1);

	tqp_->stop();
}

void print_num(int n)
{
	cout<<"["<<current_tid()<<"]"<<n<<endl;
}

void timer_task(int interval_sec, task_queue_t* tq_)
{
	int num  =  0;

	while (false == exit_flag)
	{
		 tq_->produce(task_binder::bind(print_num,num++));
		 Sleep(interval_sec*1000);
	} 
}

void test()
{
	 
     task_queue_pool_t tqp_(4);
	 thread_t thread_;
     
	 tqp_.start(thread_);

	 tid_t tid;
	 thread_.create(task_binder::bind(timer_task,1,tqp_.alloc(0)), tid);
	 thread_.create(task_binder::bind(recv_key_input,tqp_.alloc(1),&tqp_), tid);

	 
	 thread_.join_all();
}

struct	zz_zz
{
public:
	void put_char() const
	{
		cout<<"Hello, sdw"<<endl;
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	
 
 test();
	 
	singleton_t<zz_zz>::instance_ptr()->put_char();
	singleton_t<zz_zz>::instance_ptr()->put_char();
// _CrtDumpMemoryLeaks();
	system("pause");


	return 0;
}
*/

// cppTest.cpp : 定义控制台应用程序的入口点。
//
