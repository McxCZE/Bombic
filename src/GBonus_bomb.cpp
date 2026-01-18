// GBonus_bomb.cpp: implementation of the GBonus_bomb class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "GBonus_bomb.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GBonus_bomb::GBonus_bomb()
{
	m_shx = 1;
	m_shy = 0;
	m_onetime = true;
	m_illness = false;
}

GBonus_bomb::~GBonus_bomb()
{

}

void GBonus_bomb::OneTime()
{
	if (m_bomber->m_bomb < 9) m_bomber->m_bomb++;
}
