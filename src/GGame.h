// GGame.h: interface for the GGame class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GGAME_H_
#define GGAME_H_

#define MAX_BOMBERS    4
#define MAX_GMRCH    256

#include <string>
#include "GBase.h"
#include "GMap.h"
#include "GBomber.h"
#include "GMrcha.h"
#include "GView.h"


class GGame : public GBase
{
public:
	int  AddMrcha(int x, int y, int mrchaID);
	bool m_gameended;
	bool EndGame();  // zda uz je konec hry (vybiti bombicu) / vybiti mrch

	bool m_deadmatch;
	void LoadMap(const std::string file, int players, bool deadmatch, bool monsters, int bonuslevel);
	virtual int OnKey(int nChar);
	virtual void Destroy();
	virtual void Move();
	virtual void Draw();
	virtual void Init(CMainFrame *parent);
	GGame();
	virtual ~GGame();

	GView    m_view;

	int      m_gameendig; // trva nekolik snimku, nez hra skonci

	int      m_game_time; // kolik framu uz trva hra

	int      m_show_game_speed; // jak dlouho se ma jeste zobrazovat rychlost hry

	bool     m_paused;  // zda je hra zapauzovana

	GMap     m_map;
	GBomber  m_bomber[MAX_BOMBERS];
	GMrcha   m_mrcha[MAX_GMRCH];
	int      m_bombers;
	int      m_mrch;

	int      m_mrtvol; // pocet mrtvych mrch
	bool     m_show_mrtvol; // zda se ma ukazovat pocet mrtvol

	int  m_bBomberBMP[MAX_BOMBERS];
	int  m_bBombaBMP;
	int  m_bMrchaBMP[MAX_MRCH];

	int  m_bBomberBMP_s;
	int  m_bBombaBMP_s;
	int  m_bMrchaBMP_s[MAX_MRCH];

	int  m_bBomber[MAX_BOMBERS];
	int  m_bBomba;
	int  m_bMrcha[MAX_MRCH];

	int  m_bBomber_s;
	int  m_bBomba_s;
	int  m_bMrcha_s[MAX_MRCH];
};

#endif // GGAME_H_

