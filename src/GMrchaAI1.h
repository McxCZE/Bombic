// GMrchaAI1.h: interface for the GMrchaAI1 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GMRCHAAI1_H_
#define GMRCHAAI1_H_


#include "GMrchaAI0.h"

class GMrchaAI1 : public GMrchaAI0  
{
public:
	virtual void AIMove();
	GMrchaAI1(GGame *game, GMap *map, GMrcha *mrcha);
	virtual ~GMrchaAI1();

};

#endif // GMRCHAAI1_H_
