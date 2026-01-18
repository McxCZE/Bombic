// GMrchaAI2.h: interface for the GMrchaAI2 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GMRCHAAI2_H_
#define GMRCHAAI2_H_


#include "GMrchaAI1.h"

class GMrchaAI2 : public GMrchaAI1  
{
public:
	virtual void AIMove();
	GMrchaAI2(GGame *game, GMap *map, GMrcha *mrcha);
	virtual ~GMrchaAI2();

};

#endif // GMRCHAAI2_H_
