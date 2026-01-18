// GBonus_timer.h: interface for the GBonus_timer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GBONUS_TIMER_H_
#define GBONUS_TIMER_H_


#include "GBonus.h"

class GBonus_timer : public GBonus  
{
public:
	void End() override;
	void OneTime() override;
	GBonus_timer();
	~GBonus_timer() override;

};

#endif // GBONUS_TIMER_H_
