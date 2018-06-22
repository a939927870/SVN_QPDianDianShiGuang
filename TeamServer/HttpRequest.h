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
	bool			HttpGet(std::string url, std::string& result, bool isHttps = false);
	//post请求,支持https
	bool			HttpPost(std::string url, std::string postData,std::string& result,bool isHttps=false);
	//md5加密算法
	Lstring			Md5Hex(const void* data,Lsize size);

	//解码url
	void            DeodeUrlParams(const std::string& str, std::map<std::string, std::string>& params, bool urldecode);

	Lstring			UrlDecode(const char *src);
};

#endif