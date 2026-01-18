// GMrchaAI_BOSS_DUDLIK.h: interface for the GMrchaAI_BOSS_DUDLIK class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GMRCHAAI_BOSS_DUDLIK_H_
#define GMRCHAAI_BOSS_DUDLIK_H_


#include "GMrchaAI3.h"

class GMrchaAI_BOSS_DUDLIK : public GMrchaAI3  
{
public:
	void AIMove();
	GMrchaAI_BOSS_DUDLIK(GGame *game, GMap *map, GMrcha *mrcha);
	virtual ~GMrchaAI_BOSS_DUDLIK();

	int m_addmrnous;
	int m_addblesk;
};

#endif // GMRCHAAI_BOSS_DUDLIK_H_
