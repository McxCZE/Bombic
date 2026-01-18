// GBonus_n_slow.h: interface for the GBonus_n_slow class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GBONUS_N_SLOW_H_
#define GBONUS_N_SLOW_H_


#include "GBonus.h"

class GBonus_n_slow : public GBonus  
{
public:
	GBonus* GetCopy() override;
	void BeforeMove(float *speed) override;
	GBonus_n_slow();
	~GBonus_n_slow() override;

};

#endif // GBONUS_N_SLOW_H_
