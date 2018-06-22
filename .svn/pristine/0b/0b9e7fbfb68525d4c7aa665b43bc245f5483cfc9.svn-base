#pragma once

#include "LBase.h"
//#include <Windows.h>
#include <vector>
#include <assert.h>
#include "GameDefine.h"
#include "gr_card.h"

typedef std::vector<CardStyleNumber> CardStyleNumberVec;

class Card_Wakeng
{
public:
	Card_Wakeng();
	~Card_Wakeng();

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

typedef std::vector<Card_Wakeng*> CardVec_Wakeng;
