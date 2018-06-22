#ifndef __TCPClient_
#define __TCPClient_

#include <string>
#include <boost/shared_ptr.hpp>
#include "LSocket.h"

using namespace std;

class LNet;
class LTime;

typedef struct
{
	bool			on;
	Lint			interval;	// unit:ms
	Lint			timeBeforeProbe;	// unit:ms
}KeepAliveSetting;

class TCPClient
{
public:
	TCPClient(LNet& net, const string& serverIP, unsigned short serverPort, const KeepAliveSetting& setting, MsgPackReceiver receiver = MsgPackReceiver());
	~TCPClient(void);

public:
	bool						Connect();
	void						Disconnect();
	void						Send(LBuffPtr msg);
	void						OnTick(LTime& cur);
	void						OnKeepAliveAck();
	void						OnConnectionEstablished();
	void						OnConnectionClosed();
	bool						IsEqual(const LSocketPtr& socket);
	const string&				GetServerIP(){return m_serverIP;}
	unsigned short				GetServerPort(){return m_serverPort;}
	const KeepAliveSetting&		GetKeepAliveSetting(){return m_keepaliveSetting;}
	void						SetKeepAliveSetting(const KeepAliveSetting& setting){m_keepaliveSetting = setting;}

private:
	void		On1SecTick(LTime& cur);
	void		On5SecTick(LTime& cur);
	void		SendKeepAlivePacket();
	void		CheckAlive(void);
	void		TryToReconnect();
	LSocketPtr	NewSocket();

private:
	LNet&					m_net;
	const string			m_serverIP;
	const unsigned short	m_serverPort;
	KeepAliveSetting		m_keepaliveSetting;
	LSocketPtr				m_socket;
	Llong					m_1SecTick;
	Llong					m_5SecTick;
	Llong					m_timeKeepAliveSent;
	Lint					m_startTime;
	MsgPackReceiver			m_receiver;
};

typedef boost::shared_ptr<TCPClient>		TCPClientPtr;

bool operator == (const TCPClientPtr& client, const LSocketPtr& socket);
bool operator == (const LSocketPtr& socket, const TCPClientPtr& client);

#endif		// __TCPClient_