#ifndef _strtool_h_
#define _strtool_h_

#include <vector>
using namespace std;

namespace zz {

struct strtool_t
{
	static	void split(const string& src, const char* delimiter, vector<string>& vec_dest)
	{
		vec_dest.clear();

		size_t pos = 0;

		do 
		{
			if (pos >= src.length() )
				break;

			size_t new_pos = src.find(delimiter, pos);
			
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
				pos = new_pos + strlen(delimiter);
			}
			
		} while (1);
		

	}

};

}

#endif