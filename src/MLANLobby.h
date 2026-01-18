// MLANLobby.h: interface for the MLANLobby class.
// LAN multiplayer - Pre-game lobby
//////////////////////////////////////////////////////////////////////

#ifndef MLANLOBBY_H_
#define MLANLOBBY_H_

#include <string>
#include "GBase.h"

#define MAX_LAN_MAPS 30
#define MAX_LAN_LOBBY_SEL 6

class MLANLobby : public GBase
{
public:
	virtual int OnKey(int nChar);
	virtual void Move();
	void CreateFileList();
	void Destroy();
	void Draw();
	virtual void Init(CMainFrame *parent);
	MLANLobby();
	virtual ~MLANLobby();

	int m_bMenuBMP;
	int m_bDeadMap[3];
	int m_bDeadBMP;
	int m_bDead;

	int m_sel;

	// Game settings (host can change, client receives)
	int m_mapID;
	bool m_monsters;
	int m_bonuslevel;
	int m_victories;

	// Map list (loaded same as deathmatch)
	struct st_filelist {
		std::string file;
		int pozadi;
	} m_filelist[MAX_LAN_MAPS];
	int m_files;

	// Ready state
	bool m_localReady;

	// Disconnect auto-transition
	bool m_disconnected;
};

#endif // MLANLOBBY_H_
