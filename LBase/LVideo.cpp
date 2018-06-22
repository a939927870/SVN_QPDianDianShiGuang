#include "LVideo.h"
#include "GameDefine.h"

bool operator== (const CardValue& card1, const CardValue& card2)
{
	return (card1.m_color == card2.m_color && card1.m_number == card2.m_number);
}

VideoLog::VideoLog() : m_Id("")
{
	memset(m_userId, 0, sizeof(m_userId));
	m_time = 0;
	m_curCircle = 0;
	m_maxCircle = 0;
	m_deskId = 0;
	memset(m_score,0,sizeof(m_score));
	memset(m_calScore, 0, sizeof(m_calScore));
	m_flag = 0;
	m_zhuang = 0;
	m_type = EMGameLogicType_DouDiZhuQP;
}

VideoLog::~VideoLog()
{

}

void VideoLog::AddOper(Lint oper, Lint pos, const std::vector<Lint>& cards)
{
	VideoOper o;
	o.m_oper = oper;
	o.m_pos = pos;
	for (size_t i = 0; i < cards.size(); i ++)
	{
		o.m_cards.push_back(cards[i]);
	}
	m_oper.push_back(o);
}


void VideoLog::AddOper(Lint oper, Lint pos, const std::vector<CardValue>& cards)
{
	VideoOper2 o;
	o.m_oper = oper;
	o.m_pos = pos;
	for (int i = 0; i < cards.size(); i++)
	{
		o.m_cards.push_back(cards[i]);
	}
	m_oper2.push_back(o);
}


void VideoLog::AddOper(Lint oper, Lint pos, const std::vector<Lint>& cards, const std::vector<Lint>& addCards)
{
	VideoOper o;
	o.m_oper = oper;
	o.m_pos = pos;
	for (size_t i = 0; i < cards.size(); i ++)
	{
		o.m_cards.push_back(cards[i]);
	}
	for (size_t i = 0; i < addCards.size(); i ++)
	{
		o.m_addCards.push_back(addCards[i]);
	}
	m_oper.push_back(o);
}

void VideoLog::DealCard(Lint* userId, std::vector<Lint>* vec,Lint time,Lint zhuang, Ldouble* score,Lint deskId,Lint cur,Lint max,Lint flag, std::vector<Lint>& playType)
{
	Lint userCount = GetUserCounts();
	for (Lint i = 0; i < userCount; ++i)
	{
		m_handCard[i] = vec[i];
		m_userId[i] = userId[i];
		m_score[i] = score[i];
	}
	m_time = time;
	m_deskId = deskId;
	m_maxCircle = max;
	m_curCircle = cur;
	m_zhuang = zhuang;
	m_flag = flag;
	m_oper.clear();
	m_oper2.clear();
	m_type = GetGameLogicTypeByFlag(m_flag);

	m_playType.clear();
	for (size_t j = 0; j < playType.size(); ++j)
	{
		m_playType.push_back(playType[j]);
	}
}

void VideoLog::DealCard(Lint* userId, std::vector<CardValue>* vec, Lint time, Lint zhuang, Ldouble* score, Lint deskId, Lint cur, Lint max, Lint flag, std::vector<Lint>* playType)
{
	memcpy(m_userId, userId, sizeof(m_userId));
	memcpy(m_score, score, sizeof(m_score));
	for (Lint i = 0; i < 4; ++i)
	{
		m_handCard2[i] = vec[i];
	}
	m_time = time;
	m_deskId = deskId;
	m_maxCircle = max;
	m_curCircle = cur;
	m_zhuang = zhuang;
	m_flag = flag;
	m_type = GetGameLogicTypeByFlag(m_flag);
	auto itype = playType->begin();
	for (; itype != playType->end(); ++itype)
	{
		m_playType.push_back(*itype);
	}
}


Lstring	VideoLog::ToString()
{
	switch (m_type)
	{
	case EMGameLogicType_DouDiZhuQP:
		return ToString1();
		break;
	case EMGameLogicType_ChangShaMJ:
		return ToString2();
		break;
	case EMGameLogicType_GuangDongMJ:
		return ToString2();
		break;
	default:
		break;
	}
	return ToString2();
}

Lstring	VideoLog::ToString1()
{
	std::stringstream ss;
	Lint userCount = GetUserCounts();
	for (Lint i = 0; i < userCount; ++i)
	{
		for (size_t j = 0; j < m_handCard[i].size(); ++j)
		{
			ss << m_handCard[i][j];
			if (j + 1 == m_handCard[i].size())
			{
				ss << ";";
			}
			else
			{
				ss << ",";
			}
		}
	}

	for (size_t i = 0; i < m_oper.size(); ++i)
	{
		ss << m_oper[i].ToString();
	}
	return ss.str();
}

Lstring	VideoLog::ToString2()
{
	std::stringstream ss;

	for (Lint i = 0; i < 4; ++i)
	{
		for (Lint j = 0; j < m_handCard[i].size(); ++j)
		{
			ss << m_handCard2[i][j].m_color << "," << m_handCard2[i][j].m_number;
			if (j + 1 == m_handCard[i].size())
			{
				ss << ";";
			}
			else
			{
				ss << ",";
			}
		}
	}

	for (Lint i = 0; i < m_oper.size(); ++i)
	{
		ss << m_oper2[i].ToString();
	}
	return ss.str();
}

void VideoLog::FromString(const Lstring& str)
{
	std::vector<Lstring> des,subDes;
	L_ParseString(str, des, ";");
	
	Lint userCount = GetUserCounts();

	if (des.size() >= (size_t)userCount)
	{
		for (Lint i = 0; i < userCount; ++i)
		{
			subDes.clear();
			L_ParseString(des[i], subDes, ",");
			for (size_t j = 0; j < subDes.size(); j+=1)
			{
				Lint v;
				v = atoi(subDes[j].c_str());
				m_handCard[i].push_back(v);
			}
		}

		for (size_t i = userCount; i < des.size(); ++i)
		{
			VideoOper o;
			o.FromString(des[i]);
			m_oper.push_back(o);
		}
	}
}

void VideoLog::FromString(Lstring str, Lint user_count)
{
	std::vector<Lstring> des, subDes;
	L_ParseString(str, des, ";");
	if (des.size() >= user_count)
	{
		for (Lint i = 0; i < user_count; ++i)
		{
			subDes.clear();
			L_ParseString(des[i], subDes, ",");
			for (Lint j = 0; j + 1 < subDes.size(); j += 2)
			{
				CardValue v;
				v.m_color = atoi(subDes[j].c_str());
				v.m_number = atoi(subDes[j + 1].c_str());
				m_handCard2[i].push_back(v);
			}
		}

		for (Lint i = user_count; i < des.size(); ++i)
		{
			VideoOper2 o;
			o.FromString(des[i]);
			if (o.m_oper != 0)
			{
				m_oper2.push_back(o);
			}
		}
	}
}

void VideoLog::Write(LBuff& buff)
{
	buff.Write(m_Id);
	buff.Write(m_time);
	buff.Write(m_userId[0]);
	buff.Write(m_userId[1]);
	buff.Write(m_userId[2]);
	buff.Write(m_userId[3]);
	buff.Write(m_zhuang);
	buff.Write(m_deskId);
	buff.Write(m_curCircle);
	buff.Write(m_maxCircle);
	buff.Write(m_score[0]);
	buff.Write(m_score[1]);
	buff.Write(m_score[2]);
	buff.Write(m_score[3]);
	buff.Write(m_calScore[0]);
	buff.Write(m_calScore[1]);
	buff.Write(m_calScore[2]);
	buff.Write(m_calScore[3]);
	buff.Write(m_flag);

	buff.Write(m_str);
	buff.Write(m_str_playtype);
}

void VideoLog::Read(LBuff& buff)
{
	buff.Read(m_Id);
	buff.Read(m_time);
	buff.Read(m_userId[0]);
	buff.Read(m_userId[1]);
	buff.Read(m_userId[2]);
	buff.Read(m_userId[3]);
	buff.Read(m_zhuang);
	buff.Read(m_deskId);
	buff.Read(m_curCircle);
	buff.Read(m_maxCircle);
	buff.Read(m_score[0]);
	buff.Read(m_score[1]);
	buff.Read(m_score[2]);
	buff.Read(m_score[3]);
	buff.Read(m_calScore[0]);
	buff.Read(m_calScore[1]);
	buff.Read(m_calScore[2]);
	buff.Read(m_calScore[3]);
	buff.Read(m_flag);
	m_str.clear();
	buff.Read(m_str);
	m_str_playtype.clear();
	buff.Read(m_str_playtype);
}

void VideoLog::Clear()
{
	m_time = 0;
	m_Id = "";
	memset(m_userId, 0, sizeof(m_userId));
	for (Lint i = 0; i < 4; ++i)
	{
		m_handCard[i].clear();
		m_handCard2[i].clear();
	}
	m_oper.clear();
	m_oper2.clear();
	m_str.clear();
	m_str_playtype.clear();
}

Lstring VideoLog::GetInsertSql()
{
	std::stringstream ss;
	ss << "INSERT INTO Video (Id,Time,UserId1,UserId2,UserId3,UserId4,Zhuang,DeskId,MaxCircle,CurCircle,Score1,Score2,Score3,Score4,CalScore1,CalScore2,CalScore3,CalScore4,Flag,Data, PlayType) VALUES (";
	ss << "'" << m_Id << "',";
	ss << "'" << m_time << "',";
	ss << "'" << m_userId[0] << "',";
	ss << "'" << m_userId[1] << "',";
	ss << "'" << m_userId[2] << "',";
	ss << "'" << m_userId[3] << "',";

	ss << "'" << m_zhuang << "',";
	ss << "'" << m_deskId << "',";
	ss << "'" << m_maxCircle << "',";
	ss << "'" << m_curCircle << "',";
	ss << "'" << std::to_string(m_score[0]) << "',";
	ss << "'" << std::to_string(m_score[1]) << "',";
	ss << "'" << std::to_string(m_score[2]) << "',";
	ss << "'" << std::to_string(m_score[3]) << "',";
	ss << "'" << std::to_string(m_calScore[0]) << "',";
	ss << "'" << std::to_string(m_calScore[1]) << "',";
	ss << "'" << std::to_string(m_calScore[2]) << "',";
	ss << "'" << std::to_string(m_calScore[3]) << "',";
	ss << "'" << m_flag << "',";

	ss << "'" << ToString() << "',";
	ss << "'" << PlayTypeToStrint() << "')";
	return ss.str();
}

Lint VideoLog::GetUserCounts()
{
	Lint nUserCounts = 3;
	switch (m_flag)
	{
	case game_ddz_2players:
		nUserCounts = 2;
		break;
	case game_ddz_sichuan_pz:
		nUserCounts = 4;
		break;
	case game_ddz_3lai:
	case game_ddz_sichuan:
	case game_ddz_classical:
	case game_wakeng_shanxi:
		nUserCounts = 3;
		break;
	}
	return nUserCounts;
}

Lstring VideoLog::PlayTypeToStrint()
{
	switch (m_type)
	{
	case EMGameLogicType_DouDiZhuQP:
		return PlayTypeToStrint1();
		break;
	case EMGameLogicType_ChangShaMJ:
		return PlayTypeToStrint2();
		break;
	case EMGameLogicType_GuangDongMJ:
		return PlayTypeToStrint2();
		break;
	default:
		break;
	}
	return PlayTypeToStrint2();
}

Lstring VideoLog::PlayTypeToStrint1()
{
	std::stringstream ss;

	for (Lsize i = 0; i + 1 < m_playType.size(); i += 2)
	{
		ss << m_playType[i] << ";";  //两个一组，分别为参数类型和参数值;
		ss << m_playType[i + 1] << ";";
	}

	return ss.str();
}

Lstring VideoLog::PlayTypeToStrint2()
{
	std::stringstream ss;

	for (Lint i = 0; i < m_playType.size(); ++i)
	{
		ss << m_playType[i] << ";";
	}
	return ss.str();
}

void VideoLog::PlayTypeFromString(const Lstring& str)
{
	std::vector<Lstring> des;
	L_ParseString(str, des, ";");
	if (des.size() > 0)
	{
		for(size_t j = 0 ; j < des.size(); ++j)
		{
			m_playType.push_back( atoi(des[j].c_str()));
		}
	}
}


//第一个参数指示玩家个数;
void VideoLog::SetEveryResult(int nUserCounts, ...)
{
	va_list argp;
	Lint argno = 0;

	va_start(argp, nUserCounts);
	while (argno < nUserCounts && argno < 4)
	{
		m_score[argno] = va_arg(argp, int);
		argno++;
	}
	va_end(argp);
}

//第一个参数指示玩家个数;
void VideoLog::SetEveryResult2(int nUserCounts, ...)
{
	va_list argp;
	Lint argno = 0;

	va_start(argp, nUserCounts);
	while (argno < nUserCounts && argno < 4)
	{
		m_calScore[argno] = va_arg(argp, int);
		argno++;
	}
	va_end(argp);
}

