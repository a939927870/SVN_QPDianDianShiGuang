#ifndef __InvitingFriendTask_
#define __InvitingFriendTask_

#include <vector>
#include "PlayerTaskBase.h"

using namespace std;

class InvitingFriendTask : public PlayerTaskBase
{
public:
	InvitingFriendTask(const PlayerTaskAttribute& attribute, User* doTaskUser);

public:
	virtual void			AttachEvents(PlayerTaskManager& mgr);
	virtual bool			IsExplicit(void) { return false; }
	virtual bool			Process(LMsg* msg);

private:
	ErrorCode::ErrorCode					OnBindedByInvitee(Lint inviteeId);
	void						OnPlayingNumIncreased(void);
	void						OnInviteeFinishedTask(Lint inviteeId);
	void						LoadBindingRelationships(void);
	bool						GetUserBasicInfo(Lint userId, UserBasicInfo & info);
	bool						Process(LMsgLDB2LMGBindingRelationships* msg);
	bool						Process(LMsgC2SGetInvitingInfo* msg);
	bool						Process(LMsgC2SBindInviter* msg);
	bool						Process(LMsgLDB2LMGInsertBindingRelationship * msg);
	void						InsertBindingRelationshipIntoDB(void);
	void						SetRewardedUser(Lint userId);

private:
	virtual void				OnFinished(void);

private:
	Lint						m_inviterId;
	bool						m_taskFinished;

	typedef struct
	{
		Lint		inviteeId;
		bool		taskFinished;
	}InvitingContext;

	vector<InvitingContext>					m_invitingContext;

	bool									m_invitingInfoLoaded;
	bool									m_inBinding;
};

#endif		// __InvitingFriendTask_