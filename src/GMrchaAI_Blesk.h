// GMrchaAI_Blesk.h: interface for the GMrchaAI_Blesk class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GMRCHAAI_BLESK_H_
#define GMRCHAAI_BLESK_H_


#include "GMrchaAI0.h"

class GMrchaAI_Blesk : public GMrchaAI0  
{
public:
	void AIMove();
	GMrchaAI_Blesk(GGame *game, GMap *map, GMrcha *mrcha);
	virtual ~GMrchaAI_Blesk();

};

#endif // GMRCHAAI_BLESK_H_
