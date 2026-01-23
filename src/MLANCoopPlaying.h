// MLANCoopPlaying.h: interface for the MLANCoopPlaying class.
// LAN Co-op Story Mode - Game session manager
//////////////////////////////////////////////////////////////////////

#ifndef MLANCOOPPLAYING_H_
#define MLANCOOPPLAYING_H_

#include <string>
#include "GBase.h"
#include "MSinglePlaying.h"  // For st_singleplay

class MLANCoopPlaying : public GBase
{
public:
	virtual int OnKey(int nChar);
	virtual void Move();
	void Destroy();
	void Draw();
	virtual void Init(CMainFrame *parent);
	MLANCoopPlaying();
	virtual ~MLANCoopPlaying();

	// Start campaign at specified level (used by both host and client)
	void StartCampaign(int startLevel);

	// Level data loaded from single.dat
	MSinglePlaying::st_singleplay m_data[MAX_SINGLE_LEVELS];
	int m_levels;
	int m_level;  // Current level (0-39)

	// Menu state: 0=intro, 1=playing, 2=victory, 3=defeat
	int m_menustate;

	// Background screens for each state
	int m_bScreens[4];

	// Disconnect flag
	bool m_disconnected;

private:
	void LoadLevelData();
	int LoadLevel();
	void RunGame();
	void ProcessGame();
	void AdvanceLevel();
};

#endif // MLANCOOPPLAYING_H_
