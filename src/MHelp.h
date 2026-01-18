// MHelp.h: interface for the MHelp class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MHELP_H_
#define MHELP_H_


#include "GBase.h"

class MHelp : public GBase  
{
public:
	MHelp();
	virtual ~MHelp();

	virtual int OnKey(int nChar);
	void Init(CMainFrame *parent);
	virtual void Draw();
	virtual void Destroy();

	int      m_bPozadi;
};

#endif // MHELP_H_
