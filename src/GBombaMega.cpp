// GBombaMega.cpp: implementation of the GBombaMega class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "GBombaMega.h"
#include "GGame.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GBombaMega::GBombaMega()
{
	m_shy = 1;
}

GBombaMega::~GBombaMega()
{

}

void GBombaMega::Move()
{
	if (!m_valid) return;

	MoveXY();

	if (--m_bombtime < 1) {
		m_game->m_map.BombMegaExpolode(this);  // Handles deletion via BombExpolode
		g_sb[SND_GAME_MEGA_EXPLODE].Play(false);
		return;  // Object deleted, do not access 'this'
	}

	if (((m_size += m_dsize) > 1.3) || m_size < 1) m_dsize = -m_dsize;
}

void GBombaMega::SetDanger()
{
	int x, y;
	GetDest(&x, &y);

	m_game->m_map.SetBombDanger(x+1, y+1, m_dosah);
	m_game->m_map.SetBombDanger(x+1, y, m_dosah);
	m_game->m_map.SetBombDanger(x+1, y-1, m_dosah);
	m_game->m_map.SetBombDanger(x, y+1, m_dosah);
	m_game->m_map.SetBombDanger(x, y, m_dosah);
	m_game->m_map.SetBombDanger(x, y-1, m_dosah);
	m_game->m_map.SetBombDanger(x-1, y+1, m_dosah);
	m_game->m_map.SetBombDanger(x-1, y, m_dosah);
	m_game->m_map.SetBombDanger(x-1, y-1, m_dosah);
}
