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

#include "stdafx.h"
#include "net_factory.h"
#include "strtool.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

using namespace zz;

void SayHello()
{
	cout<<"This is a test from task binder !"<<endl;
}

int SayHello_ret()
{
	cout<<"This is a test from task binder with ret value !"<<endl;
	return 1;
}

int PrintStr_ret(string & str)
{
	cout<<"[ret]str is"<<str<<endl;
	return 1;
}

void PrintStr(string & str)
{
	cout<<"str is"<<str<<endl;

}

class phone_t
{
public:

	void call(string&  phonenum)
	{
		cout<<"call:"<<phonenum<<endl;

		phone_t * pThis = this;

		task_binder::bind(&phone_t::send_sm,this);
	}

	virtual void send_sm()
	{
		cout<<"phone send message to:"<<"10086"<<endl;

		string str = "dfce";
		
	}

	int m;
};

class android_phone_t : public phone_t
{
public:

	virtual void send_sm()
	{
		cout<<"android_phone send message to:"<<"10086"<<endl;

		string str = "dfce";

	}
};

void test()
{

	task_t t_ = task_binder::bind(SayHello);

	t_.exe();

	string str = "zk";
	task_t t1_ = task_binder::bind(PrintStr,str);

	t1_.exe();

	task_t t2_(t1_);

	t2_.exe();

	t2_ = t_;

	t2_.exe();

	phone_t ph;

	string strnum = "13437265183";
	task_t t3_ = task_binder::bind(&phone_t::send_sm,&ph);

	t3_.exe();
	char ch;
	cin>>ch;
}

void test2()
{

	std::string listen_host = "tcp://127.0.0.1:12345";
	net_factory_t::listen(listen_host);

	
	char ch;
	cin>>ch;
	net_factory_t::global_data.stop();
}

void test3()
{
	void (phone_t::*func_ptr)() = &phone_t::send_sm;

	int (phone_t::*varible_ptr) = &phone_t::m;

	phone_t* phone = new android_phone_t;
	(phone->*func_ptr)();
 
}

void test4()
{
	string host = "tcp://127.0.0.1:8001";
	vector<string> vec_strs;
	strtool_t::split(host, ":", vec_strs);
}

int _tmain(int argc, _TCHAR* argv[])
{


	test2();
	 
	return 0;
}

