// GMrchaAI_BOSS_TV.h: interface for the GMrchaAI_BOSS_TV class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GMRCHAAI_BOSS_TV_H_
#define GMRCHAAI_BOSS_TV_H_


#include "GMrchaAI3.h"

class GMrchaAI_BOSS_TV : public GMrchaAI3  
{
public:
	void AIMove();
	GMrchaAI_BOSS_TV(GGame *game, GMap *map, GMrcha *mrcha);
	virtual ~GMrchaAI_BOSS_TV();

	int m_addmrnous;
};

#endif // GMRCHAAI_BOSS_TV_H_
