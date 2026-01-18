// MCredits.h: interface for the MCredits class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MCREDITS_H_
#define MCREDITS_H_


#include "GBase.h"

class MCredits : public GBase  
{
public:
	MCredits();
	virtual ~MCredits();

	virtual int OnKey(int nChar);
	void Init(CMainFrame *parent);
	virtual void Draw();
	virtual void Destroy();

	int      m_bPozadi;
};

#endif // MCREDITS_H_
