// GBonus_n_kicker.h: interface for the GBonus_n_kicker class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GBONUS_N_KICKER_H_
#define GBONUS_N_KICKER_H_


#include "GBonus.h"

class GBonus_n_kicker : public GBonus  
{
public:
	void AfterPut(int bombID) override;
	GBonus* GetCopy() override;
	GBonus_n_kicker();
	~GBonus_n_kicker() override;

};

#endif // GBONUS_N_KICKER_H_
