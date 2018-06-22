#ifndef _CARD_MANAGER3_PIZI_H_
#define _CARD_MANAGER3_PIZI_H_

#include "gr_card.h"
#include "LVideo.h"

//////////////////////////////////////////////////////////////////////////
class CardStyle_SiChuan3_PiZi
{
public:
	CardStyle_SiChuan3_PiZi();
	~CardStyle_SiChuan3_PiZi();

	void        SortCard(CardVec& vce);//���ư���С����;
	void        SortCardLaiziFront(CardVec& vce);//��ӷ�ǰ��;
	void        SortCardLaiziInsert(CardVec& vce);//����滻��;

	void        EarseCard(CardVec& des,CardVec& src);//ɾ��des������src����;
	void		CheckCardStyle(CardVec const& src,CardStyleNumberVec& style,bool last, BYTE currCardStyle = 0);//������з�������;
	bool		CheckCardStyle(CardVec const& src,BYTE style,BYTE& cardNumber);//��ĳ����������鵱ǰ����;
	int			CanBig(DWORD dsCount,BYTE dsStyle,BYTE dsNumber,CardStyleNumberVec& style);

	void		GetSingleDoubleThreeFour(CardVec const& vec,CardVec& single,CardVec& duizi,CardVec& three,CardVec& four);
	void		GetJokeNumber2(CardVec const& vec,CardVec& joke,CardVec& number2,CardVec& other);
	void		GetLaizi(CardVec const& vec,CardVec& laizi,CardVec& other);
	void		GetBombLaiziJokeNumber2(CardVec const& vec,CardVec& bomb,CardVec& laizi,CardVec& joke,CardVec& number2,CardVec& other);
	bool		CheckContinuous(std::vector<BYTE>& vec);
	
	bool        HasRocket(CardVec const& vec);
	bool		Has2Count4Bomb(CardVec const& vec);
	bool        CheckHasBomb(CardVec const& Src, Lint* nBombCard = NULL);//�鿴�������Ƿ���ը��;

	int			GetCardScore(CardVec& vec);
	bool        IsBiZhua(CardVec const& vec);	//�Ƿ��Ǳ�ץ��
	bool        IsBiDao(CardVec const& vec);	//�ж��Ƿ��Ǳص���

private:

	bool        IsRocket(CardVec const& vec,CardVec const& laizi, BYTE& cardType);	//�Ƿ���˫��;
	bool        IsSuperRocket(CardVec const& vec,CardVec const& laizi, BYTE& cardType);	//�Ƿ���˫��(��ը��;
	bool        IsDouble(CardVec const& vec,CardVec const& laizi,BYTE& cardType);		//����;
	bool        IsBomb1(CardVec const& vec,CardVec const& laizi,BYTE& cardType);		//����ը��;
	bool        IsBomb2(CardVec const& vec,CardVec const& laizi,BYTE& cardType);		//��Ӳը��;
	bool        IsBomb3(CardVec const& vec,CardVec const& laizi,BYTE& cardType);		//��ը��;
	bool		IsSuperBomb(CardVec const& vec,CardVec const& laizi,BYTE& cardNumber);
	bool		IsDoubleBomb(CardVec const& vec,CardVec const& laizi,BYTE& cardType);	//��ը��3333��4444)
	bool        IsBombToSingle(CardVec const& vec,CardVec const& laizi,BYTE& cardType);	//�Ĵ���;
	bool        IsBombToDouble(CardVec const& vec,CardVec const& laizi,BYTE& cardType);	//�Ĵ���;
	bool        IsThree(CardVec const& vec,CardVec const& laizi,BYTE& cardType);		//����;
	bool        IsThreeToSingle(CardVec const& vec,CardVec const& laizi,BYTE& cardType);//������;
	bool		IsThreeToDouble(CardVec const& vec,CardVec const& laizi,BYTE& cardType);//������;
	bool        IsThreeList(CardVec const& vec,CardVec const& laizi,BYTE& cardType);		//�ɻ�;
	bool        IsThreeListToSingle(CardVec const& vec,CardVec const& laizi,BYTE& cardType);//�ɻ�����;
	bool		IsThreeListToDouble(CardVec const& vec,CardVec const& laizi,BYTE& cardType);//�ɻ�����;
	bool        IsSingleList(CardVec const& vec,CardVec const& laizi,BYTE& cardType);//�Ƿ��ǵ�˳;
	bool        IsDoubleList(CardVec const& vec,CardVec const& laizi,BYTE& cardType);//�Ƿ���˫˳;

public:
	static CardStyle_SiChuan3_PiZi* Instance();
};

///////////////////////////////////////////////////////////////////////////
//���Ƶ���;
class CardDeal_SiChuan3_PiZi
{
public:
	CardDeal_SiChuan3_PiZi();
	~CardDeal_SiChuan3_PiZi();

	void		ClearLaizi();
	void		SetLaizi(BYTE bCardNumber);
	void		DealCard(CardVec * receivers, Lint receiversLength, CardValue* changeCard = NULL, Lint nChangeLength = 0);
	void        GetLeftCard(CardVec& leftCard);
	void        SS(Lint nCardPos, Lint nColor, Lint nNumber);         // ���Ƹ�������;
	void        TiaoPai(CardValue tiaopai[], Lint nLenth);                                            // ���ƺ���;

private:
	void		RandCard();
	void        RandCard_15cards();

private:
	Card		m_oCard[CARD_COUNT_DDZ + 1];
	CardVec		m_pCard;

	Lint		m_nSize;
};


////////////////////////////////////////////////////////////////////////////////////////////
class CardHelp_SiChuan3_PiZi
{
public:
	CardHelp_SiChuan3_PiZi();
	~CardHelp_SiChuan3_PiZi();

	//��ʾ;
	std::vector<std::vector<Lint>> HelpMe(Lint cardMode, Lint cardType, Lint cardCount, CardVec& handcard, std::vector<Lint>& m_curHelpUnusable, bool baojing);


	void	CheckOne(Lint cardType,CardVec& o,CardVec& t,CardVec& th, CardVec& f,std::vector<std::vector<Lint>>& ret,bool baojing);
	void	CheckTwo(Lint cardType,CardVec& o,CardVec& t,CardVec& th, CardVec& f, std::vector<std::vector<Lint>>& ret);
	void	CheckThreeToTwo(Lint cardType,CardVec& o,CardVec& t,CardVec& th, CardVec& f, std::vector<std::vector<Lint>>& ret);
	void	CheckThreeFlyToTwo(Lint cardType,CardVec& o,CardVec& t,CardVec& th, CardVec& f,CardVec& handcard,Lint count, std::vector<std::vector<Lint>>& ret);
	void	CheckOneList(Lint cardType,CardVec& o,CardVec& t,CardVec& th, CardVec& f,Lint count, std::vector<std::vector<Lint>>& ret);
	void	CheckTwoList(Lint cardType,CardVec& o,CardVec& t,CardVec& th, CardVec& f,Lint count, std::vector<std::vector<Lint>>& ret);
	void    CheckThreeToOne(Lint cardType,CardVec& o,CardVec& t,CardVec& th, CardVec& f, std::vector<std::vector<Lint>>& ret);
	void    CheckFourToTwo(Lint cardType,CardVec& o,CardVec& t,CardVec& th, CardVec& f, std::vector<std::vector<Lint>>& ret);
	void    CheckThree(Lint cardType,CardVec& o,CardVec& t,CardVec& th, CardVec& f, std::vector<std::vector<Lint>>& ret);
	void    CheckThreeList(Lint cardType,CardVec& o,CardVec& t,CardVec& th, CardVec& f,CardVec& handcard,Lint count, std::vector<std::vector<Lint>>& ret);
	void    CheckThreeFlyToSingle(Lint cardType,CardVec& o,CardVec& t,CardVec& th, CardVec& f,CardVec& handcard,Lint count, std::vector<std::vector<Lint>>& ret);
};
#endif