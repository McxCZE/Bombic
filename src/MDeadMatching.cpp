// MDeadMatching.cpp: implementation of the MDeadMatching class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "MDeadMatching.h"
#include "MainFrm.h"
#include <string>

using namespace std;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MDeadMatching::MDeadMatching()
{

}

MDeadMatching::~MDeadMatching()
{

}

void MDeadMatching::StartGame(string file, int players, bool monsters, int bonuslevel, int vicotries)
{
	m_players = players;
	m_monsters = monsters;
	m_victories = vicotries;
	m_bonuslevel = bonuslevel;
	m_file = file;

	for (int i = 0; i < m_players; i++)	m_score[i] = 0;

	RunGame();	
	m_gameend = false;
}

void MDeadMatching::Init(CMainFrame *parent)
{
	g_sb[1].Play();

	GBase::Init(parent);

	if (m_gamerunned) ProcessGame();


	m_bDeadBMP = d3dx.CreateTextureFromFile( "menudead.bmp");
	m_bDead = d3dx.CreateVirualTexture(m_bDeadBMP, 1, 1, 50, 70);

	if (m_gameend)
		m_bPozadi = d3dx.CreateTextureFromFile("mdeadmatching2.jpg");
	else
		m_bPozadi = d3dx.CreateTextureFromFile("mdeadmatching.jpg");

}

void MDeadMatching::RunGame()
{

	m_pParent->StartGame(m_file, m_players, true, m_monsters, m_bonuslevel); 
	m_gamerunned = true;
}

void MDeadMatching::Draw()
{
	d3dx.Draw(m_bPozadi, 400, 300, 0, 1.5625);

	for (int i = 0, y = 330-(30*m_players); i < m_players; i++, y += 60) {
		if (m_gameend && m_score[i] < m_victories)
			d3dx.Draw(m_bDead, 100, y, 0, 1, 1, 255, i, 2);
		else {
			d3dx.Draw(m_bDead, 100, y, 0, 1, 1, 128, 0, 1);
			d3dx.Draw(m_bDead, 100, y, 0, 1, 1, 255, i);
		}

		for (int j = 0; j < m_score[i]; j++)
			d3dx.Draw(m_bDead, 160+j*60, y, 0, 1, 1, 255, 2, 1);
	}
}

void MDeadMatching::ProcessGame()
{
	GGame *g = &m_pParent->m_game;
	if (!g->m_gameended) return;

	for (int i = 0; i < m_players; i++)
		if (!g->m_bomber[i].m_dead) {
			if (++m_score[i] == m_victories) m_gameend = true;
			break;
		}

	m_gamerunned = false;
}

void MDeadMatching::Destroy()
{
	if (m_bPozadi != -1) d3dx.DestroyTexture(m_bPozadi);
	m_bPozadi = -1;
	d3dx.DestroyTexture(m_bDeadBMP);
	d3dx.DestroyTexture(m_bDead);
}

int MDeadMatching::OnKey(int nChar)
{
	switch (nChar) {
	case SDLK_ESCAPE : g_sb[1].Play(); return MENU_DEADMATCH; break;
	case SDLK_RETURN :
		g_sb[1].Play();
		if (m_gameend) return MENU_DEADMATCH;
		RunGame();
		break;
	}

	return MENU_DEFAULT;
}
