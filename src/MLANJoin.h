// MLANJoin.h: interface for the MLANJoin class.
// LAN multiplayer - Join game screen (enter IP)
//////////////////////////////////////////////////////////////////////

#ifndef MLANJOIN_H_
#define MLANJOIN_H_

#include "GBase.h"

class MLANJoin : public GBase
{
public:
	virtual int OnKey(int nChar);
	virtual void Move();
	void Destroy();
	void Draw();
	virtual void Init(CMainFrame *parent);
	MLANJoin();
	virtual ~MLANJoin();

	int m_bMenuBMP;
	bool m_connecting;
	char m_hostIP[64];
	int m_ipCursor;
	int m_blinkTimer;
	bool m_autoTransition;  // Auto-transition to lobby when connected
};

#endif // MLANJOIN_H_
