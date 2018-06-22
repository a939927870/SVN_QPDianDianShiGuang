/*************************************************************************
	> File Name: RLogHttp.cpp
	> Author: 
	> Mail: 
	> Created Time: Mon 24 Oct 2016 02:00:28 PM PDT
 ************************************************************************/

#include<iostream>
#include "RLogHttp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _LINUX
#include <unistd.h>
#else
#include <io.h>
#include <errno.h>
#endif
#include <sys/stat.h>

#include "LLog.h"

using namespace std;

#define MAX_LOG_SIZE   1024
#define DEFAULT_MAX_CACHED_LOG_SIZE   1024

RLogHttp *RLogHttp::_instance = NULL;

RLogHttp::RLogHttp()
{ 
    _cacheFileHandle = NULL;
    _cached_log_num = 0;
    curl = NULL;
    list = NULL;
    
    _cache_file_length = 0;
    _max_cached_log_size = 0;
    _max_cached_log_num = 0;
    _max_file_log_size = 0;
    memset(_cache_file_path, 0, MAX_PATH_LEN);

    _buff = NULL;

    list = curl_slist_append(list, "Content-Type: text/plain; charset=UTF-8");
    list = curl_slist_append(list, "Expect:");
}

RLogHttp::~RLogHttp()
{
    if(list != NULL)
    {
        curl_slist_free_all(list);
    }

    if(curl != NULL)
    {
		curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    if(_buff != NULL)
    {
        free(_buff);
        _buff = NULL;
    }

    if(_cacheFileHandle != NULL)
    {
        fclose(_cacheFileHandle);
    }
}

RLogHttp *RLogHttp::getInstance()
{
    if (!_instance) {
        _instance = new RLogHttp();
    }

    return _instance;
}

int RLogHttp::init(const char *url, const char *cache_file_path, size_t max_cached_log_size, size_t max_cached_log_num, size_t max_file_log_size)
{
    if (curl) {
        LLOG_DEBUG("[WARN] You have inited RLogHttp.\n");
        return 0;
    }

    if(url == NULL || cache_file_path == NULL || std::string(url).empty() || std::string(cache_file_path).empty())
    {
        LLOG_ERROR("[ERROR] url or cache_file_path is empty, please check config file.\n");
	return -1;
    }

    if(strlen(cache_file_path) > MAX_PATH_LEN - 1)
    {
        LLOG_ERROR("[ERROR] invalid path: %s.\n", cache_file_path);
        return -1;
    }
    
    strcpy(_cache_file_path, cache_file_path); 

    if(max_cached_log_size < DEFAULT_MAX_CACHED_LOG_SIZE)
    {
       LLOG_DEBUG("[WARN] max_cached_log_size : %d < %d, set to %d.\n", max_cached_log_size, DEFAULT_MAX_CACHED_LOG_SIZE, DEFAULT_MAX_CACHED_LOG_SIZE); 
       _max_cached_log_size = DEFAULT_MAX_CACHED_LOG_SIZE;
    }
    else
    {
        _max_cached_log_size = max_cached_log_size;    
    }

    if(max_cached_log_num < 10)
    {
        LLOG_DEBUG("[WARN] max_cached_log_num : %d < 10, set to 10.\n", max_cached_log_num); 
        _max_cached_log_num = 10;
    }
    else
    {
        _max_cached_log_num = max_cached_log_num;
    }

    if(max_file_log_size < 5 * 1024 * 1024)
    {
        LLOG_DEBUG("[WARN] max_file_log_size : %d < 5 * 1024 * 1024, set to 5 * 1024 * 1024.\n", max_file_log_size); 
        _max_file_log_size = 5 * 1024 * 1024; 
    }
    else
    {
        _max_file_log_size = max_file_log_size;    
    }

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (!curl) {
        LLOG_ERROR("[ERROR] Libcurl init failed.\n");
        return -1;
    }
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10); // 超时时间10秒
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

    _cacheFileHandle = fopen(_cache_file_path, "a+");
	if(_cacheFileHandle == NULL)
	{
		LLOG_ERROR("[ERROR] function: %s, _cacheFileHandle is null!", __FUNCTION__);
		return -1;
	}

    _buff = (char *)calloc(_max_cached_log_size, 1);
    memset(_buff, 0, _max_cached_log_size);

    struct stat st;
    if(stat(_cache_file_path, &st) < 0)
    {
        LLOG_ERROR("[ERROR] cannot stat file, file name: %s.\n", _cache_file_path);
        return -1;    
    }

    if(st.st_size > 0)
    {
        LLOG_DEBUG("cache file size > 0\n");
        _cache_file_length = st.st_size;
        send_cached_file();   
    }
    else
    {    
        LLOG_DEBUG("cache file size <= 0\n");
    }

    return 0;
}

int RLogHttp::rlog(const Lstring& log)
{
    int ret = 0;
    size_t log_size = log.size();
    if(log_size > MAX_LOG_SIZE - 2)
    {
       LLOG_ERROR("[ERROR] this log is too large, log size: %d, limit max log size: %d\n", log_size, MAX_LOG_SIZE - 1); 
       return -1;
    }
    
	if(_buff == NULL)
	{
		LLOG_ERROR("[ERROR] function: %s, _buff is null!", __FUNCTION__);
		return -1;
	}

    size_t bf_len = strlen(_buff);
    LLOG_DEBUG("[DBUG] log size: %d, limit max log size: %d, buff_size: %d, max_cached_log_size: %d\n", log_size, MAX_LOG_SIZE - 2, bf_len, _max_cached_log_size); 

    if ((strlen(_buff) + log_size) > _max_cached_log_size - 2 || _cached_log_num > _max_cached_log_num) {
        ret = sendLog();
        if (ret < 0) {
            cached_to_file();    
            memset(_buff, 0, _max_cached_log_size);
        } else {
            memset(_buff, 0, _max_cached_log_size);
            // TODO: send cached file.
            if(_cache_file_length > 0)
            {
                send_cached_file();
            }
        }
        _cached_log_num = 0;
    }

    _cached_log_num++;
    memcpy(_buff+strlen(_buff), log.c_str(), log.size());
    _buff[strlen(_buff)] = '\n';
    return 0;
}

void RLogHttp::stop_write_log()
{
	if(_buff == NULL)
	{
		LLOG_ERROR("[ERROR] function: %s, _buff is null!", __FUNCTION__);
		return;
	}

    if(sendLog() < 0)
    {
        cached_to_file();
    }
    memset(_buff, 0, _max_cached_log_size);
    return;   
}

int RLogHttp::sendLog()
{
	if(_buff == NULL)
	{
		LLOG_ERROR("[ERROR] function: %s, _buff is null!", __FUNCTION__);
		return -1;
	}

	if(curl == NULL)
	{
		LLOG_ERROR("[ERROR] function: %s, curl is null!", __FUNCTION__);
		return -1;
	}

    if(strlen(_buff) == 0)
    {
        return 0;
    }

    CURLcode res;

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, _buff);
    res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        return -1;
    }
    return 0;
}

int RLogHttp::cached_to_file()
{
	if(_buff == NULL)
	{
		LLOG_ERROR("[ERROR] function: %s, _buff is null!", __FUNCTION__);
		return -1;
	}

	if(_cacheFileHandle == NULL)
	{
		LLOG_ERROR("[ERROR] function: %s, _cacheFileHandle is null!", __FUNCTION__);
		return -1;
	}

    if(strlen(_buff) == 0)
    {
        return 0; 
    }

    if(_cache_file_length > _max_file_log_size)
    {
        LLOG_DEBUG("[ERROR] cache file length is too large, now size: %d, max size: %d.\n", _cache_file_length, _max_file_log_size);
        return -1; 
    }

    size_t buff_length = strlen(_buff);
    _cache_file_length += buff_length;
    fwrite(_buff, buff_length, 1, _cacheFileHandle);
    fflush(_cacheFileHandle);
    return 0;
}

int RLogHttp::send_cached_file()
{
	if(_buff == NULL)
	{
		LLOG_ERROR("[ERROR] function: %s, _buff is null!", __FUNCTION__);
		return -1;
	}

	if(_cacheFileHandle == NULL)
	{
		LLOG_ERROR("[ERROR] function: %s, _cacheFileHandle is null!", __FUNCTION__);
		return -1;
	}

    char line[MAX_LOG_SIZE];
    memset(line, 0, MAX_LOG_SIZE);

    while(fgets(line, MAX_LOG_SIZE, _cacheFileHandle) != NULL)
    {
        //printf("get line: %s.\n", line);
        if(_buff == NULL)
        {
            LLOG_DEBUG("[WARN] get log buff failed!\n");
            continue;    
        }

        size_t buff_length = strlen(_buff);
        size_t line_length = strlen(line);
        if(buff_length + line_length > _max_cached_log_size - 1) 
        {
            int ret = sendLog(); 
            if(ret < 0)
            {
                printf("[ERROR] send file data failed!\n"); 
                memset(_buff, 0, _max_cached_log_size);
                return -1;    
            }
            memset(_buff, 0, _max_cached_log_size);
        }
        
        memcpy(_buff + strlen(_buff), line, line_length);
    }

    int ret = sendLog();
    if(ret < 0)
    {   
        printf("[ERROR] send file last data failed!\n");
        memset(_buff, 0, _max_cached_log_size);
        return -1;
    } 
    memset(_buff, 0, _max_cached_log_size);

    _cache_file_length = 0;
#ifdef _LINUX
    truncate(_cache_file_path, 0);
#else
	_chsize_s(_fileno(_cacheFileHandle),0);
#endif  
    fseek(_cacheFileHandle, 0, SEEK_SET);

	return 0;
}

