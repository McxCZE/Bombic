// GBonus_kicker.cpp: implementation of the GBonus_kicker class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "GBonus_kicker.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GBonus_kicker::GBonus_kicker()
{
	m_shx = 3;
	m_shy = 1;
	m_onetime = true;
	m_illness = false;
}

GBonus_kicker::~GBonus_kicker()
{

}

void GBonus_kicker::OneTime()
{
	m_bomber->m_kopani = true;
}
