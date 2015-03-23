#ifndef _zzslot_h_
#define _zzslot_h_

#include <map>
#include <string>
using namespace std;

#include "base/zztype_def.h"

namespace zz {

class zzslot_callback_arg_t
{
public:
	/**
     *	回调类型.
	 */
	virtual std::string type() const = 0;
};

class zzslot_callback_t
{
public:
	virtual void exe(zzslot_callback_arg_t& args_) = 0;
};

class zzslot_t
{
public:
	~zzslot_t()
	{
		clear();
	}

	void add(uint32_t id, zzslot_callback_t* callback)
	{
		m_id_to_callback[id] = callback;
	}

	zzslot_callback_t* get(uint32_t id)
	{
		map<uint32_t, zzslot_callback_t*>::iterator it = m_id_to_callback.find(id);

		if (it != m_id_to_callback.end())
		{
			return it->second;
		}

		return NULL;
	}

	void remove(uint32_t id, zzslot_callback_t* callback)
	{
		map<uint32_t, zzslot_callback_t*>::iterator it = m_id_to_callback.find(id);

		if (it != m_id_to_callback.end())
		{
			m_id_to_callback.erase(it);
		}
	}

	void clear()
	{
		map<uint32_t, zzslot_callback_t*>::iterator it = m_id_to_callback.begin();

		while (it != m_id_to_callback.end())
		{
			delete it->second;
			it++;
		}
		m_id_to_callback.clear();
	}

private:
	map<uint32_t, zzslot_callback_t*>	m_id_to_callback;

};

}
#endif	// _zzslot_h_