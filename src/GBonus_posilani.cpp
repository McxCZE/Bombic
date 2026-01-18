// GBonus_posilani.cpp: implementation of the GBonus_posilani class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "GBonus_posilani.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GBonus_posilani::GBonus_posilani()
{
	m_shx = 0;
	m_shy = 2;
	m_onetime = true;
	m_illness = false;
}

GBonus_posilani::~GBonus_posilani()
{

}

void GBonus_posilani::OneTime()
{
	m_bomber->m_posilani = true;
}
