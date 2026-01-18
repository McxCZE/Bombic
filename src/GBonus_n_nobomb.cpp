// GBonus_n_nobomb.cpp: implementation of the GBonus_n_nobomb class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "GBonus_n_nobomb.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GBonus_n_nobomb::GBonus_n_nobomb()
{
	m_shx = 2;
	m_shy = 0;
	m_onetime = false;
	m_illness = true;
	m_bView = 3;
	m_timer_MAX = TIME_BONUS_ILL;
}

GBonus_n_nobomb::~GBonus_n_nobomb()
{

}

void GBonus_n_nobomb::BeforeMove(float* /*speed*/)
{
	m_bomber->m_action = false;
}

GBonus* GBonus_n_nobomb::GetCopy()
{
	GBonus_n_nobomb *n;
	n = new GBonus_n_nobomb;

	*n = *this;

	return (GBonus*) n;
}
