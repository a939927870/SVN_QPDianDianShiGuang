#ifndef _L_BASE_H_
#define _L_BASE_H_

#include <cctype>
#include <cstdio>
#include <ctime>
#include <cstdarg>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <queue>
#include <sstream>
#include <algorithm>

#if !defined(_LINUX) && !defined(_WIN32_WINNT)
#define  _WIN32_WINNT  0x0603
#endif

#include <boost/thread.hpp>
#include <boost/asio.hpp>
//#include <boost/make_shared.hpp>
//#include <boost/enable_shared_from_this.hpp>
//#include <boost/lexical_cast.hpp>

#include <mysql.h>
//#include <mysql/mysql.h>

//#include <msgpack/msgpack.hpp>
#include <msgpack.hpp>


#if _LINUX
#define sprintf_s snprintf
#define gmtime_s gmtime_r
#include <json/json.h>
#else
#include <jsoncpp/json.h>
#endif


//#include <Windows.h>


//基本的整数类型如下
//char(8) short(16) int(32) float(32) double(64)
//
//
typedef unsigned long       DWORD;
typedef void *PVOID;

typedef char Lchar;
typedef unsigned char Luchar;

typedef short Lshort;
typedef unsigned short Lushort;

typedef int	Lint;
typedef unsigned int Luint;
typedef unsigned long long  U64;

typedef long long Llong;

typedef float Lfloat;

typedef double Ldouble;

typedef std::size_t Lsize;

typedef std::string Lstring;

#if _LINUX
typedef bool BOOL;
typedef unsigned char BYTE;
#define TRUE                1
#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#endif


typedef char *NPSTR, *LPSTR, *PSTR;

//var 转换成字符串 var";
#define NAME_TO_STR(var)  (#var)

#endif