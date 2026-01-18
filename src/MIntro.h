// MIntro.h: interface for the MIntro class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MINTRO_H_
#define MINTRO_H_


#include "GBase.h"

#define MAX_INTRO     2
#define SCREEN_TIME  80
#define SHOW_TIME    20

class MIntro : public GBase  
{
public:
	virtual void Move();
	virtual void Destroy();
	virtual int OnKey(int nChar);
	virtual void Init(CMainFrame *parent);
	virtual void Draw();
	MIntro();
	virtual ~MIntro();

	int  m_timer;
	int  m_screen;

	int  m_bIntro[MAX_INTRO];
};

#endif // MINTRO_H_
