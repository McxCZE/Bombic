// GBomber.cpp: implementation of the GBomber class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "GBomber.h"
#include "GGame.h"
#include "data.h"
#include "GBonus.h"
#ifdef HAVE_SDL2_NET
#include "Network.h"
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GBomber::GBomber()
{
}

GBomber::~GBomber()
{
	if (m_bonus != nullptr) {
		delete m_bonus;
		m_bonus = nullptr;
	}
}

void GBomber::Init(GGame *game, int x, int y, int bBmp, int bBmp_s, int ID)
{
	GObject::Init(game, x, y, bBmp, bBmp_s);
	m_map = &game->m_map;
	m_ID = ID;
	m_bombdosah = 1;
	m_bomb = 1;
	m_bombused = 0;
	m_score = 0;
	m_dead = false;
	m_x = 25;
	m_y = 25;
	m_dir = 0;
	m_smer = 3;
	m_action = false;
	m_basespeed = P_bomber.speed;
	m_bonus = nullptr;
	m_lives = 1;
	m_megabombs   = 0;
	m_napalmbombs = 0;
	m_hitting = false;

	m_casovac = false;
	m_undertimer = false;
	m_kopani = false;
	m_posilani = false;
}

void GBomber::Draw(int x, int y, bool dead, int bx, int by)
{
	if (m_mx != x || m_my != y) return;

	if ((m_dead && dead) || (m_dead && m_anim < P_bomber.anims)) {
		d3dx.Draw(m_bBmp_s, bx - 25 + m_x, by+P_bomber.dy+m_y, 0, 1, 1, 128, m_anim/P_bomber.animspeed, 4);
		d3dx.Draw(m_bBmp, bx - 25 + m_x, by+P_bomber.dy+m_y, 0, 1, 1, 255, m_anim/P_bomber.animspeed, 4);
	}


	if (!dead && !m_dead) {
		d3dx.Draw(m_bBmp_s, bx - 25 + m_x, by+P_bomber.dy+m_y, 0, 1, 1, 128, m_anim/P_bomber.animspeed, m_smer);
		d3dx.Draw(m_bBmp, bx - 25 + m_x, by+P_bomber.dy+m_y, 0, 1, 1, 255, m_anim/P_bomber.animspeed, m_smer);
	}

	if (m_bonus) m_bonus->DrawOnPlayer((int)(bx - 25 + m_x), (int)(by+P_bomber.dy+m_y));

}

void GBomber::Move()
{
	if (m_bonus) {
		m_bonus->Move();
		// Check if bonus marked itself for deletion
		if (m_bonus && m_bonus->m_shouldDelete) {
			delete m_bonus;
			m_bonus = nullptr;
		}
	}

	float speed = m_basespeed * gspeed;

	if (m_bonus) m_bonus->BeforeMove(&speed);


	if (m_dead) {
		if (m_anim < P_bomber.anims*P_bomber.animspeed) m_anim++;
		return;
	}
	else {
		if (m_dir != 0) {
			m_smer = m_dir-1;
			if (++m_anim > P_bomber.anims*P_bomber.animspeed) m_anim = 0;
		}
		else m_anim = 0;
	}

	GetBonus();

	switch (m_dir)
	{
	case 1: // Doleva
		if (m_x > 25 || m_map->IsFree(m_mx-1,m_my)) {
			m_x -= speed;
			if (m_y > 25+speed) m_y -= speed;
			else if (m_y < 25-speed) m_y += speed;
				  else m_y = 25;
		}
		else KickBomb();
		break;
	case 2: // doprava
		if (m_x < 25 || m_map->IsFree(m_mx+1,m_my)) {
			m_x += speed;
			if (m_y > 25+speed) m_y -= speed;
			else if (m_y < 25-speed) m_y += speed;
				  else m_y = 25;
		}
		else KickBomb();
		break;
	case 3: // nahoru
		if (m_y > 25 || m_map->IsFree(m_mx,m_my-1)) {
			m_y -= speed;
			if (m_x > 25+speed) m_x -= speed;
			else if (m_x < 25-speed) m_x += speed;
				  else m_x = 25;
		}
		else KickBomb();
		break;
	case 4: // dolu
		if (m_y < 25 || m_map->IsFree(m_mx,m_my+1)) {
			m_y += speed;
			if (m_x > 25+speed) m_x -= speed;
			else if (m_x < 25-speed) m_x += speed;
				  else m_x = 25;
		}
		else KickBomb();
		break;
	}

	if (m_x > 50) {m_mx++; m_x -= 50;};
	if (m_y > 50) {m_my++; m_y -= 50;};
	if (m_x < 0) {m_mx--; m_x += 50;};
	if (m_y < 0) {m_my--; m_y += 50;};

	if (m_casovac) {
		if (m_action)
			if (!m_undertimer) 
				if (PutBomb()) 	m_undertimer = true;
	}
	else 
		if ((m_action && m_bombused < m_bomb) && !m_undertimer) PutBomb();
			

	if (!m_action && m_undertimer) {
		m_undertimer = false;
#ifdef HAVE_SDL2_NET
		// In LAN mode, only host can detonate timer bombs
		// Client waits for detonate notification from host
		if (m_game->m_networkMode == GAME_MODE_LAN) {
			if (g_network.IsClient()) {
				// Client: do nothing, wait for host's detonation packet
			} else if (g_network.IsHost()) {
				// Host: detonate bombs and notify client
				for (int i = 0; i < MAX_BOMBS; i++)
					if (m_map->m_bomba[i] != nullptr)
						if (m_map->m_bomba[i]->m_bomberID == m_ID) m_map->m_bomba[i]->m_bombtime = 1;
				g_network.SendBombDetonate(m_ID);
			}
		} else
#endif
		{
			// Local mode: normal timer bomb detonation
			for (int i = 0; i < MAX_BOMBS; i++)
				if (m_map->m_bomba[i] != nullptr)
					if (m_map->m_bomba[i]->m_bomberID == m_ID) m_map->m_bomba[i]->m_bombtime = 1;
		}
	}

}

void GBomber::Input(bool left, bool right, bool up, bool down, bool action)
{
	if (m_bonus) m_bonus->BeforeInput(&left, &right, &up, &down, &action);

	m_dir = 0;
	if (left)   m_dir = 1;
	if (right)  m_dir = 2;
	if (up)     m_dir = 3;
	if (down)   m_dir = 4;

	m_action = action;
}

int GBomber::Hit()
{
	if (m_dead) return -1;

	if (m_map->m_bmap[m_mx][m_my].cas > 0 || m_map->m_mmap[m_mx][m_my] > 0) {
		if (!m_hitting && ((m_bonus && !m_bonus->HitProtect()) || (!m_bonus)) ) {
			if (--m_lives == 0) {
				g_sb[SND_GAME_DEAD_BOMBER].Play(false);
				m_dead = true;
				m_anim = 0;
#ifdef HAVE_SDL2_NET
				// Send immediate hit event to client for death
				if (m_game->m_networkMode == GAME_MODE_LAN && g_network.IsHost()) {
					g_network.SendPlayerHit(m_ID, true, m_lives, m_dead);
				}
#endif
				if (m_map->m_bmap[m_mx][m_my].cas > 0)
					return m_map->m_bmap[m_mx][m_my].barva;
				else return -2;
			}
		}
		// Only send if state changed from false to true
		bool wasHitting = m_hitting;
		m_hitting = true;
#ifdef HAVE_SDL2_NET
		// Send immediate hit event to client when hitting state changes
		if (!wasHitting && m_game->m_networkMode == GAME_MODE_LAN && g_network.IsHost()) {
			g_network.SendPlayerHit(m_ID, m_hitting, m_lives, m_dead);
		}
#endif
	}
	else {
		// Only send if state changed from true to false
		bool wasHitting = m_hitting;
		m_hitting = false;
#ifdef HAVE_SDL2_NET
		// Send immediate hit event to client when hitting state ends
		if (wasHitting && m_game->m_networkMode == GAME_MODE_LAN && g_network.IsHost()) {
			g_network.SendPlayerHit(m_ID, m_hitting, m_lives, m_dead);
		}
#endif
	}

	return -1;
}

void GBomber::GetBonus()
{
	// nakazlivost (illness transfer)
#ifdef HAVE_SDL2_NET
	if (m_game->m_networkMode == GAME_MODE_LAN) {
		// In LAN mode, only host handles illness transfer
		// Client receives illness transfer notifications via network
		if (g_network.IsHost()) {
			if (m_bonus == nullptr) {
				for (int i = 0; i < m_game->m_bombers; i++) {
					if (m_game->m_bomber[i].m_bonus != nullptr &&
						m_game->m_bomber[i].m_bonus->m_illness &&
						m_game->m_bomber[i].m_mx == m_mx && m_game->m_bomber[i].m_my == m_my)
					{
						if (m_bonus) {
							m_bonus->End();
							delete m_bonus;
						}
						m_bonus = m_game->m_bomber[i].m_bonus->GetCopy();
						m_bonus->m_self = &m_bonus;
						// Send illness transfer to client
						g_network.SendIllnessTransfer(i, m_ID, m_game->m_bomber[i].m_bonus->m_type);
						break;  // Only transfer once
					}
				}
			}
		}
		// Client: illness transfer is handled via network packets in GGame::Move()
	} else
#endif
	{
		// Local mode: original illness transfer logic
		if (m_bonus == nullptr)
			for (int i = 0; i < m_game->m_bombers; i++)
				if (m_game->m_bomber[i].m_bonus != nullptr &&
					m_game->m_bomber[i].m_bonus->m_illness &&
					m_game->m_bomber[i].m_mx == m_mx && m_game->m_bomber[i].m_my == m_my)
				{
					if (m_bonus) {
						m_bonus->End();
						delete m_bonus;
					}
					m_bonus = m_game->m_bomber[i].m_bonus->GetCopy();
					m_bonus->m_self = &m_bonus;
				}
	}


	// bonus z mapy
#ifdef HAVE_SDL2_NET
	// In LAN mode, only host can pick up bonuses
	// Client receives bonus pickup notification from host
	if (m_game->m_networkMode == GAME_MODE_LAN && g_network.IsClient()) {
		// Client: do NOT pick up bonus directly - wait for host notification
		return;
	}
#endif

	GBonus *bonus = m_game->m_map.m_bonusmap[m_mx][m_my];

	if (bonus == nullptr) return;

	// Remove bonus from map BEFORE processing (prevents double pickup)
	m_game->m_map.m_bonusmap[m_mx][m_my] = nullptr;

#ifdef HAVE_SDL2_NET
	// Host: notify client that bonus was picked up
	if (m_game->m_networkMode == GAME_MODE_LAN && g_network.IsHost()) {
		g_network.SendBonusPicked(m_mx, m_my, m_ID);
	}
#endif

	if (!bonus->m_onetime && m_bonus) {
		m_bonus->End();
		delete m_bonus;
	}

	// Store pointer before Picked() may clear it via m_self
	GBonus *pickedBonus = bonus;
	bonus->Picked(this, &bonus);

	// Check if bonus was marked for deletion (one-time bonus)
	if (pickedBonus->m_shouldDelete) {
		delete pickedBonus;
		g_sb[SND_GAME_BONUS].Play(false);
	}
	else if (bonus != nullptr) {
		m_bonus = bonus;
		m_bonus->m_self = &m_bonus;
		g_sb[SND_GAME_TIME_BONUS].Play(false);
	}

}

bool GBomber::PutBomb()
{
	int id;

	if (m_hitting) return false;

#ifdef HAVE_SDL2_NET
	// In LAN mode, only the host can create bombs
	// Client waits for bomb placement packets from host
	if (m_game->m_networkMode == GAME_MODE_LAN && g_network.IsClient()) {
		// Client doesn't place bombs directly - host is authoritative
		return false;
	}
#endif

	int bombType = NET_BOMB_REGULAR;
	if (m_napalmbombs) {
		id = m_map->AddNapalmBomb(m_ID, m_mx, m_my, m_bombdosah);
		bombType = NET_BOMB_NAPALM;
	}
	else if (m_megabombs) {
		id = m_map->AddMegaBomb(m_ID, m_mx, m_my, m_bombdosah);
		bombType = NET_BOMB_MEGA;
	}
	else {
		id = m_map->AddBomb(m_ID, m_mx, m_my, m_bombdosah);
		bombType = NET_BOMB_REGULAR;
	}

	if (id == -1) return false;

#ifdef HAVE_SDL2_NET
	// In LAN mode, host sends bomb placement to client
	if (m_game->m_networkMode == GAME_MODE_LAN && g_network.IsHost()) {
		g_network.SendBombPlaced(m_ID, bombType, m_mx, m_my, m_bombdosah, m_casovac);
	}
#endif

	g_sb[SND_GAME_BOMBPUT].Play(false);

	if (m_posilani && m_map->m_bomba[id] && (m_dir == 0)) {
		m_map->m_bomba[id]->m_dir = m_smer+1;
#ifdef HAVE_SDL2_NET
		// In LAN mode, host sends bomb throw direction to client
		if (m_game->m_networkMode == GAME_MODE_LAN && g_network.IsHost()) {
			g_network.SendBombKicked(m_mx, m_my, m_smer+1);
		}
#endif
	}

	if (m_bonus)
		m_bonus->AfterPut(id);

	return true;
}

void GBomber::KickBomb()
{
	if (!m_kopani) return;

#ifdef HAVE_SDL2_NET
	// In LAN mode, only host can kick bombs
	// Client waits for kick notification from host
	if (m_game->m_networkMode == GAME_MODE_LAN && g_network.IsClient()) {
		return;
	}
#endif

	int kickX = -1, kickY = -1;
	switch (m_dir) {
	case 1: // Doleva
		if (m_map->m_bmap[m_mx-1][m_my].bomba != nullptr) {
			m_map->m_bmap[m_mx-1][m_my].bomba->m_dir = 1;
			kickX = m_mx-1; kickY = m_my;
		}
		break;
	case 2: // doprava
		if (m_map->m_bmap[m_mx+1][m_my].bomba != nullptr) {
			m_map->m_bmap[m_mx+1][m_my].bomba->m_dir = 2;
			kickX = m_mx+1; kickY = m_my;
		}
		break;
	case 3: // nahoru
		if (m_map->m_bmap[m_mx][m_my-1].bomba != nullptr) {
			m_map->m_bmap[m_mx][m_my-1].bomba->m_dir = 3;
			kickX = m_mx; kickY = m_my-1;
		}
		break;
	case 4: // dolu
		if (m_map->m_bmap[m_mx][m_my+1].bomba != nullptr) {
			m_map->m_bmap[m_mx][m_my+1].bomba->m_dir = 4;
			kickX = m_mx; kickY = m_my+1;
		}
		break;
	}

#ifdef HAVE_SDL2_NET
	// Host sends kick notification to client
	if (kickX >= 0 && m_game->m_networkMode == GAME_MODE_LAN && g_network.IsHost()) {
		g_network.SendBombKicked(kickX, kickY, m_dir);
	}
#endif
}
