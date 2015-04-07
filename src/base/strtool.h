#ifndef _strtool_h_
#define _strtool_h_

#include "base/zz_compile_opt.h"

#include <vector>
using namespace std;

namespace zz {

struct LIBRARY_API strtool_t
{
	static	void split(const string& src, const char* separate, vector<string>& vec_dest)
	{
		vec_dest.clear();

		size_t pos = 0;

		do 
		{
			if (pos >= src.length() )
				break;

			size_t new_pos = src.find(separate, pos);
			
			size_t end_pos;

			if(-1 == new_pos)
			{
				end_pos = src.length();
			}
			else
			{
				end_pos = new_pos;
			}

			string sub_str = src.substr(pos,end_pos-pos);

			vec_dest.push_back(sub_str);

			if (new_pos == -1)
			{
				break;
			}
			else
			{
				pos = new_pos + strlen(separate);
			}
			
		} while (1);
		

	}

};

}

#endif