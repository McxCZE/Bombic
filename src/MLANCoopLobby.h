// MLANCoopLobby.h: interface for the MLANCoopLobby class.
// LAN Co-op Story Mode - Pre-game lobby for level selection
//////////////////////////////////////////////////////////////////////

#ifndef MLANCOOPLOBBY_H_
#define MLANCOOPLOBBY_H_

#include <string>
#include "GBase.h"
#include "MSinglePlaying.h"  // For st_singleplay

#define MAX_COOP_LOBBY_SEL 3

class MLANCoopLobby : public GBase
{
public:
	virtual int OnKey(int nChar);
	virtual void Move();
	void Destroy();
	void Draw();
	virtual void Init(CMainFrame *parent);
	MLANCoopLobby();
	virtual ~MLANCoopLobby();

	// Get selected level for game start
	int GetSelectedLevel() const { return m_level; }

	int m_bMenuBMP;
	int m_sel;

	// Level data loaded from single.dat
	MSinglePlaying::st_singleplay m_data[MAX_SINGLE_LEVELS];
	int m_levels;
	int m_level;  // Selected starting level (0-39)

	// Cheat code entry (for host to skip to levels)
	std::string m_code;
	int m_blinkTimer;

	// Ready state
	bool m_localReady;

	// Disconnect auto-transition
	bool m_disconnected;

private:
	void LoadLevelData();
	void SendLevelInfo();
};

#endif // MLANCOOPLOBBY_H_
