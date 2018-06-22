#include "TCPClient.h"
#include "LNet.h"
#include "LTime.h"
#include "LLog.h"
#include "LMsgS2S.h"
#include "BaseCommonFunc.h"

TCPClient::TCPClient(LNet& net, const string& serverIP, unsigned short serverPort, const KeepAliveSetting& setting, MsgPackReceiver receiver)
	: m_net(net), m_serverIP(serverIP), m_serverPort(serverPort), m_keepaliveSetting(setting), m_receiver(receiver), m_timeKeepAliveSent(0)
{
	LTime cur;
	m_1SecTick = cur.MSecs();
	m_5SecTick = cur.MSecs();
	m_startTime = cur.Secs() + setting.timeBeforeProbe / 1000;
}

TCPClient::~TCPClient(void)
{
}

bool TCPClient::Connect()
{
	if(!m_socket)
		m_socket = NewSocket();

	bool succeed = false;

	m_socket->Connect(m_serverIP, m_serverPort, succeed);

	return succeed;
}

void TCPClient::Disconnect()
{
	if(m_socket)
		m_socket->Stop();
}

void TCPClient::Send(LBuffPtr msg)
{
	if(m_socket)
		m_socket->Send(msg);
}

void TCPClient::OnTick(LTime& cur)
{
	if(cur.MSecs() - m_1SecTick > 1000)
	{
		m_1SecTick = cur.MSecs();
		On1SecTick(cur);
	}

	if(cur.MSecs() - m_5SecTick > 5000)
	{
		m_5SecTick = cur.MSecs();
		On5SecTick(cur);
	}
}

void TCPClient::OnKeepAliveAck()
{
	m_timeKeepAliveSent = GetOSTickCount();
	//LLOG_DEBUG("Keepalive ack received from %s:%d.", m_serverIP.c_str(), m_serverPort);
}

void TCPClient::OnConnectionEstablished()
{
}

void TCPClient::OnConnectionClosed()
{
}

bool TCPClient::IsEqual(const LSocketPtr& socket)
{
	return m_socket && m_socket == socket;
}

void TCPClient::On1SecTick(LTime& cur)
{
	CheckAlive();
	TryToReconnect();
}

void TCPClient::On5SecTick(LTime& cur)
{
	if(m_keepaliveSetting.on && cur.Secs() >= m_startTime)
		SendKeepAlivePacket();
}

void TCPClient::SendKeepAlivePacket()
{
	if(m_socket && m_socket->GetSocketConnectStatus() == SOCKET_CONNECT_STATUS_CONNECTED/* && m_timeKeepAliveSent == 0*/)
	{
		LMsgS2SKeepAlive msg;
		m_socket->Send(msg.GetSendBuff());
//		m_timeKeepAliveSent = GetOSTickCount();
		//LLOG_DEBUG("Keepalive packet was sent to server %s:%d.", m_serverIP.c_str(), m_serverPort);
	}
}

void TCPClient::CheckAlive(void)
{
	if(m_keepaliveSetting.on && m_timeKeepAliveSent > 0 && m_socket && m_socket->GetSocketConnectStatus() == SOCKET_CONNECT_STATUS_CONNECTED)
	{
		if(GetOSTickCount() - m_timeKeepAliveSent > m_keepaliveSetting.interval)
		{
			m_socket->Stop();
			m_socket.reset();
			m_timeKeepAliveSent = 0;
			LLOG_ERROR("Because keepalive ack has not been received during %d seconds, close the connection with server %s:%d.", m_keepaliveSetting.interval / 1000, m_serverIP.c_str(), m_serverPort);
		}
	}
}

void TCPClient::TryToReconnect()
{
	if(!m_socket || m_socket->GetSocketConnectStatus() == SOCKET_CONNECT_STATUS_DISCONNECTED)
	{
		LLOG_ERROR("Disconneted with server %s:%d, reconnect...", m_serverIP.c_str(), m_serverPort);
		m_socket = NewSocket();
		m_socket->AsyncConnect(m_serverIP, m_serverPort);
	}
}

LSocketPtr TCPClient::NewSocket()
{
	LSocketPtr socket;
	if(m_receiver)
		socket = m_net.GetNewSocket(m_receiver);
	else
		socket = m_net.GetNewSocket();
	return socket;
}

bool operator == (const TCPClientPtr& client, const LSocketPtr& socket)
{
	bool equal = false;
	if(client)
		equal = client->IsEqual(socket);
	return equal;
}

bool operator == (const LSocketPtr& socket, const TCPClientPtr& client)
{
	return client == socket;
}