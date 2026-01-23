// GBase.h: interface for the GBase class.
//
//////////////////////////////////////////////////////////////////////

#ifndef GBASE_H_
#define GBASE_H_


#include "HFont.h"


#define MENU_DEFAULT          0  // zadna zmena
#define MENU_MAIN             1  // hlavni menu
#define MENU_DEADMATCH        2  // dead match menu
#define MENU_DEADMATCHING     3  // probihajici souboj
#define MENU_SINGLE           4
#define MENU_SINGLEPLAYING    5
#define MENU_END              6  // koec hry
#define MENU_MRCHOVNIK        7  // seznam mrch
#define MENU_SETTING          8
#define MENU_HELP             9
#define MENU_CREDITS          10
#define MENU_LAN              11  // LAN multiplayer menu
#define MENU_LAN_HOST         12  // Host LAN game
#define MENU_LAN_JOIN         13  // Join LAN game
#define MENU_LAN_LOBBY        14  // LAN lobby
#define MENU_LAN_PLAYING      15  // LAN game in progress
#define MENU_LAN_COOP_LOBBY   16  // Co-op story lobby
#define MENU_LAN_COOP_PLAYING 17  // Co-op story game in progress

class CMainFrame;
class CD3DFont;

extern float gspeed; // koeficinet rychlosti hry

#include "HTranslate.h"     // jazyk pro preklad

class GBase  
{
public:
	virtual void Destroy();
	GBase();
	virtual ~GBase();

	virtual void Init(CMainFrame *parent);

	virtual int  OnKey(int /*nChar*/) { return MENU_DEFAULT; }
	virtual void Draw() {}
	virtual void Move() {}

protected:
	CMainFrame *m_pParent;
	HFont   *m_font;
	HFont   *m_fontL;
};

#endif // GBASE_H_
