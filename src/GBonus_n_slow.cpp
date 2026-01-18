// GBonus_n_slow.cpp: implementation of the GBonus_n_slow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "GBonus_n_slow.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GBonus_n_slow::GBonus_n_slow()
{
	m_shx = 2;
	m_shy = 0;
	m_onetime = false;
	m_illness = true;
	m_bView = 3;
	m_timer_MAX = TIME_BONUS_ILL;
}

GBonus_n_slow::~GBonus_n_slow()
{

}

void GBonus_n_slow::BeforeMove(float *speed)
{
	*speed *= (float)0.4;
}

GBonus* GBonus_n_slow::GetCopy()
{
	GBonus_n_slow *n;
	n = new GBonus_n_slow;

	*n = *this;

	return (GBonus*) n;
}
