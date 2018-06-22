#include "gr_card.h"

Card::Card():m_nColorType(card_color_type_null),m_nNumberType(card_number_type_null),m_nIndex(0),m_bLaizi(false),m_nLaiziNumber(card_number_type_null)
{
}
Card::~Card()
{
}

BYTE Card::GetColorType()const
{
	return BYTE(m_nColorType);
}

void Card::SetColorAndNumber(BYTE color,BYTE number)
{
	m_nColorType = card_color_type(color);
	m_nNumberType = card_number_type(number);
	if(m_nNumberType == card_number_type_SJ)
	{
		m_nIndex = 52;
	}
	else if(m_nNumberType == card_number_type_BJ)
	{
		m_nIndex = 53;
	}
	else
	{
		m_nIndex =  (m_nColorType-1) + (m_nNumberType-1)*4;
	}
}

BYTE Card::GetNumberType()const
{
	return BYTE(m_nNumberType);
}

void Card::SetIndex(BYTE index)
{
	m_nIndex = index;
	if(m_nIndex == 53)
	{
		m_nNumberType = card_number_type_BJ;
	}
	else if(m_nIndex == 52)
	{
		m_nNumberType = card_number_type_SJ;
	}
	else if(m_nIndex == 54)
	{
		m_nNumberType = card_number_type_LZ;
	}
	else
	{
		m_nNumberType = card_number_type(m_nIndex/4 + 1);
		m_nColorType = card_color_type(m_nIndex%4 + 1);
	}
}

BYTE Card::GetIndex()const
{
	return m_nIndex;
}

BYTE Card::GetReplaceType()const
{
	if(m_bLaizi)
	{
		return BYTE(m_nLaiziNumber);
	}

	return BYTE(m_nNumberType);
}

void Card::SetLaizi(bool laizi)
{
	m_bLaizi = laizi;
	SetLaiziNumber(m_nNumberType);
}

bool Card::GetLaizi()
{
	return m_bLaizi;
}

void Card::SetLaiziNumber(BYTE number)
{
	m_nLaiziNumber = number;
}
BYTE Card::GetLaiziNumber()
{
	return m_nLaiziNumber;
}

int Card::GetScore()
{
	return 0;
}