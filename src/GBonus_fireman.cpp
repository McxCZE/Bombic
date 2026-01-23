// GBonus_fireman.cpp: implementation of the GBonus_fireman class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GBonus_fireman.h"
#include "GMap.h"
#include "GGame.h"
#ifdef HAVE_SDL2_NET
#include "Network.h"
#endif

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

void GBonus_fireman::BeforeInput(bool* /*left*/, bool* /*right*/, bool* /*up*/, bool* /*down*/, bool *action)
{
	// Fireman bonus: action key creates instant explosion at player position
	if (*action) {
#ifdef HAVE_SDL2_NET
		// In LAN mode, only host creates the bomb and explosion
		// Client waits for network sync via NET_PACKET_BOMB_PLACED
		GGame* game = m_bomber->m_game;
		if (game->m_networkMode == GAME_MODE_LAN && g_network.IsClient()) {
			// Client: just set hitting state (will be synced from host)
			// Don't create bomb - host is authoritative
			*action = false;
			return;
		}
#endif
		m_bomber->m_hitting = true;
		int b = m_map->AddBomb(m_bomber->m_ID, m_bomber->m_mx, m_bomber->m_my, 1);
		if (b >= 0 && b < MAX_BOMBS && m_map->m_bomba[b] != nullptr) {
			m_map->m_bomba[b]->m_valid = false;
			m_map->BombExpolode(m_map->m_bomba[b], 2);
			m_bomber->m_bombused--;
#ifdef HAVE_SDL2_NET
			// Host: notify client of this special instant bomb (already exploded)
			// Client will receive bombused sync via GameState packets
			if (game->m_networkMode == GAME_MODE_LAN && g_network.IsHost()) {
				g_network.SendPlayerHit(m_bomber->m_ID, m_bomber->m_hitting, m_bomber->m_lives, m_bomber->m_dead);
			}
#endif
		}
		*action = false;
	}
}
