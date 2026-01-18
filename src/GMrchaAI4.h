// GMrchaAI4.h: interface for the GMrchaAI4 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GMRCHAAI4_H_
#define GMRCHAAI4_H_


#include "GMrchaAI0.h"

class GMrchaAI4 : public GMrchaAI0  
{
public:
	virtual void AIMove();
	GMrchaAI4(GGame *game, GMap *map, GMrcha *mrcha);
	virtual ~GMrchaAI4();

};

#endif // GMRCHAAI4_H_
