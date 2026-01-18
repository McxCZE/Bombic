// MMain.cpp: implementation of the MMain class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "MMain.h"
#include "MainFrm.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MMain::MMain()
{
	m_sel = 0;
}

MMain::~MMain()
{

}

void MMain::Init(CMainFrame *parent)
{
	GBase::Init(parent);

	m_bMainBMP = d3dx.CreateTextureFromFile("mmain.jpg");
	g_sb[1].Play();
}

void MMain::Draw()
{
	d3dx.Draw(m_bMainBMP, 400, 300, 0, 1.5625);


	m_font->MenuText( 400, 180, _LNG_MAIN_STORY_, 0, m_sel);
	m_font->MenuText( 400, 220, _LNG_MAIN_DEAD_, 1, m_sel);
	m_font->MenuText( 400, 260, _LNG_MAIN_MRCHOVNIK_, 2, m_sel);
	m_font->MenuText( 400, 300, _LNG_MAIN_SETTING_, 3, m_sel);
	m_font->MenuText( 400, 340, _LNG_MAIN_HELP_, 4, m_sel);
	m_font->MenuText( 400, 380, _LNG_MAIN_CREDITS_, 5, m_sel);
	m_font->MenuText( 400, 420, _LNG_MAIN_EXIT_, 6, m_sel);
}

void MMain::Destroy()
{
	d3dx.DestroyTexture(m_bMainBMP);
}

int MMain::OnKey(int nChar)
{
	switch (nChar) {
	case SDLK_DOWN : if (++m_sel > MAX_MAIN_SEL-1) m_sel = 0; g_sb[0].Play(); break;
	case SDLK_UP : if (--m_sel < 0) m_sel = MAX_MAIN_SEL-1; g_sb[0].Play(); break;
	case SDLK_ESCAPE : return MENU_END; break;
	case SDLK_RETURN : 
		g_sb[1].Play();
		switch (m_sel) {
		case 0 : return MENU_SINGLE; break;
		case 1 : return MENU_DEADMATCH; break;
		case 2 : return MENU_MRCHOVNIK; break;
		case 3 : return MENU_SETTING; break;
		case 4 : return MENU_HELP; break;
		case 5 : return MENU_CREDITS; break;
		case 6 : return MENU_END; break;
		}
		break;
	}
	return MENU_DEFAULT;

}
