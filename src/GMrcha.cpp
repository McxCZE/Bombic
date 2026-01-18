// GMrcha.cpp: implementation of the GMrcha class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "GMrcha.h"
#include "GGame.h"
#include "GMrchaAI0.h"
#include "GMrchaAI1.h"
#include "GMrchaAI2.h"
#include "GMrchaAI3.h"
#include "GMrchaAI4.h"
#include "GMrchaAI5.h"
#include "GMrchaAI_BOSS_TV.h"
#include "GMrchaAI_BOSS_DUDLIK.h"
#include "GMrchaAI_Blesk.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GMrcha::GMrcha()
{
	m_AI = nullptr;
}

GMrcha::~GMrcha()
{
	SAFE_DELETE(m_AI);
}

void GMrcha::Draw(int x, int y, bool dead, int bx, int by)
{
	if (m_mx != x || m_my != y) return;
	
	float sx = 1, sy = 1;

	if (m_dir < 3) sx = m_size;
	else sy = m_size;

	if (m_lives != m_params.lives) {
		sx *= 0.6f+0.4f*((float)m_lives)/((float)m_params.lives);
		sy *= 0.6f+0.4f*((float)m_lives)/((float)m_params.lives);
	}


	if ((m_dead && dead) || (m_dead && m_anim < m_params.danims)) {
		d3dx.Draw(m_bBmp_s, bx - 25 + m_x, by+m_params.dy+m_y, 0, 1, 1, 128, m_anim/m_params.animspeed, 4);
		d3dx.Draw(m_bBmp, bx - 25 + m_x, by+m_params.dy+m_y, 0, 1, 1, m_params.alpha, m_anim/m_params.animspeed, 4);
	}

	if (!m_dead && !dead) {
		d3dx.Draw(m_bBmp_s, bx - 25 + m_x, by+m_params.dy+m_y, 0, sx, sy, 128, m_anim/m_params.animspeed, m_smer);
		d3dx.Draw(m_bBmp, bx - 25 + m_x, by+m_params.dy+m_y, 0, sx, sy, m_params.alpha, m_anim/m_params.animspeed, m_smer);
	}
}

void GMrcha::Init(GGame *game, int x, int y, int bBmp, int bBmp_s, int mrchaID)
{
	GObject::Init(game, x, y, bBmp, bBmp_s);
	m_map = &game->m_map;
	m_map->m_mmap[x][y]++;
	m_params = P_mrcha[mrchaID];
	m_typ = mrchaID;
	
	m_dead = false;
	m_hitting = false;

	m_x = 25;
	m_y = 25;
	m_dir = 0;
	m_smer = 3;

	m_size = 1;
	m_lives = m_params.lives;

	if (m_params.sizing)
		m_dsize = (float) 0.015;
	else m_dsize = 0;

	SAFE_DELETE(m_AI);
	switch (m_params.ai) {
	case 0 : m_AI = new GMrchaAI0(m_game, m_map, this); break;
	case 1 : m_AI = new GMrchaAI1(m_game, m_map, this); break;
	case 2 : m_AI = new GMrchaAI2(m_game, m_map, this); break;
	case 3 : m_AI = new GMrchaAI3(m_game, m_map, this); break;
	case 4 : m_AI = new GMrchaAI4(m_game, m_map, this); break;
	case 5 : m_AI = new GMrchaAI_BOSS_TV(m_game, m_map, this); break;
	case 6:  m_AI = new GMrchaAI_Blesk(m_game, m_map, this); break;
	case 7:  m_AI = new GMrchaAI_BOSS_DUDLIK(m_game, m_map, this); break;
	case 8:  m_AI = new GMrchaAI5(m_game, m_map, this); break;
	}
}

void GMrcha::Move()
{
	m_speed = gspeed * m_params.speed;
	if (m_dead) {
		if (m_anim < m_params.danims*m_params.animspeed) m_anim++;
		m_size = 1;
		return;
	}
	else {
		if (m_dir != 0) {
			m_smer = m_dir-1;
			if (++m_anim > m_params.anims*m_params.animspeed) m_anim = 0;
		}
		else m_anim = 0;
	}


	float speed = m_speed;

	switch (m_dir)
	{
	case 1: // Doleva
		if (m_x > 25 || IsFree(m_mx-1,m_my)) {
			m_x -= speed;
			if (m_y > 25+speed) m_y -= speed;
			else if (m_y < 25-speed) m_y += speed;
				  else m_y = 25;
		}
		break;
	case 2: // doprava
		if (m_x < 25 || IsFree(m_mx+1,m_my)) {
			m_x += speed;
			if (m_y > 25+speed) m_y -= speed;
			else if (m_y < 25-speed) m_y += speed;
				  else m_y = 25;
		}
		break;
	case 3: // nahoru
		if (m_y > 25 || IsFree(m_mx,m_my-1)) {
			m_y -= speed;
			if (m_x > 25+speed) m_x -= speed;
			else if (m_x < 25-speed) m_x += speed;
				  else m_x = 25;
		}
		break;
	case 4: // dolu
		if (m_y < 25 || IsFree(m_mx,m_my+1)) {
			m_y += speed;
			if (m_x > 25+speed) m_x -= speed;
			else if (m_x < 25-speed) m_x += speed;
				  else m_x = 25;
		}
		break;
	}

	if (m_x > 50) {m_map->m_mmap[m_mx][m_my]--; m_mx++; m_x -= 50; m_map->m_mmap[m_mx][m_my]++;};
	if (m_y > 50) {m_map->m_mmap[m_mx][m_my]--; m_my++; m_y -= 50; m_map->m_mmap[m_mx][m_my]++;};
	if (m_x < 0) {m_map->m_mmap[m_mx][m_my]--; m_mx--; m_x += 50; m_map->m_mmap[m_mx][m_my]++;};
	if (m_y < 0) {m_map->m_mmap[m_mx][m_my]--; m_my--; m_y += 50; m_map->m_mmap[m_mx][m_my]++;};


	if (((m_size += m_dsize) > 1.0) || m_size < 0.8) m_dsize = -m_dsize;
	if (m_dead) m_size = 1;

}

int GMrcha::Hit()
{
	if (m_dead) return -1;

	if (m_map->m_bmap[m_mx][m_my].cas > 0) {
		if (!m_hitting) {
			if (--m_lives == 0) {
				m_dead = true;
				m_anim = 0;
				m_map->m_mmap[m_mx][m_my]--;
				m_game->m_mrtvol += m_params.score;
				g_sb[10+m_typ].Play(false);
				return m_map->m_bmap[m_mx][m_my].barva;
			}
			m_hitting = true;
		}
	}
	else m_hitting = false;

	return -1;
}

void GMrcha::Automove()
{
	if (!m_dead)
		if (m_AI) m_AI->AIMove();
}


void GMrcha::Kill()
{
	m_dead = true;
	m_anim = 0;
	m_map->m_mmap[m_mx][m_my]--;
	m_game->m_mrtvol += m_params.score;
}

bool GMrcha::IsFree(int x, int y)
{
	if (m_params.fly) return m_map->IsFlyFree(x, y);
	else return m_map->IsFree(x, y);
}
