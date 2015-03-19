#ifndef _zz_codec_h_
#define _zz_codec_h_

#include <string>
#include <stdexcept>
using namespace std;

namespace zz {

class codec_i
{
public:
	virtual string encode() = 0;
	virtual void   decode(string& src) = 0;
};

class bin_encoder_t
{
public:
	string get_encoded_string()
	{
		return m_dest_buf;
	}

	void copy_value(const char* buf, size_t size)
	{
		m_dest_buf.append(buf, size);
	}

	template<typename T>
	bin_encoder_t& operator << (T & t)
	{
		bin_tool_t<T>::encode(*this,t);
		return *this;
	}

private:
	string m_dest_buf;
};

class bin_decoder_t;

template<typename T>
class bin_tool_t
{
public:
	static void encode(bin_encoder_t& encoder, T & t)
	{
		encoder.copy_value((const char*)&t, sizeof(T));
	}

	static void decode(bin_decoder_t& decoder, T & t)
	{
		decoder.copy_value((char*)&t, sizeof(T));
	}
};

template<>
class bin_tool_t<string>
{
public:
	static void encode(bin_encoder_t& encoder, string & t)
	{
		size_t len = t.length();
		encoder.copy_value((const char*)&len, sizeof(len));

		encoder.copy_value(t.c_str(),len);
	}
};

class bin_decoder_exception_t : public std::exception
{
public:
	bin_decoder_exception_t()
	{
		m_error_info = "bin_decoder_t::copy_value access out of max len";
	}

	virtual const char* what() const
	{
		return m_error_info.c_str();
	}

private:
	std::string m_error_info;
};

class bin_decoder_t
{
public:
	void init_decoder(string& src)
	{
		m_src_buf = src;
		m_cur_pos = 0;
		m_remained_len = m_src_buf.length();
	}

	void copy_value(char* buf, size_t size)
	{
		 if (size>m_remained_len)
		 {
			 throw bin_decoder_exception_t();
			 return ;
		 }

		 memcpy(buf,m_src_buf.c_str()+m_cur_pos,size);

		 m_cur_pos += size;
		 m_remained_len -= size;
	}

	template<typename T>
	bin_decoder_t& operator >> (T & t)
	{
		bin_tool_t<T>::decode(*this,t);
		return *this;
	}

private:
	size_t m_cur_pos;
	size_t m_remained_len;

	string m_src_buf;
};

}

#endif // _zz_codec_h_