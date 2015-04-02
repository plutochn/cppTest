#ifndef _arg_helper_h_
#define _arg_helper_h_

#include <string>
#include <map>
using namespace std;

namespace zz {

class arg_helper_t
{
public:
	typedef std::map<std::string /* 选项 */,std::string /* 值 */> arg_tbl_t;
	typedef std::map<std::string /* 选项 */,std::string /* 值 */>::const_iterator arg_tbl_iterator;

public:
	arg_helper_t(int argc, char* argv[]);

	/**
	 *	not implement yet.
     */
	arg_helper_t(std::string& args);

	bool is_enable_option(const char* opt) const ;

	std::string get_option_value(const char* opt) const ;

private:
	arg_tbl_t	m_arg_tbl;
};

}

#endif