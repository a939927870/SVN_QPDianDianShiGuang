#ifndef _C_CARD_H_
#define _C_CARD_H_
#include "LBase.h"
//#include <Windows.h>
#include <vector>
#include <assert.h>
#include "GameDefine.h"

struct CardStyleNumber
{
	CardStyleNumber()
	{
		m_bCardNumber = 0;
		m_bCardStyle = 0;
		m_bCardCount = 0;
	}

	CardStyleNumber(CardStyleNumber const& other)
	{
		m_bCardCount = other.m_bCardCount;
		m_bCardNumber = other.m_bCardNumber;
		m_bCardStyle = other.m_bCardStyle;
	}

	CardStyleNumber& operator = (CardStyleNumber const& other)
	{
		m_bCardCount = other.m_bCardCount;
		m_bCardNumber = other.m_bCardNumber;
		m_bCardStyle = other.m_bCardStyle;
		return *this;
	}

	BYTE m_bCardStyle;
	BYTE m_bCardNumber;
	BYTE m_bCardCount;
};

typedef std::vector<CardStyleNumber> CardStyleNumberVec;

class Card
{
public:
	Card();
	~Card();

	void SetColorAndNumber(BYTE color,BYTE number);
	void SetIndex(BYTE index);

	BYTE GetColorType()const;
	BYTE GetNumberType()const;
	BYTE GetIndex()const;
	BYTE GetReplaceType()const;

	void SetLaizi(bool laizi);
	bool GetLaizi();

	void SetLaiziNumber(BYTE number);
	BYTE GetLaiziNumber();

	int	 GetScore();

private:
	card_color_type		m_nColorType;
	card_number_type	m_nNumberType;
	BYTE	m_nIndex;
	BYTE	m_nLaiziNumber;
	bool	m_bLaizi;
};

typedef std::vector<Card*> CardVec;

#endif