// GBonus_n_dement.cpp: implementation of the GBonus_n_dement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "GBonus_n_dement.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GBonus_n_dement::GBonus_n_dement()
{
	m_illness = true;
	m_shx = 2;
	m_shy = 0;
	m_onetime = false;
	m_bView = 3;
	m_timer_MAX = TIME_BONUS_ILL;
}

GBonus_n_dement::~GBonus_n_dement()
{

}

void GBonus_n_dement::BeforeMove(float* /*speed*/)
{
	switch (m_bomber->m_dir) {
	case 1: m_bomber->m_dir = 2; break;
	case 2: m_bomber->m_dir = 1; break;
	case 3: m_bomber->m_dir = 4; break;
	case 4: m_bomber->m_dir = 3; break;
	}
}

GBonus* GBonus_n_dement::GetCopy()
{
	GBonus_n_dement *n;
	n = new GBonus_n_dement;

	*n = *this;

	return (GBonus*) n;
}

