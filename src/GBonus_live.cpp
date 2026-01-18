// GBonus_live.cpp: implementation of the GBonus_live class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "GBonus_live.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GBonus_live::GBonus_live()
{
	m_shx = 1;
	m_shy = 1;
	m_onetime = true;
	m_illness = false;
}

GBonus_live::~GBonus_live()
{

}

void GBonus_live::OneTime()
{
	m_bomber->m_lives++;
}
