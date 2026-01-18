// GBonus_fireman.h: interface for the GBonus_fireman class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GBONUS_FIREMAN_H_
#define GBONUS_FIREMAN_H_


#include "GBonus.h"

class GBonus_fireman : public GBonus  
{
public:
	void BeforeInput(bool *left, bool *right, bool *up, bool *down, bool *action) override;
	bool HitProtect();
	GBonus_fireman();
	~GBonus_fireman() override;

};

#endif // GBONUS_FIREMAN_H_
