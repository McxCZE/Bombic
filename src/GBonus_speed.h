// GBonus_speed.h: interface for the GBonus_speed class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GBONUS_SPEED_H_
#define GBONUS_SPEED_H_


#include "GBonus.h"

class GBonus_speed : public GBonus  
{
public:
	void OneTime() override;
	GBonus_speed();
	~GBonus_speed() override;

};

#endif // GBONUS_SPEED_H_
