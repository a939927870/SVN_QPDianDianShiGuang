#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "WeiXinAuthService.h"
#include "LMsgS2S.h"
#include "Config.h"
#include "Work.h"
#include "HttpRequest.h"

#define MAX_QUEUE_SIZE  50000

WeiXinAuthService::WeiXinAuthService()
	: m_running(false)
{
  m_workerNum = 8;
}

WeiXinAuthService::~WeiXinAuthService()
{
  Stop();
}

bool WeiXinAuthService::Init()
{
  if (!HttpRequest::Instance().Init()) {
    return false;
  }
  return true;
}

bool WeiXinAuthService::Final()
{
  if (!HttpRequest::Instance().Final()) {
    return false;
  }
  return true;
}


void WeiXinAuthService::Start()
{
    if (!m_running) {
        m_queues.resize(m_workerNum);
        for( int idx = 0; idx < m_workerNum; ++idx ) {
            MsgQueuePtr queue( new MsgQueue ); 
            assert(queue != NULL);
            if(queue->init(MAX_QUEUE_SIZE + 1) < 0) {
                LLOG_ERROR("init queue failed, queue_capacity: %d", MAX_QUEUE_SIZE);
                return;
            }
            m_queues[idx] = queue;
        }

        for(int i = 0; i < m_workerNum; i++)
            m_threads.create_thread(boost::bind(&WeiXinAuthService::AuthenticationLoop, this, m_queues[i]));
        m_running = true;
  }
}

void WeiXinAuthService::Stop()
{
    if (m_running) {
        for( Lsize idx = 0; idx < m_queues.size(); ++idx ) {
            auto queue = m_queues[idx];
            if(queue != NULL)
            {
                queue->push(NULL);    
            }
        }

        m_threads.join_all();
        m_running = false;
    }
}

void WeiXinAuthService::Push(LMsg *msg)
{
    if (m_threads.size() == 0 || m_running == false) {
        return;
    }

    static unsigned int requestCount = 0;
    auto queue = m_queues[requestCount % m_queues.size()];
    if(queue == NULL)
    {
        LLOG_ERROR("function: %s, queue is NULL!", __FUNCTION__);
        return;
    }

    LMsg* dlvmsg = LMsgFactory::Instance().CreateMsg(msg->m_msgId);
    if(dlvmsg == NULL)
    {
        LLOG_ERROR("function: %s, dlvmsg is NULL!", __FUNCTION__);   
        return;
    }

    LBuff buff;
    msg->Write(buff);
    dlvmsg->Read(buff);

    queue->push(dlvmsg);
    ++requestCount;
}

void WeiXinAuthService::AuthenticationLoop(MsgQueuePtr queue)
{
    while(true) {
        if(queue == NULL)
        {
            LLOG_ERROR("function: %s, queue is NULL, thread will exit!", __FUNCTION__);
            return;
        }

        if(queue->empty())
        {
            boost::this_thread::sleep(boost::posix_time::millisec(1));
            continue;    
        }

        LMsg* msg = queue->front();
        queue->pop();

        if(msg)
        {
            HandleMsg(msg);
            delete msg;
        }
        else
        {
            LLOG_ERROR("function: %s, thread exit!");
            break;
        }
    }
}

void WeiXinAuthService::HandleMsg(LMsg *msg)
{
  if (msg == NULL)
    return;

  switch(msg->m_msgId) {
  case MSG_LG_CE_USER_MSG_WRAPPER:
    HandleUserAuth((LMsgLGCEUserMsgWrapper *)msg);
    break;
  default:
    LLOG_ERROR("WeiXinAuthService.HandleMsg -- Wrong msg : msgid=%d", msg->m_msgId);
    break;
  }
}

void WeiXinAuthService::HandleUserAuth(LMsgLGCEUserMsgWrapper *msg)
{
  LMsgC2SMsg *umsg = (LMsgC2SMsg *)(msg->m_userMsg);
  if (DoAuthentication(umsg))
    {
      LLOG_DEBUG("WeiXinAuthService::HandleUserAuth WeiXin Auth Success");
      umsg->m_wxConfirmed = 1;
      LMsgLGCEUserMsgWrapper wrapper;
      wrapper.m_idOnGate = msg->m_idOnGate;
      wrapper.m_userMsg = umsg;
      wrapper.m_deleteUserMsg = false;
      gWork.SendToCenter(wrapper);
    }
  else {
    LLOG_ERROR("WeiXinAuthService::HandleUserAuth WeiXin Auth failed");
      LMsg* dlvmsg = LMsgFactory::Instance().CreateMsg(msg->m_msgId);
	  umsg->m_wxConfirmed = 2; // 表示验证失败
      if(dlvmsg != NULL)
      {
          LBuff buff;
          msg->Write(buff);
          dlvmsg->Read(buff);	  
		  LLOG_ERROR("WeiXinAuthService::HandleUserAuth Auth Failed Push to Work to del socket.");
          gWork.Push(dlvmsg);
      }
  }
}

bool WeiXinAuthService::DoAuthentication(LMsgC2SMsg * msg)
{
	LLOG_DEBUG("Start to authenticate user %s.", msg->m_uuid.c_str());

	bool succeed = false;
	
	if (msg->m_plate == "wechat")
		succeed = DoWeiXinAuthentication(msg);
	else if(msg->m_plate == "local")
		succeed = true;

	if (succeed)
	  LLOG_DEBUG("user %s , plate: %s authentication succeed.", msg->m_uuid.c_str(), msg->m_plate.c_str());
	else
	  LLOG_ERROR("Failed to authenticate user %s plate: %s.", msg->m_uuid.c_str(), msg->m_plate.c_str());

	return succeed;
}

bool WeiXinAuthService::DoWeiXinAuthentication(LMsgC2SMsg * msg)
{
	stringstream ss;
	ss << "https://api.weixin.qq.com/sns/userinfo?access_token=";
	ss << msg->m_accessToken << "&openid=";
	ss << msg->m_openId;

	Lstring result;
	if (!HttpRequest::Instance().HttpGet(ss.str(), result, true))
	{
		LLOG_ERROR("WeiXinAuthService::DoWeiXinAuthentication get user info error %s", ss.str().c_str());
		return false;
	}

	// 读取结果值
	boost::property_tree::ptree parser;
	stringstream resultstream;
	resultstream << result;
	boost::property_tree::json_parser::read_json(resultstream, parser);

	// 含有errcode 表示错误了
	string errcode = parser.get<string>("errcode", "");
	if(!errcode.empty())
	{
		LLOG_ERROR("WeiXinAuthService::DoWeiXinAuthentication wechat error=%s", errcode.c_str());
		return false;
	}

	// 验证下uuid
	string uuid = parser.get<string>("unionid", "");
	if (uuid != msg->m_uuid)
	{
		LLOG_ERROR("WeiXinAuthService::DoWeiXinAuthentication uuid error, %s:%s", uuid.c_str(), msg->m_uuid.c_str());
		return false;
	}

	return true;
}
