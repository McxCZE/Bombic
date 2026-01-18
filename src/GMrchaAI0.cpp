// GMrchaAI0.cpp: implementation of the GMrchaAI0 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "GMrchaAI0.h"
#include "GGame.h"
#include "GMrcha.h"
#include "GMap.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GMrchaAI0::GMrchaAI0(GGame *game, GMap *map, GMrcha *mrcha)
{
	m_tochange = 0;
	m_game = game;
	m_map = map;
	m_mrcha = mrcha;
}

GMrchaAI0::~GMrchaAI0()
{

}

void GMrchaAI0::AIMove()
{
	m_x = (int)m_mrcha->m_x;
	m_y = (int)m_mrcha->m_y;
	m_mx = m_mrcha->m_mx;
	m_my = m_mrcha->m_my;

	if (NeedChange())
		ChangeDir();
	
	m_tochange--;
}

bool GMrchaAI0::ChangeDir()
{
	do {
		m_mrcha->m_dir = rand()%4 + 1;
		m_tochange = 10 + rand()%5 + rand()%40;

		if (!m_mrcha->IsFree(m_mx-1,m_my) && !m_mrcha->IsFree(m_mx+1,m_my) &&
			!m_mrcha->IsFree(m_mx,m_my-1) && !m_mrcha->IsFree(m_mx,m_my+1)) {
			m_mrcha->m_dir = 0;
			m_tochange = 5;
			return false;
		}
	} while (NeedChange());

	return true;
}

bool GMrchaAI0::NeedChange()
{
	return ((m_x <= 25 && !m_mrcha->IsFree(m_mx-1,m_my) && m_mrcha->m_dir == 1) ||
           (m_x >= 25 && !m_mrcha->IsFree(m_mx+1,m_my) && m_mrcha->m_dir == 2) ||
           (m_y <= 25 && !m_mrcha->IsFree(m_mx,m_my-1) && m_mrcha->m_dir == 3) ||
           (m_y >= 25 && !m_mrcha->IsFree(m_mx,m_my+1) && m_mrcha->m_dir == 4) ||
		     m_tochange == 0);
}

void GMrchaAI0::FakeMove()
{
	m_fx = m_mx;
	m_fy = m_my;
	switch (m_mrcha->m_dir) {
	case 1 : m_fx = m_mx - 1; break;
	case 2 : m_fx = m_mx + 1; break;
	case 3 : m_fy = m_my - 1; break;
	case 4 : m_fy = m_my + 1; break;
	}	
}


void GMrchaAI0::CenterMove()
{
	m_mrcha->m_dir = 0;
	if (m_x < 25 - m_mrcha->m_params.speed) m_mrcha->m_dir = 2;
	if (m_x > 25 + m_mrcha->m_params.speed) m_mrcha->m_dir = 1;
	if (m_y < 25 - m_mrcha->m_params.speed) m_mrcha->m_dir = 4;
	if (m_x > 25 + m_mrcha->m_params.speed) m_mrcha->m_dir = 3;
}
