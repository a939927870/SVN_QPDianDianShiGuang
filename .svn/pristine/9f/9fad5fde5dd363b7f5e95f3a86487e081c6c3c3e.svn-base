#ifndef _L_BUFF_H_
#define _L_BUFF_H_

#include "LBase.h"

#define BUFF_SIZE 1024


class LBuff
{
private:
	LBuff(const LBuff&);
	LBuff& operator = (const LBuff&);

public:
	LBuff():m_pData(NULL),m_offset(0),m_curPos(0),m_size(BUFF_SIZE)
	{
		m_pData = new char[m_size];
	}

	~LBuff()
	{
		if(m_pData != NULL)
		{
			delete[] m_pData;
		}
	}
	/*
	*	brief 重新初始化
	*/
	PVOID Alloc(DWORD dwSize)
	{
		if(m_pData != NULL)
		{
			delete[] m_pData;
		}
		m_pData = new char[dwSize];
		m_curPos = m_offset = 0;
		m_size = dwSize;
		return m_pData;
	}

	/*
	*	brief 获取LBuff的起始指针
	*/
	char* Data()const
	{
		return m_pData;
	}

	/*
	*	brief 获取LBuff的容量
	*/
	int Capacity()const
	{
		return m_size;
	}

	/*
	*	brief 获取LBuff的当前长度
	*/
	int   Size()const
	{
		return m_curPos;
	}

	/*
	*	brief 获取LBuff的当前读取位置的偏移
	*/
	int   GetOffset()const
	{
		return m_offset;
	}

	/*
	*	brief 设置LBuff的当前读取位置的偏移
	*/
	void  SetOffset(int offset)
	{
		m_offset = offset;
	}

	/*
	*	brief 设置LBuff的当前读取位置的偏移
	*/
	void  Cut(int len)
	{
		if(len >= m_curPos)
		{
			m_curPos = 0;
		}
		else
		{
			memcpy(m_pData,m_pData+len,m_curPos-len);
			m_curPos -= len;
		}
	}

	/*
	*	brief 向LBuff里面写入数据
	*/
	void  Write(void const* data,int len)
	{
		if(m_curPos + len > m_size)
		{
			m_size = BUFF_SIZE*((m_curPos + len)/BUFF_SIZE + 1);

			char* pNew = new char[m_size];
			memcpy(pNew,m_pData,m_curPos);
			delete[] m_pData;
			m_pData = pNew;
		}

		memcpy(m_pData+m_curPos,data,len);
		m_curPos += len;
	}

	/*
	*	brief 从LBuff里读出数据
	*/
	void  Read(void* data,int len)
	{
		if(m_offset + len > m_curPos)
			return;

		memcpy(data,m_pData+m_offset,len);
		m_offset += len;
	}

	template<class T> 
	void Write(T const & value)
	{
		Write(&value,sizeof(value));
	}

	template<class T>
	LBuff& operator << (T const& value)
	{
		Write(value);
		return *this;
	}

	template<class T>
	void Read(T& value)
	{
		Read(&value,sizeof(value));
	}

	template<class T>
	LBuff& operator >> (T& value)
	{
		Read(value);
		return *this;
	}

private:
	char* m_pData;
	int m_offset;
	int m_curPos;
	int m_size;
};

//char* 
template<> 
inline void LBuff::Write(char* const & value)
{
	int size = 0;
	if(value == NULL || (size = strlen(value)) == 0)
	{
		Write(&size,sizeof(size));
	}
	else
	{
		Write(&size,sizeof(size));
		Write(value,size);
	}
}

//std::string
template<> 
inline void LBuff::Write(std::string const& value)
{
	int size = value.size();
	Write(&size,sizeof(size));
	if(size != 0)
	{
		Write(value.c_str(),size);
	}
}

//LBuff 
template<> 
inline void LBuff::Write(LBuff const& value)
{
	Write(value.Size());
	Write(value.Data(),value.Size());
}

template<> 
inline void LBuff::Read(std::string& value)
{
	int len = 0;
	Read(&len,sizeof(len));
	if(len > 0 && len < m_curPos)
	{
		char* tmp = new char[len+1];
		Read(tmp,len);
		tmp[len] = 0;
		value = tmp;
		// 代码风险点处理 [6/22/2016 mcc]
		delete[] tmp;
	}
	else
	{
		value.clear();
	}
}

template<> 
inline void LBuff::Read(LBuff& value)
{
	int size = 0;
	Read(size);
	value.Write(Data()+GetOffset(),size);
	SetOffset(GetOffset()+size);
}

/*
*	LBuff的智能指针
*/
typedef boost::shared_ptr<LBuff> LBuffPtr;

#endif