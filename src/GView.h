// GView.h: interface for the GView class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GVIEW_H_
#define GVIEW_H_


class GGame;
class CMainFrame;
class GObject;

#include "data.h"
#include "GBase.h"

#define MAX_OBMAP 10

class GView : public GBase
{
public:
	virtual void Destroy();
	void StartTres(int doba);
	void UpdateView();
	void Draw();

	void LoadMap(st_map *map);
	GGame *m_game;
	void Init(CMainFrame *parent, GGame *game);
	GView();
	virtual ~GView();

	struct st_obmap {
		GObject *objects[MAX_OBMAP]; // vsechny
		int      max;         // pocet na aktualnim policku
	} m_obmap[MAX_X][MAX_Y]; // mapa objektu - mrch i bombicu dohromady


	int m_sy;
	int m_sx;

	int m_bx;
	int m_by;

	int m_y;
	int m_x;

	int m_tres;

	int m_bBombic; // velka bitmpapa bombice
	int m_bBonus;  // bonusy
};

#endif // GVIEW_H_
