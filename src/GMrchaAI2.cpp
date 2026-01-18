// GMrchaAI2.cpp: implementation of the GMrchaAI2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "GMrchaAI2.h"

#include "GGame.h"
#include "GMrcha.h"
#include "GMap.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GMrchaAI2::GMrchaAI2(GGame *game, GMap *map, GMrcha *mrcha)
:GMrchaAI1(game, map, mrcha)
{

}

GMrchaAI2::~GMrchaAI2()
{

}

void GMrchaAI2::AIMove()
{
	GMrchaAI1::AIMove();

	int x = m_mx;
	int y = m_my;


	if ((m_map->m_bmap[x+1][y].danger || !m_mrcha->IsFree(x+1, y)) &&
		 (m_map->m_bmap[x-1][y].danger || !m_mrcha->IsFree(x-1, y)) &&
		 (m_map->m_bmap[x][y+1].danger || !m_mrcha->IsFree(x, y+1)) &&
		 (m_map->m_bmap[x][y-1].danger || !m_mrcha->IsFree(x, y-1))) 
	{
		 if (m_map->m_bmap[x][y].danger == false) CenterMove();
		 return;
	}

	FakeMove();

	while (m_map->m_bmap[m_fx][m_fy].danger) {
		if (!ChangeDir()) return;
		FakeMove();
	}
}
