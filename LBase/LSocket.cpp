#include "LSocket.h"
#include "LNet.h"
#include "LMsg.h"
#include "LLog.h"

#define RECV_BUFFER_SIZE	(8 * 1024)

LSocket::LSocket(LNet& ls, MsgPackReceiver receiver, MsgHeaderChecker checker)
	:m_ns(ls),
	m_receiver(receiver),
	m_headerChecker(checker),
	m_socket(ls.GetIoservice()),
	m_strand(ls.GetIoservice()),
	m_connectStatus(SOCKET_CONNECT_STATUS_DISCONNECTED),
	m_start(false),
	m_sending(false),
	m_remoteIp(0),
	m_remotePort(0),
	m_active(false)
{
	m_pRecvBuff = new char[RECV_BUFFER_SIZE];
	memset(m_pRecvBuff, 0, RECV_BUFFER_SIZE);

	m_iCurrentSendSize = 0;
}

LSocket::LSocket(LNet& ls)
	:m_ns(ls),
	m_socket(ls.GetIoservice()),
	m_strand(ls.GetIoservice()),
	m_connectStatus(SOCKET_CONNECT_STATUS_DISCONNECTED),
	m_start(false),
	m_sending(false),
	m_remoteIp(0),
	m_remotePort(0),
	m_active(false)
{
	m_pRecvBuff = new char[RECV_BUFFER_SIZE];
	memset(m_pRecvBuff, 0, RECV_BUFFER_SIZE);

	m_iCurrentSendSize	= 0;
}

LSocket::~LSocket()
{
	if(m_pRecvBuff)
	{
		delete [] m_pRecvBuff;
	}
}

void LSocket::Connect(const boost::asio::ip::tcp::endpoint& endpoint)
{
	if(m_connectStatus != SOCKET_CONNECT_STATUS_DISCONNECTED)
	{
		LLOG_ERROR("LSocket::Connect -- Need to stop firtly");
		return;
	}
	m_connectStatus = SOCKET_CONNECT_STATUS_CONNECTING;
	boost::system::error_code ec;
	m_socket.connect(endpoint, ec);
	if(ec)
	{
		m_connectStatus = SOCKET_CONNECT_STATUS_DISCONNECTED;
		return;
	}
	m_connectStatus = SOCKET_CONNECT_STATUS_CONNECTED;
	Start();
}

void LSocket::Connect(const Lstring& ip,Lshort port)
{
	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address_v4::from_string(ip), port);
	Connect(endpoint);
}

void LSocket::Connect(const Lstring& ip,Lshort port, bool& success)
{
	success = false;

	if(m_connectStatus != SOCKET_CONNECT_STATUS_DISCONNECTED)
	{
		LLOG_ERROR("LSocket::Connect -- Need to stop firtly");
		return;
	}

	m_connectStatus = SOCKET_CONNECT_STATUS_CONNECTING;
	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address_v4::from_string(ip), port);
	boost::system::error_code ec;
	m_socket.connect(endpoint, ec);
	if(ec)
	{
		m_connectStatus = SOCKET_CONNECT_STATUS_DISCONNECTED;
		return;
	}
	m_connectStatus = SOCKET_CONNECT_STATUS_CONNECTED;
	Start();
	success = true;
	return;
}

void LSocket::AsyncConnect(const boost::asio::ip::tcp::endpoint& endpoint)
{
	if(m_connectStatus != SOCKET_CONNECT_STATUS_DISCONNECTED)
	{
		LLOG_ERROR("LSocket::AsyncConnect -- Need to stop firtly");
		return;
	}
	m_connectStatus = SOCKET_CONNECT_STATUS_CONNECTING;

	m_socket.async_connect(endpoint,
		boost::bind(&LSocket::HandleAsyncConnect,
		shared_from_this(),
		boost::asio::placeholders::error));
}

void LSocket::AsyncConnect(const Lstring& ip,Lshort port)
{
	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address_v4::from_string(ip), port);
	AsyncConnect(endpoint);
}

bool LSocket::HandleAsyncConnect(const boost::system::error_code& e)
{
	if(!e)
	{
		m_connectStatus = SOCKET_CONNECT_STATUS_CONNECTED;
		Start();
	}
	else
		m_connectStatus = SOCKET_CONNECT_STATUS_DISCONNECTED;

	return !e;
}

void LSocket::Start()
{
	if(m_start)
	{
		LLOG_ERROR("LSocket::Start -- Start had been invoked");
		return;
	}

	boost::system::error_code ec;
	boost::asio::ip::tcp::socket::endpoint_type p = m_socket.remote_endpoint(ec);
	if(ec)
	{
		Stop();
		return;
	}
	m_remoteIp = p.address().to_v4().to_ulong();

	m_ip = p.address().to_v4().to_string();
	m_remotePort = p.port();

	boost::asio::ip::tcp::socket::endpoint_type pl = m_socket.local_endpoint(ec);
	if(ec)
	{
		Stop();
		return;
	}
	m_remoteIp = pl.address().to_v4().to_ulong();

	//std::string ip = pl.address().to_v4().to_string();
	Lushort m_Port = pl.port();


	//LLOG_DEBUG("client login ip=%s,port=%d,local_port=%d",p.address().to_v4().to_string().c_str(),m_remotePort,m_Port);

	m_curretSendBuffer.reset();
	m_currentReadBuffer.reset(new LBuff);
	m_iCurrentSendSize	= 0;

	m_start = true;

	LMsgIn msg;
	LBuffPtr pBuff(new LBuff());
	msg.WriteMsgId(*pBuff );
	OnRecvOneMsgPack(pBuff);	

	m_socket.async_read_some(boost::asio::buffer(m_pRecvBuff, RECV_BUFFER_SIZE),
		boost::bind(&LSocket::HandleRead,
		shared_from_this(),
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

void LSocket::Stop()
{
	if(m_connectStatus == SOCKET_CONNECT_STATUS_CONNECTING || m_connectStatus == SOCKET_CONNECT_STATUS_DISCONNECTING)
	{
		LLOG_ERROR("Socket is operating. Status = %d", m_connectStatus);
		return;
	}

	if(!m_start)
	{
		return;
	}

	try
	{
		m_strand.post(boost::bind(&LSocket::_closeSocket, shared_from_this()));
	}
	catch(...)
	{
		LLOG_ERROR("LSocket::Stop -- Exception on stop");
	}
}

int LSocket::getSendPoolSize()
{
	boost::mutex::scoped_lock l(m_mutexSocketForWrite);
	if(!m_start)
	{
		return 0;
	}

	return m_sendPool.size();
}

void LSocket::OnRecvOneMsgPack(LBuffPtr buff)
{
	m_ns.RecvMsgPack(buff,shared_from_this());
}

void LSocket::HandleRead(const boost::system::error_code& e,std::size_t readSize)
{
	boost::mutex::scoped_lock l(m_mutexSocketForRead);
	if(!m_start)
	{
		return;
	}

	if(!e)
	{ 
		if(readSize > 0)
		{
			m_currentReadBuffer->Write(m_pRecvBuff, readSize);

			//LLOG_ERROR("read size = %d", m_iCurrentReadSize);
			
			if(OnRecv())
			{
				m_socket.async_read_some(boost::asio::buffer(m_pRecvBuff, RECV_BUFFER_SIZE),
					boost::bind(&LSocket::HandleRead,
					shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
			}
			else
			{
				Kick();
			}
			
		}
		else //网络被关闭
		{
			Kick();
		}
	}
	else
	{
		Kick();
	}
}

void LSocket::HandleWrite(const boost::system::error_code& e, std::size_t writeSize)
{
	boost::mutex::scoped_lock l(m_mutexSocketForWrite);
	if(!m_start)
	{
		return;
	}
	//LLOG_ERROR("send111 size = %d", writeSize);
	if(!e)
	{
		if(/*writeSize >= 0 &&*/ m_curretSendBuffer.get() != NULL)
		{
			m_iCurrentSendSize += writeSize;

			int iRemainSize = m_curretSendBuffer->Size() - m_iCurrentSendSize;
			if(iRemainSize == 0)
			{
				m_iCurrentSendSize = 0;
				m_curretSendBuffer.reset();

				m_sending = false;
				m_sendPool.pop_front();
				Check();
			}
			else
			{
				//LLOG_DEBUG("send size = %d   %d", m_iCurrentSendSize, iRemainSize);
				m_socket.async_write_some(boost::asio::buffer(m_curretSendBuffer->Data() + m_iCurrentSendSize, iRemainSize),
					boost::bind(&LSocket::HandleWrite,
					shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
			}
		}
		else
		{
			Kick();
		}
	}	
	else
	{
		Kick();
	}
}

bool LSocket::Check()
{
	if(!m_sending && m_sendPool.size() != 0)
	{
		m_sending = true;

		m_curretSendBuffer = m_sendPool.front();
		m_socket.async_write_some(boost::asio::buffer(m_curretSendBuffer->Data(), m_curretSendBuffer->Size()),
			boost::bind(&LSocket::HandleWrite,
			shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));        
	}

	return true;
}

void LSocket::Send(LBuffPtr pBuff)
{
	boost::mutex::scoped_lock l(m_mutexSocketForWrite);
	if(m_start)
	{
		m_sendPool.push_back(pBuff);
		Check();
	}  
}

boost::asio::ip::tcp::socket& LSocket::Socket()
{
	return m_socket;
}

void LSocket::Kick()
{
	if(m_start)
	{
		Stop();

		LMsgKick msg;
		LBuffPtr pBuff(new LBuff());
		msg.WriteMsgId(*pBuff );
		OnRecvOneMsgPack(pBuff);
		//LLOG_DEBUG("LSocket::Kick");
	}
	//m_start = false;
}

bool LSocket::OnRecv()
{
	if(m_ns.GetIsHttp())
	{
		return onRecvHttp();
	}
	else
	{
		return onRecvBinary();
	}
}

Lstring LSocket::GetRemoteIp()
{
	return m_ip;
}

int LSocket::GetRemotePort()
{
	return m_remotePort;
}

bool LSocket::GetActive()
{
	return m_active;
}

bool LSocket::onRecvBinary()
{
	Lint offsetData = 0;
	Lint iDataPacketSize = m_currentReadBuffer->Size();
	Lint iFormatLen = sizeof(MsgHeader);	// size + checksum + time + msgid;
	bool bRet = true;
	do 
	{
		if(iDataPacketSize < iFormatLen) //len
		{
			break;
		}

		MsgHeader header = {0};

		m_currentReadBuffer->SetOffset(offsetData);
		m_currentReadBuffer->Read((char*)&header, sizeof(header));

		if (m_headerChecker)
		{
			if (!m_headerChecker(shared_from_this(), header))
			{
				LLOG_ERROR("Message header from %s:%d was invalid.", m_ip.c_str(), m_remotePort);
				bRet = false;
				break;
			}
		}

		unsigned short uDataLen = header.bodySize;

		if(uDataLen > (0xFFFF - iFormatLen))
		{
			LLOG_ERROR("LSocket::onRecvBinary -- The data len more than max value");
			bRet = false;
			break;
		}

		Lint iTotalSize = uDataLen + iFormatLen; 

		if(iDataPacketSize < iTotalSize) //对比总的字节数
		{
			if(m_currentReadBuffer->Capacity() < iTotalSize)
			{
				//重新分配空间
				LBuff tmpBuffer;
				tmpBuffer.Write(m_currentReadBuffer->Data(), m_currentReadBuffer->Size());

				Lint allocSize = ((iTotalSize / BUFF_SIZE) + 1) * BUFF_SIZE;
				m_currentReadBuffer->Alloc(allocSize);
				m_currentReadBuffer->Write(tmpBuffer.Data(), tmpBuffer.Size());
			}
			
			break;
		}

		LBuffPtr data(new LBuff());
		if(uDataLen > data->Capacity())
		{
			data->Alloc(uDataLen);
		}
		
		data->Write(m_currentReadBuffer->Data() + offsetData + iFormatLen, uDataLen);
		OnRecvOneMsgPack(data);

		offsetData += iTotalSize;
		iDataPacketSize -= iTotalSize;

		//接收到一个完整的消息，这个链接就算是激活了
		m_active = true;

	}while(true);

	if(offsetData > 0)
	{
		_completeReadData(offsetData);
	}
	
	return bRet;
}

bool LSocket::onRecvHttp()
{
	Lint offsetData = 0;
	Lint iDataPacketSize = m_currentReadBuffer->Size();
	do 
	{
		if(iDataPacketSize == 0)
		{
			break;
		}

		std::string head,body;
		std::string httpStr(m_currentReadBuffer->Data() + offsetData, iDataPacketSize);

		std::string::size_type posHeadEnd = httpStr.find("\r\n\r\n", 0);
		if(posHeadEnd == std::string::npos)
		{
			break;
		}

		//解析正文长度
		head = httpStr.substr(0, posHeadEnd+4);

		std::string::size_type posContentLenBegin = httpStr.find("Content-Length:",0);
		if(posContentLenBegin == std::string::npos)
		{
			LBuffPtr buff(new LBuff());
			buff->Write(int(MSG_HTTP));
			buff->Write(head);
			buff->Write(body);
			OnRecvOneMsgPack(buff);

			offsetData += posHeadEnd + 4;
			iDataPacketSize -= posHeadEnd + 4; 
			continue;
		}

		std::string::size_type posContentEnd = httpStr.find("\r\n", posContentLenBegin);
		if(posContentEnd == std::string::npos)
		{
			break;
		}

		std::string strContentLength = httpStr.substr(posContentLenBegin+15, posContentEnd-posContentLenBegin-15);
		int dataLen = atoi(strContentLength.c_str());
		if((httpStr.size() - posHeadEnd - 4) < (std::size_t)dataLen )
		{
			break;
		}

		head = httpStr.substr(0,posHeadEnd+4);
		body = httpStr.substr(posHeadEnd+4,dataLen);
		LBuffPtr buff(new LBuff());
		buff->Write(int(MSG_HTTP));
		buff->Write(head);
		buff->Write(body);
		OnRecvOneMsgPack(buff);

		offsetData += posHeadEnd + dataLen + 4;
		iDataPacketSize -= posHeadEnd + dataLen + 4;

	}while(true);

	if(offsetData > 0)
	{
		_completeReadData(offsetData);
	}
	return true;
}

void LSocket::_completeReadData(Lint iCompleteLen)
{
	if(m_currentReadBuffer.get() == NULL || iCompleteLen <= 0)
	{
		return;
	}

	LBuff* pReplace = new LBuff;
	if(iCompleteLen < m_currentReadBuffer->Size())
	{
		Lint iRemain = m_currentReadBuffer->Size() - iCompleteLen;
		Lint allocSize = ((iRemain / BUFF_SIZE) + 1) * BUFF_SIZE;
		pReplace->Alloc(allocSize);
		pReplace->Write(m_currentReadBuffer->Data() + iCompleteLen, iRemain);
	}

	m_currentReadBuffer.reset(pReplace);
}

void LSocket::_closeSocket()
{
	try
	{
		//销毁之前先拿到读和写的权限
		boost::mutex::scoped_lock lr(m_mutexSocketForRead);
		boost::mutex::scoped_lock lw(m_mutexSocketForWrite);
		//if(!m_socket.is_open())
		//{
		//	return;
		//}

		m_connectStatus = SOCKET_CONNECT_STATUS_DISCONNECTING;

		boost::system::error_code error;
		m_socket.shutdown(boost::asio::socket_base::shutdown_both, error);
		if(error)
		{
			LLOG_DEBUG("LSocket::Stop -- Fail to shut down. Error = %s", error.message().c_str());
		}

		m_socket.close(error);
		if(error)
		{
			LLOG_DEBUG("LSocket::Stop -- Fail to close socket. Error = %s", error.message().c_str());
		}
	}
	catch(...)
	{
		LLOG_DEBUG("LSocket::_closeSocket -- Have exception on close socket");
	}

	m_connectStatus = SOCKET_CONNECT_STATUS_DISCONNECTED;
	m_start = false;
}


//////////////////////////////////////////////////////////////////////////
