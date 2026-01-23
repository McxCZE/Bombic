// MLAN.cpp: implementation of the MLAN class.
// LAN multiplayer menu - Host/Join selection
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "MLAN.h"
#include "MainFrm.h"

// Global flag to indicate co-op mode (vs deathmatch)
bool g_coopMode = false;

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

	m_font->MenuText( 400, 220, _LNG_LAN_DEATHMATCH_, 0, m_sel);
	m_font->MenuText( 400, 270, _LNG_LAN_COOP_, 1, m_sel);
	m_font->MenuText( 400, 320, _LNG_LAN_JOIN_, 2, m_sel);
	m_font->MenuText( 400, 400, _LNG_LAN_BACK_, 3, m_sel);
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
		case 0 : g_coopMode = false; return MENU_LAN_HOST; break;  // Deathmatch host
		case 1 : g_coopMode = true; return MENU_LAN_HOST; break;   // Co-op host
		case 2 : return MENU_LAN_JOIN; break;                       // Join (mode determined by host)
		case 3 : return MENU_MAIN; break;
		}
		break;
	}
	return MENU_DEFAULT;
}
