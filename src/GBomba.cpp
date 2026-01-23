// GBomba.cpp: implementation of the GBomba class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "GBomba.h"
#include "GGame.h"

#include <iostream>

#define BOMBTIME 100

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GBomba::GBomba()
{
	m_shy = 0;
}

GBomba::~GBomba()
{

}

void GBomba::Draw(int x, int y, int bx, int by)
{
#ifdef DEBUG
	if (!m_valid || m_mx != x || m_my != y) {
		std::cerr << "Invalid bomb draw call" << std::endl;
		return;
	}
#else
	(void)x; (void)y;  // Silence unused parameter warnings in release mode
#endif
	if (m_bombtime <= BOMBTIME) {
		d3dx.Draw(m_bBmp_s, bx+m_x-25, by+m_y-25, 0, m_size, m_size, 128, 6-(m_bombtime*7)/BOMBTIME, m_shy);
		d3dx.Draw(m_bBmp, bx+m_x-25, by+m_y-25, 0, m_size, m_size, 255, 6-(m_bombtime*7)/BOMBTIME, m_shy);
	}
	else {
		d3dx.Draw(m_bBmp_s, bx+m_x-25, by+m_y-25, 0, m_size, m_size, 128, 0, m_shy);
		d3dx.Draw(m_bBmp, bx+m_x-25, by+m_y-25, 0, m_size, m_size, 255, 0, m_shy);
	}
}

void GBomba::Init(GGame *game, int x, int y, int bBmp, int bBmp_s, int bomberID, int dosah, bool fromNetwork)
{
	GObject::Init(game, x, y, bBmp, bBmp_s);
	m_valid = true;
	m_bombtime = (int)(((float)BOMBTIME)/gspeed);
	m_bomberID = bomberID;
	m_dosah = dosah;
	m_size = 1;
	m_dsize = (float)0.05;
	m_fromNetwork = fromNetwork;

	m_speed = (int)(5.0*gspeed);
	m_dir   = 0;

	m_x = 25;
	m_y = 25;

	// BUG #36 FIX: Validate m_bomberID before array access to prevent buffer overflow
	// Malformed network packets could contain invalid bomberID values
	if (m_bomberID >= 0 && m_bomberID < MAX_BOMBERS && m_game->m_bomber[m_bomberID].m_casovac) m_bombtime = 100000;
}

void GBomba::Move()
{
	if (!m_valid) return;

	MoveXY();

	if (--m_bombtime < 1) {
		// Only decrement m_bombused if bomb was NOT created from network
		// Network bombs don't increment on create, so shouldn't decrement on explode
		// Also validate bomberID bounds to prevent buffer overflow from malformed packets
		if (m_bomberID >= 0 && m_bomberID < MAX_BOMBERS && !m_fromNetwork) m_game->m_bomber[m_bomberID].m_bombused--;
		m_valid = false;
		m_game->m_map.BombExpolode(this);  // BombExpolode now handles deletion
		g_sb[SND_GAME_EXPLODE].Play(false);
		return;  // Object deleted by BombExpolode, do not access 'this' after
	}

	if (((m_size += m_dsize) > 1.3) || m_size < 1) m_dsize = -m_dsize;
}

void GBomba::MoveXY()
{
	GMap *map = &m_game->m_map;
	
	m_moveing = false;
	switch (m_dir)
	{
	case 1: // Doleva
		if (m_x > 25 || map->IsFree(m_mx-1,m_my)) {
			m_moveing = true;
			m_x -= m_speed;
			if (m_y > 25+m_speed) m_y -= m_speed;
			else if (m_y < 25-m_speed) m_y += m_speed;
				  else m_y = 25;
		}
		break;
	case 2: // doprava
		if (m_x < 25 || map->IsFree(m_mx+1,m_my)) {
			m_moveing = true;
			m_x += m_speed;
			if (m_y > 25+m_speed) m_y -= m_speed;
			else if (m_y < 25-m_speed) m_y += m_speed;
				  else m_y = 25;
		}
		break;
	case 3: // nahoru
		if (m_y > 25 || map->IsFree(m_mx,m_my-1)) {
			m_moveing = true;
			m_y -= m_speed;
			if (m_x > 25+m_speed) m_x -= m_speed;
			else if (m_x < 25-m_speed) m_x += m_speed;
				  else m_x = 25;
		}
		break;
	case 4: // dolu
		if (m_y < 25 || map->IsFree(m_mx,m_my+1)) {
			m_moveing = true;
			m_y += m_speed;
			if (m_x > 25+m_speed) m_x -= m_speed;
			else if (m_x < 25-m_speed) m_x += m_speed;
				  else m_x = 25;
		}
		break;
	}

	map->m_bmap[m_mx][m_my].bomba = nullptr;

	if (m_x > 50) {m_mx++; m_x -= 50;};
	if (m_y > 50) {m_my++; m_y -= 50;};
	if (m_x < 0) {m_mx--; m_x += 50;};
	if (m_y < 0) {m_my--; m_y += 50;};

	map->m_bmap[m_mx][m_my].bomba = this;

}

void GBomba::SetDanger()
{
	int x, y;

	GetDest(&x, &y);

	m_game->m_map.SetBombDanger(x, y, m_dosah);
}

void GBomba::GetDest(int *x, int *y)
{
	*x = m_mx;
	*y = m_my;

	if (m_dir == 0 || !m_moveing) return;

	*x = m_mx*50 + (int)m_x;
	*y = m_my*50 + (int)m_y;
	
	int time = m_bombtime;
	GMap *map = &m_game->m_map;

	int kx = m_mx, ky = m_my;
	int dir = 0;
	while ((time-- > 0) && !map->m_bmap[kx][ky].danger && 
		((kx == m_mx && m_my == ky) || 
		 (map->IsFree(kx, ky) || (map->m_bmap[kx][ky].bomba != nullptr && map->m_bmap[kx][ky].bomba->m_dir == m_dir)))) {
		switch (m_dir)
		{
		case  1: (*x) -= m_speed; break;
		case  2: (*x) += m_speed; break;
		case  3: (*y) -= m_speed; break;
		case  4: (*y) += m_speed; break;
		}
		dir = m_dir;
		kx = (*x-25)/50;
		ky = (*y-25)/50;
	}

	if (!map->IsFree(kx, ky)) 
		switch (dir)
		{
		case  1: kx++; break;
		case  2: kx--; break;
		case  3: ky++; break;
		case  4: ky--; break;
		}

	*x = kx;
	*y = ky;

}
