// GBonus_fireman.cpp: implementation of the GBonus_fireman class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GBonus_fireman.h"
#include "GMap.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GBonus_fireman::GBonus_fireman()
{
	m_shx = 2;
	m_shy = 2;
	m_onetime = false;
	m_illness = false;
	m_bView = 7;
	m_timer_MAX = TIME_BONUS_EXTRA/4;
}

GBonus_fireman::~GBonus_fireman()
{

}


bool GBonus_fireman::HitProtect()
{
	return true;
}

void GBonus_fireman::BeforeInput(bool *left, bool *right, bool *up, bool *down, bool *action)
{
	// Fireman bonus: action key creates instant explosion at player position
	// In LAN mode, both machines run this independently based on shared input
	// This works because input is synchronized via network
	if (*action) {
		m_bomber->m_hitting = true;
		int b = m_map->AddBomb(m_bomber->m_ID, m_bomber->m_mx, m_bomber->m_my, 1);
		if (b >= 0 && b < MAX_BOMBS && m_map->m_bomba[b] != nullptr) {
			m_map->m_bomba[b]->m_valid = false;
			m_map->BombExpolode(m_map->m_bomba[b], 2);
			m_bomber->m_bombused--;
		}
		*action = false;
	}
}
