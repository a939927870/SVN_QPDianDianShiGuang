#include "ActiveManager_SignCollect.h"
#include "ActiveManager.h"
#include "Work.h"

//注意：当字符串为空时，也会返回一个空字符串  
//void split(std::string& s, std::string& delim,std::vector< std::string >* ret)  
//{  
//	size_t last = 0;  
//	size_t index=s.find_first_of(delim,last);  
//	while (index!=std::string::npos)  
//	{  
//		ret->push_back(s.substr(last,index-last));  
//		last=index+1;  
//		index=s.find_first_of(delim,last);  
//	}  
//	if (index-last>0)  
//	{  
//		ret->push_back(s.substr(last,index-last));  
//	}  
//}

void ActiveMgr_SignCollect::ReSet()
{
	for(int i = 0; i < EMax ; ++i)
	{
		m_actives[i].clear();
	}

	for(int i = 0; i< EMax; ++i)
		m_isLoaded[i] = false;
};

bool ActiveMgr_SignCollect::IsActiveOpen(Lint date)
{
	if(IsActiveOpen(ECollectCard, date) || IsActiveOpen(ESign, date))
	{
		return true;
	}

	return false;
}

bool ActiveMgr_SignCollect::IsActiveOpen(ActiveType type, Lint date)
{
	Lint begin = atoi("20161001");
	Lint end = atoi("20161007");
	if (begin > 0 && end > 0)
	{
		if (date >= begin && date <= end)
		{
			return true;
		}
	}
	return false;
}

bool ActiveMgr_SignCollect::IsActiveLoaded(ActiveType type)
{
	if(!m_isLoaded[type]) return false;	//没有加载完

	return true;
}

bool ActiveMgr_SignCollect::IsSignByDate(Lint date)	//@1@2可以合并
{
	//判断自已今天是否抽过奖
	if (m_actives[ESign].size() > 0)
	{
		std::vector< LActiveSignCollectSS>::iterator itor = m_actives[ESign].begin();
		for (;itor != m_actives[ESign].end(); ++itor)
		{
			if ((itor)->m_date == date)
			{
				return true;
			}
		}
	}

	return false;
}

bool ActiveMgr_SignCollect::IsLeftDrawByDate(Lint date)
{
	auto& active = m_actives[ECollectCard];
	if (active.size() > 0)
	{
		if (active[0].m_drawChanceCount > 0)
		{
			return true;
		}
	}

	return false;
}

bool ActiveMgr_SignCollect::run(ActiveType type, Lint date, ActiveOutInfo & out)
{
	if(type == ESign)	//
	{
		return runSign(date, out);
	}
	else if(type == ECollectCard)
	{
		return runCollect(date, out);
	}

	return false;
}

bool ActiveMgr_SignCollect::runSign(Lint date, ActiveOutInfo & out)
{
	if(!IsActiveLoaded(ESign)) return false;	//没有初始化

	//写内存
	LActiveSignCollectSS active ;
	active.m_activeType = ESign;
	active.m_drawChanceCount = 0;
	active.m_date = date;
	m_actives[ESign].push_back(active);
	
	out.m_date = date;
	out.m_drawChanceCount = 0;
	out.m_activeType = ESign;

	return true;
}

bool ActiveMgr_SignCollect::runCollect(Lint date, ActiveOutInfo & out)
{
	if(!IsActiveLoaded(ECollectCard)) return false;	//没有初始化

	auto& active = m_actives[ECollectCard];

	if(active.size() <= 0) return false;	//没有打过局数

	if(active[0].m_drawChanceCount <= 0) return false; //没有机会

	if(gActiveManager.GetWinSize(ActiveManager::EAWin) >= ActiveManager::EWinMaxSize) return false;	//中奖人数超过了

	auto& words = active[0].m_words;
	Lint word = L_Rand(1, 8);

	//扣除一次机会
	DelChance(1);

	bool find = false;
	for(auto itor = words.begin(); itor != words.end(); ++itor)
	{
		if(*itor == word)
		{
			find = true;
		}
	}

	//
	out.m_activeType = active[0].m_activeType;
	out.m_drawChanceCount = active[0].m_drawChanceCount;
	out.m_hadGift = active[0].m_hadGift != 0;
	out.m_isUpdate = active[0].m_isUpdate;
	out.m_word = word;
	
	if(find)
	{	
		out.m_isNewWord = false;
	}
	else	//
	{	
		out.m_isNewWord = true;  //
		words.push_back(word);
	}

	out.m_words = words;

	return true;
}

//void ActiveMgr_SignCollect::AddWinInfo(Lint word)
//{
//	LActiveSignCollectSS info;
//	//info.m_nickName = "";//m_pUser->;
//	m_actives[EWin].push_back(info);
//}


void ActiveMgr_SignCollect::AddChance(Lint count)
{
	if(!GetLoaded(ECollectCard)) return;

	auto& active = m_actives[ECollectCard];
	Lint length = active.size();

	if(length > 0)
	{
		active[0].m_drawChanceCount += count;
		if(active[0].m_drawChanceCount > ECollectMax)
		{
			active[0].m_drawChanceCount = ECollectMax;
		}
		return;
	}
	else
	{
		//写内存(
		LActiveSignCollectSS activeInfo ;
		activeInfo.m_isUpdate = false;
		activeInfo.m_activeType = ECollectCard;
		activeInfo.m_drawChanceCount = count;
		activeInfo.m_date = INVAILD_VAL;	//不需要
		activeInfo.m_hadGift = 0;
		active.push_back(activeInfo);
		return ;
	}
}

void ActiveMgr_SignCollect::DelChance(Lint count)
{
	auto& active = m_actives[ECollectCard];
	Lint length = active.size();


	bool find = false;
	if(length > 0)
	{
		if(active[0].m_drawChanceCount - count >= 0)
		{
			active[0].m_drawChanceCount -= count;
			count = 0;
			return ;
		}
		else
		{
			count = count - active[0].m_drawChanceCount;
			active[0].m_drawChanceCount = 0;
		}
	}
}

bool ActiveMgr_SignCollect::CheckFinalGift(ActiveType type)
{
	if(type == ESign)
	{
		return CheckFinalSignGift();
	}
	else if(type == ECollectCard)
	{
		return CheckFinalCollectGift();
	}
	return false;
}

bool ActiveMgr_SignCollect::CheckFinalSignGift()
{
	auto& active = m_actives[ESign];
	Lint length = active.size();

	if(length != 7) return false;	//没签到到7天

	std::set<Lint> filterData;

	for(int i = 0; i< length; ++i)
	{
		filterData.insert(active[i].m_date);
	}

	if(filterData.size() == 7)	//时间和签到都符合
		return true;

	return false;
}

bool ActiveMgr_SignCollect::CheckFinalCollectGift()
{
	auto& active = m_actives[ECollectCard];
	Lint length = active.size();

	if(length <= 0) return false;	//没有集齐8个字
	//if(length > 1) return false;	//数据错误（只可能有一条）
	if(active[0].m_hadGift == 1) return false; //已经领过

	Lint count = 0;
	std::set<Lint> filterWorld;
	Lint nWorld = 0;

	length = active[0].m_words.size();
	if(length < 8) return false;

	for(int i = 0; i< length; ++i)
	{
		nWorld = active[0].m_words[i];
		if( nWorld> 0 && nWorld <= 8)	//1到8
		{
			filterWorld.insert(active[0].m_words[i]);
		}
	}

	if(filterWorld.size() == 8)	//长度和字数都合适
	{
		active[0].m_hadGift = 1;
		return true;
	}

	return false;
}

std::vector<ActiveMgr_SignCollect::ActiveType> ActiveMgr_SignCollect::GetActiveTypes()
{
	std::vector<ActiveType> ids;
	ids.push_back(ESign);
	ids.push_back(ECollectCard);
	return ids;
}

void ActiveMgr_SignCollect::LoadData(ActiveType type, std::vector<LActiveSignCollectSS> &actives)
{
	assert(type == ESign || type == ECollectCard);
	if(!(type == ESign || type == ECollectCard)) return;

	//ReSet();

	auto itor = actives.begin();
	for(; itor != actives.end(); ++itor)
	{
		m_actives[type].push_back(*itor);
	}

	m_isLoaded[type] = true;
}

std::vector<LActiveSignCollectSS> ActiveMgr_SignCollect::GetActive(ActiveType type)
{
	if(!(type == ESign || type == ECollectCard)) return std::vector<LActiveSignCollectSS>();

	return m_actives[type];
}

void ActiveMgr_SignCollect::SetLoaded(ActiveMgr_SignCollect::ActiveType type, bool isLoaded)
{
	if(!(type == ESign || type == ECollectCard)) return ;

	m_isLoaded[type] = isLoaded;
}

bool ActiveMgr_SignCollect::GetLoaded(ActiveType type)
{
	assert(type == ESign || type == ECollectCard);

	if(!(type == ESign || type == ECollectCard)) return false;

	return m_isLoaded[type];
}

void ActiveMgr_SignCollect::SaveToDb(Lint userId)
{
	auto& active = m_actives[ECollectCard];
	if(active.size() > 0)
	{
		LActiveSignCollectSS& info = active[0];
	
		//保存集字信息
		LMsgL2LDBSaveSignCollectWin syncColl;
		syncColl.m_isUpdate = info.m_isUpdate;
		syncColl.m_userId = userId;
		syncColl.m_activeType = ActiveMgr_SignCollect::ECollectCard;
		assert(info.m_drawChanceCount >= 0);
		syncColl.m_drawChanceCount = info.m_drawChanceCount;
		//assert(infoOut.m_word.size() > 0);
		syncColl.m_words = info.m_words;
		syncColl.m_date = info.m_date;
		syncColl.m_hadGift = info.m_hadGift;
		gWork.SendMsgToDb(syncColl);

		SetCollectUpdate(true);
	}
}

void ActiveMgr_SignCollect::SetCollectUpdate(bool isUpdate)
{
	auto& active = m_actives[ECollectCard];
	if(active.size() > 0)
	{
		LActiveSignCollectSS& info = active[0];
		info.m_isUpdate = isUpdate;
	}
}

bool ActiveMgr_SignCollect::GetCollectUpdate()
{
	auto& active = m_actives[ECollectCard];
	if(active.size() > 0)
	{
		LActiveSignCollectSS& info = active[0];
		return (info.m_isUpdate == 1);
	}

	return false;
}
