// GBonus_live.h: interface for the GBonus_live class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GBONUS_LIVE_H_
#define GBONUS_LIVE_H_


#include "GBonus.h"

class GBonus_live : public GBonus  
{
public:
	void OneTime() override;
	GBonus_live();
	~GBonus_live() override;

};

#endif // GBONUS_LIVE_H_
