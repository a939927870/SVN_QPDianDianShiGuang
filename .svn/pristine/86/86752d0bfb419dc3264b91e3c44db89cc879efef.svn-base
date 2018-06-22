#include <algorithm>
#include <boost/filesystem.hpp>
#include "PlayerTaskAttributeCollection.h"
#include "LTabFile.h"
#include "LLog.h"
#include "../Work.h"

using namespace std;

#define PATH_TASKATTRIBUTEFILE		"settings/tasks.txt"

PlayerTaskAttributeCollection::PlayerTaskAttributeCollection(void)
{
	LoadTaskAttributes();
	boost::system::error_code ec;
	m_lastModifiedTime = boost::filesystem::last_write_time(PATH_TASKATTRIBUTEFILE, ec);
	if (ec)
		m_lastModifiedTime = 0;
}

PlayerTaskAttributeCollection * PlayerTaskAttributeCollection::Instance(void)
{
	static PlayerTaskAttributeCollection	instance;
	return &instance;
}

bool PlayerTaskAttributeCollection::CheckUpdates(void)
{
	bool updated = false;
	boost::system::error_code ec;
	std::time_t lastModifiedTime = boost::filesystem::last_write_time(PATH_TASKATTRIBUTEFILE, ec);
	if (!ec && m_lastModifiedTime != lastModifiedTime)
	{
		LoadTaskAttributes();
		m_lastModifiedTime = lastModifiedTime;
		updated = true;
	}
	else
		LLOG_DEBUG("There are no updates in task attribute collection.");

	return updated;
}

Lsize PlayerTaskAttributeCollection::GetTaskNum(void) const
{
	return m_tasks.size();
}

const PlayerTaskAttribute& PlayerTaskAttributeCollection::GetTask(Lsize index) const
{
	if (index >= 0 && index < m_tasks.size())
		return m_tasks[index];
	else
		throw new std::invalid_argument("index exceeds collection's size");
}

void PlayerTaskAttributeCollection::LoadTaskAttributes(void)
{
	LTabFile file;
	if (file.Load(PATH_TASKATTRIBUTEFILE))
	{
		LLOG_DEBUG("Loading task attributes from file %s", PATH_TASKATTRIBUTEFILE);
		m_tasks.clear();
		auto line = move(file.getTapFileLine());
		while (true)
		{
			if (!line.empty())
			{
				if(line.size() >= 12)
				{
					PlayerTaskAttribute attribute;
					attribute.id = atoi(line[0].c_str());
					attribute.type = ParseType(line[1].c_str());
					attribute.desc = line[2];
					attribute.duration = ParseDuration(line[3].c_str());
					attribute.total = atoi(line[4].c_str());
					attribute.reward = ParseRewardType(line[5].c_str());
					attribute.rewardCount = atoi(line[6].c_str());
					attribute.repeated = atoi(line[7].c_str()) != 0;
					attribute.recvManually = atoi(line[8].c_str()) != 0;
					attribute.valid = atoi(line[9].c_str()) != 0;
					attribute.startDate = atoi(line[10].c_str());
					attribute.endDate = atoi(line[11].c_str());
					if (line.size() >= 13)
						attribute.extParams = line[12];
					else
						attribute.extParams.clear();
					if(Validate(attribute))
						m_tasks.push_back(move(attribute));
					line = move(file.getTapFileLine());
				}
				else
				{
					LLOG_ERROR("file tasks.txt damanged!");
					break;
				}
			}
			else
				break;
		}

		LLOG_DEBUG("%d tasks were loaded.", m_tasks.size());
	}
}

PlayerTaskAttribute::TaskType PlayerTaskAttributeCollection::ParseType(const Lstring & str)
{
	PlayerTaskAttribute::TaskType type = PlayerTaskAttribute::TaskNone;
	Lstring lower(str);
	transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
	if (lower == "playinggame")
		type = PlayerTaskAttribute::PlayingGame;
	else if (lower == "usershared")
		type = PlayerTaskAttribute::UserShared;
	else if (lower == "invitingfriend")
		type = PlayerTaskAttribute::InvitingFriend;
	else if (lower == "completinggame")
		type = PlayerTaskAttribute::CompletingGame;
	return type;
}

PlayerTaskAttribute::TaskDuration PlayerTaskAttributeCollection::ParseDuration(const Lstring & str)
{
	PlayerTaskAttribute::TaskDuration duration = PlayerTaskAttribute::Invalid;
	Lstring lower(str);
	transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
	if (lower == "day")
		duration = PlayerTaskAttribute::Day;
	else if (lower == "week")
		duration = PlayerTaskAttribute::Week;
	else if (lower == "unlimited")
		duration = PlayerTaskAttribute::Unlimited;
	return duration;
}

PlayerTaskAttribute::RewardType PlayerTaskAttributeCollection::ParseRewardType(const Lstring & str)
{
	PlayerTaskAttribute::RewardType type = PlayerTaskAttribute::RewardNone;
	Lstring lower(str);
	transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
	if (lower == "coupon")
		type = PlayerTaskAttribute::Coupon;
	else if (lower == "luckydraw")
		type = PlayerTaskAttribute::LuckyDraw;
	else if (lower == "card")
		type = PlayerTaskAttribute::Card;
	return type;
}

bool PlayerTaskAttributeCollection::Validate(const PlayerTaskAttribute& attribute)
{
	bool valid = false;
	if (attribute.valid)
	{
		valid = (attribute.id > 0 && attribute.type != PlayerTaskAttribute::TaskNone && !attribute.desc.empty() &&
				attribute.duration != PlayerTaskAttribute::Invalid && attribute.total > 0 &&
				attribute.reward != PlayerTaskAttribute::RewardNone && attribute.rewardCount > 0);
	}
	return valid;
}
