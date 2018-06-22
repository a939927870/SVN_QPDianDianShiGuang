/*************************************************************************
	> File Name: RLogHttp.h
	> Author: 
	> Mail: 
	> Created Time: Mon 24 Oct 2016 02:00:03 PM PDT
 ************************************************************************/

#ifndef _RLOGHTTP_H
#define _RLOGHTTP_H

#ifdef _LINUX
#include <curl/curl.h>
#else
#include <libcurl/curl.h>
#endif

#include <sstream>
#include "LBase.h"

#define MAX_PATH_LEN 256
#define RLOG_VERSION "V1.00.00"

class RLogHttp
{
public:
    static RLogHttp *getInstance();
    int init(const char *url, const char *cache_file_path, size_t max_cached_log_size, size_t max_cached_log_num, size_t max_file_log_size);
    int rlog(const Lstring& log);
    void stop_write_log();

private:
    RLogHttp();
    virtual ~RLogHttp();

    int sendLog();
    int cached_to_file();
    int send_cached_file();

    void clean_buff_data();

    FILE *_cacheFileHandle;

    int _cached_log_num;

    static RLogHttp *_instance;
    CURL *curl;
    struct curl_slist *list;

    size_t _cache_file_length;

    size_t _max_cached_log_size;
    size_t _max_cached_log_num;
    size_t _max_file_log_size;
    char _cache_file_path[MAX_PATH_LEN];
    char* _buff;
};

#endif
