// MMenu.h: interface for the MMenu class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MMENU_H_
#define MMENU_H_


#include "GBase.h"

#include "MMain.h"
#include "MDeadMatch.h"
#include "MDeadMatching.h"
#include "MSingle.h"
#include "MSinglePlaying.h"
#include "MMrchovnik.h"
#include "MHelp.h"
#include "MSetting.h"
#include "MCredits.h"



class MMenu : public GBase  
{
public:
	virtual void Destroy();
	virtual void Draw();
	virtual int OnKey(int nChar);
	virtual void Move();
	virtual void Init(CMainFrame *parent);
	MMenu();
	virtual ~MMenu();

	GBase *m_actual;

	MMain          m_Mmain;
	MDeadMatch     m_Mdeadmatch;
	MDeadMatching  m_Mdeadmatching;
	MSingle        m_Msingle;
	MSinglePlaying m_Msingleplaying;
	MMrchovnik     m_Mmrchovnik;
	MHelp          m_Mhelp;
	MSetting       m_Msetting;
	MCredits       m_Mcredits;

	int m_bMenuBMP;
	int m_bLogo;
	int m_bHLogo;

	float m_size;
	float m_dsize;

};

#endif // MMENU_H_
