// GMrchaAI_BOSS_DUDLIK.cpp: implementation of the GMrchaAI_BOSS_DUDLIK class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GMrchaAI_BOSS_DUDLIK.h"
#include "GGame.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GMrchaAI_BOSS_DUDLIK::GMrchaAI_BOSS_DUDLIK(GGame *game, GMap *map, GMrcha *mrcha)
:GMrchaAI3(game, map, mrcha)
{
	m_addmrnous = 250;
	m_addblesk  = 50;
}

GMrchaAI_BOSS_DUDLIK::~GMrchaAI_BOSS_DUDLIK()
{

}

void GMrchaAI_BOSS_DUDLIK::AIMove()
{
	GMrchaAI3::AIMove();

	if (--m_addblesk < 1) {
		int m = m_game->AddMrcha(m_mrcha->m_mx, m_mrcha->m_my, 5);
		m_addblesk = 5 + rand()%25;


		// Aby mrcha "strilela" male mrchy
		if (m != -1) m_game->m_mrcha[m].m_dir = m_mrcha->m_dir;
	}

	if (--m_addmrnous < 1) {
		int m = m_game->AddMrcha(m_mrcha->m_mx, m_mrcha->m_my, 3);
		m_addmrnous = 10 + rand()%400;


		// Aby mrcha "strilela" male mrchy
		if (m != -1) m_game->m_mrcha[m].m_dir = m_mrcha->m_dir;
	}

}
