#ifndef    _DDZ_HANDLER_CLASSICAL_H_
#define    _DDZ_HANDLER_CLASSICAL_H_

#include "GameHandler.h"
#include "Desk.h"
#include "gr_card_manager.h"
#include "LTime.h"
#include "LVideo.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "Work.h"

#define USER_COUNT_CLASSICAL (3)

class GameHandler_DDZ_Classical : public GameHandler
{
public:
	bool startup(Desk *desk);
	void SetDeskDeal();
	void CheckSameIP();
	void QiangDiZhu(Lint nPos);
	void HanderUserQiangDiZhu(User* pUser,LMsgC2SQiangDiZhu* msg);
	void HanderUserPlayCard(User* pUser,LMsgC2SUserPlay* msg);
	void SetDeskPlay();
	bool OnUserReconnect(User* pUser);
	void dealCard_new();
	void ProcessRobot(Lint pos, User * pUser);

	//3��������״̬
	enum UsersJiaoState
	{
		EWait,
		EOk,
		EAllFail,
	};
	enum JiaoState
	{
		EJIao,
		EQiang,
		EReady,
		ERefuse,
	};

	enum ClassicalSubType
	{
		EClassical,
		EJiaoFen,
		EJIaoDizhu,
	};

	//////////////////////////////////////////////////////////////////////////
protected:
	void StartQiangDizhu();
	
	void clearHandler(void);
	void _clearGame();
	Lint getDeskState();

	void NotPlayCard(Lint pos);
	void PlayCard(Lint pos, CardVec& card, Lchar card_mode, Lchar card_type);
	void CheckPlayState(Lint pos);
	bool CheckPlayEnd(Lint pos);
	void SetDeskEnd(Lchar pos);
	void SetGoldDeskEnd(Lchar pos);
	void SendPlayInfo(bool islast = false);
	void SendPlayCard(Lchar pos, Lchar ret, Lchar action, CardVec& vec, bool isFirstOut = false);

	///////////////////////////////////////////// ����desk����; //////////////////////////////////////////////////
protected:
	void BoadCast(LMsg& msg){ m_pDesk->BoadCast(msg); }
	void BoadCastWithOutUser(LMsg& msg, User* user){ m_pDesk->BoadCastWithOutUser(msg, user); }
	Lint GetUserPos(User* user){ return m_pDesk->GetUserPos(user); }
	Lint GetNextPos(Lint m_curPos){ return m_pDesk->GetNextPos(m_curPos); }
	Lint GetPrevPos(Lint m_curPos){ return m_pDesk->GetPrevPos(m_curPos); }

	void SetDeskWait(){ m_pDesk->SetDeskWait(); }
	void ClearUser(){ m_pDesk->ClearUser(); };
	void SetDeskFree(){m_pDesk->SetDeskFree();}
	void ResetClear(){m_pDesk->ResetClear();}

	//
	//��ͬ��֧
	void HanderUserQiangDiZhuOfClassical(User* pUser,LMsgC2SQiangDiZhu* msg); //
	void HanderUserQiangDiZhuOfJiaofen(User* pUser,LMsgC2SQiangDiZhu* msg);	//�з�
	void HanderUserQiangDiZhuOfJiaoDizhu(User* pUser,LMsgC2SQiangDiZhu* msg);	//�е���

	//��ʼ����������(��ͬ��֧��
	void StartQiangDizhuOfClassical();
	void StartQiangDizhuOfJiaofen();
	void StartQiangDizhuOfJiaoDizhu();


	//
	Lint GetNextValidPos(Lint pos);
	UsersJiaoState CheckUsersJiaoDizhuState();	//����Ƿ���ɽе�������
	void DecideDizhu(Lint pos);
	void ReStartJiaoDizhu(Lint pos);
	void BroadCastJiaoDizhu(Lint pos, BYTE yaobu, BYTE restart);	//�㲥��ǰ��ҽе���
	void RecordVideoQiangDizhu(Lint pos, bool isQiang, const std::vector<Lint> & vec = std::vector<Lint>());

private:
	Desk* m_pDesk;

private:
	//User*			m_user[USER_COUNT_CLASSICAL];
	//Lint            m_nUserCounts;
	Lint            m_nCardCounts;

//	PK_GAME_STYLE   m_gameStyle;      // ��Ϸ����;
	Lint            m_nFirstCardIndex;  // ȷ��ׯ��λ�õ���������;û������ʱΪ����3��������ʱ������������������к�������С��һ��;
	Lint            m_nDiZhuPos;       //����λ��;
//	Lint            m_nMaxFanshu;      //��Ϸ��󷭱���;
	Lint            m_nDifen;          //��Ϸ�׷�;
	Lint            m_nDizhuPlayCounts;//�������ƴ��������ڼ���ũ����;
	Lint            m_nQiangDizhuState;//�������A�Σ�0�Ǔ������A�Σ�1��ץ�A�Σ�2ץ��ץ�A�Σ�3���A�Σ�4���A��;

	Lint            m_menzhua[USER_COUNT_CLASSICAL];
	Lint            m_daobudao[USER_COUNT_CLASSICAL];
	Lint            m_labula[USER_COUNT_CLASSICAL];
	Lint            m_nUserBeishu[USER_COUNT_CLASSICAL];
	JiaoState		m_jiaoDizhu[USER_COUNT_CLASSICAL];	//�е����׶ε�״̬ 0����״̬ 1����״̬ 2��׼���ã� 3������

	CardVec         m_putCard;//��ȥ����;
	CardVec         m_handCard[USER_COUNT_CLASSICAL];//���ϵ��� ;
	CardVec         m_leftCard;//����������;

	CardVec			m_outCard[USER_COUNT_CLASSICAL];

	Lchar           m_playerCardType[USER_COUNT_CLASSICAL];//��������;
	Lchar           m_curCardType;
	Lchar           m_curCardMode;
	Lchar           m_check;
	Lchar			m_curPlayCount;
	Lchar           m_win;
//	Lchar           m_readyState[USER_COUNT_CLASSICAL];//���׼��״̬;

	Lchar           m_bombCount;//ը������;
	Lchar			m_firstPos;// ��������λ��,��һ��Ϊ���������Ϊ�Ͼ�Ӯ��;

	Lint			m_bomb[USER_COUNT_CLASSICAL];

	CardDeal		m_dealCard;

//	Lint			m_deskState;//����״̬;

	Lint			m_curPos;//��ǰ�������;

	Llong			m_ticktime;

	//VipLogItem*		m_vip;

	//Lint			m_reset[USER_COUNT_CLASSICAL];
	//Lstring			m_resetUser;//��������;
	//Lint			m_resetTime;//��������ʱ��;

	//Lint            m_black3first;     //��һ�ֳ�������Ƿ���Բ�������3��1Ϊ���Բ�����0Ϊ���������3;
	//Lint            m_tiaopaishu;       //������ҵ���������0����������;
	//CardValue       m_specialCard[56];  //�洢����ض������ƾ�;

	//VideoLog		m_video;//¼��;

	CardHelp		m_help;
	std::vector<std::vector<Lint>>			m_curHelpInfo;
	std::vector<Lint>                       m_curHelpUnusable;
	Lint			m_baojing[USER_COUNT_CLASSICAL];

	ClassicalSubType m_subType;		//0, ����(�У��� 1���з� 2��������
};

DECLARE_GAME_HANDLER_CREATOR(game_ddz_classical, GameHandler_DDZ_Classical)

#endif