// GBombaMega.h: interface for the GBombaMega class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GBOMBAMEGA_H_
#define GBOMBAMEGA_H_


#include "GBomba.h"

class GBombaMega : public GBomba
{
public:
	void SetDanger() override;
	void Move() override;
	GBombaMega();
	~GBombaMega() override;
};

#endif // GBOMBAMEGA_H_
