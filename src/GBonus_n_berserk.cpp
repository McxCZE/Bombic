// GBonus_n_berserk.cpp: implementation of the GBonus_n_berserk class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "GBonus_n_berserk.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GBonus_n_berserk::GBonus_n_berserk()
{
	m_illness = true;
	m_shx = 2;
	m_shy = 0;
	m_onetime = false;
	m_bView = 3;
	m_timer_MAX = TIME_BONUS_ILL;
}

GBonus_n_berserk::~GBonus_n_berserk()
{

}

void GBonus_n_berserk::BeforeMove(float *speed)
{
	m_bomber->m_action = true;
}

GBonus* GBonus_n_berserk::GetCopy()
{
	GBonus_n_berserk *n;
	n = new GBonus_n_berserk;

	*n = *this;

	return (GBonus*) n;
}
