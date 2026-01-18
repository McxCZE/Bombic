// GBonus_shield.h: interface for the GBonus_shield class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GBONUS_SHIELD_H_
#define GBONUS_SHIELD_H_


#include "GBonus.h"

class GBonus_shield : public GBonus  
{
public:
	void OneTime();
	bool HitProtect() override;
	GBonus_shield();
	~GBonus_shield() override;

};

#endif // GBONUS_SHIELD_H_
