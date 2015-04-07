#ifndef _zz_compile_opt_h_
#define _zz_compile_opt_h_

#pragma warning( disable: 4251 )

#ifdef ZZ_DLL
#define LIBRARY_API __declspec(dllexport)
#else
#define LIBRARY_API __declspec(dllimport)
#endif

#endif // _zz_compile_opt_h_