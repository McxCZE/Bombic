// GBonus_shield.cpp: implementation of the GBonus_shield class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "GBonus_shield.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GBonus_shield::GBonus_shield()
{
	m_shx = 2;
	m_shy = 1;
	m_onetime = false;
	m_illness = false;
	m_bView = 2;
	m_timer_MAX = TIME_BONUS_EXTRA/2;
}

GBonus_shield::~GBonus_shield()
{

}

bool GBonus_shield::HitProtect()
{
	return true;
}

void GBonus_shield::OneTime()
{
	m_timer = TIME_BONUS_EXTRA/2; // bylo by moc silne
}
