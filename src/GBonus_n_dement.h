// GBonus_n_dement.h: interface for the GBonus_n_dement class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GBONUS_N_DEMENT_H_
#define GBONUS_N_DEMENT_H_


#include "GBonus.h"

class GBonus_n_dement : public GBonus  
{
public:
	GBonus* GetCopy() override;
	void BeforeMove(float *speed) override;
	GBonus_n_dement();
	~GBonus_n_dement() override;

};

#endif // GBONUS_N_DEMENT_H_
