// MLANPlaying.h: interface for the MLANPlaying class.
// LAN multiplayer game wrapper - handles networked deathmatch rounds
//////////////////////////////////////////////////////////////////////

#ifndef MLANPLAYING_H_
#define MLANPLAYING_H_

#include <string>
#include "GBase.h"

class MLANPlaying : public GBase
{
public:
	virtual int OnKey(int nChar);
	virtual void Move();
	void Destroy();
	void Draw();
	virtual void Init(CMainFrame *parent);
	void StartGame(std::string file, bool monsters, int bonuslevel, int victories);
	void RunGame();
	void ProcessGame();
	MLANPlaying();
	virtual ~MLANPlaying();

	bool m_monsters;
	int m_victories;
	int m_bonuslevel;
	std::string m_file;

	int m_bPozadi;
	int m_bDeadBMP;
	int m_bDead;

	bool m_gamerunned;
	bool m_gameend;
	bool m_waitingForNextRound;  // Client waiting for host to start next round
	bool m_disconnected;         // Other player disconnected

	int m_score[2];  // Only 2 players in LAN
};

#endif // MLANPLAYING_H_
