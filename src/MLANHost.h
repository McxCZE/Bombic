// MLANHost.h: interface for the MLANHost class.
// LAN multiplayer - Host waiting for connection screen
//////////////////////////////////////////////////////////////////////

#ifndef MLANHOST_H_
#define MLANHOST_H_

#include "GBase.h"

class MLANHost : public GBase
{
public:
	virtual int OnKey(int nChar);
	virtual void Move();
	void Destroy();
	void Draw();
	virtual void Init(CMainFrame *parent);
	MLANHost();
	virtual ~MLANHost();

	int m_bMenuBMP;
	bool m_hostStarted;
	char m_localIP[64];
	bool m_autoTransition;  // Auto-transition to lobby when connected
};

#endif // MLANHOST_H_
