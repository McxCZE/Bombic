// GMrchaAI_Blesk.cpp: implementation of the GMrchaAI_Blesk class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GMrchaAI_Blesk.h"
#include "GMrcha.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GMrchaAI_Blesk::GMrchaAI_Blesk(GGame *game, GMap *map, GMrcha *mrcha)
:GMrchaAI0(game, map, mrcha)
{

}

GMrchaAI_Blesk::~GMrchaAI_Blesk()
{

}

void GMrchaAI_Blesk::AIMove()
{
	int dir = m_mrcha->m_dir;
	
	m_tochange = 10;

	GMrchaAI0::AIMove();

	if (m_mrcha->m_dir != dir || dir == 0) m_mrcha->Kill();
}
