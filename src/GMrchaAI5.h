// GMrchaAI5.h: interface for the GMrchaAI5 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GMRCHAAI5_H_
#define GMRCHAAI5_H_


#include "GMrchaAI3.h"

class GMrchaAI5 : public GMrchaAI3  
{
public:
	virtual void AIMove();
	GMrchaAI5(GGame *game, GMap *map, GMrcha *mrcha);
	virtual ~GMrchaAI5();

};

#endif // GMRCHAAI5_H_
