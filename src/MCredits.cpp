// MCredits.cpp: implementation of the MCredits class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MCredits.h"
#include <SDL.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MCredits::MCredits()
{

}

MCredits::~MCredits()
{

}

void MCredits::Destroy()
{
	if (m_bPozadi != -1) d3dx.DestroyTexture(m_bPozadi);
	m_bPozadi = -1;
}

void MCredits::Draw()
{
	d3dx.Draw(m_bPozadi, 400, 300, 0, 1.5625);
	
	// Cerne stiny
	m_fontL->DrawText( 131, 221,  "Bernard", 0, 0, 0);
	m_font->DrawText(  211, 217,  "LIDICKY", 0, 0, 0);
	m_fontL->DrawText( 131, 246,  "bernard@matfyz.cz", 0,0,0);

	m_fontL->DrawText( 511, 341,  "Zdenek", 0,0,0);
	m_font->DrawText(  586, 337,  "BoSWART", 0,0,0);
	m_fontL->DrawText( 511, 366,  "2zdeny@seznam.cz", 0,0,0);

// bile
	m_fontL->DrawText( 130, 220,  "Bernard", 255, 255, 255);
	m_font->DrawText(  210, 216,  "LIDICKy", 255, 255, 255);
	m_fontL->DrawText( 130, 245,  "bernard@matfyz.cz", 255,255,255);

	m_fontL->DrawText( 510, 340,  "Zdenek", 255,255,255);
	m_font->DrawText(  585, 336,  "BoSWART", 255,255,255);
	m_fontL->DrawText( 510, 365,  "2zdeny@seznam.cz", 255,255,255);

}

void MCredits::Init(CMainFrame *parent)
{
	GBase::Init(parent);

	m_bPozadi = d3dx.CreateTextureFromFile("mcredits.jpg");
}

int MCredits::OnKey(int nChar)
{
	switch (nChar) {
	case 27 : 
	case SDLK_RETURN : 
	case SDLK_SPACE  :
		return MENU_MAIN; 
		break;
	}

	return MENU_DEFAULT;
}
