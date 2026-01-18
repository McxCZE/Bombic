// GMrchaAI3.h: interface for the GMrchaAI3 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GMRCHAAI3_H_
#define GMRCHAAI3_H_


#include "GMrchaAI2.h"

class GMrchaAI3 : public GMrchaAI2  
{
public:
	virtual void AIMove();
	GMrchaAI3(GGame *game, GMap *map, GMrcha *mrcha);
	virtual ~GMrchaAI3();

};

#endif // GMRCHAAI3_H_
