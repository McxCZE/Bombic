// MLAN.h: interface for the MLAN class.
// LAN multiplayer menu - Host/Join selection
//////////////////////////////////////////////////////////////////////

#ifndef MLAN_H_
#define MLAN_H_

#include "GBase.h"

#define MAX_LAN_SEL 4

// Global flag to indicate co-op mode (vs deathmatch)
extern bool g_coopMode;

class MLAN : public GBase
{
public:
	virtual int OnKey(int nChar);
	void Destroy();
	void Draw();
	virtual void Init(CMainFrame *parent);
	MLAN();
	virtual ~MLAN();

	int m_sel;
	int m_bMenuBMP;
};

#endif // MLAN_H_
