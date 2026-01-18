// GMrchaAI1.cpp: implementation of the GMrchaAI1 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "GMrchaAI1.h"

#include "GGame.h"
#include "GMrcha.h"
#include "GMap.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GMrchaAI1::GMrchaAI1(GGame *game, GMap *map, GMrcha *mrcha)
:GMrchaAI0(game, map, mrcha)
{

}

GMrchaAI1::~GMrchaAI1()
{

}

void GMrchaAI1::AIMove()
{
	GMrchaAI0::AIMove();

	int x = m_mx;
	int y = m_my;


	if ((m_map->m_bmap[x+1][y].cas > 0 || !m_mrcha->IsFree(x+1, y)) &&
		 (m_map->m_bmap[x-1][y].cas > 0 || !m_mrcha->IsFree(x-1, y)) &&
		 (m_map->m_bmap[x][y+1].cas > 0 || !m_mrcha->IsFree(x, y+1)) &&
		 (m_map->m_bmap[x][y-1].cas > 0 || !m_mrcha->IsFree(x, y-1))) 
	{
		 if (m_map->m_bmap[x][y].cas == 0) CenterMove();
		 return;
	}

	FakeMove();

	while (m_map->m_bmap[m_fx][m_fy].cas > 0) {
		if (!ChangeDir()) return;
		FakeMove();
	}
}
