// GBonus_speed.cpp: implementation of the GBonus_speed class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "GBonus_speed.h"
#include "data.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GBonus_speed::GBonus_speed()
{
	m_illness = false;
	m_shx = 3;
	m_shy = 0;
	m_onetime = true;
}

GBonus_speed::~GBonus_speed()
{

}

void GBonus_speed::OneTime()
{
	if (m_bomber->m_basespeed < P_bomber.speed+2) m_bomber->m_basespeed += 1;
}
