// GBonus_fire.h: interface for the GBonus_fire class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GBONUS_FIRE_H_
#define GBONUS_FIRE_H_


#include "GBonus.h"

class GBonus_fire : public GBonus  
{
public:
	void OneTime() override;
	GBonus_fire();
	~GBonus_fire() override;

};

#endif // GBONUS_FIRE_H_
