#include "LNet.h"
#include "LMsg.h"
#include "LLog.h"

LNet::LNet()
	:m_iose(),m_acceptor(0),m_endpoint(NULL),m_ThreadCount(2),m_isHttp(false)
{

}

LNet::~LNet()
{
	if(m_acceptor)
	{
		delete m_acceptor;
	}
	if(m_endpoint)
	{
		delete m_endpoint;
	}
}

bool LNet::Init(const std::string& ipAddress,short port,int threadCount,bool listen,bool isHttp)
{
	if(m_acceptor || m_endpoint)
	{
		return false;
	}

	LLOG_ERROR("LNet Init   ipAdd:%s   port:%d", ipAddress.c_str(), port);
	m_ThreadCount = threadCount;
	m_endpoint = new boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::from_string(ipAddress),port);

	if(listen)
	{
		m_acceptor = new boost::asio::ip::tcp::acceptor(m_iose,*m_endpoint);
		LLOG_INFO("listen ip:%s,port:%d",ipAddress.c_str(),port);
	}

	m_isHttp = isHttp;
	return true;
}

void LNet::Start()
{
	StartAccept();
	for(int i = 0 ; i < m_ThreadCount; ++i)
	{
		boost::thread* p = new boost::thread(boost::bind(&boost::asio::io_service::run,&m_iose));
		m_ThreadVec.push_back(p);
	}
}

void LNet::StartAccept()
{
	if(m_acceptor)
	{
		LSocketPtr sp = GetNewSocket();
		m_acceptor->async_accept(sp->Socket(),
			boost::bind(&LNet::AcceptHandler,
			this,
			boost::asio::placeholders::error,
			sp));
	}
}

void LNet::AcceptHandler(const boost::system::error_code& ec, LSocketPtr sp)
{
	if(!ec)
	{
		sp->Start();
	}

	StartAccept();
}

void LNet::Stop()
{
	m_iose.stop();
}

void LNet::Join()
{
	for(int i = 0 ; i < m_ThreadCount; ++i)
	{
		m_ThreadVec[i]->join();
	}
}

LSocketPtr LNet::GetNewSocket()
{
	MsgPackReceiver receiver = boost::bind(&LNet::RecvMsgPack, this, boost::placeholders::_1, boost::placeholders::_2);
	return LSocketPtr(new LSocket(*this, receiver, m_headerChecker));
}

LSocketPtr LNet::GetNewSocket(MsgPackReceiver receiver, MsgHeaderChecker checker)
{
	return LSocketPtr(new LSocket(*this, receiver, checker));
}

boost::asio::ip::tcp::endpoint& LNet::GetEndpoint()
{
	return *m_endpoint;
}

boost::asio::io_service& LNet::GetIoservice()
{
	return m_iose;
}

bool LNet::GetIsHttp()
{
	return m_isHttp;
}