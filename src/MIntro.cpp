// MIntro.cpp: implementation of the MIntro class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MIntro.h"
#include "MainFrm.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MIntro::MIntro()
{

}

MIntro::~MIntro()
{

}

void MIntro::Draw()
{
	int alpha = 255;

	if (m_timer < SHOW_TIME) alpha = (m_timer * 255)/SHOW_TIME ;
	else if (m_timer > SCREEN_TIME) alpha = 0;
		  else if (m_timer > SCREEN_TIME-SHOW_TIME) alpha = ((SCREEN_TIME-m_timer)*255)/SHOW_TIME;

	d3dx.Draw(m_bIntro[m_screen], 400, 300, 0, 1, 1, alpha);

}

void MIntro::Init(CMainFrame *parent)
{
	GBase::Init(parent);
	m_bIntro[0] = d3dx.CreateTextureFromFile("menu3.bmp");
	m_bIntro[1] = d3dx.CreateTextureFromFile("menu.bmp");
	m_timer = 0;
	m_screen = 0;
}

int MIntro::OnKey(int nChar)
{
	m_pParent->StartMenu();
	return 0;
}

void MIntro::Destroy()
{
	for (int i = 0; i < MAX_INTRO; i++)
		d3dx.DestroyTexture(m_bIntro[i]);
}

void MIntro::Move()
{
	if (++m_timer > SCREEN_TIME) {
		if (++m_screen >= MAX_INTRO) m_pParent->StartMenu();
		m_timer = 0;
	}
}
