// GMrchaAI3.cpp: implementation of the GMrchaAI3 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "GMrchaAI3.h"

#include "GGame.h"
#include "GMrcha.h"
#include "GMap.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GMrchaAI3::GMrchaAI3(GGame *game, GMap *map, GMrcha *mrcha)
:GMrchaAI2(game, map, mrcha)
{

}

GMrchaAI3::~GMrchaAI3()
{

}

void GMrchaAI3::AIMove()
{
	GMrchaAI2::AIMove();

	for (int i = 0; i < m_game->m_bombers; i++)
		if (m_game->m_bomber[i].m_dead == false) {
			if (m_mx == m_game->m_bomber[i].m_mx && (m_my == m_game->m_bomber[i].m_my+1)) m_mrcha->m_dir = 3;
			if (m_mx == m_game->m_bomber[i].m_mx && (m_my == m_game->m_bomber[i].m_my-1)) m_mrcha->m_dir = 4;
			if (m_my == m_game->m_bomber[i].m_my && (m_mx == m_game->m_bomber[i].m_mx+1)) m_mrcha->m_dir = 1;
			if (m_my == m_game->m_bomber[i].m_my && (m_mx == m_game->m_bomber[i].m_mx-1)) m_mrcha->m_dir = 2;
		}
}
