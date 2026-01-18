// MSetting.h: interface for the MSetting class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MSETTING_H_
#define MSETTING_H_


#include "GBase.h"

#define MAX_SETTING_SEL 7

class MSetting : public GBase  
{
public:
	MSetting();
	virtual ~MSetting();

	virtual int OnKey(int nChar);
	void Init(CMainFrame *parent);
	virtual void Draw();
	virtual void Destroy();

	int      m_bPozadi;
	int      m_sel;
};

#endif // MSETTING_H_
