// GBonus_n_stop.h: interface for the GBonus_n_stop class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GBONUS_N_STOP_H_
#define GBONUS_N_STOP_H_


#include "GBonus.h"

class GBonus_n_stop : public GBonus  
{
public:
	void BeforeMove(float *speed) override;
	GBonus* GetCopy() override;
	GBonus_n_stop();
	~GBonus_n_stop() override;

};

#endif // GBONUS_N_STOP_H_
