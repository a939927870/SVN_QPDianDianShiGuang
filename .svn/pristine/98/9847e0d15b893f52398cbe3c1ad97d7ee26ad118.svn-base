#ifndef _L_NET_H_
#define _L_NET_H_

#include "LSocket.h"
#include <vector>

/*
*	brief 网络管理类
*	主要实现：网络监听，管理连接，消息管理的功能
*/

class LNet
{
public:
	LNet();
	virtual~LNet();

	/* 
	*	brief 设置参数
	*	param ipAddress 连接或者监听的ip地址
	*	param port 连接或者监听的端口
	*	param threadCount 启动的线程数量
	*	param listen 是否需要监听该端口
	*	param isHttp 是否是http服务器，http目前只支持GET，POST请求
	*/
	virtual bool Init(const std::string& ipAddress,short port,int threadCount=2,bool listen=false,bool isHttp=false);

	/*
	*	brief 开启网络服务
	*/
	virtual void Start();

	/*
	*	brief 开启监听端口，等待客户端连接进来
	*/
	virtual void StartAccept();

	/*
	*	brief 关闭网络服务
	*/
	virtual void Stop();

	/*
	*	brief 等待网络服务器线程终止
	*/
	virtual void Join();

	/*
	*	brief 收到一个完整的消息包
	*	param recv 一个完整的消息buff
	*	param s 收到消息的socket
	*	这个函数必须继承，须根据具体的逻辑情况，具体处理
	*/
	virtual void RecvMsgPack(LBuffPtr recv,LSocketPtr s) = 0;

	/*
	*	brief 监听回调函数
	*	param ec 网络回调错误码
	*	param sp 成功连接上的socket
	*/
	void AcceptHandler(const boost::system::error_code& ec, LSocketPtr sp);

	/*
	*	brief 获取新的socket
	*	return	new socket
	*/
	LSocketPtr GetNewSocket();

	LSocketPtr GetNewSocket(MsgPackReceiver receiver, MsgHeaderChecker checker = MsgHeaderChecker());
	void	   SetMsgHeaderChecker(MsgHeaderChecker checker) { m_headerChecker = checker; }

	/*
	*	brief 获取服务器的endpoint
	*/
	boost::asio::ip::tcp::endpoint& GetEndpoint();

	/*
	*	brief 获取服务器的io_service
	*/
	boost::asio::io_service& GetIoservice();

	/*
	*	brief 获取服务器的是否是http服务器
	*/
	bool GetIsHttp();


private:
	boost::asio::io_service m_iose;
	boost::asio::ip::tcp::acceptor* m_acceptor;
	boost::asio::ip::tcp::endpoint* m_endpoint;
	int m_ThreadCount;
	std::vector<boost::thread*> m_ThreadVec;
	bool	m_isHttp;

	MsgHeaderChecker		m_headerChecker;
};

#endif