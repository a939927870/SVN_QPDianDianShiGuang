#ifndef _L_TIME_H_
#define _L_TIME_H_

#include "LBase.h"

enum EDelLogVideoTime
{
	THREE0CLOCK = 3,
	FOUR0CLOCK = 4,
	FIVE0CLOCK = 5,
	SIX0CLOCK = 6,
	SEVEN0CLOCK = 7,
};

/*
*	时间类
*/
class LTime
{
public:
	/*
	*	brief 默认以当前时间构造
	*/
	LTime();

	/*
	*	brief 拷贝构造函数
	*/
	LTime(LTime const& rt);

	/*
	*	brief 设定时间为当前时间
	*/
	void Now();

	/*
	*	brief 延迟时间
	*	param time 延迟的时间秒数 
	*/
	void Delay(Lint time);

	/*
	*	brief 设定时间，
	*	param msecs 毫秒为单位的时间戳
	*/
	void SetMSecs(Llong msecs);

	/*
	*	brief 获取时间戳，毫秒为单位
	*/
	Llong MSecs();

	/*
	*	brief 设定时间，
	*	param secs 秒为单位的时间戳
	*/
	void SetSecs(Lint secs);

	/*
	*	brief 获取年月日格式为  20160220
	*/
	Lint GetDate();

	/*
	*	brief 获取时间格式为  020528(即02:05:28);
	*/
	Lint GetTime();

	/*
	*	brief 获取时间戳，秒为单位
	*/
	Lint Secs();

	/*
	*	brief 复制函数
	*/
	LTime& operator = (LTime const& rt);

	/*
	*	比较大小
	*/
	bool operator > (LTime const& rt);

	/*
	*	比较大小
	*/
	bool operator >= (LTime const& rt);

	/*
	*	比较大小
	*/
	bool operator < (LTime const& rt);

	/*
	*	比较大小
	*/
	bool operator <= (LTime const& rt);

	/*
    *	brief 这一年中的第几天
	*	return [0-365]
    */
	Lint GetYday();

	/*
    *	brief 获取几号
	*	return [1-31] 1号-31号
    */
	Lint GetMday();

	/*
    *	brief 获取星期
	*	return [0-6] 周日-周六
    */
	Lint GetWday();

	/*
    *	brief 获取月份
	*	return [0-11] 1月-12月
    */
	Lint GetMonth();

	/*
    *	brief 获取年份
	*	return 2014
    */
	Lint GetYear();

	/*
    *	brief 获取小时
	*	return [0-59]
    */
	Lint GetHour();

	/*
    *	brief 获取分钟
	*	return [0-59]
    */
	Lint GetMin();

	/*
    *	brief 获取秒数
	*	return [0-59]
    */
	Lint GetSec();

	/*
	*	biref 获取毫秒数
	*	return [0-999]
	*/
	Lint GetMSec();

private:
	time_t  _time;	//时间戳
	tm		_tm;   //时间结构体
};

/*
*	每日时间 比如下午2点半零5秒 14:30:05
*/
class LDateTime
{
public:
	/*
	*	biref 默认由当前时间构造
	*/
	LDateTime();

	/*
	*	biref 由字符串产生时间
	*	param timestr 格式12:05:35
	*/
	LDateTime(Lstring timestr);

	/*
	*	biref 有小时，分钟，秒构造产生时间
	*	param hour 小时[0-23]
	*	param min 分钟[0-59]
	*	param sec 秒数[0-59]
	*/
	LDateTime(Lint hour,Lint min,Lint sec);

	/*
	*	biref 获取小时
	*/
	Lint GetHour();

	/*
	*	biref 获取分钟
	*/
	Lint GetMin();

	/*
	*	biref 获取秒数
	*/
	Lint GetSec();


private:
	Lint _data;//0-hour,1-min,2-sec,3-null
};


/*
*	每周时间 比如周二下午2点半零5秒 字符串表示 2-14:30:05
*/
class LWeekTime
{
public:
	/*
	*	biref 默认获取当前的星期，时间
	*/
	LWeekTime();

	/*
	*	biref 由字符串产生时间
	*	param timestr 格式2-12:05:35
	*/
	LWeekTime(Lstring timestr);

	/*
	*	biref 由星期，小时，分钟，秒构造产生时间
	*	param wday 星期[0-6]
	*	param hour 小时[0-23]
	*	param min 分钟[0-59]
	*	param sec 秒数[0-59]
	*/
	LWeekTime(Lint wday,Lint hour,Lint min,Lint sec);

	/*
	*	biref 获取星期
	*/
	Lint GetWDay();

	/*
	*	biref 获取小时
	*/
	Lint GetHour();

	/*
	*	biref 获取分钟
	*/
	Lint GetMin();

	/*
	*	biref 获取秒数
	*/
	Lint GetSec();

private:
	Lint _data;//0-hour,1-min,2-sec,3-wd
};


/*
*	brief 时间操作工具类
*/
class LTimeTool
{
public:
	/*
    *	brief 获取UTC时间戳，微秒为单位
    */
    static Llong GetUTCStampInMicSecond();

    /*
    *	brief 获取UTC时间戳，毫秒为单位
    */
    static Llong GetUTCStampInMilSecond();
    
    /*
    *	brief 获取UTC时间戳，秒为单位
    */
    static Lint GetUTCStampInSecond();

	/*
    *	brief 获取本地时间字符串，格式2013-09-13 12:02:30 895 精确到毫秒
	*		  默认获取当前的时间
    */
    static Lstring GetLocalTimeInString(LTime time = LTime());
    
    /*
    *	brief 获取本地日期字符串。格式2014-12-03
	*		  默认获取当前的时间
    */
    static Lstring GetLocalDateInString(LTime time = LTime());


	/*
	*	biref 获取2个时间之间是否跨日
	*	param time1 时间1
	*	param time2 时间2
	*	param timepoint跨日时间点，比如下午五点半17:30:00 默认是00:00:00
	*	return 
	*/
	static bool UpdateDay(LTime& time1,LTime& time2,LDateTime timepoint = LDateTime(0,0,0));

	/*
	*	biref 获取2个时间之间间隔的天数
	*	param time1 时间1
	*	param time2 时间2
	*	param timepoint跨日时间点，比如零点 00:00:00 下午五点半17:30:00
	*	return 2个时间之间间隔的天数
	*/
	static Lint IntervalDay(LTime& time1,LTime& time2,LDateTime timepoint=LDateTime(0,0,0));

	/*
	*	biref 获取2个时间之间是否跨周
	*	param time1 时间1
	*	param time2 时间2
	*	param daypoint跨周时间点
	*	return 
	*/
	static bool UpdateWeek(LTime& time1,LTime& time2,LWeekTime daypoint=LWeekTime(0,0,0,0));

	/*
	*	biref 获取指定时间是否在2个时间点之间
	*	param time 指定的时间
	*	param timepoint1 时间点1
	*	param timepoint2 时间点2
	*	return 
	*/
	static bool TimeInDataTime(LTime& time,LDateTime timepoint1,LDateTime timepoint2);

	/*
	*	biref 获取指定时间是否在2个周时间点之间
	*	param time 指定的时间
	*	param timepoint1 周时间点1
	*	param timepoint2 周时间点2
	*	return 
	*/
	static bool TimeInWeekTime(LTime& time,LWeekTime timepoint1,LWeekTime timepoint2);

	static bool IsInSameDay(Lint nOneTimestamp, Lint nTwoTimestamp);

	static bool StringToLTime(const std::string& strtime, LTime& ltime);

	static Lstring GetLocalTimeInStringToS(LTime time /*= LTime()*/);
};
#endif