#include "HttpRequest.h"
#include "LLog.h"
#include <openssl/md5.h>
#include "LTool.h"

bool HttpRequest::Init()
{
	return true;
}

bool HttpRequest::Final()
{
	return true;
}

size_t httpWriteCallBack(char *ptr, size_t size, size_t nmemb, void *data)
{
	size_t res_size = size * nmemb;
	if(res_size > 1024*64)
		return 0;
	char *res_buf = (char*)(data);
	memcpy(res_buf, ptr, res_size);
	return res_size;
}

bool HttpRequest::HttpGet(const std::string& httpurl, std::string& result,bool isHttps)
{
	LLOG_DEBUG("http get url :%s", httpurl.c_str());

	CURL *curl = curl_easy_init();
	if(curl ==NULL)
	{
		LLOG_DEBUG("http get curl init fail..");
		return false;
	}
	curl_easy_setopt(curl, CURLOPT_URL, httpurl.c_str());
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2);			 //设置超时
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);        //屏蔽其它信号
	if(isHttps)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); 
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); 
	}
	char recvBuff[1024*64];
	memset(recvBuff, 0,sizeof(recvBuff));
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, httpWriteCallBack);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, recvBuff);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	if(res != CURLE_OK)
	{
		LLOG_DEBUG("http request fail:%s,%d", httpurl.c_str(), res);
		return false;
	}
	else
	{
		recvBuff[sizeof(recvBuff)-1]=0;
		result = recvBuff;
		return true;
	}
}

bool HttpRequest::HttpPost(const std::string& httpurl,const std::string& postData, std::string& result,bool isHttps)
{
	LLOG_DEBUG("http post:%s", httpurl.c_str());

	CURL *curl = curl_easy_init();
	if(curl ==NULL)
	{
		LLOG_DEBUG("http post curl init fail..");
		return false;
	}

	curl_easy_setopt(curl, CURLOPT_URL, httpurl.c_str());
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);        //设置超时
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);        //屏蔽其它信号
	curl_easy_setopt(curl, CURLOPT_POST, 1); //设置post  
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());   

	if(isHttps)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); 
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); 
	}

	char recvBuff[1024*64];
	memset(recvBuff, 0,sizeof(recvBuff));
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &httpWriteCallBack);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, recvBuff);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	if(res != CURLE_OK)
	{
		LLOG_DEBUG("http post fail:%s,%d", httpurl.c_str(), res);
		return false;
	}
	else
	{
		recvBuff[sizeof(recvBuff)-1]=0;
		result = recvBuff;
		return true;
	}
}

Lstring HttpRequest::Md5Hex(const void* src ,Lsize len)
{
	unsigned char sign[17];
	memset(sign,0,sizeof(sign));

	MD5((const unsigned char*)src,len,sign);

	//转换成16进制
	char signHex[33];
	memset(signHex,0,sizeof(signHex));
	for(int i = 0 ; i < 16; ++i)
	{
		sprintf(signHex + i*2,"%02x", sign[i]);
	}

	return std::string(signHex);
}

unsigned char fromHex(const unsigned char &c)
{
	unsigned char x = c;
	if (islower(x))
		x = toupper(x);
	return isdigit(x) ? x - '0' : x - 'A' + 10;
}

Lstring HttpRequest::UrlDecode(const char *src)
{
	std::string strdes;
	size_t len = strlen(src);
	for (size_t ix = 0; ix < len; ++ix)
	{
		unsigned char ch = 0;
		if (src[ix] == '%')
		{
			ch = (fromHex(src[ix + 1]) << 4);
			ch |= fromHex(src[ix + 2]);
			ix += 2;
		}
		else if (src[ix] == '+')
		{
			ch = ' ';
		}
		else
		{
			ch = src[ix];
		}
		strdes += (char)ch;
	}
	return strdes;
}

void HttpRequest::DeodeUrlParams(const std::string& str, std::map<std::string, std::string>& params, bool urldecode)
{
	std::vector<std::string> strsArray;
	L_ParseString(str,strsArray, "&");
	for (size_t i = 0; i < strsArray.size(); ++i)
	{
		std::vector<std::string> strP;
		L_ParseString(strsArray[i],strP, "=");
		if (strP.size() == 2)
		{
			std::string key = strP[0];
			std::string value = strP[1];
			if (!urldecode)
				params[key] = value;
			else
				params[key] = UrlDecode(value.c_str());
		}
		else if (strsArray[i].at(strsArray[i].size() - 1) == '=')
		{
			std::string key = strsArray[i].substr(0, strsArray[i].size() - 1);
			params[key].clear();
		}
		else
		{
			
		}
	}
}