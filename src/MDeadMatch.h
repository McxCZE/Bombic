// MDeadMatch.h: interface for the MDeadMatch class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MDEADMATCH_H_
#define MDEADMATCH_H_

#include <string>
#include "GBase.h"

#define MAX_FILES   30
#define MAX_DEAD_SEL 7
#define MAX_MAPS     3

class MDeadMatch : public GBase  
{
public:
	virtual int OnKey(int nChar);
	void CreateFileList();
	virtual void Destroy();
	virtual void Draw();
	virtual void Init(CMainFrame *parent);
	MDeadMatch();
	virtual ~MDeadMatch();

	int      m_anim;

	int      m_ID; // ve m_filelist
	int      m_players;
	bool     m_monsters;
	int      m_victories;
	int      m_bonuslevel;

	int      m_bMenuDead;

	int      m_bDeadBMP;
	int      m_bDead;
	int      m_bDeadMap[3];

	int      m_bDeadBonusBMP;
	int      m_bDeadBonus;


	int      m_sel;

	struct st_filelist {
		std::string file;
		int     pozadi;
	} m_filelist[MAX_FILES];
	int     m_files;

};

#endif // MDEADMATCH_H_
