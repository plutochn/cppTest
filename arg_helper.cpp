#include "arg_helper.h"

namespace zz {

arg_helper_t::arg_helper_t(int argc, char* argv[])
{
	if (argc>1)
	{
		for (int i =1; i<argc; i+=2)
		{
			m_arg_tbl.insert( make_pair( argv[i],argv[i+1] ) );
		}
	}
}

/**
 *	not implement yet.
 */
arg_helper_t::arg_helper_t(std::string& args)
{
	 
}

bool arg_helper_t::is_enable_option(const char* opt) const 
{
	arg_tbl_iterator it = m_arg_tbl.find(opt);
	if (it != m_arg_tbl.end() )
	{
		return true;
	}
	return false;
}

std::string arg_helper_t::get_option_value(const char* opt) const 
{
	arg_tbl_iterator it = m_arg_tbl.find(opt);
	if (it != m_arg_tbl.end() )
	{
		return it->second;
	}
	return "";
}

}