// GBonus_megabomb.h: interface for the GBonus_megabomb class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GBONUS_MEGABOMB_H_
#define GBONUS_MEGABOMB_H_


#include "GBonus.h"

class GBonus_megabomb : public GBonus  
{
public:
	void OneTime() override;
	GBonus_megabomb();
	~GBonus_megabomb() override;

};

#endif // GBONUS_MEGABOMB_H_
