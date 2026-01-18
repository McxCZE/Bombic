// GBonus_posilani.h: interface for the GBonus_posilani class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GBONUS_POSILANI_H_
#define GBONUS_POSILANI_H_


#include "GBonus.h"

class GBonus_posilani : public GBonus  
{
public:
	void OneTime() override;
	GBonus_posilani();
	~GBonus_posilani() override;

};

#endif // GBONUS_POSILANI_H_
