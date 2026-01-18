// MLAN.cpp: implementation of the MLAN class.
// LAN multiplayer menu - Host/Join selection
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "MLAN.h"
#include "MainFrm.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MLAN::MLAN()
{
	m_sel = 0;
}

MLAN::~MLAN()
{
}

void MLAN::Init(CMainFrame *parent)
{
	GBase::Init(parent);
	m_bMenuBMP = d3dx.CreateTextureFromFile("mmain.jpg");
	g_sb[1].Play();
}

void MLAN::Draw()
{
	d3dx.Draw(m_bMenuBMP, 400, 300, 0, 1.5625);

	m_font->MenuText( 400, 240, _LNG_LAN_HOST_, 0, m_sel);
	m_font->MenuText( 400, 290, _LNG_LAN_JOIN_, 1, m_sel);
	m_font->MenuText( 400, 370, _LNG_LAN_BACK_, 2, m_sel);
}

void MLAN::Destroy()
{
	d3dx.DestroyTexture(m_bMenuBMP);
}

int MLAN::OnKey(int nChar)
{
	switch (nChar) {
	case SDLK_DOWN : if (++m_sel > MAX_LAN_SEL-1) m_sel = 0; g_sb[0].Play(); break;
	case SDLK_UP : if (--m_sel < 0) m_sel = MAX_LAN_SEL-1; g_sb[0].Play(); break;
	case SDLK_ESCAPE : return MENU_MAIN; break;
	case SDLK_RETURN :
		g_sb[1].Play();
		switch (m_sel) {
		case 0 : return MENU_LAN_HOST; break;
		case 1 : return MENU_LAN_JOIN; break;
		case 2 : return MENU_MAIN; break;
		}
		break;
	}
	return MENU_DEFAULT;
}
