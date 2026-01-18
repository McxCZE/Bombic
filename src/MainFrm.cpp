// MainFrm.cpp : implementation of the CMainFrame class
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include "stdafx.h"
#include "D3DXApp.h"

#include "MainFrm.h"


using namespace std;


/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_actual = nullptr;

	m_font  = new HFont("FreeMono.ttf", 16);
	m_fontL = new HFont("FreeMono.ttf", 25);

	// Window title is set in HD3DX::Create via SDL_CreateWindow
}

CMainFrame::~CMainFrame()
{
	if (m_font)	delete m_font;
	if (m_fontL)	delete m_fontL;
}



void CMainFrame::OnTimer() 
{

	static int t1, t2, dm = 0, dd = 0, fs = 200; 

	t1 = SDL_GetTicks();
	Move();
	t2 = SDL_GetTicks();
	dm += t2-t1;

	t1 = t2;
	Draw();
	t2 = SDL_GetTicks();
	dd += t2-t1;

	if (--fs <= 0) {
		cout << "Move: " << dm/200.0 << " ms/frame; Draw: "
		     << dd/200.0 << " ms/frame" << endl;
		dm = dd = 0;
		fs = 200;
	}
}

void CMainFrame::Move()
{
	if (m_actual) m_actual->Move();

}

void CMainFrame::Draw()
{
	if (d3dx.m_valid == false) return;

	d3dx.BeginScene();

	if (m_actual) m_actual->Draw();


	m_font->DrawText( 400, 10, "Beta", 255,255,0);
#ifdef VERSION	
	m_font->DrawText( 400, 30, VERSION, 255,255,0);
#endif	


	d3dx.EndScene();
}


bool CMainFrame::OnKeyDown(int nChar) 
{

	if (m_actual) {
		if (m_actual->OnKey(nChar) == MENU_END) return false;
	}
	return true;
}

bool CMainFrame::StartGame(const std::string file, int players, bool deadmatch, bool monsters, int bonuslevel)
{
	if (m_actual != nullptr) m_actual->Destroy();
	d3dx.DestroyTextures();
	m_game.Init(this);
	m_game.LoadMap(file, players, deadmatch, monsters, bonuslevel);
	m_actual = &m_game;

	return true;
}

bool CMainFrame::StartMenu()
{

	if (m_actual != nullptr) m_actual->Destroy();
	d3dx.DestroyTextures();
	m_menu.Init(this);
	m_actual = &m_menu;
	

	return true;
}

