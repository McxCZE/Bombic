// GBonus_fire.cpp: implementation of the GBonus_fire class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "GBonus_fire.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GBonus_fire::GBonus_fire()
{
	m_shx = 0;
	m_shy = 0;
	m_onetime = true;
	m_illness = false;
}

GBonus_fire::~GBonus_fire()
{

}

void GBonus_fire::OneTime()
{
	if (m_bomber->m_bombdosah < 9) m_bomber->m_bombdosah++;
}
