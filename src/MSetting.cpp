// MSetting.cpp: implementation of the MSetting class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MSetting.h"
#include "D3DXApp.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern	float gspeed; // koeficinet rychlosti hry
extern	bool  gdeadmatch_bombkill; // zda se po urcite dobe ukoncuje dedmatch nasilne
extern	bool  gexplode_predict_visible; // zda je viditelna predikce exploze
extern   int   glangfile;                   // ID jmena souboru s jazykem
extern   bool gsoundavailable;


MSetting::MSetting()
{

}

MSetting::~MSetting()
{

}

void MSetting::Destroy()
{
	if (m_bPozadi != -1) d3dx.DestroyTexture(m_bPozadi);
	m_bPozadi = -1;
}

#define MAX_BUFF 50

void MSetting::Draw()
{
	d3dx.Draw(m_bPozadi, 400, 300, 0, 1.5625);

	m_font->MenuText( 400, 180, lng.Translate(30), 0, m_sel);
	if (gdeadmatch_bombkill) 
		m_font->MenuText( 400, 220, lng.Translate(31), 1, m_sel);
	else 	m_font->MenuText( 400, 220, lng.Translate(32), 1, m_sel);
	if (gexplode_predict_visible) 
		m_font->MenuText( 400, 260, lng.Translate(33), 2, m_sel);
	else 	m_font->MenuText( 400, 260, lng.Translate(34), 2, m_sel);

	char s[MAX_BUFF];
	snprintf(s, MAX_BUFF, "%s : %.1f", lng.Translate(35).c_str(), gspeed);
	m_font->MenuText( 400, 300, s, 3, m_sel);

	m_font->MenuText( 400, 340, lng.Translate(37), 4, m_sel);

	if (gsoundavailable)
		m_font->MenuText( 400, 380, lng.Translate(42), 5, m_sel);
	else    m_font->MenuText( 400, 380, lng.Translate(43), 5, m_sel);
	
	m_font->MenuText( 400, 420, lng.Translate(36), 6, m_sel);
}

void MSetting::Init(CMainFrame *parent)
{
	GBase::Init(parent);

	m_sel = 0;

	m_bPozadi = d3dx.CreateTextureFromFile("msetting.jpg");
}

int MSetting::OnKey(int nChar)
{
	switch (nChar) {
	case SDLK_DOWN   : if (++m_sel > MAX_SETTING_SEL-1) m_sel = 0; g_sb[0].Play();  break;
	case SDLK_UP     : if (--m_sel < 0) m_sel = MAX_SETTING_SEL-1; g_sb[0].Play();  break;
	case SDLK_RETURN : 
	case SDLK_SPACE  :
		g_sb[0].Play();
		switch (m_sel) {
		case 4:
		//TODO: Should be a piece of cake with SDL :-)
		//	CD3DXAppApp::m_fullscreen = !CD3DXAppApp::m_fullscreen;
		//	cerr << "Not yet :-)" << endl;
			break;
		case 6:
			return MENU_MAIN; 
			break;

		}
	case SDLK_RIGHT :
		switch (m_sel) {
		case 0 :
			if (++glangfile >= lng.m_langs) glangfile = 0;
			lng.LoadLanguage(glangfile);
			break;
		case 1:
			gdeadmatch_bombkill = !gdeadmatch_bombkill;
			break;
		case 2: 
			gexplode_predict_visible = !gexplode_predict_visible;
			break;
		case 3:
			if ( (gspeed += 0.1f) > 4) gspeed = 1;
			break;
		case 5: 
			gsoundavailable = !gsoundavailable;
			break;
		}
		break;
	case SDLK_LEFT :
		switch (m_sel) {
		case 0 :
			if (--glangfile < 0) glangfile = lng.m_langs-1;
			lng.LoadLanguage(glangfile);
			g_sb[0].Play();
			break;
		case 1:
			gdeadmatch_bombkill = !gdeadmatch_bombkill;
			g_sb[0].Play();
			break;
		case 2: 
			gexplode_predict_visible = !gexplode_predict_visible;
			g_sb[0].Play();
			break;
		case 3:
			if ( (gspeed -= 0.1f) < 1) gspeed = 4;
			g_sb[0].Play();
			break;
		case 5: 
			gsoundavailable = !gsoundavailable;
			g_sb[0].Play();
			break;
		}
		break;
	case SDLK_ESCAPE : 
		return MENU_MAIN; 
		break;
	}

	return MENU_DEFAULT;
}
