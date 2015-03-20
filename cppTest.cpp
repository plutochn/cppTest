
#include "stdafx.h"
#include "net_factory.h"
#include "strtool.h"
#include "arg_helper.h"
#include "codec.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

using namespace zz;

void pause()
{
	char ch;
	cin>>ch;
}

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

class msg_handler_impl : public msg_handler_i
{
public:
	virtual int handle_msg(msg_t& msg_, socket_ptr_t sock_)
	{

	    cout<<"client handle_msg:"<<endl;

		cout<<"cmd_id "<<msg_.cmd_id()<<endl;
		cout<<"msg body "<<msg_.body<<endl;

		cout<<endl;

		return 0;
	}

	virtual int handle_broken(socket_ptr_t sock_)
	{
		sock_connection_info& addr = sock_->connection_info();
		cout<<"client broken:"<<endl;
		cout<<"local:"<<addr.local_addr<<addr.local_port<<endl;
		cout<<"peer:"<<addr.remote_addr<<addr.remote_port<<endl;
		cout<<endl;

		return 0;
	}

	virtual int handle_open(socket_ptr_t sock_)
	{
		sock_connection_info& addr = sock_->connection_info();
		cout<<"client connect:"<<endl;
		cout<<"local:"<<addr.local_addr<<addr.local_port<<endl;
		cout<<"peer:"<<addr.remote_addr<<addr.remote_port<<endl;

		cout<<endl;
		return 0;
	}
};

void test2()
{

	msg_handler_impl handler;

	std::string listen_host = "tcp://127.0.0.1:12345";
	net_factory_t::listen(listen_host, &handler);

	
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

void test4(int argc, char** argv)
{
	string host = "tcp://127.0.0.1:8001";
	vector<string> vec_strs;
	strtool_t::split(host, ":", vec_strs);

	arg_helper_t arg_helper(argc, argv);

	if (arg_helper.is_enable_option("-listen"))
	{
		string str = arg_helper.get_option_value("-listen");
		cout<<str.c_str()<<endl;
	}

	if (arg_helper.is_enable_option("-broker"))
	{
		string str = arg_helper.get_option_value("-listen");
		cout<<str.c_str()<<endl;
	}
	else
	{
		cout<<"-broker arg not exist"<<endl;
	}
}

void test5()
{
	bin_encoder_t encoder_;

	char v1 = '1';
	unsigned char v2 = 0xaa;

	unsigned short v3 = 65523;

	short zz = 12324;

	int m  = 12 ;

	unsigned int um = 1245778;

	string str = "123456789";
	double c = 123e+12;

	encoder_<<m<<zz<<str; 
 
	bin_decoder_t decoder_;

	decoder_.init_decoder(encoder_.get_encoded_string());

	string str_r;
	int n;
	short cc;
	try
	{
		decoder_>>n>>cc>>str_r;
	}
	catch (exception& e_)
	{
		cout<<e_.what()<<endl;
	}
	pause();
}

void test6()
{
	bin_encoder_t encoder_;
	bin_decoder_t decoder_;

	vector<int> vec_src;
	vector<int> vec_dest;

	list<int> list_src;
	list<int> list_dest;

	for (int i = 0; i<10; i++)
	{
		vec_src.push_back(i);
		list_src.push_back(i+10);
	}

	encoder_<<vec_src<<list_src;

	decoder_.init_decoder(encoder_.get_encoded_string());

	decoder_>>vec_dest>>list_dest;

	pause();
}

int _tmain(int argc, _TCHAR* argv[])
{
	test6();

	return 0;
}

