#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include "LBase.h"
#include "LSingleton.h"
#include <libcurl/curl.h>

class HttpRequest:public LSingleton<HttpRequest>
{
public:
	virtual bool	Init();
	virtual bool	Final();

	//get请求,支持https
	bool			HttpGet(const std::string& url, std::string& result,bool isHttps=false);
	//post请求,支持https
	bool			HttpPost(const std::string& url, const std::string& postData,std::string& result,bool isHttps=false);
	//md5加密算法
	Lstring			Md5Hex(const void* data,Lsize size);

	//解码url
	void			DeodeUrlParams(const Lstring& str, std::map<Lstring, Lstring>& params, bool urldecode);

	Lstring			UrlDecode(const char *src);
};

#endif