// GMrchaAI5.cpp: implementation of the GMrchaAI5 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GMrchaAI5.h"
#include "GMrcha.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GMrchaAI5::GMrchaAI5(GGame *game, GMap *map, GMrcha *mrcha)
:GMrchaAI3(game, map, mrcha)
{

}

GMrchaAI5::~GMrchaAI5()
{

}

void GMrchaAI5::AIMove()
{
	GMrchaAI3::AIMove();
	if (m_mrcha->m_dir != 0) m_tochange = 10;
}
