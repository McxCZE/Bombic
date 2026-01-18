// MSingle.cpp: implementation of the MSingle class.
//
//////////////////////////////////////////////////////////////////////

#include <string>
#include "stdafx.h"
#include "D3DXApp.h"
#include "MSingle.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MSingle::MSingle()
{

}

MSingle::~MSingle()
{

}

void MSingle::Init(CMainFrame *parent)
{
	GBase::Init(parent);

	m_bDeadBMP = d3dx.CreateTextureFromFile("menudead.bmp");
	m_bDead = d3dx.CreateVirualTexture(m_bDeadBMP, 1, 1, 50, 70);

	m_code = "";
	m_sel = 0;
	m_players = 1;
	m_bPozadi = d3dx.CreateTextureFromFile("mstory.jpg");
}

void MSingle::Destroy()
{
	d3dx.DestroyTexture(m_bDeadBMP);
	d3dx.DestroyTexture(m_bDead);
	if (m_bPozadi != -1) d3dx.DestroyTexture(m_bPozadi);
	m_bPozadi = -1;
}

#define MAX_BUFF 50
void MSingle::Draw()
{
	int i;

	d3dx.Draw(m_bPozadi, 400, 300, 0, 1.5625);


	for (i = 0; i < m_players; i++)
		d3dx.Draw(m_bDead, 400-30*(m_players-1)+i*60, 200, 0, 1, 1, 128, 0, 1);
	for (i = 0; i < m_players; i++)
		d3dx.Draw(m_bDead, 400-30*(m_players-1)+i*60, 200, 0, 1, 1, 255, i);

	m_font->MenuText( 400, 290, _LNG_SINGLE_START_, 0, m_sel);
	m_font->MenuText( 400, 330,  _LNG_SINGLE_PLAYERS_, 1, m_sel);

	string s = _LNG_SINGLE_CODE_ + ": " + m_code + "_";
	m_font->MenuText( 400, 370, s, 2, m_sel);

	m_font->MenuText( 400, 410, _LNG_SINGLE_BACK_, 3, m_sel);
}

int MSingle::OnKey(int nChar)
{
	switch (nChar) {
	case SDLK_DOWN : if (++m_sel > MAX_SINGLE_SEL-1) m_sel = 0;  g_sb[0].Play(); break;
	case SDLK_UP : if (--m_sel < 0) m_sel = MAX_SINGLE_SEL-1;  g_sb[0].Play(); break;
	case SDLK_ESCAPE : return MENU_MAIN; break;
	case SDLK_SPACE :
	case SDLK_RETURN : 
		switch (m_sel) {
		case 2 : 
		case 0 : return MENU_SINGLEPLAYING; break;
		case 1 : g_sb[0].Play(); if (++m_players > 4) m_players = 1; break;
		case 3 : return MENU_MAIN; break;
		}
		break;
	case SDLK_LEFT : 
		switch (m_sel) {
		case 1 : g_sb[0].Play(); if (--m_players < 1) m_players = 4; break;
		}
		break;
	case SDLK_RIGHT : 
		switch (m_sel) {
		case 1 : g_sb[0].Play(); if (++m_players > 4) m_players = 1; break;
		}
		break;
	case SDLK_DELETE:
	case SDLK_CLEAR:
	case SDLK_BACKSPACE:
	
		if (m_code.size()) {
			m_code.erase(m_code.size()-1,1);
			g_sb[9].Play();
		}
		
		break;
	default:
	
		if (nChar >= SDLK_a && nChar <= SDLK_z && m_sel == 2)
			if (m_code.size() < 6) { 
				m_code += (nChar-SDLK_a+'A');
				g_sb[9].Play();
			}		
			
	}

	return MENU_DEFAULT;

}
