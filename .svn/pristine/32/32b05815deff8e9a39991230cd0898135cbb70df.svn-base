#include "RemoteLogThread.h"
#include "LLog.h"

static unsigned int g_push_index = 0;
static unsigned int g_pop_index = 0;

int RemoteLogThread::init(int start, 
                        const char* url, 
                        const char* file_name, 
                        size_t max_cached_log_size, 
                        size_t max_cached_log_num, 
                        size_t max_file_log_size)
{
#ifndef _LINUX
    return 0;
#endif

    _start = start;
    if(_start == 0)
    {
        LLOG_DEBUG("RemoteLogThread not start, config start = %d.", _start);
        return 0; 
    }

    _queue_ptr = boost::shared_ptr<LogQueue>(new LogQueue());
    assert(_queue_ptr != NULL);
    _queue_ptr->init(100000 + 1);


    _remote_log = RLogHttp::getInstance();
    assert(_remote_log != NULL);
    return _remote_log->init(url, file_name, max_cached_log_size, max_cached_log_num, max_file_log_size);
}

void RemoteLogThread::start()
{
#ifndef _LINUX
    return;
#endif
    if(_start == 0)
    {
        return;    
    }

    _thread = new boost::thread(boost::bind(&RemoteLogThread::run, this));
    assert(_thread != NULL); 
}

void RemoteLogThread::stop()
{
#ifndef _LINUX
    return;
#endif
    if(_start == 0)
    {
        return;    
    }

    if(_queue_ptr != NULL)
    {
        _queue_ptr->push(Lstring());     
    }

    /** join函数，作用是等待直到线程执行结束；可不加，但不能保证退出Destroy函数前线程被终结 */
    if(_thread != NULL)
    {
        _thread->join();
    }

    LLOG_DEBUG("remote log thread exit, push_index=%d, pop_index=%d\n", g_push_index, g_pop_index);
}

void RemoteLogThread::write_log(Lstring&& log)
{
#ifndef _LINUX
    return;
#endif

    if(_start == 0)
    {
        return;    
    }

    if(move(log).empty())
    {
        LLOG_DEBUG("[WARN] write log is empty!\n");   
        return;
    }

    ++g_push_index;
    if(_queue_ptr == NULL)
    {
        LLOG_ERROR("[ERROR] function:%s, _queue_ptr is null!", __FUNCTION__);    
        return;
    }
    int ret = _queue_ptr->push(move(log));
    //printf("push log: %d, push ret: %d\n", i, ret);
}

void RemoteLogThread::run()
{
#ifndef _LINUX
	return;
#endif
    while(true)
    {
        if(_start == 0)
        {
            return;    
        }

        if(_queue_ptr == NULL)
        {
            LLOG_ERROR("[ERROR] function:%s, _queue_ptr is null!", __FUNCTION__);    
            return;
        }
        
        if(_remote_log == NULL)
        {
            LLOG_ERROR("[ERROR] function:%s, _remote_log is null!", __FUNCTION__);
            return;    
        }

        if(_queue_ptr->empty())
        {
#ifdef _LINUX
			::usleep(10);
#else
			Sleep(1);
#endif // _LINUX
            continue;
        }
        Lstring log = _queue_ptr->front();
        _queue_ptr->pop();
        ++g_pop_index;
        if(log.empty())
        {
            LLOG_DEBUG("[WARN] log is empty, break run!\n");
            _remote_log->stop_write_log();
            break;
        }

        _remote_log->rlog(log); 
    }
}
