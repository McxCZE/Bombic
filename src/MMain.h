// MMain.h: interface for the MMain class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MMAIN_H_
#define MMAIN_H_


#include "GBase.h"

#ifdef HAVE_SDL2_NET
#define MAX_MAIN_SEL 8
#else
#define MAX_MAIN_SEL 7
#endif

class MMain : public GBase  
{
public:
	virtual int OnKey(int nChar);
	void Destroy();
	void Draw();
	virtual void Init(CMainFrame *parent);
	MMain();
	virtual ~MMain();

	int m_sel;
	int m_bMainBMP;
};

#endif // MMAIN_H_
