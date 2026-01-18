// GBonus_bomb.h: interface for the GBonus_bomb class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GBONUS_BOMB_H_
#define GBONUS_BOMB_H_


#include "GBonus.h"

class GBonus_bomb : public GBonus  
{
public:
	void OneTime() override;
	GBonus_bomb();
	~GBonus_bomb() override;

};

#endif // GBONUS_BOMB_H_
