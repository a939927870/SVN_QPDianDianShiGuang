#include <boost/thread/scoped_thread.hpp>
#include "Work.h"
#include "LLog.h"
#include "Config.h"
#include "InsideNet.h"
#include "LMsgS2S.h"

//初始化
bool Work::Init()
{
	//设置log文件
	LLog::Instance().SetFileName("LogicDB");

	//初始化配置文件
	if(!gConfig.Init())
	{
		LLOG_ERROR("gConfig.Init error");
		return false;
	}

	LLog::Instance().SetLogLevel(gConfig.GetLogLevel());

	LMsgFactory::Instance().Init();

	if(!m_dbsession.Init(gConfig.GetDbHost(),gConfig.GetDbUser(),gConfig.GetDbPass(),gConfig.GetDbName(),"utf8mb4",gConfig.GetDbPort()))
	{
		LLOG_ERROR("m_dbsession.Init error");
		return false;
	}

	if (!gInsideNet.Init())
	{
		LLOG_ERROR("gInsideNet.Init error");
		return false;
	}
	
	return true;
}

bool Work::Final()
{
	return true;
}

//启动
void Work::Start()
{
	gInsideNet.Start();

	LRunnable::Start();
}

//等待
void Work::Join()
{
	gInsideNet.Join();

	//等待逻辑线程终止
	LRunnable::Join();

}

//停止
void Work::Stop()
{
	m_logicManager.reset();
	
	m_logicServer.clear();

	gInsideNet.Stop();

	LRunnable::Stop();
}

void Work::Clear()
{

}

void Work::Run()
{
	while(!GetStop())
	{
		//这里处理游戏主循环
		LTime cur;
		if(cur.MSecs() - m_tickTimer.MSecs() > 1)
		{
			m_tickTimer = cur;
			Tick(cur);	
		}

		LMsg* msg = (LMsg*)Pop();
		if(msg == NULL)
		{
			Sleep(1);
			continue;
		}
		else
		{
			HanderMsg(msg);
			delete msg;
		}
	}
}

void Work::Tick(LTime& cur)
{
	//30毫秒循环一次
	if (cur.MSecs() - m_30MSceTick > 30)
	{
		m_30MSceTick = cur.MSecs();
	}

	//200毫秒循环一次
	if (cur.MSecs() - m_200MSceTick > 200)
	{
		m_200MSceTick = cur.MSecs();
	}

	//1000毫秒循环一次
	if (cur.MSecs() - m_1SceTick > 1000)
	{
		m_1SceTick = cur.MSecs();
	}

	//30秒循环一次
	if (cur.MSecs() - m_30SceTick > 30*1000)
	{
		m_30SceTick = cur.MSecs();
	}

	//5分钟循环一次
	if (cur.MSecs() - m_600SceTick > 5*60 * 1000)
	{
		m_600SceTick = cur.MSecs();

		//删除LOG
		Lint hour = gWork.GetCurTime().GetHour();
		Lint min = gWork.GetCurTime().GetMin();
		if (CheckDelLogVideoTime(hour, min))
		{
			boost::scoped_thread<boost::detach> t((boost::thread(boost::bind(&Work::ClearOldLogVideo, this, hour))));
		}
	}
}

LTime& Work::GetCurTime()
{
	return m_tickTimer;
}

void Work::HanderMsg(LMsg* msg)
{
	//玩家请求登录;
	switch(msg->m_msgId)
	{
	case MSG_CLIENT_KICK:
		HanderUserKick((LMsgKick*)msg);
		break;
	case MSG_CLIENT_IN:
		HanderClientIn((LMsgIn*)msg);
		break;
	//////////////////////////////////////////////////////////////////////////
	//logicmanager 与logicdb;
	case MSG_LMG_2_LDB_LOGIN:
		HanderLogicManagerLogin((LMsgLMG2LdbLogin*)msg);
		break;
	//logic 跟 logicdb 之间的消息交互;
	case MSG_L_2_LDB_LOGIN:
		HanderLogicLogin((LMsgL2LdbLogin*)msg);
		break;
	case MSG_L_2_LDB_VIDEO:
		HanderLogicReqVideo((LMsgL2LDBReqVideo*) msg);
		break;
	case MSG_L_2_LDB_ROOM_SAVE:
		HanderLogicReqSaveLog((LMsgL2LDBSaveLogItem*) msg);
		break;
	case MSG_L_2_LDB_VIDEO_SAVE:
		HanderLogicReqSaveVideo((LMsgL2LDBSaveVideo*) msg);
		break;
	case MSG_L_2_LDB_ONE_VIP_LOG:
		HanderLogicReqOneVipLog((LMsgL2LDBReqOneVipLog*) msg);
		break;
	case MSG_L_2_LDB_VIP_LOG:
		HanderLogicReqVipLog((LMsgL2LBDReqVipLog*) msg);
		break;
	case MSG_L_2_LDB_REQ_ACTIVE:
		HandleLogicReqActive((LMsgL2LDBReqActive*) msg);
		break;
	case MSG_L_2_LDB_SYC_DRAW:
		HandleLogicSaveSyncDraw((LMsgL2LDBSyncDraw*) msg);
		break;
	case MSG_L_2_LDB_SYC_PHONE:
		HandleLogicUpdatePhone((LMsgL2LDBSyncPhone*) msg);
		break;
		//logic 玩家请求分享某录像，先去sharevideo中查看是否已被分享;
	case MSG_L_2_LDB_GETSHAREVIDEOID:
		HanderLogicGetShareVideoId((LMsgL2LDBGGetShareVideoId*) msg);
		break;
		//logic 请求保存某已转移到sharevideo中的某录像的shareid;
	case MSG_L_2_LDB_SAVESHAREVIDEOID:
		HanderLogicSaveShareVideoId((LMsgL2LDBSaveShareVideoId*) msg);
		break;
		//logic 玩家通过shareid来获取该分享码下的录像 在sharevideo中查询;
	case MSG_L_2_LDB_GETSHAREVIDEO:
		HanderLogicReqShareVideo((LMsgL2LDBGetShareVideo*) msg);
		break;
		//loigc 通过精确的videoid来查找sharevideo中的对应录像;
	case MSG_L_2_LDB_GETSHAREVIDEOBYVIDEOID:
		HanderLogicGetShareVideo((LMsgL2LDBGetSsharedVideo*) msg);
		break;
	case MSG_LMG_2_LDB_BREATH:
		HandleLMGHeartBeat((LMsgLMG2LDBHEARTBEAT*) msg);
		break;
	case MSG_L_2_LDB_BREATH:
		HandleLHeartBeat((LMsgL2LDBHEARTBEAT*) msg);
		break;
	case MSG_L_2_LDB_REQ_SIGN_COLLECT_WIN:	//请求十一活动数据
		HandleLogicReqSignCollectWin((LMsgL2LDBReqSignCollectWin*) msg);
		break;

	case MSG_L_2_LDB_SAVE_SIGN_COLLECT_WIN:
		HandleLogicSaveSignCollectWin((LMsgL2LDBSaveSignCollectWin*) msg);	//签到
		break;
	case MSG_LMG_2_LDB_INSERT_BINDING_RELATIONSHIP:
		HandleInsertBindingRelationship((LMsgLMG2LDBInsertBindingRelationship*)msg);
		break;
	case MSG_LMG_2_LDB_REQ_TASK_PROGRESS:
		HandleReqTaskProgress((LMsgLMG2LDBReqTaskProgress*)msg);
		break;
	case MSG_L_2_LDB_EXEC_SQL:
		HandleExecSQL((LMsgL2LDBExecSql*)msg);
		break;
	case MSG_LMG_2_LDB_INSERT_TASK_PROGRESS:
		HandleInsertTaskProgress((LMsgLMG2LDBInsertTaskProgress*)msg);
		break;
	case MSG_LMG_2_LDB_REQ_BINDING_RELATIONSHIPS:
		HandleReqBindingRelationships((LMsgLMG2LDBReqBindingRelationships*)msg);
		break;
	case MSG_S_2_S_KEEP_ALIVE:
		HanderKeepAlive((LMsgS2SKeepAlive*)msg);
		break;
	default:
		LLOG_ERROR("MSG ID error! id:%d", msg->m_msgId);
		break;
	}
}

void Work::HanderUserKick(LMsgKick* msg)
{
	
}

void Work::HanderClientIn(LMsgIn* msg)
{
	
}

void Work::ClearOldLogVideo(Lint hour)
{
	DBSession session;
	if (!session.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
	{
		LLOG_ERROR("database session init error");
		return;
	}
	MYSQL* m = session.GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("Work::ClearOldLogVideo mysql null");
		return;
	}

	Lint nTimes = gWork.GetCurTime().Secs() - (Lint)((72 + 7.5) * 60 * 60);// 保留3天数据（7.5为零点offset);
	std::stringstream ss;
	if (hour == THREE0CLOCK)
	{
		ss << "delete from video where time < '";
		ss << nTimes << "'";

		Lstring* sql = new Lstring(ss.str());
		session.Excute(ss.str());
		LLOG_ERROR("Work::ClearOldLog, the time = %d", hour);
	}

	if (hour >= THREE0CLOCK && hour <= SIX0CLOCK)
	{
		ss.str("");
		ss << "delete from log where time < '";
		ss << nTimes << "'";

		Lstring* sql = new Lstring(ss.str());
		session.Excute(ss.str());
		LLOG_ERROR("Work::ClearOldVideo, the time = %d", hour);
	}

	LLOG_ERROR("Work::ChearOldLogVideo");
}

void Work::SendToLogic(LMsg& msg)
{
	if(m_logicManager)
	{
		m_logicManager->Send(msg.GetSendBuff());
	}
}
//logicmanager
void Work::HanderLogicManagerLogin(LMsgLMG2LdbLogin* msg)
{
	
	if(msg->m_key != gConfig.GetLogicDBKey())
	{
		LLOG_ERROR("Work::HanderLogicLogin key error %s",msg->m_key.c_str());
		return;
	}

	LLOG_ERROR("Logic Manager login");

	//直接替换
	//if(msg->m_sp == m_logicManager)
	//{
	//	return;
	//}

	m_logicManager = msg->m_sp;

}

//logic登录进来
void Work::HanderLogicLogin(LMsgL2LdbLogin* msg)
{

	LLOG_ERROR("Logic Server login");

	//if(msg->m_key != gConfig.GetLogicDBKey())
	//{
	//	LLOG_ERROR("Work::HanderLogicLogin key error %s",msg->m_key.c_str());
	//	return;
	//}
	//auto logic = m_logicServer.begin();
	//for (; logic != m_logicServer.end(); ++ logic)
	//{
	//	if (logic->second == msg->m_sp)
	//	{
	//		return;
	//	}
	//}
	//
	//m_logicServer[msg->m_serverID] = msg->m_sp;

}

//logic请求玩家战绩
void Work::HanderLogicReqVipLog(LMsgL2LBDReqVipLog* msg)
{
	//DbTool tool;
	MYSQL* m = m_dbsession.GetMysql();

	LMsgLBD2LReqVipLog log;
	log.m_userid = msg->m_userId;
	log.m_count = 0;
	if (m == NULL)
	{
		LLOG_ERROR("Work::HanderLogicReqVipLog mysql null");
		SendToLogic(log);
		return;
	}

	LLOG_DEBUG("Work::HanderLogicReqVipLog %d:%d",msg->m_userId,msg->m_reqUserId);
	//log id
	std::stringstream ss;	
	ss << "SELECT Id,Time,Pos1,Pos2,Pos3,Pos4,Flag,DeskId,MaxCircle,CurCircle,Pass,Score1,Score2,Score3,Score4,CalScore1,CalScore2,CalScore3,CalScore4,IniScore1,IniScore2,IniScore3,IniScore4,Reset,PlayType,Data FROM Log WHERE (Pos1=";
	ss << msg->m_reqUserId << " OR Pos2=";
	ss << msg->m_reqUserId << " OR Pos3=";
	ss << msg->m_reqUserId << " OR Pos4=";
	ss << msg->m_reqUserId << " ) AND ";
	ss << "Time>" << gWork.GetCurTime().Secs()-3*24*60*60; //3天数据

	ss << " ORDER BY Time DESC";

	if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("Work::HanderLogicReqVipLog sql error %s", mysql_error(m));
		SendToLogic(log);
		return ;
	}

	MYSQL_RES* res = mysql_store_result(m);
	MYSQL_ROW row = mysql_fetch_row(res);
	while (row)
	{
		LogInfo& info = log.m_info[log.m_count++];
		info.m_id = *row++;
		info.m_time = atoi(*row++);
		info.m_posUserId[0] = atoi(*row++);
		info.m_posUserId[1] = atoi(*row++);
		info.m_posUserId[2] = atoi(*row++);
		info.m_posUserId[3] = atoi(*row++);
		info.m_flag = atoi(*row++);
		info.m_deskId = atoi(*row++);
		info.m_maxCircle = atoi(*row++);
		info.m_curCircle = atoi(*row++);
		info.m_secret = *row++;
		info.m_score[0] = atof(*row++);
		info.m_score[1] = atof(*row++);
		info.m_score[2] = atof(*row++);
		info.m_score[3] = atof(*row++);
		info.m_calScore[0] = atof(*row++);
		info.m_calScore[1] = atof(*row++);
		info.m_calScore[2] = atof(*row++);
		info.m_calScore[3] = atof(*row++);
		info.m_iniScore[0] = atof(*row++);
		info.m_iniScore[1] = atof(*row++);
		info.m_iniScore[2] = atof(*row++);
		info.m_iniScore[3] = atof(*row++);
		info.m_reset = atoi(*row++);
		info.m_playtype = *row++;
		info.m_data = *row++;

		row = mysql_fetch_row(res);
		if(log.m_count >= 20)
			break;
	}

	mysql_free_result(res);
	SendToLogic(log);
}

//logic请求玩家录像
void Work::HanderLogicReqVideo(LMsgL2LDBReqVideo* msg)
{
	LMsgLDB2LReqVideo log;
	log.m_userid = msg->m_userId;
	log.m_count = 0;

	//DbTool tool;
	MYSQL* m = m_dbsession.GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("Work::HanderLogicReqVipLog mysql null");
		SendToLogic(log);
		return;
	}

	LLOG_DEBUG("Work::HanderLogicReqVideo %d:%s",msg->m_userId, msg->m_videoId.c_str());
	//log id
	std::stringstream ss;	
	ss << "SELECT Id,Time,UserId1,UserId2,UserId3,UserId4,Zhuang,DeskId,MaxCircle,CurCircle,Score1,Score2,Score3,Score4,CalScore1,CalScore2,CalScore3,CalScore4,Flag,Data, PlayType FROM Video WHERE Id='";
	ss << msg->m_videoId<<"'";

	if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("UserManager::HanderLogicReqVideo sql error %s", mysql_error(m));
		SendToLogic(log);
		return ;
	}

	MYSQL_RES* res = mysql_store_result(m);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (row)
	{
		VideoLog& info = log.m_video;
		info.m_Id = *row++;
		info.m_time = atoi(*row++);

		info.m_userId[0] = atoi(*row++);
		info.m_userId[1] = atoi(*row++);
		info.m_userId[2] = atoi(*row++);
		info.m_userId[3] = atoi(*row++);

		if (info.m_userId[2] == 0 && info.m_userId[3] == 0)
		{
			log.m_userCount = 2;
		}
		else if (info.m_userId[3] == 0)
		{
			log.m_userCount = 3;
		}

		info.m_zhuang = atoi(*row++);
		info.m_deskId = atoi(*row++);
        info.m_maxCircle = atoi(*row++);
		info.m_curCircle = atoi(*row++);

		info.m_score[0] = atof(*row++);
		info.m_score[1] = atof(*row++);
		info.m_score[2] = atof(*row++);
		info.m_score[3] = atof(*row++);
		info.m_calScore[0] = atof(*row++);
		info.m_calScore[1] = atof(*row++);
		info.m_calScore[2] = atof(*row++);
		info.m_calScore[3] = atof(*row++);

		info.m_flag = atoi(*row++);

		info.m_str = *row++;
		info.m_str_playtype = *row++;

		log.m_count = 1;
	}

	mysql_free_result(res);
	SendToLogic(log);
}

void Work::HandleLogicReqActive(LMsgL2LDBReqActive* msg)
{
	if (msg == NULL)
	{
		return;
	}
	if (msg->m_userId > 0 && msg->m_activeId > 0)
	{
		//检测玩家是否有抽卡记录
		//DbTool tool;
		MYSQL* m = m_dbsession.GetMysql();
		if (m == NULL)
		{
			return;
		}

		std::stringstream ss;	
		ss << "select activeID, reward, date, phone from activelog where userID = "
		<< msg->m_userId
		<< " and activeID in ("
		<< msg->m_activeId
		<< ","
		<< DAILY_SHARE_CARD << ") order by date desc ";

		if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
		{
			LLOG_ERROR("UserManager::HandleLogicReqActive sql error %s", mysql_error(m));
			return ;
		}
		Lint nCount = 0;
		LMsgLDB2LActiveInfo activeinfo;
		MYSQL_RES* res = mysql_store_result(m);
		MYSQL_ROW row = mysql_fetch_row(res);
		while (row)
		{
			activeinfo.m_DrawInfo[nCount].m_activeID = atoi(*row++);
			activeinfo.m_DrawInfo[nCount].m_rewardID = atoi(*row++);
			activeinfo.m_DrawInfo[nCount].m_date   = atoi(*row++);
			activeinfo.m_DrawInfo[nCount].m_phoneNum	= *row++;
			if (activeinfo.m_DrawInfo[nCount].m_rewardID <= 0 || activeinfo.m_DrawInfo[nCount].m_date <= 0)
			{
				break;
			}
			nCount ++;
			if (nCount >= 10)
				break;
			row = mysql_fetch_row(res);
		}
		mysql_free_result(res);

		activeinfo.m_userid = msg->m_userId;
		activeinfo.m_count = nCount;
		gWork.SendToLogic(activeinfo);
	}	
}

void Work::HandleLogicSaveSyncDraw(LMsgL2LDBSyncDraw* msg)
{
	if (msg == NULL)
	{
		return;
	}
	if (msg->m_userId > 0 )
	{
		std::stringstream ss;	
		ss << "insert into activelog(activeID, userID, reward, phone, date) values(";
		ss << msg->m_activeID << ",";
		ss << msg->m_userId << ",";
		ss << msg->m_giftID << ",";
		ss <<"''" << ",";
		ss << msg->m_date << ")";

		m_dbsession.Excute(ss.str());
	}

}

void Work::HandleLogicUpdatePhone(LMsgL2LDBSyncPhone* msg)
{
	if (msg == NULL)
	{
		return;
	}
	if (msg->m_activeID > 0 )
	{
		std::stringstream ss;
		ss << "update activelog set phone = '";
		ss << msg->m_phone << "' where id = ";
		ss << msg->m_activeID;

		m_dbsession.Excute(ss.str());
	}
}

//logic请求保存战绩;
void Work::HanderLogicReqSaveLog(LMsgL2LDBSaveLogItem* msg)
{
	LLOG_DEBUG("Work::HanderLogincSaveVipLog save %d:%s",msg->m_type,msg->m_sql.c_str());

	m_dbsession.Excute(msg->m_sql);
}

//logic请求保存录像;
void Work::HanderLogicReqSaveVideo(LMsgL2LDBSaveVideo* msg)
{
	LLOG_DEBUG("Work::HanderLogincSaveVideo save %d:%s", msg->m_type, msg->m_sql.c_str());

	m_dbsession.Excute(msg->m_sql);
}


void Work::HanderLogicReqShareVideo(LMsgL2LDBGetShareVideo* msg)
{
	if(msg==NULL)
	{
		return;
	}
	LMsgLDB2LRetShareVideo log;
	log.m_userid = msg->m_userId;

	//DbTool tool;
	MYSQL* m = m_dbsession.GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("Work::HanderLogicReqVipLog mysql null");
		SendToLogic(log);
		return;
	}

	//log id
	std::stringstream ss;	
	ss << "SELECT VideoId,Time,UserId1,UserId2,UserId3,UserId4,Score1,Score2,Score3,Score4 FROM sharevideo WHERE ShareID=";
	ss << msg->m_shareId;

	if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("UserManager::HanderLogicReqShareVideo sql error %s", mysql_error(m));
		SendToLogic(log);
		return ;
	}

	MYSQL_RES* res = mysql_store_result(m);
	MYSQL_ROW row = mysql_fetch_row(res);
	while (row)
	{
		ShareVideoInfo info;
		info.m_videoId = *row++;
		info.m_time = atoi(*row++);

		info.m_userId[0] = atoi(*row++);
		info.m_userId[1] = atoi(*row++);
		info.m_userId[2] = atoi(*row++);
		info.m_userId[3] = atoi(*row++);

		info.m_score[0] = atof(*row++);
		info.m_score[1] = atof(*row++);
		info.m_score[2] = atof(*row++);
		info.m_score[3] = atof(*row++);

		log.m_count++;
		log.m_info.push_back(info);
		row = mysql_fetch_row(res);
	}

	mysql_free_result(res);
	SendToLogic(log);
}

void Work::HanderLogicGetShareVideoId(LMsgL2LDBGGetShareVideoId* msg)
{
	if(msg==NULL)
	{
		return;
	}
	LMsgLDB2LRetShareVideoId sharevideo_id;
	sharevideo_id.m_userId = msg->m_userId;
	sharevideo_id.m_videoId = msg->m_videoId;
	//加载所有的玩家;

	//DbTool tool;
	MYSQL* m = m_dbsession.GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("Work::HanderLogicReqVipLog mysql null");
		SendToLogic(sharevideo_id);
		return;
	}

	//log id
	std::stringstream ss;	
	ss << "SELECT ShareId FROM sharevideo WHERE VideoId='";
	ss << msg->m_videoId<<"'";

	auto ssStr = ss.str();
	if (mysql_real_query(m, ssStr.c_str(), ssStr.size()))
	{
		LLOG_ERROR("UserManager::HanderLogicGetShareVideoId _ 1 _ sql error %s", mysql_error(m));
		SendToLogic(sharevideo_id);
		return ;
	}

	MYSQL_RES* res = mysql_store_result(m);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (row)
	{
		sharevideo_id.m_shareId = atoi(*row++);
	}
	mysql_free_result(res);

	if(sharevideo_id.m_shareId==0)
	{
		std::stringstream ss;	
		ss << "SELECT UserId1,UserId2,UserId3,UserId4,Score1,Score2,Score3,Score4,Time,Zhuang,Data,PlayType,DeskId,Flag,CurCircle,MaxCircle FROM video WHERE Id='";
		ss << msg->m_videoId<<"'";

		if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
		{
			LLOG_ERROR("UserManager::HanderLogicGetShareVideoId _ 2 _ sql error %s", mysql_error(m));
			SendToLogic(sharevideo_id);
			return ;
		}

		MYSQL_RES* res = mysql_store_result(m);
		MYSQL_ROW row = mysql_fetch_row(res);
		Lint userId[4];
		Lint Score[4];
		Lint time;
		Lint zhuang;
		Lstring data;
		Lstring playtype;
		Lint deskid;
		Lint flag;
		Lint curcircle;
		Lint maxcircle;

		if (row)
		{
			userId[0]= atoi(*row++);
			userId[1]= atoi(*row++);
			userId[2]= atoi(*row++);
			userId[3]= atoi(*row++);
			Score[0] = atof(*row++);
			Score[1] = atof(*row++);
			Score[2] = atof(*row++);
			Score[3] = atof(*row++);
			time = atoi(*row++);
			zhuang = atoi(*row++);
			data = *row++;
			playtype = *row++;
			deskid = atoi(*row++);
			flag = atoi(*row++);
			curcircle = atoi(*row++);
			maxcircle = atoi(*row++);

			std::stringstream ss1;	
			ss1 << "insert into sharevideo(VideoId,time,UserId1,UserId2,UserId3,UserId4,Score1,Score2,Score3,Score4,Zhuang,Data,PlayType,DeskId,Flag,MaxCircle,CurCircle) values('";
			ss1 << msg->m_videoId.c_str() <<"',"<<time<<","<<userId[0]<<","<<userId[1]<<","<<userId[2]<<","<<userId[3]<<","<<Score[0]<<","<<Score[1];
			ss1 << ","<<Score[2]<<","<<Score[3] << ","<<zhuang <<",'"<< data.c_str() <<"','" << playtype.c_str() <<"'," << deskid << "," << flag << "," << maxcircle <<"," << curcircle <<")";


			m_dbsession.Excute(ss1.str());
		}
		else{
			sharevideo_id.m_shareId = -1;
		}

		mysql_free_result(res);

	}
	SendToLogic(sharevideo_id);
}

void Work::HanderLogicSaveShareVideoId(LMsgL2LDBSaveShareVideoId* msg)
{
	if (msg == NULL)
	{
		return;
	}
	if (msg->m_videoId != "" )
	{
		//log id
		std::stringstream ss;
		ss << "update sharevideo set ShareId = "<< msg->m_shareId <<" where Videoid = '"<< msg->m_videoId << "' ";

		m_dbsession.Excute(ss.str());
	}
}

void Work::HanderLogicGetShareVideo(LMsgL2LDBGetSsharedVideo* msg)
{
	LMsgLDB2LReqVideo log;
	log.m_userid = msg->m_userId;
	log.m_count = 0;

	//DbTool tool;
	MYSQL* m = m_dbsession.GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("Work::HanderLogicReqVipLog mysql null");
		SendToLogic(log);
		return;
	}

	//log id
	std::stringstream ss;	
	ss << "SELECT VideoId,Time,UserId1,UserId2,UserId3,UserId4,Zhuang,DeskId,MaxCircle,CurCircle,Score1,Score2,Score3,Score4,CalScore1,CalScore2,CalScore3,CalScore4,Flag,Data,PlayType FROM sharevideo WHERE VideoId='";
	ss << msg->m_videoId<<"'";

	if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("UserManager::HanderLogicGetShareVideo sql error %s", mysql_error(m));
		SendToLogic(log);
		return ;
	}

	MYSQL_RES* res = mysql_store_result(m);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (row)
	{
		VideoLog& info = log.m_video;
		info.m_Id = *row++;
		info.m_time = atoi(*row++);

		info.m_userId[0] = atoi(*row++);
		info.m_userId[1] = atoi(*row++);
		info.m_userId[2] = atoi(*row++);
		info.m_userId[3] = atoi(*row++);

		if (info.m_userId[3] == 0 && info.m_userId[2] == 0)
		{
			log.m_userCount = 2;
		}
		else if (info.m_userId[3] == 0)
		{
			log.m_userCount = 3;
		}

		info.m_zhuang = atoi(*row++);
		info.m_deskId = atoi(*row++);
		info.m_curCircle = atoi(*row++);
		info.m_maxCircle = atoi(*row++);

		info.m_score[0] = atof(*row++);
		info.m_score[1] = atof(*row++);
		info.m_score[2] = atof(*row++);
		info.m_score[3] = atof(*row++);
		info.m_calScore[0] = atof(*row++);
		info.m_calScore[1] = atof(*row++);
		info.m_calScore[2] = atof(*row++);
		info.m_calScore[3] = atof(*row++);

		info.m_flag = atoi(*row++);

		info.m_str = *row++;

		info.m_str_playtype = *row++;
		log.m_count = 1;
	}

	mysql_free_result(res);
	SendToLogic(log);
}

void Work::HandleLMGHeartBeat(LMsgLMG2LDBHEARTBEAT* msg)
{
	LLOG_DEBUG("HeartBeat: LMG_2_LDB");
	if (msg == NULL)
	{
		return;
	}
	LMsgLDB2LMGHEARTBEAT_BACK back;
	SendToLogic(back);
}

void Work::HandleLHeartBeat(LMsgL2LDBHEARTBEAT* msg)
{
	LLOG_DEBUG("HeartBeat: L_2_LDB");
	if (msg == NULL)
	{
		return;
	}
	LMsgLDB2LHEARTBEAT_BACK back;
	msg->m_sp->Send(back.GetSendBuff());
}

void Work::HanderLogicReqOneVipLog(LMsgL2LDBReqOneVipLog* msg)
{
	LMsgLBD2LReqOneVipLog log;
	log.m_userid = msg->m_reqUserId;

	//DbTool tool;
	MYSQL* m = m_dbsession.GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("Work::HanderLogic ReqOneVipLog mysql null");
		SendToLogic(log);
		return;
	}

	//log id
	std::stringstream ss;	
	ss << "SELECT Data,Flag, PlayType FROM Log where Id='";
	ss << msg->m_log_Id.c_str()<<"'";

	if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("UserManager::HanderLogicGetShareVideo sql error %s", mysql_error(m));
		SendToLogic(log);
		return;
	}

	MYSQL_RES* res = mysql_store_result(m);
	MYSQL_ROW row = mysql_fetch_row(res);
	Lstring playtype;
	if(row)
	{
		log.m_data = *row++;
		log.m_flag = atoi(*row++);
		playtype = *row++;
		log.m_size++;
	}

	std::vector<Lstring> des;
	L_ParseString(playtype, des, ";");
	if (des.size() > 0)
	{
		for (size_t j = 0; j < des.size(); ++j)
		{
			std::vector<Lstring> des_1;
			L_ParseString(des[j], des_1, ",");
			if (des_1.size() < 2)
			{
				continue;
			}
			log.m_playType.push_back(atoi(des_1[0].c_str()));
			log.m_playType.push_back(atoi(des_1[1].c_str()));
		}
	}
	mysql_free_result(res);
	SendToLogic(log);
}

//十一活动
void Work::HandleLogicReqSignCollectWin(LMsgL2LDBReqSignCollectWin* msg)
{
	if (msg == NULL)
	{
		return;
	}

	Lint type = msg->m_activeType;
	Lint userId = msg->m_userId;

	if(type == 0)
	{
		ResLoadActiveSign(userId);
	}
	else if(type == 1)
	{
		ResLoadActiveCollect(userId);
	}
	else if(type == 2)
	{
		ResLoadActiveWin(userId);
	}
		
}

void Work::ResLoadActiveSign(Lint userId)
{
	if (userId > 0)
	{
		Lint activeType = 0; //ESign

		//检测玩家是否有抽卡记录
		//DbTool tool;
		MYSQL* m = m_dbsession.GetMysql();
		if (m == NULL)
		{
			return;
		}

		std::stringstream ss;	
		ss << "select date from activelog1 where userID = ";
		ss << userId;
		ss << " and activeType = ";
		ss << activeType;
		ss <<" order by date desc ";

		Lstring queryStr = move(ss.str());

		if (mysql_real_query(m, queryStr.c_str(), queryStr.size()))
		{
			LLOG_ERROR("UserManager::HandleLogicReqActive sql error %s", mysql_error(m));
			return ;
		}
		Lint nCount = 0;
		LMsgLDB2LResSignCollectWin activeinfo;
		MYSQL_RES* res = mysql_store_result(m);
		MYSQL_ROW row = mysql_fetch_row(res);
		while (row)
		{
			activeinfo.m_DrawInfo[nCount].m_date   = atoi(*row++);
			nCount ++;
			if (nCount >= 10)
				break;
			row = mysql_fetch_row(res);
		}
		mysql_free_result(res);

		activeinfo.m_userid = userId;
		activeinfo.m_count = nCount;
		activeinfo.m_type = activeType;
		gWork.SendToLogic(activeinfo);
	}	
}

void split(std::string& s, const std::string& delim,std::vector< std::string >* ret)  
{  
	size_t last = 0;  
	size_t index=s.find_first_of(delim,last);  
	while (index!=std::string::npos)  
	{  
		ret->push_back(s.substr(last,index-last));  
		last=index+1;  
		index=s.find_first_of(delim,last);  
	}  
	if (index != last)  
	{  
		ret->push_back(s.substr(last,index-last));  
	}  
}  

void Work::ResLoadActiveCollect(Lint userId)
{
	if (userId > 0)
	{
		Lint activeType = 1;
		//检测玩家是否有抽卡记录
		//DbTool tool;
		MYSQL* m = m_dbsession.GetMysql();
		if (m == NULL)
		{
			return;
		}

		std::stringstream ss;	
		ss << "select id, drawChanceCount, date, word1,word2,word3,word4,word5,word6,word7,word8, hadGift from activelog1 where userID = ";
		ss << userId;
		ss << " and activeType = ";
		ss << activeType;
		ss <<" order by date desc ";

		Lstring queryStr = move(ss.str());

		if (mysql_real_query(m, queryStr.c_str(), queryStr.size()))
		{
			LLOG_ERROR("UserManager::HandleLogicReqActive sql error %s", mysql_error(m));
			return ;
		}
		Lint nCount = 0;
		LMsgLDB2LResSignCollectWin activeinfo;
		MYSQL_RES* res = mysql_store_result(m);
		MYSQL_ROW row = mysql_fetch_row(res);
		while (row)
		{
			activeinfo.m_DrawInfo[nCount].m_isUpdate = (atoi(*row++) != 0);
			activeinfo.m_DrawInfo[nCount].m_drawChanceCount = atoi(*row++);
			activeinfo.m_DrawInfo[nCount].m_date   = atoi(*row++);
			Lint word1 = atoi(*row++);
			Lint word2 = atoi(*row++);
			Lint word3 = atoi(*row++);
			Lint word4 = atoi(*row++);
			Lint word5 = atoi(*row++);
			Lint word6 = atoi(*row++);
			Lint word7 = atoi(*row++);
			Lint word8 = atoi(*row++);

			if(word1 != 0)
				activeinfo.m_DrawInfo[nCount].m_words.push_back(word1);
			if(word2 != 0)
				activeinfo.m_DrawInfo[nCount].m_words.push_back(word2);
			if(word3 != 0)
				activeinfo.m_DrawInfo[nCount].m_words.push_back(word3);
			if(word4 != 0)
				activeinfo.m_DrawInfo[nCount].m_words.push_back(word4);
			if(word5 != 0)
				activeinfo.m_DrawInfo[nCount].m_words.push_back(word5);
			if(word6 != 0)	
				activeinfo.m_DrawInfo[nCount].m_words.push_back(word6);
			if(word7 != 0)
				activeinfo.m_DrawInfo[nCount].m_words.push_back(word7);
			if(word8 != 0)
				activeinfo.m_DrawInfo[nCount].m_words.push_back(word8);

			activeinfo.m_DrawInfo[nCount].m_hadGift = atoi(*row++);
			 
			nCount ++;
			if (nCount >= 10)
				break;
			row = mysql_fetch_row(res);
		}
		mysql_free_result(res);

		activeinfo.m_userid = userId;
		activeinfo.m_count = nCount;
		activeinfo.m_type = activeType;
		gWork.SendToLogic(activeinfo);
	}	
}

void Work::ResLoadActiveWin(Lint userId)
{
	if (userId > 0)
	{
		Lint activeType = 2;
		//检测玩家是否有抽卡记录
		//DbTool tool;
		MYSQL* m = m_dbsession.GetMysql();
		if (m == NULL)
		{
			return;
		}

		std::stringstream ss;	
		ss << "select name, date from activelog1Win where ";
		ss << "activeType = ";
		ss << activeType;
		ss <<" order by date desc ";

		Lstring queryStr = move(ss.str());

		if (mysql_real_query(m, queryStr.c_str(), queryStr.size()))
		{
			LLOG_ERROR("UserManager::HandleLogicReqActive sql error %s", mysql_error(m));
			return ;
		}
		Lint nCount = 0;
		LMsgLDB2LResSignCollectWin activeinfo;
		MYSQL_RES* res = mysql_store_result(m);
		MYSQL_ROW row = mysql_fetch_row(res);
		while (row)
		{
			activeinfo.m_DrawInfo[nCount].m_nickName = (*row++);
			activeinfo.m_DrawInfo[nCount].m_date = atoi(*row++);

			nCount ++;
			if (nCount >= 100)
				break;
			row = mysql_fetch_row(res);
		}
		mysql_free_result(res);

		activeinfo.m_userid = userId;
		activeinfo.m_count = nCount;
		activeinfo.m_type = activeType;
		gWork.SendToLogic(activeinfo);
	}	
}

void Work::HandleLogicSaveSignCollectWin(LMsgL2LDBSaveSignCollectWin * msg)
{
	if (msg == NULL)
	{
		return;
	}

	Lint type = msg->m_activeType;
	Lint userId = msg->m_userId;
	Lint date = msg->m_date;
	Lint isUpdate = msg->m_isUpdate;
	Lint drawChanceCount = msg->m_drawChanceCount;

	std::vector<Lint> words;
	auto itor = msg->m_words.begin();
	for(; itor != msg->m_words.end(); ++itor)
	{
		if(*itor != 0)
		{
			words.push_back(*itor);
		}
	}
	words.resize(8);

	Lstring name = msg->m_name;
	Lint hadGift = msg->m_hadGift;


	if(type == 0)	//ESign
	{
		SaveSign(userId, date);
	}
	else if(type == 1) //ECollect
	{
		

		SaveCollect(isUpdate, userId, date, words, drawChanceCount, hadGift);
	}
	else if(type == 2) //EWin
	{
		SaveWin(userId, date, name);
	}
}

void Work::SaveSign(Lint userid, Lint date)
{
	Lint activeType = 0;	//Esign

	if (userid > 0 )
	{
		std::stringstream ss;	
		ss << "insert into activelog1(activeType, userid, date) values(";
		ss << activeType << ",";
		ss << userid << ",";
		ss << date << ")";

		m_dbsession.Excute(ss.str());
	}
}



void Work::SaveCollect(Lint isUpdate, Lint userid, Lint date, const std::vector<Lint> &words, Lint drawChanceCount, Lint hadGift)
{
	Lint activeType = 1;	//Esign

	if (userid > 0)
	{
		if(isUpdate > 0)
		{
			std::stringstream ss;	
			ss << "update activelog1 set ";
			ss << "drawChanceCount = ";
			ss << drawChanceCount << ",";
			ss << "word1 = ";
			ss << words[0] << ",";
			ss << "word2 = ";
			ss << words[1] << ",";
			ss << "word3 = ";
			ss << words[2] << ",";
			ss << "word4 = ";
			ss << words[3] << ",";
			ss << "word5 = ";
			ss << words[4] << ",";
			ss << "word6 = ";
			ss << words[5] << ",";
			ss << "word7 = ";
			ss << words[6] << ",";
			ss << "word8 = ";
			ss << words[7] << ",";
			ss << "hadGift = ";
			ss << hadGift << ",";
			ss << "date = ";
			ss << date << " ";
			ss << "WHERE userID ='" << userid << "'";
			ss << "and activeType = " << activeType << " ";

			Lstring queryStr = move(ss.str());

			m_dbsession.Excute(queryStr);
		}
		else
		{
			std::stringstream ss;	
			ss << "insert into activelog1(activeType, userID, drawChanceCount, word1,word2,word3,word4,word5,word6,word7,word8, date, hadGift) values(";
			ss << activeType << ",";
			ss << userid << ",";
			ss << drawChanceCount << ",";
			ss << words[0] <<",";
			ss << words[1] <<",";
			ss << words[2] <<",";
			ss << words[3] <<",";
			ss << words[4] <<",";
			ss << words[5] <<",";
			ss << words[6] <<",";
			ss << words[7] <<",";
			ss << date << ",";
			ss << hadGift << ")";

			Lstring queryStr = move(ss.str());
			m_dbsession.Excute(queryStr);
		}
	}
}

void Work::SaveWin(Lint userId, Lint date, const Lstring& name)
{

	Lint activeType = 2;

	if ( userId > 0 )
	{
		std::stringstream ss;	
		ss << "insert into activelog1Win(activeType, userID, name, date) values(";
		ss << activeType << ",";
		ss << userId << ",";
		ss <<"'"<< name <<"'" <<",";
		ss << date << ")";

		Lstring queryStr = move(ss.str()); 
		m_dbsession.Excute(queryStr);
	}
}

void Work::HandleInsertBindingRelationship(LMsgLMG2LDBInsertBindingRelationship* msg)
{
	if(msg == NULL)
		return;

	LMsgLDB2LMGInsertBindingRelationship ret;
	ret.m_userId = msg->m_userId;
	ret.m_inviter = msg->m_inviter;
	ret.m_errorCode = ErrorCode::Unknown;

	MYSQL* m = m_dbsession.GetMysql();
	if (m != NULL)
	{
		std::stringstream ss;
		ss << "INSERT INTO bindingrelationships(Invitee, Inviter, BindingTime, TaskFinished) select ";
		ss << msg->m_userId << ",";
		ss << msg->m_inviter << ",";
		ss << gWork.GetCurTime().Secs() << ", 0 ";
		ss << "FROM DUAL WHERE (SELECT COUNT(*) FROM bindingrelationships WHERE inviter = ";
		ss << msg->m_inviter << ") < " << msg->m_maxInviteeNum;

		if (!mysql_real_query(m, ss.str().c_str(), ss.str().size()))
		{
			Lint affectedRows = (Lint)mysql_affected_rows(m);
			ret.m_errorCode = affectedRows == 1 ? ErrorCode::ErrorNone : ErrorCode::ExceedMaximum;
		}
		else
		{
			ret.m_errorCode = ErrorCode::ExceedMaximum;
			LLOG_ERROR("Work::HandleInsertBindingRelationship:failed to execute sql %s", ss.str().c_str());
		}
	}
	else
		LLOG_ERROR("Work::HandleReqLuckyDrawNum mysql null");

	SendToClient(msg->m_sp, ret);
}

void Work::SendToClient(LSocketPtr client, LMsg & msg)
{
	if (client)
		client->Send(msg.GetSendBuff());
}

void Work::HandleReqTaskProgress(LMsgLMG2LDBReqTaskProgress * msg)
{
	if (msg == NULL)
		return;

	LMsgLDB2LMGTaskProgress ret;
	ret.m_userId = msg->m_userId;
	ret.m_taskId = msg->m_taskId;
	ret.m_result = 0;
	MYSQL* m = m_dbsession.GetMysql();
	if (m != NULL)
	{
		if (!mysql_real_query(m, msg->m_sql.c_str(), msg->m_sql.size()))
		{
			MYSQL_RES* res = mysql_store_result(m);
			if (res)
			{
				MYSQL_ROW row = mysql_fetch_row(res);

				if (row)
				{
					ret.m_progressId = atoi(*row++);
					ret.m_progress = atoi(*row++);
					ret.m_rewardRecved = atoi(*row++);
					ret.m_result = 1;
				}
				mysql_free_result(res);
			}
		}
		else
			LLOG_ERROR("Work::HandleReqTaskProgress error = %s,sql = %s ", mysql_error(m), msg->m_sql.c_str());
	}
	else
		LLOG_ERROR("Work::HandleReqTaskProgress mysql null");

	SendToClient(msg->m_sp, ret);
}

void Work::HandleExecSQL(LMsgL2LDBExecSql * msg)
{
	if (msg == NULL)
	{
		return;
	}
	LLOG_DEBUG("Work::HandleExecSQL sql=%s", msg->m_sql.c_str());
	m_dbsession.Excute(msg->m_sql);
}

void Work::HandleInsertTaskProgress(LMsgLMG2LDBInsertTaskProgress * msg)
{
	if (msg == NULL)
		return;
	LMsgLDB2LMGInsertTaskProgress ret;
	ret.m_userId = msg->m_userId;
	ret.m_taskId = msg->m_taskId;
	ret.m_progressId = -1;
	MYSQL* m = m_dbsession.GetMysql();
	if (m != NULL)
	{
		if (!mysql_real_query(m, msg->m_sql.c_str(), msg->m_sql.size()))
		{
			std::stringstream ss;
			ss << "SELECT LAST_INSERT_ID()";
			if (!mysql_real_query(m, ss.str().c_str(), ss.str().size()))
			{
				MYSQL_RES* res = mysql_store_result(m);
				MYSQL_ROW row = mysql_fetch_row(res);

				if (row)
					ret.m_progressId = atoi(*row++);
				mysql_free_result(res);
			}
			else
				LLOG_ERROR("Work::HandleInsertTaskProgress:Excute error = %s,sql = %s ", mysql_error(m), ss.str().c_str());
		}
		else
			LLOG_ERROR("Work::HandleInsertTaskProgress:Excute error = %s,sql = %s ", mysql_error(m), msg->m_sql.c_str());
	}
	else
		LLOG_ERROR("Work::HandleReqTaskProgress mysql null");

	SendToClient(msg->m_sp, ret);
}

void Work::HandleReqBindingRelationships(LMsgLMG2LDBReqBindingRelationships * msg)
{
	LLOG_DEBUG("Work::HandleReqBindingRelationships");
	if (msg == NULL)
		return;

	LMsgLDB2LMGBindingRelationships ret;
	ret.m_userId = msg->m_userId;
	ret.m_inviterId = -1;
	ret.m_finished = 0;
	ret.m_inviteeCount = 0;

	MYSQL* m = m_dbsession.GetMysql();
	if (m != NULL)
	{
		// read inviter
		std::stringstream ss;
		ss << "SELECT Inviter, TaskFinished FROM bindingrelationships WHERE Invitee='" << msg->m_userId << "' limit 1";
		auto ssStr = ss.str();
		if (!mysql_real_query(m, ssStr.c_str(), ssStr.size()))
		{
			MYSQL_RES* res = mysql_store_result(m);

			if (res)
			{
				MYSQL_ROW row = mysql_fetch_row(res);
				if (row)
				{
					ret.m_inviterId = atoi(*row++);
					ret.m_finished = atoi(*row++);
				}
				mysql_free_result(res);
			}

			// read invitee
			ss.str("");
			ss << "SELECT Invitee, TaskFinished FROM bindingrelationships WHERE Inviter='" << msg->m_userId << "' limit 20";

			if (!mysql_real_query(m, ss.str().c_str(), ss.str().size()))
			{
				MYSQL_RES* res = mysql_store_result(m);
				if (res)
				{
					MYSQL_ROW row = mysql_fetch_row(res);

					while (row)
					{
						ret.m_invitees[ret.m_inviteeCount] = atoi(*row++);
						ret.m_taskFinished[ret.m_inviteeCount] = atoi(*row++);
						ret.m_inviteeCount++;
						row = mysql_fetch_row(res);
					}
					mysql_free_result(res);
				}
			}
		}
	}
	else
		LLOG_ERROR("Work::HandleReqTaskProgress mysql null");

	SendToClient(msg->m_sp, ret);
}

void Work::HanderKeepAlive(LMsgS2SKeepAlive* msg)
{
	if (msg == NULL || !msg->m_sp)
		return;
	//LLOG_DEBUG("KeepAlive from %s:%d received.", msg->m_sp->GetRemoteIp().c_str(), msg->m_sp->GetRemotePort());

	LMsgS2SKeepAliveAck ack;
	msg->m_sp->Send(ack.GetSendBuff());
}

bool Work::CheckDelLogVideoTime(Lint hour, Lint min)
{
	if (hour >= THREE0CLOCK && hour <= SIX0CLOCK && min >= 0 && min < 5)
	{
		return true;
	}
	return false;
}
