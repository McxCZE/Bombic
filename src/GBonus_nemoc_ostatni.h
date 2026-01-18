// GBonus_nemoc_ostatni.h: interface for the GBonus_nemoc_ostatni class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GBONUS_NEMOC_OSTATNI_H_
#define GBONUS_NEMOC_OSTATNI_H_


#include "GBonus.h"
#include "GGame.h"

class GBonus_nemoc_ostatni : public GBonus  
{
public:
	void OneTime() override;
	GBonus_nemoc_ostatni(GGame *game);
	~GBonus_nemoc_ostatni() override;

	GGame *m_game;
	
};

#endif // GBONUS_NEMOC_OSTATNI_H_
