// GBonus_n_stop.cpp: implementation of the GBonus_n_stop class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "GBonus_n_stop.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GBonus_n_stop::GBonus_n_stop()
{
	m_shx = 2;
	m_shy = 0;
	m_onetime = false;
	m_illness = true;
	m_bView = 3;
	m_timer_MAX = TIME_BONUS_ILL; // tak silna nemoc trva jen 1/2 casu
}

GBonus_n_stop::~GBonus_n_stop()
{

}

GBonus* GBonus_n_stop::GetCopy()
{
	GBonus_n_stop *n;
	n = new GBonus_n_stop;

	*n = *this;

	return (GBonus*) n;
}

void GBonus_n_stop::BeforeMove(float *speed)
{
	*speed = 0;
}

