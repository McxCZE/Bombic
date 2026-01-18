// GBonus_napalmbomb.h: interface for the GBonus_napalmbomb class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GBONUS_NAPALMBOMB_H_
#define GBONUS_NAPALMBOMB_H_


#include "GBonus.h"

class GBonus_napalmbomb : public GBonus  
{
public:
	void OneTime();
	GBonus_napalmbomb();
	~GBonus_napalmbomb() override;

};

#endif // GBONUS_NAPALMBOMB_H_
