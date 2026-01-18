// GMrchaAI4.cpp: implementation of the GMrchaAI4 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GMrchaAI4.h"
#include "GMrcha.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GMrchaAI4::GMrchaAI4(GGame *game, GMap *map, GMrcha *mrcha)
:GMrchaAI0(game, map, mrcha)
{

}

GMrchaAI4::~GMrchaAI4()
{

}

void GMrchaAI4::AIMove()
{
	GMrchaAI0::AIMove();
	if (m_mrcha->m_dir != 0) m_tochange = 10;
}
