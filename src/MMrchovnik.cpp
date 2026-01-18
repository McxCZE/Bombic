// MMrchovnik.cpp: implementation of the MMrchovnik class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "stdafx.h"
#include "MMrchovnik.h"
#include "data.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern int gmaxmrchovnik;

const int to_P_mrcha[] =
{
	-1, 0, 1, 2, 7, 6, 8, 14, 4, 9, 3, 10,
};

const int id_to_IQ[] =
{
	-10, 5, 20, 50, 0, 50, 50, 50, 0, 0,
};

MMrchovnik::MMrchovnik()
{
	m_sel = 0;
	m_bMrcha = -1;
}

MMrchovnik::~MMrchovnik()
{

}

void MMrchovnik::Destroy()
{
	if (m_bMrcha != -1) d3dx.DestroyTexture(m_bMrcha);
	m_bMrcha = -1;
}

#define MAX_BUFF 50
void MMrchovnik::Draw()
{
	if (m_bMrcha != -1)
		d3dx.Draw(m_bMrcha, 400, 300, 0, 1.5625);

	char s[MAX_BUFF];
// popisky mrch
	m_fontL->DrawMultiline( 25, 125, lng.Translate(100+m_sel), 255, 255, 255);
	m_font->DrawMultiline( 25, 420,  lng.Translate(120+m_sel), 255, 255, 255);


// IQ
	m_font->DrawText( 25, 320, lng.Translate(61), 255, 255, 255);
	if (m_sel != 0) {
		snprintf(s, MAX_BUFF, "%d", id_to_IQ[P_mrcha[to_P_mrcha[m_sel]].ai]);
		m_fontL->DrawText( 268, 324, s, 255, 255, 255, 255);
	}
	else {
		m_fontL->DrawText( 268, 324, lng.Translate(62), 255, 255, 255);
	}

// Rychlost
	m_font->DrawText( 25, 350, lng.Translate(60), 255, 255, 255);
	if (m_sel != 0) {
		snprintf(s, MAX_BUFF, "%.0f", P_mrcha[to_P_mrcha[m_sel]].speed);
		m_fontL->DrawText( 268, 354, s, 255, 255, 255);
	}
	else {
		snprintf(s, MAX_BUFF,"%.0f", P_bomber.speed);
		m_fontL->DrawText( 268, 354, s, 255, 255, 255);
	}

	// Zivoty
	m_font->DrawText( 25, 380, lng.Translate(63), 255, 255, 255);
	if (m_sel != 0) {
		snprintf(s, MAX_BUFF, "%d", P_mrcha[to_P_mrcha[m_sel]].lives);
		m_fontL->DrawText( 268, 384, s, 255, 255, 255);
	}
	else {
		m_fontL->DrawText( 268, 384, "1", 255, 255, 255);
	}

}

void MMrchovnik::Init(CMainFrame *parent)
{
	GBase::Init(parent);

	m_max_sel = gmaxmrchovnik;
	LoadMrcha();
}

int MMrchovnik::OnKey(int nChar)
{
	switch (nChar) {
	case SDLK_RIGHT :
		if (++m_sel > m_max_sel) m_sel = 0; 
		LoadMrcha(); 
		g_sb[0].Play(); 
		break;
	case SDLK_LEFT :
		if (--m_sel < 0) m_sel = m_max_sel; 
		LoadMrcha(); 
		g_sb[0].Play(); 
		break;
	case SDLK_ESCAPE : 
	case SDLK_RETURN : 
	case SDLK_SPACE :
		return MENU_MAIN; 
		break;
	}

	return MENU_DEFAULT;
}

void MMrchovnik::LoadMrcha()
{
	if (m_bMrcha != -1) d3dx.DestroyTexture(m_bMrcha);
	char s[MAX_BUFF];
	snprintf(s, MAX_BUFF, "mrchovnik%d.jpg", m_sel);
	m_bMrcha = d3dx.CreateTextureFromFile(s);
}
