#ifndef _CARD_MANAGER_H_
#define _CARD_MANAGER_H_

#include "gr_card.h"
#include "LVideo.h"

//////////////////////////////////////////////////////////////////////////
class CardStyle
{
public:
	CardStyle();
	~CardStyle();

	void        SortCard(CardVec& vce);//���ư���С����;
	void        SortCardLaiziFront(CardVec& vce);//��ӷ�ǰ��;
	void        SortCardLaiziInsert(CardVec& vce);//����滻��;

	void        EarseCard(CardVec& des,CardVec& src);//ɾ��des������src����;
	void		CheckCardStyle(CardVec const& src,CardStyleNumberVec& style,bool last);//������з�������;
	bool		CheckCardStyle(CardVec const& src,BYTE style,BYTE& cardNumber);//��ĳ����������鵱ǰ����;
	int			CanBig(DWORD dsCount,BYTE dsStyle,BYTE dsNumber,CardStyleNumberVec& style);
	bool        CheckHasBomb(CardVec const& Src, Lint* nBombCard = NULL);//�鿴�������Ƿ���ը��;

	void		GetSingleDoubleThreeFour(CardVec const& vec,CardVec& single,CardVec& duizi,CardVec& three,CardVec& four);
	void		GetJokeNumber2(CardVec const& vec,CardVec& joke,CardVec& number2,CardVec& other);
	void		GetLaizi(CardVec const& vec,CardVec& laizi,CardVec& other);
	void		GetBombLaiziJokeNumber2(CardVec const& vec,CardVec& bomb,CardVec& laizi,CardVec& joke,CardVec& number2,CardVec& other);
	bool		CheckContinuous(std::vector<BYTE>& vec);
	bool        HasRocket(CardVec const& vec);

	bool        IsBiZhua(CardVec const& vec);
	bool        IsBiDao(CardVec const& vec);

	int			GetCardScore(CardVec& vec);
private:

	bool        IsRocket(CardVec const& vec,BYTE& cardType);	//�Ƿ���˫��;
	bool        IsDouble(CardVec const& vec,CardVec const& laizi,BYTE& cardType);		//����;
	bool        IsBomb1(CardVec const& vec,CardVec const& laizi,BYTE& cardType);		//����ը��;
	bool        IsBomb2(CardVec const& vec,CardVec const& laizi,BYTE& cardType);		//��Ӳը��;
	bool        IsBomb3(CardVec const& vec,CardVec const& laizi,BYTE& cardType);		//��ը��;
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
	static CardStyle* Instance();
};

///////////////////////////////////////////////////////////////////////////
//���Ƶ���;
class CardDeal
{
public:
	CardDeal();
	~CardDeal();

	void		ClearLaizi();
	void		SetLaizi(BYTE bCardNumber);
	void		DealCard(CardVec& vec1, CardVec& vec2, CardVec& vec3, CardValue* tiaopai = NULL, Lint nLenth = 0);
	void		DealCard_2Player(CardVec& vec1, CardVec& vec2, CardValue* tiaopai = NULL, Lint nLenth = 0);
	void        GetLeftCard_2Player(CardVec& leftCard);
	void        GetLeftCard(CardVec& leftCard);
	void        SS(Lint nCardPos, Lint nColor, Lint nNumber);         // ���Ƹ�������;
	void        TiaoPai(CardValue tiaopai[], Lint nLenth);                                            // ���ƺ���;

private:
	void		RandCard();
	void        RealRandomCard();
	void        RandCard_2Player();
	void        RandCard_15cards();

private:
	Card		m_oCard[CARD_COUNT_DDZ];
	CardVec		m_pCard;
};


////////////////////////////////////////////////////////////////////////////////////////////
class CardHelp
{
public:
	CardHelp();
	~CardHelp();

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