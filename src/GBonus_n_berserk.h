// GBonus_n_berserk.h: interface for the GBonus_n_berserk class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GBONUS_N_BERSERK_H_
#define GBONUS_N_BERSERK_H_


#include "GBonus.h"

class GBonus_n_berserk : public GBonus  
{
public:
	GBonus* GetCopy() override;
	void BeforeMove(float *speed) override;
	GBonus_n_berserk();
	~GBonus_n_berserk() override;

};

#endif // GBONUS_N_BERSERK_H_
