// GBombaNapalm.h: interface for the GBombaNapalm class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GBOMBANAPALM_H_
#define GBOMBANAPALM_H_


#include "GBomba.h"

class GBombaNapalm : public GBomba
{
public:
	void Move() override;
	GBombaNapalm();
	~GBombaNapalm() override;
};

#endif // GBOMBANAPALM_H_
