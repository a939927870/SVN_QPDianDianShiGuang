#include "BaseCommonFunc.h"
#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include "LMsg.h"

#if _LINUX
#include <time.h> 
#include "IPFilter.h"
static bool created = false;
static boost::mutex lock;
#else
#include <windows.h>
#endif

void DisableIP(const std::string& ip)
{
#if _LINUX

	if(!created)
	{
		boost::lock_guard<boost::mutex> guard(lock);
		if(!created)
		{
			if(App_IPFilter::instance()->CreateIPFilterSocket() == 0)
			{
				created = true;
				App_IPFilter::instance()->SetIPFilterServer("127.0.0.1", 30000);
			}
		}
	}

	if(created)
	{
		App_IPFilter::instance()->SendFilterIPPacket(ip);
	}

#endif
}

static unsigned short bytecrc(unsigned short sum, unsigned char data)
{
	sum ^= data;

	for(int i =0; i <= 3; i++)
	{
		if((sum & 0x01) == 0)
			sum /= 2;
		else
			sum = (sum / 2) ^ 0x70B1;
	}
	return sum;
}

unsigned short csum (unsigned char *data, int dataSize) 
{
	unsigned short sum = 0xFFFF;
	for(int i = 0; i < dataSize; i++)
		sum = bytecrc(sum, data[i]);

	return sum;
}

std::time_t GetLastModifiedTime(const char* path)
{
	boost::system::error_code ec;
	std::time_t lastModifiedTime = boost::filesystem::last_write_time(path, ec);
	if (ec)
		lastModifiedTime = 0;
	return lastModifiedTime;
}

Lint TransfromMsgId(Lint timestamp, Lint msgId)
{
	Lint transedMsgId = (timestamp % 10000 + 1) * msgId;
	return transedMsgId;
}

Lint InverseTransfromMsgId(Lint timestamp, Lint transedMsgId)
{
	Lint msgId = timestamp > 0 ? (transedMsgId / (timestamp % 10000 + 1)) : 0;
	return msgId;
}

LMsg* ParseInsideNetPacket(LBuffPtr recv, LSocketPtr s)
{
	int msgid = 0;
	recv->Read(msgid);
	LMsg* msg = LMsgFactory::Instance().CreateMsg(msgid);
	if (msg != NULL)
	{
		msg->Read(*recv.get());
		msg->m_sp = s;
	}
	else
	{
		LLOG_ERROR("ParseInsideNetPacket: error msgId=%d", msgid);
	}
	return msg;
}

Llong GetOSTickCount()
{
#if _LINUX
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#else
	return (Llong)::GetTickCount64();
#endif
}


//×Ö·û·Ö¸î
bool tokenize(const Lstring& str, std::vector<Lstring>& tokens, const Lstring& delims)
{
	tokens.clear();
	if (str.length())
	{
		Lstring::size_type lastPos = 0;
		Lstring::size_type pos;
		while ((pos = str.find_first_of(delims, lastPos)) != str.npos) {
			tokens.push_back(str.substr(lastPos, pos - lastPos));
			lastPos = pos + 1;
		}
		tokens.push_back(str.substr(lastPos));
		return true;
	}
	else
	{
		return false;
	}
}

bool safe_atoi(const Lstring &str, Lint &num) {
	std::istringstream iss(str);
	return !(iss >> num).fail();
}

bool safe_atof(const std::string &str, Lfloat &num) {
	std::istringstream iss(str);
	return !(iss >> num).fail();
}

bool tokenize_atovi(const Lstring &str, std::vector<Lint>& tokens, const Lstring& delims)
{
	std::vector<Lstring> temps;
	tokenize(str, temps, delims);
	Lint typeSz = temps.size();
	for (Lint ts = 0; ts < typeSz; ++ts)
	{
		Lint type = 0;
		if (!safe_atoi(temps[ts], type))
		{
			LLOG_ERROR("tokenize_atovi.cover error! %s", temps[ts].c_str());
			return false;
		}
		tokens.push_back(type);
	}
	return true;
}

bool tokenize_vitoa(const std::vector<Lint>& tokens, Lstring &str, const Lstring& delims)
{
	std::stringstream os;
	Lint tkSz = tokens.size();
	if (tkSz <= 0)
	{
		return false;
	}

	for (Lint tk = 0; tk < tkSz; ++tk)
	{
		os << tokens[tk];
		if (tk != tkSz - 1)
		{
			os << ";";
		}
	}

	str = os.str();
	return true;
}