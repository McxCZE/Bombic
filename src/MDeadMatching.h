// MDeadMatching.h: interface for the MDeadMatching class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MDEADMATCHING_H_
#define MDEADMATCHING_H_


#include <string>
#include "GBase.h"

class MDeadMatching : public GBase  
{
public:
	int OnKey(int nChar);
	virtual void Destroy();
	void ProcessGame();
	virtual void Draw();
	void RunGame();
	void Init(CMainFrame *parent);
	void StartGame(std::string file, int players, bool monsters, int bonuslevel, int vicotries);
	MDeadMatching();
	virtual ~MDeadMatching();

	int      m_players;
	bool     m_monsters;
	int      m_victories;
	int      m_bonuslevel;
	std::string  m_file;

	int      m_bPozadi;

	int      m_bDeadBMP;
	int      m_bDead;

	bool     m_gamerunned;
	bool     m_gameend;

	int      m_score[4];
};

#endif // MDEADMATCHING_H_
