#include "HanderHttp.h"
#include "HttpRequest.h"
#include "TeamErrorCode.h"
#include "TeamManager.h"
#include "DbServer.h"
#include "Work.h"
#include "DbServer.h"
#include <boost/regex.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "Config.h"
#include "GateUserManager.h"
#include "BaseCommonFunc.h"


CHanderHttp::CHanderHttp()
{
}


CHanderHttp::~CHanderHttp()
{
}

bool CHanderHttp::Init()
{
	return true;
}

bool CHanderHttp::Final()
{
	return true;
}

void CHanderHttp::SendRet(const Lstring& errorInfo, LSocketPtr send)
{
	Lint nTotalBufferLen = errorInfo.length() + 512;
	char* pBuffer = new char[nTotalBufferLen];

	std::string strSendBuf;
	strSendBuf.reserve(1024);
	strSendBuf += "HTTP/1.1 200 OK\r\n";

	sprintf_s(pBuffer, nTotalBufferLen -1 , "Content-Length:%u\r\n", errorInfo.size());
	strSendBuf += pBuffer;
	strSendBuf += "Content-Type: text/html; charset=utf-8\r\n\r\n";

	memset(pBuffer, 0, nTotalBufferLen);
	sprintf_s(pBuffer, nTotalBufferLen - 1, "%s", errorInfo.c_str());
	strSendBuf += pBuffer;

	LBuffPtr buff(new LBuff());
	buff->Write(strSendBuf.c_str(), strSendBuf.size());
	send->Send(buff);

	delete [] pBuffer;
}

void CHanderHttp::SendHttpRet(Lint nErrorCode, LSocketPtr send)
{
	std::stringstream errorInfo;
	errorInfo << "{\"errorCode\":" << nErrorCode << ",\"errorMsg\":\"\"}";
	SendRet(errorInfo.str(), send);
}

void CHanderHttp::HanderHttpCreatTeam(HttpParamMap& param, LSocketPtr sp)
{
	if (param.empty() || NULL == sp)
	{
		LLOG_DEBUG("Club------Begin Create club, param empty!");
		return;
	}

	if (param.find("createrId") == param.end())
	{
		LLOG_DEBUG("Club------Begin Create club, createrId param error!");
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (param.find("teamId") == param.end())
	{
		LLOG_DEBUG("Club------Begin Create club, teamId param error!");
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	/*if (param.find("teamName") == param.end())
	{
		LLOG_DEBUG("Club------Begin Create club, teamName param error!");
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}
   */
	/*if (param.find("playTypes") == param.end())
	{
		LLOG_DEBUG("Club------Begin Create club, cards param error!");
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}*/
	
	if (param.find("deskCntLimit") == param.end())
	{
		LLOG_DEBUG("Club------Begin Create club, cards adminId error!");
		SendHttpRet(EHEEC_NOT_HAS_DESK_CNT_LIMIT, sp);
		return;
	}

	Lint nCreaterID = atoi(param["createrId"].c_str());
	Lint nTeamId = atoi(param["teamId"].c_str());
	Lstring strTeamName;
	gTeamManager.GetCreateHeadImageURL(nCreaterID, strTeamName);
	//需要自己的名字，添加一个随机数
	int nSuiJiID = gTeamManager.GetRandInsertIDFromDB();
	if (nSuiJiID <= 0)
	{
		LLOG_DEBUG("Club------Begin Create club, not find suijiid!");
		SendHttpRet(EHEEC_TEAM_NAME, sp);
		return;
	}
	strTeamName += nSuiJiID;

	Lstring playTypes;/*param["playTypes"];*/
	Lint manageId = 0;
	Lint countyId = 0;
	if (param.find("manageId") != param.end())
	{
		manageId = atoi(param["manageId"].c_str());
	}
	if (param.find("countyId") != param.end())
	{
		countyId = atoi(param["countyId"].c_str());
	}
	Lint deskCntLimit = atoi(param["deskCntLimit"].c_str());

	Lint deskProportionate = 1;
	if (param.find("deskProportionate") != param.end())
	{
		deskProportionate = atoi(param["deskProportionate"].c_str());
	}

	LLOG_ERROR("Club---Create nCreaterID %d strTeamName %s playTypes %s manageId %d countyId %d deskCntLimit %d deskProportionate %d", nCreaterID, strTeamName.c_str(), playTypes.c_str(), manageId, countyId, deskCntLimit, deskProportionate);
	if (nCreaterID <= 0 /*|| playTypes.length() <= 0*/ || nTeamId <= 0)
	{
		LLOG_ERROR("Club------Begin Create club, TeamName.length error! nCreaterID %d playTypes %s nTeamId %d", nCreaterID, playTypes.c_str(), nTeamId);
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (strTeamName.length() <= 0 || strTeamName.length() > 30 || deskCntLimit == 0 /*|| playTypes.length() <= 0*/)
	{
		LLOG_ERROR("Club------Begin Create club, TeamName.length error!");
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	std::vector<Lint> vPlayTypes;
	/*if (!tokenize_atovi(playTypes, vPlayTypes, ";") && vPlayTypes.size() == 0)
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}*/

	int nRet = gTeamManager.CreateTeam(nCreaterID, nTeamId, strTeamName, vPlayTypes, playTypes, manageId, countyId, deskCntLimit, deskProportionate);
	if (0 >= nRet)
	{
		LLOG_ERROR("Work::HanderHttpCreatTeam error");
		SendHttpRet(-nRet, sp);
	}
	else
	{
		/*std::stringstream errorInfo;
		errorInfo << "{\"errorCode\":" << EHEEC_SUCCESS << ",\"errorMsg\":\"" << nRet << "\"}";*/
		/*boost::property_tree::ptree pt_root;
		pt_root.put("errorMsg", nRet);
		pt_root.put("errorCode", EHEEC_SUCCESS);
		pt_root.put("datas", nRet);

		std::stringstream strStream;
		boost::property_tree::write_json(strStream, pt_root);
		SendRet(strStream.str(), sp);*/

		std::stringstream strStream;
		strStream << "{\"errorCode\":" << EHEEC_SUCCESS << ",\"errorMsg\":\""<< nRet <<"\"}";
		SendRet(strStream.str(), sp);

		LLOG_ERROR("Club------Create club success!");
	}
}


// 给指定的工会修改信息 [2017-7-17 willing]
void CHanderHttp::HanderHttpModifyTeam(HttpParamMap& param, LSocketPtr sp)
{
	if (param.empty() || NULL == sp)
	{
		return;
	}

	if (param.find("type") == param.end())
	{
		LLOG_DEBUG("Club------modify club, type param error!");
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (param.find("createrId") == param.end())
	{
		LLOG_DEBUG("Club------modify club, createrId param error!");
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (param.find("teamId") == param.end())
	{
		LLOG_DEBUG("Club------modify club, teamId param error!");
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	int createrId = atoi(param["createrId"].c_str());
	if (createrId <= 0)
	{
		LLOG_DEBUG("Club------modify club, createrId param error!=%d", createrId);
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	int teamId = atoi(param["teamId"].c_str());
	if (teamId <= 0)
	{
		LLOG_DEBUG("Club------modify club, teamId param error!=%d", teamId);
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	Lint type = atoi(param["type"].c_str());
	switch (type)
	{
	case ETMOT_all:
	{
		ProcessModifyTeam_all_(createrId, teamId, param, sp);
		break;
	}
	case ETMOT_playTypes:
	{
		ProcessModifyTeam_playTypes_(createrId, teamId, param, sp);
		break;
	}
	case ETMOT_deskCntLimit:
	{
		ProcesskModifyTeam_deskCntLimit_(createrId, teamId, param, sp);
		break;
	}
	case ETMOT_deskProportionate:
	{
		ProcessModifyTeam_deskProportionate_(createrId, teamId, param, sp);
		break;
	}
	case ETMOT_boardInfo:
	{
		ProcessModifyTeam_boardInfo_(createrId, teamId, param, sp);
		break;
	}
	//case ETMOT_teamName:
	//{
	//	ProcessModifyTeam_teamName_(createrId, teamId, param, sp);
	//	break;
	//}
	case ETMOT_playTypes_deskCntLimit:
	{
		ProcessModifyTeam_playTypes_deskCntLimit_(createrId, teamId, param, sp);
		break;
	}
	default:
		LLOG_ERROR("ModifyTeamData club, error type %d", type);
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		break;
	}
}


void CHanderHttp::ProcessModifyTeam_all_(Lint createrId, Lint teamId, HttpParamMap& param, LSocketPtr sp)
{
	if (param.find("playTypes") == param.end())
	{
		LLOG_DEBUG("Club------modify club, playTypes param error!");
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (param.find("deskCntLimit") == param.end())
	{
		LLOG_DEBUG("Club------modify club, deskCntLimit param error!");
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (param.find("deskProportionate") == param.end())
	{
		LLOG_DEBUG("Club------modify club, deskProportionate param error!");
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (param.find("boardInfo") == param.end())
	{
		LLOG_DEBUG("Club------modify club, boardInfo param error!");
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	/*if (param.find("teamName") == param.end())
	{
		LLOG_DEBUG("Club------modify club, teamName param error!");
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}*/

	Lstring playTypes = param["playTypes"];
	Lint deskCntLimit = atoi(param["deskCntLimit"].c_str());
	Lint deskProportionate = atoi(param["deskProportionate"].c_str());
	Lstring boardInfo = param["boardInfo"];
	//Lstring strTeamName = param["teamName"];

	LLOG_ERROR("HanderHttpModifyTeam_all_ nCreaterID %d playTypes %s boardInfo %s deskCntLimit %d deskProportionate %d", createrId, playTypes.c_str(), boardInfo.c_str(), deskCntLimit, deskProportionate);
	if (createrId <= 0 || playTypes.length() <= 0)
	{
		LLOG_ERROR("HanderHttpModifyTeam_all_, playTypes  createrId error! nCreaterID %d playTypes %s", createrId, playTypes.c_str());
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (/*strTeamName.length() <= 0 || strTeamName.length() > 30 ||*/ deskCntLimit == 0 || playTypes.length() <= 0)
	{
		LLOG_ERROR("HanderHttpModifyTeam_all_ club, TeamName.length error!");
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	std::vector<Lint> vPlayTypes;
	if (!tokenize_atovi(playTypes, vPlayTypes, ";") && vPlayTypes.size() == 0)
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}
	Lstring strTeamName;
	gTeamManager.ModifyTeamData(sp, ETMOT_all, createrId, teamId, vPlayTypes, boardInfo, strTeamName, deskCntLimit, deskProportionate);
}

void CHanderHttp::ProcessModifyTeam_playTypes_(Lint createrId, Lint teamId, HttpParamMap& param, LSocketPtr sp)
{
	if (param.find("playTypes") == param.end())
	{
		LLOG_DEBUG("Club------modify club, playTypes param error!");
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	Lstring playTypes = param["playTypes"];

	LLOG_ERROR("ProcessModifyTeam_playTypes_ nCreaterID %d playTypes %s ", createrId, playTypes.c_str());
	if (createrId <= 0 || playTypes.length() <= 0)
	{
		LLOG_ERROR("ProcessModifyTeam_playTypes_, playTypes  createrId error! nCreaterID %d playTypes %s", createrId, playTypes.c_str());
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	std::vector<Lint> vPlayTypes;
	if (!tokenize_atovi(playTypes, vPlayTypes, ";") && vPlayTypes.size() == 0)
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	Lstring boardInfo;
	Lstring strTeamName;
	gTeamManager.ModifyTeamData(sp, ETMOT_playTypes, createrId, teamId, vPlayTypes, boardInfo, strTeamName, 0, 0);
}

void CHanderHttp::ProcesskModifyTeam_deskCntLimit_(Lint createrId, Lint teamId, HttpParamMap& param, LSocketPtr sp)
{
	if (param.find("deskCntLimit") == param.end())
	{
		LLOG_DEBUG("Club------modify club, deskCntLimit param error!");
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	Lint deskCntLimit = atoi(param["deskCntLimit"].c_str());
	LLOG_ERROR("ProcesskModifyTeam_deskCntLimit_ nCreaterID %d deskCntLimit %d", createrId, deskCntLimit);
	if (createrId <= 0)
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (deskCntLimit == 0)
	{
		LLOG_ERROR("ProcesskModifyTeam_deskCntLimit_ club, deskCntLimit error!");
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	std::vector<Lint> vPlayTypes;
	Lstring boardInfo;
	Lstring strTeamName;
	gTeamManager.ModifyTeamData(sp, ETMOT_deskCntLimit, createrId, teamId, vPlayTypes, boardInfo, strTeamName, deskCntLimit, 0);
}

void CHanderHttp::ProcessModifyTeam_deskProportionate_(Lint createrId, Lint teamId, HttpParamMap& param, LSocketPtr sp)
{
	if (param.find("deskProportionate") == param.end())
	{
		LLOG_DEBUG("Club------modify club, deskCntLimit param error!");
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	Lint deskProportionate = atoi(param["deskProportionate"].c_str());
	LLOG_ERROR("ProcesskModifyTeam_deskProportionate_ nCreaterID %d deskProportionate %d", createrId, deskProportionate);
	if (createrId <= 0)
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (deskProportionate == 0)
	{
		LLOG_ERROR("ProcesskModifyTeam_deskProportionate_ club, deskProportionate error!");
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	std::vector<Lint> vPlayTypes;
	Lstring boardInfo;
	Lstring strTeamName;
	gTeamManager.ModifyTeamData(sp, ETMOT_deskProportionate, createrId, teamId, vPlayTypes, boardInfo, strTeamName, 0, deskProportionate);
}

void CHanderHttp::ProcessModifyTeam_boardInfo_(Lint createrId, Lint teamId, HttpParamMap& param, LSocketPtr sp)
{
	if (param.find("boardInfo") == param.end())
	{
		LLOG_DEBUG("ProcessModifyTeam_boardInfo_, boardInfo param error!");
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	Lstring boardInfo = param["boardInfo"];

	LLOG_ERROR("ProcessModifyTeam_boardInfo_ nCreaterID %d boardInfo %s", createrId, boardInfo.c_str());
	if (createrId <= 0)
	{
		LLOG_ERROR("ProcessModifyTeam_boardInfo_, playTypes  createrId error! nCreaterID %d", createrId);
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (boardInfo.length() <= 0 || boardInfo.length() > 200)
	{
		LLOG_ERROR("ProcessModifyTeam_boardInfo_ club, boardInfo.length error! %d", boardInfo.length());
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	std::vector<Lint> vPlayTypes;
	Lstring strTeamName;
	gTeamManager.ModifyTeamData(sp, ETMOT_boardInfo, createrId, teamId, vPlayTypes, boardInfo, strTeamName, 0, 0);
}

void CHanderHttp::ProcessModifyTeam_teamName_(Lint createrId, Lint teamId, HttpParamMap& param, LSocketPtr sp)
{
	if (param.find("teamName") == param.end())
	{
		LLOG_DEBUG("ProcessModifyTeam_teamName_, teamName param error!");
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	Lstring strTeamName = param["teamName"];
	LLOG_ERROR("ProcessModifyTeam_teamName_ nCreaterID %d strTeamName %s ", createrId, strTeamName.c_str());
	if (createrId <= 0)
	{
		LLOG_ERROR("ProcessModifyTeam_teamName_, playTypes  createrId error! nCreaterID %d", createrId);
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (strTeamName.length() <= 0 || strTeamName.length() > 30)
	{
		LLOG_ERROR("ProcessModifyTeam_teamName_ club, TeamName.length error! %d", strTeamName.length());
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	std::vector<Lint> vPlayTypes;
	Lstring boardInfo;
	gTeamManager.ModifyTeamData(sp, ETMOT_teamName, createrId, teamId, vPlayTypes, boardInfo, strTeamName, 0, 0);
}

void CHanderHttp::ProcessModifyTeam_playTypes_deskCntLimit_(Lint createrId, Lint teamId, HttpParamMap& param, LSocketPtr sp)
{
	if (param.find("playTypes") == param.end())
	{
		LLOG_DEBUG("Club------modify club, playTypes param error!");
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (param.find("deskCntLimit") == param.end())
	{
		LLOG_DEBUG("ProcessModifyTeam_playTypes_deskCntLimit_, deskCntLimit param error!");
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	Lint deskCntLimit = atoi(param["deskCntLimit"].c_str());
	Lstring playTypes = param["playTypes"];

	LLOG_ERROR("ProcessModifyTeam_playTypes_deskCntLimit_ nCreaterID %d playTypes %s ", createrId, playTypes.c_str());
	if (createrId <= 0 || playTypes.length() <= 0)
	{
		LLOG_ERROR("ProcessModifyTeam_playTypes_deskCntLimit_, playTypes  createrId error! nCreaterID %d playTypes %s", createrId, playTypes.c_str());
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	std::vector<Lint> vPlayTypes;
	if (!tokenize_atovi(playTypes, vPlayTypes, ";") && vPlayTypes.size() == 0)
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

    if (createrId <= 0)
	{
		LLOG_ERROR("ProcessModifyTeam_playTypes_deskCntLimit_, playTypes  createrId error! nCreaterID %d", createrId);
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (deskCntLimit == 0)
	{
		LLOG_ERROR("ProcessModifyTeam_playTypes_deskCntLimit_ club, TeamName.length error! deskCntLimit %d", deskCntLimit);
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	Lstring boardInfo;
	Lstring strTeamName;
	gTeamManager.ModifyTeamData(sp, ETMOT_playTypes_deskCntLimit, createrId, teamId, vPlayTypes, boardInfo, strTeamName, deskCntLimit, 0);
}

void CHanderHttp::HanderHttpQueryUserInfos(HttpParamMap& param, LSocketPtr sp)
{
	if (param.empty() || NULL == sp)
	{
		return;
	}

	if (!IsHaverParam(param, "userId"))
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (!IsHaverParam(param, "teamId"))
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	Lint nTeamId = GetHttpParamLint(param, "teamId");
	Lstring strAllUserId = GetHttpParamString(param, "userId");
	Lint	nServerID = GAME_APPID;
	if (IsHaverParam(param, "serverID"))
	{
		nServerID = GetHttpParamLint(param, "serverID");
	}

	Lstring strNike = "";
	Lstring strHeadImageUrl = "";

	std::vector<std::string> allUserId = mySplit(strAllUserId, ",");
	if (allUserId.empty())
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	boost::property_tree::ptree pt_all_user;

	Lstring strAddFailId = "";
	for (int i = 0; i < allUserId.size(); i++)
	{
		Lint nTeamUserID = atoi(allUserId[i].c_str());
		if (!IsUserExist(nTeamUserID, strNike, strHeadImageUrl))
		{
			if (strAddFailId.length() > 0)
			{
				strAddFailId = strAddFailId + ",";
			}

			strAddFailId = strAddFailId + allUserId[i];
			continue;
		}

		boost::property_tree::ptree pt_user;
		pt_user.put("userId", nTeamUserID);
		pt_user.put("nick", strNike);
		pt_user.put("headImageUrl", strHeadImageUrl);
		if (gTeamManager.IsMember(nTeamId, nTeamUserID))
		{
			pt_user.put("isJoin", 1);
		}
		else
		{
			pt_user.put("isJoin", 0);
		}

		pt_all_user.push_back(std::make_pair("", pt_user));
	}

	boost::property_tree::ptree pt_root;
	pt_root.put("errorMsg", "");
	pt_root.put("errorCode", 0);
	pt_root.add_child("datas", pt_all_user);
	std::stringstream strStream;
	boost::property_tree::write_json(strStream, pt_root);

	SendRet(strStream.str(), sp);

	return;
}

void CHanderHttp::HanderHttpQueryExchageList(HttpParamMap& param, LSocketPtr sp)
{
	if (param.empty() || NULL == sp)
	{
		return;
	}

	if (!IsHaverParam(param, "userId"))
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	Lint nUserId = GetHttpParamLint(param, "userId");
	if (!nUserId)
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	tC_SExchageGoldOptLogVec& infolist = gTeamManager.GetTeamExchageOptByToUserId(nUserId);
	Lint sz = infolist.size();

	Lstring strMemInfoList = "";
	for (Lint i = 0; i < sz; ++i)
	{
		SExchageGoldOptLog * optlog = infolist[i];
		SExchageGoldOptLog &teamMember = *optlog;
		if (teamMember.nOpt != 0) continue;
		std::stringstream strTeamStream;
		strTeamStream << "{";
		strTeamStream << "\"logId\":" << teamMember.lLogId << ",";
		strTeamStream << "\"fromUserId\":" << teamMember.nFromUserId << ",";
		strTeamStream << "\"goldNum\":" << teamMember.nGoldNum << ",";
		strTeamStream << "\"fromName\":\"" << teamMember.sFromName << "\",";
		strTeamStream << "\"reqTime\":" << teamMember.nReqTime << "";
		strTeamStream << "}";

		if (strMemInfoList.length() == 0)
		{
			strMemInfoList = strTeamStream.str();
		}
		else
		{
			strMemInfoList = strMemInfoList + ",";
			strMemInfoList = strMemInfoList + strTeamStream.str();
		}
	}

	std::stringstream strStream;
	strStream << "{";
	strStream << "\"errorMsg\":\"\",";
	strStream << "\"errorCode\":0,";
	strStream << "\"datas\":[" << strMemInfoList << "]";
	strStream << "}";
	SendRet(strStream.str(), sp);

	return;
}

void CHanderHttp::HanderHttpOptExchage(HttpParamMap& param, LSocketPtr sp)
{
	if (param.empty() || NULL == sp)
	{
		return;
	}

	if (!IsHaverParam(param, "userId"))
	{
		LLOG_DEBUG("HanderHttpOptExchage 111");
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (!IsHaverParam(param, "logId"))
	{
		LLOG_DEBUG("HanderHttpOptExchage 222");
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	Lint nUserId = GetHttpParamLint(param, "userId");
	if (!nUserId)
	{
		LLOG_DEBUG("HanderHttpOptExchage 333");
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	Llong nlongId = GetHttpParamLlong(param, "logId");
	if (!nlongId)
	{
		LLOG_DEBUG("HanderHttpOptExchage 444");
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}
	Lint nopt = GetHttpParamLint(param, "opt");
	SExchageGoldOptLog* info = gTeamManager.GetTeamExchageOpt(nlongId);
	if (!info)
	{
		LLOG_DEBUG("HanderHttpOptExchage 555");
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (info->nToUserId != nUserId)
	{
		LLOG_DEBUG("HanderHttpOptExchage 666");
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (info->nOpt != 0)
	{
		LLOG_DEBUG("HanderHttpOptExchage 777");
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	//通知两个人
	LMsgS2CExchageGoldOptNotice notice;
	notice.nFromUserId = info->nFromUserId; //谁请求的
	notice.nToUserId = info->nToUserId; //让谁回收
	notice.nGoldNum = info->nGoldNum; //回收多少
	notice.sFromName = info->sFromName; //谁申请的
	notice.sToName = info->sToName; //谁处理

	if (nopt == 0)
	{
		notice.nResCode = 0; //0 已经被回收， 1 放弃回收
		info->nOpt = 1;
		//gWork.ModifyRoomCardCount(nUserId, info->nGoldNum, CARDS_OPER_TYPE_HTTP_REQ_EXCHAGEGOLD);
	}
	else
	{
		notice.nResCode = 1; //0 已经被回收， 1 放弃回收
		info->nOpt = 3;
		gWork.ModifyRoomCardCount(info->nFromUserId, info->nGoldNum, CARDS_OPER_TYPE_HTTP_CANNCEL_EXCHAGEGOLD);
	}
	gTeamManager.AddTeamExchageOpt(info, true);

	gWork.SendToGateByUserID(notice, nUserId);
	gWork.SendToGateByUserID(notice, info->nFromUserId);
	
	Lstring strMemInfoList = "";
	std::stringstream strStream;
	strStream << "{";
	strStream << "\"errorMsg\":\"\",";
	strStream << "\"errorCode\":0,";
	strStream << "\"datas\":[" << strMemInfoList << "]";
	strStream << "}";
	SendRet(strStream.str(), sp);

	return;
}

void CHanderHttp::HanderHttpJoinTeam(HttpParamMap& param, LSocketPtr sp)
{
	if (param.empty() || NULL == sp)
	{
		return;
	}

	if (param.find("createrId") == param.end())
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (param.find("teamId") == param.end())
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (param.find("memId") == param.end())
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (param.find("opt") == param.end())
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	Lint nCreaterID = atoi(param["createrId"].c_str());
	Lint nTeamID = atoi(param["teamId"].c_str());
	Lstring nUserIDs = param["memId"];
	Lint nOpt = atoi(param["opt"].c_str());
	//nOpt  1、批准加入，2、拒绝加入，3、拒绝离开，4、同意离开，5、删除

	std::vector<std::string> errorUserList;

	Lint nRet = gTeamManager.TeamManagerOptMem(nCreaterID, nTeamID, nUserIDs, nOpt, errorUserList);
	if (nRet == EHEEC_MEM_OPT_NOTALL_SUC)
	{
		std::stringstream errorInfo;
		errorInfo << "{\"errorCode\":" << nRet << ",\"errorMsg\":\"";

		Lint nCount = errorUserList.size();
		for (int i = 0; i < nCount; i++)
		{
			if (i != 0)
			{
				errorInfo << ",";
			}
			errorInfo << errorUserList[i];
		}

		errorInfo << "\"}";

		SendRet(errorInfo.str(), sp);
	}
	else
	{
		SendHttpRet(nRet, sp);
	}
}

void CHanderHttp::HanderHttpCardAlert(HttpParamMap& param, LSocketPtr sp)
{
	if (param.empty() || NULL == sp)
	{
		return;
	}

	if (param.find("createrId") == param.end())
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (param.find("teamId") == param.end())
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (param.find("alert") == param.end())
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	Lint nCreaterID = atoi(param["createrId"].c_str());
	Lint nTeamID = atoi(param["teamId"].c_str());
	Lint nAlert = atoi(param["alert"].c_str());

	if (nAlert <= 0)
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}
	Lint nRet = gTeamManager.SetMinCardAlert(nCreaterID, nTeamID, nAlert);
	SendHttpRet(nRet, sp);
}

void CHanderHttp::HanderHttpMarkLog(HttpParamMap& param, LSocketPtr sp)
{
	if (param.empty() || NULL == sp)
	{
		return;
	}

	if (param.find("teamId") == param.end())
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (param.find("id") == param.end())
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	std::stringstream strStream;

	strStream << "update team_log set ";
	strStream << "OptState=1 ";
	strStream << "where Id=" << atoi(param["id"].c_str()) << ";";

	gDbServer.Push(move(strStream.str()), atoi(param["teamId"].c_str()));

	SendHttpRet(EHEEC_SUCCESS, sp);
}

void CHanderHttp::HanderHttpPerDayLimit(HttpParamMap& param, LSocketPtr sp)
{
	if (param.empty() || NULL == sp)
	{
		return;
	}

	if (param.find("createrId") == param.end())
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (param.find("teamId") == param.end())
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (param.find("limit") == param.end())
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	Lint nCreaterID = atoi(param["createrId"].c_str());
	Lint nTeamID = atoi(param["teamId"].c_str());
	Lint nPerDayLimit = atoi(param["limit"].c_str());

	if (nPerDayLimit < 0)
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	Lint nRet = gTeamManager.SetPerDayLimit(nCreaterID, nTeamID, nPerDayLimit);
	SendHttpRet(nRet, sp);
}

void CHanderHttp::HanderHttpDelTeam(HttpParamMap& param, LSocketPtr sp)
{
	if (param.empty() || NULL == sp)
	{
		return;
	}

	if (param.find("createrId") == param.end())
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (param.find("teamId") == param.end())
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	Lint nCreaterID = atoi(param["createrId"].c_str());
	Lint nTeamID = atoi(param["teamId"].c_str());

	if (nCreaterID <= 0 || nTeamID <= 0)
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
	}

	Lint nRet = gTeamManager.DelTeam(nCreaterID, nTeamID);

	SendHttpRet(nRet, sp);
}

void CHanderHttp::HanderHttpAddMembers(HttpParamMap& param, LSocketPtr sp)
{
	if (param.empty() || NULL == sp)
	{
		return;
	}

	if (IsHaverParam(param, "createrId") == false)
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (IsHaverParam(param, "teamId") == false)
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (IsHaverParam(param, "userId") == false)
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	Lstring strNike;
	Lstring strHeadImageUrl;
	Lint	nCreaterID = GetHttpParamLint(param, "createrId");
	Lint	nServerID = 20001;
	if (IsHaverParam(param, "serverID"))
	{
		nServerID = GetHttpParamLint(param, "serverID");
	}

	Lstring	strAllUserID = GetHttpParamString(param, "userId");
	std::vector<std::string> allUserId = mySplit(strAllUserID, ",");
	if (allUserId.empty())
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}
	Lint	nTeamID = GetHttpParamLint(param, "teamId");

	const STeamInfo& teamInfo = gTeamManager.FindTeam(nTeamID);
	if (teamInfo.nCreaterID != nCreaterID)
	{
		SendHttpRet(EHEEC_TEAM_CREATEID_ERR, sp);
		return;
	}

	Lint nUserIDCount = allUserId.size();
	Lint nClubUserCount = gTeamManager.GetTeamMemCount(nTeamID);
	Lint nTotalUserCount = nClubUserCount + nUserIDCount;
	Lint nMaxCount = gConfig.GetMaxTeamMemCount();
	if (nTotalUserCount > nMaxCount)
	{
		SendHttpRet(EHEEC_TEAM_MEM_COUNT, sp);
		LLOG_ERROR("AddMembers fail, MaxTeamMemCount = %d, club user=%d, add user count=%d", nMaxCount, nClubUserCount, nUserIDCount);
		return;
	}

	/*if (teamInfo.nServerID != nServerID)
	{
	SendHttpRet(EHEEC_TEAM_NOT_EXIST, sp);
	return;
	}*/
	// 记录添加失败的玩家ID [2017-8-14 willing]
	Lstring strAddFailId = "";
	for (int i = 0; i < nUserIDCount; i++)
	{
		Lint nTempUserID = atoi(allUserId[i].c_str());

		if (gTeamManager.IsMember(nTeamID, nTempUserID))
		{
			if (strAddFailId.length() > 0)
			{
				strAddFailId = strAddFailId + ",";
			}

			strAddFailId = strAddFailId + allUserId[i];
			continue;
		}

		if (!IsUserExist(nTempUserID, strNike, strHeadImageUrl))
		{
			if (strAddFailId.length() > 0)
			{
				strAddFailId = strAddFailId + ",";
			}

			strAddFailId = strAddFailId + allUserId[i];
			continue;
		}

		STeamMember teamMember;
		teamMember.nTeamID = nTeamID;
		teamMember.nUserID = nTempUserID;
		teamMember.strUserName = strNike;
		teamMember.strHeadImageUrl = strHeadImageUrl;
		teamMember.nTodayUserCardNum = 0;
		teamMember.nGameState = ETMGS_OFFLINE;
		teamMember.nTeamState = ETMTS_NORMAL;
		teamMember.nLastUserCardTime = 0;
		teamMember.nJoinTime = LTime().Secs();
		/*teamMember.nWinerCount = 0;
		teamMember.nTotalCreateDeskCount = 0;
		teamMember.nTotalPlayCount = 0;*/

		Lint nRet = gTeamManager.AddMemToTeam(teamMember, true);
		if (EHEEC_SUCCESS != nRet)
		{
			if (strAddFailId.length() > 0)
			{
				strAddFailId = strAddFailId + ",";
			}

			strAddFailId = strAddFailId + allUserId[i];
			continue;
		}

		// 发送消息到LogicManager上，查看该玩家是否在线 [2017-8-4 willing]
		LMsgTS2LMG_QueryIsOnLine queryIsOnLine;
		queryIsOnLine.nUserID = nTempUserID;
		gWork.SendToLogicManager(queryIsOnLine);

		// 通知玩家 [2017-7-20 willing]
		const STeamInfo& teamInfo = gTeamManager.FindTeam(nTeamID);
		LMsgS2CNoticeEvent noticeEvent;
		noticeEvent.nTeamID = teamInfo.nTeamID;
		noticeEvent.strCreaterName = teamInfo.strCreaterName;
		noticeEvent.strTeamName = teamInfo.strTeamName;
		noticeEvent.nOpt = 6;// 会长主动加人 [2017-7-20 willing]

		gWork.SendToGateByUserID(noticeEvent, nTempUserID);

		// 记录操作日志 [2017-8-9 willing]
		gTeamManager.SaveTameMemOptLog(nTeamID, nTempUserID, ETMOT_TEAM_ADD_MEM);

		LLOG_ERROR("CHanderHttp::HanderHttpAddMember teamid=%d,creater=%d,userId=%d", nTeamID, nCreaterID, nTempUserID);

		// 统计日志使用 [2017-8-22 willing]
		if (gTeamManager.GetJoinCount(nTempUserID) == 1)
		{
			LLOG_ERROR("CTeamManager::TeamManagerOptMem JoinTeamCount=1,teamid=%d,creater=%d,userid=%d", nTeamID, nCreaterID, nTempUserID);
		}
	}

	// 返回结果消息 [2017-8-14 willing]
	std::stringstream strStream;
	strStream << "{";
	strStream << "\"errorMsg\":\"\",";
	strStream << "\"errorCode\":0,";
	strStream << "\"datas\":\"" << strAddFailId << "\"";
	strStream << "}";
	SendRet(strStream.str(), sp);
}

void CHanderHttp::HanderHttpAddMember(HttpParamMap& param, LSocketPtr sp)
{
	if (param.empty() || NULL == sp)
	{
		return;
	}

	if (param.find("createrId") == param.end())
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (param.find("teamId") == param.end())
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (param.find("userId") == param.end())
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	Lstring strNike;
	Lstring strHeadImageUrl;
	Lint	nTeamID = atoi(param["teamId"].c_str());
	Lint	nUserID = atoi(param["userId"].c_str());

	if ( gTeamManager.IsMember(nTeamID, nUserID))
	{
		SendHttpRet(EHEEC_IS_IN_TEAM, sp);
		return;
	}

	if ( !IsUserExist(nUserID, strNike, strHeadImageUrl) )
	{
		SendHttpRet(EHEEC_NOT_FOUND_USER, sp);
		return;
	}
	if (gTeamManager.GetTeamMemCount(nTeamID) >= gConfig.GetMaxTeamMemCount())
	{
		SendHttpRet(EHEEC_TEAM_MEM_COUNT, sp);
		return;
	}
	// 判断加入的工会是否超过了上限 [2017-7-21 willing]
	if (gTeamManager.GetJoinCount(nUserID) >= gConfig.GetMemJoinTeamMaxCount())
	{
		SendHttpRet(EHEEC_JOIN_TEAM_MAX, sp);
		return;
	}
	STeamMember teamMember;
	teamMember.nTeamID = atoi(param["teamId"].c_str());
	teamMember.nUserID = nUserID;
	teamMember.strUserName = strNike;
	teamMember.strHeadImageUrl = strHeadImageUrl;
	teamMember.nTodayUserCardNum = 0;
	teamMember.nGameState = ETMGS_OFFLINE;
	teamMember.nTeamState = ETMTS_NORMAL;
	teamMember.nLastUserCardTime = 0;
	teamMember.nJoinTime = LTime().Secs();

	Lint nRet = gTeamManager.AddMemToTeam(teamMember, true);

	SendHttpRet(nRet, sp);

	if ( nRet == EHEEC_SUCCESS )
	{
		// 发送消息到LogicManager上，查看该玩家是否在线 [2017-8-4 willing]
		LMsgTS2LMG_QueryIsOnLine queryIsOnLine;
		queryIsOnLine.nUserID = nUserID;
		gWork.SendToLogicManager(queryIsOnLine);

		// 通知玩家 [2017-7-20 willing]
		const STeamInfo& teamInfo = gTeamManager.FindTeam(nTeamID);
		LMsgS2CNoticeEvent noticeEvent;
		noticeEvent.nTeamID = teamInfo.nTeamID;
		noticeEvent.strCreaterName = teamInfo.strCreaterName;
		noticeEvent.strTeamName = teamInfo.strTeamName;
		noticeEvent.nOpt = 6;// 会长主动加人 [2017-7-20 willing]

		gWork.SendToGateByUserID(noticeEvent, nUserID);

		// 记录操作日志 [2017-8-9 willing]
		gTeamManager.SaveTameMemOptLog(nTeamID, nUserID, ETMOT_TEAM_ADD_MEM);

		LLOG_ERROR("CHanderHttp::HanderHttpAddMember teamid=%d", nTeamID);
	}
}

void CHanderHttp::HanderHttpOtherJoinTeam(HttpParamMap& param, LSocketPtr sp)
{
	if (param.empty() || NULL == sp)
	{
		return;
	}

	if (param.find("createrId") == param.end())
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (param.find("teamId") == param.end())
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (param.find("userId") == param.end())
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	Lstring strNike;
	Lstring strHeadImageUrl;
	Lint	nTeamID = atoi(param["teamId"].c_str());
	Lint	nUserID = atoi(param["userId"].c_str());

	if (gTeamManager.IsMember(nTeamID, nUserID))
	{
		SendHttpRet(EHEEC_IS_IN_TEAM, sp);
		return;
	}

	if (!IsUserExist(nUserID, strNike, strHeadImageUrl))
	{
		SendHttpRet(EHEEC_NOT_FOUND_USER, sp);
		return;
	}
	if (gTeamManager.GetTeamMemCount(nTeamID) >= gConfig.GetMaxTeamMemCount())
	{
		SendHttpRet(EHEEC_TEAM_MEM_COUNT, sp);
		return;
	}
	// 判断加入的工会是否超过了上限 [2017-7-21 willing]
	if (gTeamManager.GetJoinCount(nUserID) >= gConfig.GetMemJoinTeamMaxCount())
	{
		SendHttpRet(EHEEC_JOIN_TEAM_MAX, sp);
		return;
	}
	STeamMember teamMember;
	teamMember.nTeamID = atoi(param["teamId"].c_str());
	teamMember.nUserID = nUserID;
	teamMember.strUserName = strNike;
	teamMember.strHeadImageUrl = strHeadImageUrl;
	teamMember.nTodayUserCardNum = 0;
	teamMember.nGameState = ETMGS_OFFLINE;
	teamMember.nTeamState = ETMTS_JOIN;
	teamMember.nLastUserCardTime = 0;
	teamMember.nJoinTime = LTime().Secs();

	Lint nRet = gTeamManager.AddMemToTeam(teamMember, true);

	SendHttpRet(nRet, sp);

	if (nRet == EHEEC_SUCCESS)
	{
		// 发送消息到LogicManager上，查看该玩家是否在线 [2017-8-4 willing]
		LMsgTS2LMG_QueryIsOnLine queryIsOnLine;
		queryIsOnLine.nUserID = nUserID;
		gWork.SendToLogicManager(queryIsOnLine);

		// 记录操作日志 [2017-8-9 willing]
		gTeamManager.SaveTameMemOptLog(nTeamID, nUserID, ETMOT_REQ_JOIN_TEAM);

		LLOG_ERROR("CHanderHttp::HanderHttpJoinTeam teamid=%d", nTeamID);
	}
}

// 校验该玩家是否存在 [2017-8-4 willing]
bool CHanderHttp::IsUserExist(Lint nUserID, Lstring& strNike, Lstring& strHeadImageUrl)
{
	strNike = "";
	strHeadImageUrl = "";

	MYSQL* m = gWork.GetDbSession().GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("CHanderHttp::IsUserExist mysql null");
		return false;
	}

	std::stringstream strStream;
	strStream << "select Nike,HeadImageUrl from user where Id=" << nUserID << ";";

	if (mysql_real_query(m, strStream.str().c_str(), strStream.str().size()))
	{
		LLOG_ERROR("CHanderHttp::IsUserExist sql error %s", mysql_error(m));
		return false;
	}

	MYSQL_RES* res = mysql_store_result(m);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
	{
		return false;
	}

	strNike = *row++;
	strHeadImageUrl = *row++;

	return true;
}

void CHanderHttp::HanderHttpQueryUserInfo(HttpParamMap& param, LSocketPtr sp)
{
	if (param.empty() || NULL == sp)
	{
		return;
	}

	if (param.find("userId") == param.end())
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	Lstring strNike = "";
	Lstring strHeadImageUrl = "";
	Lint nUserID = atoi(param["userId"].c_str());

	Lint nRet = EHEEC_SUCCESS;

	if ( IsUserExist(nUserID, strNike, strHeadImageUrl) == false )
	{
		nRet = EHEEC_NOT_FOUND_USER;
	}

	if ( EHEEC_SUCCESS == nRet )
	{
		/*std::stringstream errorInfo;
		errorInfo << "{\"errorCode\":" << nRet << ",\"errorMsg\":\"\",";
		errorInfo << "\"datas\":{\"userId\":" << nUserID << ",\"nick\":\""<<strNike <<"\",\"headImageUrl\":\""<< strHeadImageUrl <<"\"},";
		errorInfo <<"}";
		SendRet(errorInfo.str(), sp);*/
		boost::property_tree::ptree pt_root;
		pt_root.put("errorMsg", "");
		pt_root.put("errorCode", nRet);

		boost::property_tree::ptree pt_datas;
		pt_datas.put("userId", nUserID);
		pt_datas.put("nick", strNike);
		pt_datas.put("headImageUrl", strHeadImageUrl);

		pt_root.add_child("datas", pt_datas);
		std::stringstream strStream;
		boost::property_tree::write_json(strStream, pt_root);

		SendRet(strStream.str(), sp);
	}
	else
	{
		SendHttpRet(nRet, sp);
	}

	return;
}

void CHanderHttp::HanderHttpQueryTeamList(HttpParamMap& param, LSocketPtr sp)
{
	if (param.empty() || NULL == sp)
	{
		return;
	}

	if (param.find("createrId") == param.end())
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	Lint nCreaterID = atoi(param["createrId"].c_str());

	std::list<Lint> teamIdList;
	gTeamManager.GetCreateTeamID(nCreaterID, teamIdList);

	if ( teamIdList.empty() )
	{
		std::stringstream strStream;
		strStream << "{";
		strStream << "\"errorMsg\":\"\",";
		strStream << "\"errorCode\":0,";
		strStream << "\"datas\":[]";
		strStream << "}";
		SendRet(strStream.str(), sp);
		return;
	}

	std::list<Lint>::iterator iter_list = teamIdList.begin();
	std::list<Lint>::iterator iend_list = teamIdList.end();

	Lstring teamInfoList = "";

	for ( ; iter_list != iend_list; iter_list++ )
	{
		Lint nTeamID = *iter_list;
		const STeamInfo& teamInfo = gTeamManager.FindTeam(nTeamID);
		if ( teamInfo.nTeamID != nTeamID )
		{
			continue;
		}

		std::stringstream teamInfo_stream;
		teamInfo_stream << "{";
		teamInfo_stream << "\"teamId\":"<< teamInfo.nTeamID <<",";
		teamInfo_stream << "\"teamName\":\"" << teamInfo.strTeamName << "\",";
		teamInfo_stream << "\"cards\":" << teamInfo.nCards << ",";
		teamInfo_stream << "\"cardAlert\":" << teamInfo.nCardAlert << ",";
		teamInfo_stream << "\"perDayMemLimit\":" << teamInfo.nMemCardLimitPerDay << ",";
		teamInfo_stream << "\"createTime\":" << teamInfo.nCreateTime << ",";

		Lstring sPlayTypes;
		tokenize_vitoa(teamInfo.vPlayTypes, sPlayTypes, ";");

		teamInfo_stream << "\"playTypes\":\"" << sPlayTypes << "\",";
		teamInfo_stream << "\"manageId\":" << teamInfo.nManageId << ",";
		teamInfo_stream << "\"countyId\":" << teamInfo.nCountyId << ",";
		teamInfo_stream << "\"deskCntLimit\":" << teamInfo.nDeskCntLimit << ",";
		teamInfo_stream << "\"deskProportionate\":" << teamInfo.nDeskProportionate << ",";
		teamInfo_stream << "\"boardInfo\":\"" << teamInfo.m_boardInfo << "\",";

		teamInfo_stream << "\"teamState\":" << teamInfo.nState;
		teamInfo_stream << "}";

		if (teamInfoList.size() == 0 )
		{
			teamInfoList = teamInfo_stream.str();
		}
		else
		{
			teamInfoList.append(",");
			teamInfoList.append(teamInfo_stream.str());
		}
	}

	// 发送结果消息 [2017-8-10 willing]
	std::stringstream strStream;
	strStream << "{";
	strStream << "\"errorMsg\":\"\",";
	strStream << "\"errorCode\":0,";
	strStream << "\"datas\":[" << teamInfoList << "]";
	strStream << "}";
	SendRet(strStream.str(), sp);
}

void CHanderHttp::HanderHttpQueryTeamInfo(HttpParamMap& param, LSocketPtr sp)
{
	if (param.empty() || NULL == sp)
	{
		return;
	}

	if (param.find("createrId") == param.end())
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (param.find("teamId") == param.end())
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	Lint nTeamID = atoi(param["teamId"].c_str());
	Lint nCreaterID = atoi(param["createrId"].c_str());

	const STeamInfo& teamInfo = gTeamManager.FindTeam(nTeamID);
	if ( teamInfo.nTeamID != nTeamID )
	{
		SendHttpRet(EHEEC_TEAM_NOT_EXIST,sp);
		return;
	}

	if ( teamInfo.nCreaterID != nCreaterID )
	{
		SendHttpRet(EHEEC_TEAM_CREATEID_ERR, sp);
		return;
	}

	/*std::stringstream teamInfo_stream;
	teamInfo_stream << "{";
	teamInfo_stream << "\"teamId\":" << teamInfo.nTeamID << ",";
	teamInfo_stream << "\"createrId\":" << teamInfo.nCreaterID << ",";
	teamInfo_stream << "\"createrName\":\"" << teamInfo.strCreaterName << "\",";
	teamInfo_stream << "\"teamName\":\"" << teamInfo.strTeamName << "\",";
	teamInfo_stream << "\"cards\":" << teamInfo.nCards << ",";
	teamInfo_stream << "\"cardAlert\":" << teamInfo.nCardAlert << ",";
	teamInfo_stream << "\"perDayMemLimit\":" << teamInfo.nMemCardLimitPerDay << ",";
	teamInfo_stream << "\"createTime\":" << teamInfo.nCreateTime << ",";
	teamInfo_stream << "\"teamState\":" << teamInfo.nState;
	teamInfo_stream << "}";

	std::stringstream strStream;
	strStream << "{";
	strStream << "\"errorMsg\":\"\",";
	strStream << "\"errorCode\":0,";
	strStream << "\"datas\":" << teamInfo_stream.str();
	strStream << "}";
	SendRet(strStream.str(), sp);*/
	boost::property_tree::ptree pt_root;
	pt_root.put("errorMsg", "");
	pt_root.put("errorCode", 0);

	boost::property_tree::ptree pt_datas;
	pt_datas.put("teamId", teamInfo.nTeamID);
	pt_datas.put("createrId", teamInfo.nCreaterID);
	pt_datas.put("createrName", teamInfo.strCreaterName);
	pt_datas.put("teamName", teamInfo.strTeamName);
	pt_datas.put("cards", teamInfo.nCards);
	pt_datas.put("cardAlert", teamInfo.nCardAlert);
	pt_datas.put("perDayMemLimit", teamInfo.nMemCardLimitPerDay);
	pt_datas.put("createTime", teamInfo.nCreateTime);
	pt_datas.put("teamState", teamInfo.nState);

	pt_root.add_child("datas", pt_datas);
	std::stringstream strStream;
	boost::property_tree::write_json(strStream, pt_root);

	SendRet(strStream.str(), sp);
}

void CHanderHttp::HanderHttpQueryMemberList(HttpParamMap& param, LSocketPtr sp)
{
	if (param.empty() || NULL == sp)
	{
		return;
	}

	if (param.find("teamId") == param.end())
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (param.find("memStatus") == param.end())
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	Lint nTeamID = atoi(param["teamId"].c_str());
	std::vector<int> statusVec = mySplit_int(param["memStatus"], ",");
	
	const STeamInfo& teamInfo = gTeamManager.FindTeam(nTeamID);
	if ( teamInfo.nTeamID != nTeamID )
	{
		SendHttpRet(EHEEC_TEAM_NOT_EXIST, sp);
		return;
	}

	std::vector<STeamMember> members;
	gTeamManager.GetAllTeamMember(nTeamID, members);

	Lint sz = members.size();
	Lstring strMemInfoList = "";

	for (Lint i = 0; i < sz; i++)
	{
		const STeamMember& teamMember = members[i];
		auto it = std::find(statusVec.begin(), statusVec.end(), teamMember.nTeamState);
		if (it == statusVec.end())
			continue;

		std::stringstream strTeamStream;
		strTeamStream << "{";
		strTeamStream << "\"userId\":" << teamMember.nUserID << ",";
		strTeamStream << "\"nick\":\"" << teamMember.strUserName << "\",";
		strTeamStream << "\"headImageUrl\":\"" << teamMember.strHeadImageUrl << "\",";
		strTeamStream << "\"joinTime\":" << teamMember.nJoinTime << ",";
		strTeamStream << "\"useCardNum\":'" << std::to_string(teamMember.nNumOfCard2s) << "',";
		strTeamStream << "\"status\":" << teamMember.nTeamState << "";
		strTeamStream << "}";

		if (strMemInfoList.length() == 0)
		{
			strMemInfoList = strTeamStream.str();
		}
		else
		{
			strMemInfoList = strMemInfoList + ",";
			strMemInfoList = strMemInfoList + strTeamStream.str();
		}
	}

	std::stringstream strStream;
	strStream << "{";
	strStream << "\"errorMsg\":\"\",";
	strStream << "\"errorCode\":0,";
	strStream << "\"datas\":[" << strMemInfoList << "]";
	strStream << "}";
	SendRet(strStream.str(), sp);
}

std::vector<std::string> CHanderHttp::mySplit(std::string str, std::string s)
{
	boost::regex reg(s.c_str());
	std::vector<std::string> vec;

	boost::sregex_token_iterator it(str.begin(), str.end(), reg, -1);
	boost::sregex_token_iterator end;

	while (it != end)
	{
		vec.push_back(*it++);
	}

	return vec;
}

std::vector<int> CHanderHttp::mySplit_int(std::string str, std::string s)
{
	boost::regex reg(s.c_str());
	std::vector<int> vec;

	boost::sregex_token_iterator it(str.begin(), str.end(), reg, -1);
	boost::sregex_token_iterator end;

	while (it != end)
	{
		std::string tmp = *it++;
		if(!tmp.empty())
			vec.push_back(atoi(tmp.c_str()));
	}

	return vec;
}

void CHanderHttp::HanderHttpQueryMemberCount(HttpParamMap& param, LSocketPtr sp)
{
	if (param.empty() || NULL == sp)
	{
		return;
	}
	if(param.find("teamId") == param.end())
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}
	if (param.find("memStatus") == param.end())
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	Lint nTeamID = GetHttpParamLint(param, "teamId");
	Lstring strAllStatus = GetHttpParamString(param, "memStatus");
	std::vector<std::string> statusVec = mySplit(strAllStatus, ",");

	std::map<int, std::string> statusMap;
	for (int i = 0; i < statusVec.size(); i++)
	{
		Lint nTeam = atoi(statusVec[i].c_str());
		statusMap[nTeam] = statusVec[i];
	}

	const STeamInfo& teamInfo = gTeamManager.FindTeam(nTeamID);
	if (teamInfo.nTeamID != nTeamID)
	{
		SendHttpRet(EHEEC_TEAM_NOT_EXIST, sp);
		return;
	}

	Lint nTotalMemCount = 0;

	tTeamMemberMap::const_iterator iter_mem = teamInfo.m_tTeamMemberMap.begin();
	tTeamMemberMap::const_iterator iend_mem = teamInfo.m_tTeamMemberMap.end();

	for (; iter_mem != iend_mem; iter_mem++)
	{
		const STeamMember& teamMember = iter_mem->second;

		if (statusMap.find(teamMember.nTeamState) == statusMap.end())
		{
			continue;
		}

		nTotalMemCount++;
	}

	// 返回结果消息 [2017-8-14 willing]
	std::stringstream strStream;
	strStream << "{";
	strStream << "\"errorMsg\":\"\",";
	strStream << "\"errorCode\":0,";
	strStream << "\"datas\":" << nTotalMemCount << "";
	strStream << "}";
	SendRet(strStream.str(), sp);
}

void CHanderHttp::HanderHttpQueryLogList(HttpParamMap& param, LSocketPtr sp)
{
	if (param.empty() || NULL == sp)
	{
		return;
	}

	if (IsHaverParam(param, "teamId") == false)
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (IsHaverParam(param, "date") == false)
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (IsHaverParam(param, "start") == false)
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (IsHaverParam(param, "limit") == false)
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	Lint nUserId = 0;
	if (IsHaverParam(param, "userId"))
	{
		nUserId = GetHttpParamLint(param, "userId");
	}

	Lint nTeamID = GetHttpParamLint(param, "teamId");
	Lint nStartPage = GetHttpParamLint(param, "start");
	Lint nPageSize = GetHttpParamLint(param, "limit");
	Lstring strDate = GetHttpParamString(param, "date");
	struct tm tm1;

	int i = sscanf(strDate.c_str(), "%d-%d-%d",
		&(tm1.tm_year),
		&(tm1.tm_mon),
		&(tm1.tm_mday));

	tm1.tm_year -= 1900;
	tm1.tm_mon--;
	tm1.tm_isdst = -1;
	tm1.tm_hour = 0;
	tm1.tm_min = 0;
	tm1.tm_sec = 0;

	time_t time1;
	time1 = mktime(&tm1);

	if (nStartPage < 0)
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (nPageSize < 0)
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	// 构造SQL [2017-8-17 willing]
	std::stringstream strStream;
	strStream << "select Id,DeskID,OptState,from_unixtime(Time),";
	strStream << "UserId1,Nike1,Score1,HeadImageUrl1,";
	strStream << "UserId2,Nike2,Score2,HeadImageUrl2,";
	strStream << "UserId3,Nike3,Score3,HeadImageUrl3,";
	strStream << "UserId4,Nike4,Score4,HeadImageUrl4 ";
	strStream << "from team_log ";
	strStream << "where TeamID=" << nTeamID << " ";
	if (nUserId > 0)
	{
		strStream << "and (UserId1=" << nUserId << " or UserId2=" << nUserId << " or UserId3=" << nUserId << " or UserId4=" << nUserId << ") ";
	}

	strStream << "and Time>=" << time1 << " and Time<" << (time1 + 3600 * 24) << " ";
	strStream << "order by Time desc ";
	strStream << "limit " << nStartPage << "," << nPageSize;

	MYSQL* mysql = gWork.GetDbSession().GetMysql();

	if (mysql == NULL)
	{
		LLOG_ERROR("CHanderHttp::HanderHttpQueryLogList mysql null");
		return;
	}

	if (mysql_real_query(mysql, strStream.str().c_str(), strStream.str().size()))
	{
		LLOG_ERROR("CTeamManager::HanderHttpQueryLogList sql error %s", mysql_error(mysql));
		return;
	}

	MYSQL_RES* res = mysql_store_result(mysql);
	MYSQL_ROW row = mysql_fetch_row(res);

	if (!row)
	{
		// 没有查到记录 [2017-8-17 willing]
		std::stringstream strStream;
		strStream << "{";
		strStream << "\"errorMsg\":\"\",";
		strStream << "\"errorCode\":0,";
		strStream << "\"datas\":[]";
		strStream << "}";
		SendRet(strStream.str(), sp);
	}
	else
	{
		boost::property_tree::ptree all_log;

		while (row)
		{
			boost::property_tree::ptree log_item;
			log_item.put("id", atoi(*row++));
			log_item.put("deskId", atoi(*row++));
			log_item.put("optState", atoi(*row++));
			log_item.put("time", *row++);

			boost::property_tree::ptree userInfo[4];
			Lint nWinnerPos = -1;
			Lint nMaxScore = -999999999;
			for (int i = 0; i < 4; i++)
			{
				userInfo[i].put("userId", atoi(*row++));
				userInfo[i].put("nick", Lstring(*row++));
				Lint nScore = atoi(*row++);
				userInfo[i].put("score", nScore);
				userInfo[i].put("headImageUrl", Lstring(*row++));
				userInfo[i].put("isOwner", (i == 0) ? 1 : 0);

				if (nScore > nMaxScore)
				{
					nWinnerPos = i;
					nMaxScore = nScore;
				}
			}

			for (int i = 0; i < 4; i++)
			{
				if ((nMaxScore > 0) && (nWinnerPos == i))
				{
					userInfo[i].put("isWinner", 1);
				}
				else
				{
					userInfo[i].put("isWinner", 0);
				}
			}

			boost::property_tree::ptree allUserInfo;
			for (int i = 0; i < 4; i++)
			{
				allUserInfo.push_back(std::make_pair("", userInfo[i]));
			}

			log_item.add_child("detailList", allUserInfo);

			all_log.push_back(std::make_pair("", log_item));
			row = mysql_fetch_row(res);
		}

		mysql_free_result(res);

		boost::property_tree::ptree root;
		root.put("errorMsg", "");
		root.put("errorCode", 0);
		root.add_child("datas", all_log);

		std::stringstream strStream;
		boost::property_tree::write_json(strStream, root);

		SendRet(strStream.str(), sp);
	}
}

void CHanderHttp::HanderHttpQueryWinnum(HttpParamMap& param, LSocketPtr sp)
{
	if (param.empty() || NULL == sp)
	{
		return;
	}

	if (IsHaverParam(param, "teamId") == false)
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (IsHaverParam(param, "userId") == false)
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	if (IsHaverParam(param, "date") == false)
	{
		SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	Lint nUserId = GetHttpParamLint(param, "userId");
	Lint nTeamID = GetHttpParamLint(param, "teamId");
	Lstring strDate = GetHttpParamString(param, "date");

	struct tm tm1;

	int i = sscanf(strDate.c_str(), "%d-%d-%d",
		&(tm1.tm_year),
		&(tm1.tm_mon),
		&(tm1.tm_mday));

	tm1.tm_year -= 1900;
	tm1.tm_mon--;
	tm1.tm_isdst = -1;
	tm1.tm_hour = 0;
	tm1.tm_min = 0;
	tm1.tm_sec = 0;

	time_t time1;
	time1 = mktime(&tm1);

	// 构造SQL [2017-8-17 willing]
	std::stringstream strStream;
	strStream << "select OptState,UserId1,UserId2,UserId3,UserId4,";
	strStream << "Score1,Score2,Score3,Score4 ";
	strStream << "from team_log ";
	strStream << "where TeamID=" << nTeamID << " ";
	strStream << "and (UserId1=" << nUserId << " or UserId2=" << nUserId << " or UserId3=" << nUserId << " or UserId4=" << nUserId << ") ";
	strStream << "and Time>=" << time1 << " and Time<" << (time1 + 3600 * 24) << " ";
	strStream << "order by Time desc;";

	MYSQL* mysql = gWork.GetDbSession().GetMysql();

	if (mysql == NULL)
	{
		LLOG_ERROR("CHanderHttp::HanderHttpQueryLogList mysql null");
		return;
	}

	if (mysql_real_query(mysql, strStream.str().c_str(), strStream.str().size()))
	{
		LLOG_ERROR("CTeamManager::HanderHttpQueryLogList sql error %s", mysql_error(mysql));
		return;
	}

	MYSQL_RES* res = mysql_store_result(mysql);
	MYSQL_ROW row = mysql_fetch_row(res);

	if (!row)
	{
		// 没有查到记录 [2017-8-17 willing]
		std::stringstream strStream;
		strStream << "{";
		strStream << "\"errorMsg\":\"\",";
		strStream << "\"errorCode\":" << EHEEC_NOT_FOUND_USER << ",";
		strStream << "\"datas\":{\"winnerNum\":0,\"notIptNum\":0}";
		strStream << "}";
		SendRet(strStream.str(), sp);
	}
	else
	{
		Lint nWinnerCount = 0;
		Lint nOptStateCount = 0;
		while (row)
		{
			Lint nOptState = atoi(*row++);
			if (nOptState == 0)
			{
				nOptStateCount++;
			}

			Lint nUserIdArray[4] = { 0 };
			for (int i = 0; i < 4; i++)
			{
				nUserIdArray[i] = atoi(*row++);
			}

			Lint nScore[4] = { 0 };
			for (int i = 0; i < 4; i++)
			{
				nScore[i] = atoi(*row++);
			}

			Lint nMaxScore = -999999999;
			Lint nWinerUserID = 0;

			for (int i = 0; i < 4; i++)
			{
				if (nScore[i] > 0 && nScore[i] > nMaxScore)
				{
					nMaxScore = nScore[i];
					nWinerUserID = nUserIdArray[i];
				}
			}

			if (nWinerUserID == nUserId)
			{
				nWinnerCount++;
			}

			row = mysql_fetch_row(res);
		}
		mysql_free_result(res);

		boost::property_tree::ptree root;
		root.put("errorMsg", "");
		root.put("errorCode", 0);
		boost::property_tree::ptree datas_item;
		datas_item.put("winnerNum", nWinnerCount);
		datas_item.put("notIptNum", nOptStateCount);

		root.add_child("datas", datas_item);

		std::stringstream strStream;
		boost::property_tree::write_json(strStream, root);

		SendRet(strStream.str(), sp);
	}
}

Lstring	CHanderHttp::GetHttpParamString(HttpParamMap& param, const Lstring& strKey)
{
	if (strKey.empty())
		return "";

	HttpParamMap::iterator iter = param.find(strKey);
	if (iter != param.end() && !iter->second.empty())
	{
		return iter->second;
	}

	return "";
}

Lint CHanderHttp::GetHttpParamLint(HttpParamMap& param, const Lstring& strKey)
{
	if (strKey.empty())
		return -1;

	HttpParamMap::iterator iter = param.find(strKey);
	if (iter != param.end() && !iter->second.empty())
	{
		return atoi(iter->second.c_str());
	}

	return -1;
}

Lint CHanderHttp::GetHttpParamLlong(HttpParamMap& param, const Lstring& strKey)
{
	if (strKey.empty())
		return -1;

	HttpParamMap::iterator iter = param.find(strKey);
	if (iter != param.end() && !iter->second.empty())
	{
		return atol(iter->second.c_str());
	}

	return -1;
}

bool CHanderHttp::IsHaverParam(HttpParamMap& param, const Lstring& strKey)
{
	return (param.find(strKey) != param.end());
}

