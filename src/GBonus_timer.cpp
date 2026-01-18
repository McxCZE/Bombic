// GBonus_timer.cpp: implementation of the GBonus_timer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "GBonus_timer.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GBonus_timer::GBonus_timer()
{
	m_shx = 4;
	m_shy = 0;
	m_onetime = false;
	m_illness = false;
	m_bView = 4;
	m_timer_MAX = TIME_BONUS_EXTRA;
}

GBonus_timer::~GBonus_timer()
{

}

void GBonus_timer::OneTime()
{
	m_bomber->m_casovac = true;
}

void GBonus_timer::End()
{
	m_bomber->m_casovac = false;
}
