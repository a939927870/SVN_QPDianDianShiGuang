#include "gr_card_wakeng.h"

Card_Wakeng::Card_Wakeng():m_nColorType(card_color_type_null),m_nNumberType(card_number_type_null),m_nIndex(0),m_bLaizi(false),m_nLaiziNumber(card_number_type_null)
{
}
Card_Wakeng::~Card_Wakeng()
{
}

BYTE Card_Wakeng::GetColorType()const
{
	return BYTE(m_nColorType);
}

void Card_Wakeng::SetColorAndNumber(BYTE color,BYTE number)
{
	m_nColorType = card_color_type(color);
	m_nNumberType = card_number_type(number);
	m_nIndex =  (m_nColorType-1) + (m_nNumberType-1)*4;

	if (m_nNumberType == card_number_type_3)
	{
		m_nNumberType = (card_number_type)Shanxi_Wakeng::card_number_type_3;
	}
}

BYTE Card_Wakeng::GetNumberType()const
{
	return BYTE(m_nNumberType);
}

void Card_Wakeng::SetIndex(BYTE index)
{
	m_nIndex = index;
	m_nNumberType = card_number_type(m_nIndex / 4 + 1);
	m_nColorType = card_color_type(m_nIndex % 4 + 1);
	if (m_nNumberType == card_number_type_3)
	{
		m_nNumberType = (card_number_type)Shanxi_Wakeng::card_number_type_3;
	}
}

BYTE Card_Wakeng::GetIndex()const
{
	return m_nIndex;
}

BYTE Card_Wakeng::GetReplaceType()const
{
	if(m_bLaizi)
	{
		return BYTE(m_nLaiziNumber);
	}

	return BYTE(m_nNumberType);
}

void Card_Wakeng::SetLaizi(bool laizi)
{
	m_bLaizi = laizi;
	SetLaiziNumber(m_nNumberType);
}

bool Card_Wakeng::GetLaizi()
{
	return m_bLaizi;
}

void Card_Wakeng::SetLaiziNumber(BYTE number)
{
	m_nLaiziNumber = number;
}
BYTE Card_Wakeng::GetLaiziNumber()
{
	return m_nLaiziNumber;
}

int Card_Wakeng::GetScore()
{
	return 0;
}