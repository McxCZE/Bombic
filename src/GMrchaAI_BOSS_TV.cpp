// GMrchaAI_BOSS_TV.cpp: implementation of the GMrchaAI_BOSS_TV class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GMrchaAI_BOSS_TV.h"
#include "GGame.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GMrchaAI_BOSS_TV::GMrchaAI_BOSS_TV(GGame *game, GMap *map, GMrcha *mrcha)
:GMrchaAI3(game, map, mrcha)
{
	m_addmrnous = 25;
}

GMrchaAI_BOSS_TV::~GMrchaAI_BOSS_TV()
{

}

void GMrchaAI_BOSS_TV::AIMove()
{
	GMrchaAI3::AIMove();

	if (--m_addmrnous < 1) {
		int m = m_game->AddMrcha(m_mrcha->m_mx, m_mrcha->m_my, 5);
		m_addmrnous = 5 + rand()%25;


		// Aby mrcha "strilela" male mrchy
		if (m != -1) m_game->m_mrcha[m].m_dir = m_mrcha->m_dir;
	}
}
