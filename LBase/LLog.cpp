#include "LLog.h"
#include "LTime.h"

#define LLOG_SIZE 1024

struct LLogData
{
	LLogData()
		:data(NULL)
	{
	}

	~LLogData()
	{
		if(data)
		{
			delete[] data;
		}
	}
	char* data;
};

LLog::LLog()
	:LRunnable(),LSingleton<LLog>(),m_level(LLOG_LEVEL_DEBUG)
{

}

LLog::~LLog()
{

}

bool LLog::Init()
{
	return true;
}

bool LLog::Final()
{
	Clear();
	return true;
}

void LLog::Clear()
{

}

void LLog::Run()
{
	while(!m_stop)
	{
		LLogData* pLog = (LLogData*)Pop();

		if(pLog == NULL)
		{
			Sleep(1);
			continue;
		}

		std::cout << pLog->data << std::endl;
		WriteFile(pLog->data);
		delete pLog;
	}
}

void LLog::SetFileName(const char* fileName)
{
	m_fileName = fileName;
	m_fileName += ".log";
	Start();
}

void LLog::SetLogLevel(Lint logLevel)
{
	m_level = logLevel;
}

void LLog::LogError(const char* format,...)
{
	if(m_level < LLOG_LEVEL_ERROR)
		return;

	va_list ap;
	va_start(ap,format);
	Log(LLOG_LEVEL_ERROR,format,ap);
	va_end(ap);
}

void LLog::LogInfo(const char* format,...)
{
	if(m_level < LLOG_LEVEL_INFO)
		return;

	va_list ap;
	va_start(ap,format);
	Log(LLOG_LEVEL_INFO,format,ap);
	va_end(ap);
}

void LLog::LogDebug(const char* format,...)
{
	if(m_level < LLOG_LEVEL_DEBUG)
		return;

	va_list ap;
	va_start(ap,format);
	Log(LLOG_LEVEL_DEBUG,format,ap);
	va_end(ap);
}

void LLog::Log(int level,const char* format,va_list ap)
{
	char* str = new char[LLOG_SIZE+1];
	str[LLOG_SIZE] = 0;

	Lstring time = LTimeTool::GetLocalTimeInString();
	memcpy(str,time.c_str(),time.size());
	str[time.size()] = ' ';
	vsnprintf(str+time.size() + 1, LLOG_SIZE - 64, format, ap);
	LLogData* pLog = new LLogData();
	pLog->data = str;	
	Push(pLog);
}

void LLog::WriteFile(char const* content)
{
	Lstring	file = LTimeTool::GetLocalDateInString();
	file += "_";
	file += m_fileName;
	std::ofstream ofs(file.c_str(), std::ios::app | std::ios::out);
	if(ofs)
	{
		ofs << content << std::endl;
		ofs.close();
	}
}