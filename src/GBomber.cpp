// GBomber.cpp: implementation of the GBomber class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "GBomber.h"
#include "GGame.h"
#include "data.h"
#include "GBonus.h"


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
		for (int i = 0; i < MAX_BOMBS; i++)
			if (m_map->m_bomba[i] != nullptr)
				if (m_map->m_bomba[i]->m_bomberID == m_ID) m_map->m_bomba[i]->m_bombtime = 1;
	
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
				if (m_map->m_bmap[m_mx][m_my].cas > 0)
					return m_map->m_bmap[m_mx][m_my].barva;
				else return -2;
			}
		}
		m_hitting = true;
	}
	else m_hitting = false;

	return -1;
}

void GBomber::GetBonus()
{
	// nakazlivost
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


	// bonus z mapy
	GBonus *bonus = m_game->m_map.m_bonusmap[m_mx][m_my];

	m_game->m_map.m_bonusmap[m_mx][m_my] = nullptr;
	if (bonus == nullptr) return;

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

	if (m_napalmbombs) id = m_map->AddNapalmBomb(m_ID, m_mx, m_my, m_bombdosah);
	else
		if (m_megabombs) id = m_map->AddMegaBomb(m_ID, m_mx, m_my, m_bombdosah);
		else id = m_map->AddBomb(m_ID, m_mx, m_my, m_bombdosah);

	if (id == -1) return false;

	g_sb[SND_GAME_BOMBPUT].Play(false);

	if (m_posilani && m_map->m_bomba[id] && (m_dir == 0)) 
		m_map->m_bomba[id]->m_dir = m_smer+1;

	if (m_bonus)
		m_bonus->AfterPut(id);

	return true;
}

void GBomber::KickBomb()
{
	if (!m_kopani) return;

	switch (m_dir) {
	case 1: // Doleva
		if (m_map->m_bmap[m_mx-1][m_my].bomba != nullptr) 
			m_map->m_bmap[m_mx-1][m_my].bomba->m_dir = 1;
		break;
	case 2: // doprava
		if (m_map->m_bmap[m_mx+1][m_my].bomba != nullptr)
			m_map->m_bmap[m_mx+1][m_my].bomba->m_dir = 2;
		break;
	case 3: // nahoru
		if (m_map->m_bmap[m_mx][m_my-1].bomba != nullptr)
			m_map->m_bmap[m_mx][m_my-1].bomba->m_dir = 3;
		break;
	case 4: // dolu
		if (m_map->m_bmap[m_mx][m_my+1].bomba != nullptr)
			m_map->m_bmap[m_mx][m_my+1].bomba->m_dir = 4;
		break;
	}
}
