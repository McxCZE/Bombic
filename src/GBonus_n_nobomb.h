// GBonus_n_nobomb.h: interface for the GBonus_n_nobomb class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GBONUS_N_NOBOMB_H_
#define GBONUS_N_NOBOMB_H_


#include "GBonus.h"

class GBonus_n_nobomb : public GBonus  
{
public:
	GBonus* GetCopy() override;
	void BeforeMove(float *speed) override;
	GBonus_n_nobomb();
	~GBonus_n_nobomb() override;

};

#endif // GBONUS_N_NOBOMB_H_
