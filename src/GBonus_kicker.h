// GBonus_kicker.h: interface for the GBonus_kicker class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GBONUS_KICKER_H_
#define GBONUS_KICKER_H_


#include "GBonus.h"

class GBonus_kicker : public GBonus  
{
public:
	void OneTime() override;
	GBonus_kicker();
	~GBonus_kicker() override;

};

#endif // GBONUS_KICKER_H_
