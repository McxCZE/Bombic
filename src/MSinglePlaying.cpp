// MSinglePlaying.cpp: implementation of the MSinglePlaying class.
//
//////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include "stdafx.h"
#include "D3DXApp.h"
#include "MSinglePlaying.h"
#include "MainFrm.h"


using namespace std;

extern int gmaxmrchovnik;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MSinglePlaying::MSinglePlaying()
{
	for (int i = 0; i < 4; i++) m_bScreens[i] = -1;

// nacteni souboru na singleplayer
	FILE *f = fopen_search("single.dat", "rt");
	if (f == nullptr) {
		cout << "Error loading single.dat" << endl;
		exit(1);
	}

	fscanf(f, "%d\n", &m_levels);

	for (int i = 0; i < m_levels; i++)
		fscanf(f, "%s %d %s %d %d %d %d %d %d %d %d\n", m_data[i].file, &m_data[i].bonuslevel, m_data[i].code, 
		&m_data[i].picturepre, &m_data[i].picturepost, &m_data[i].picturedead, &m_data[i].mrchovnik,
		&m_data[i].text[0], &m_data[i].text[2], &m_data[i].text[3], &m_data[i].needwon);

	m_menustate = -1;

	fclose(f);
}

MSinglePlaying::~MSinglePlaying()
{

}

void MSinglePlaying::Destroy()
{
	for (int i = 0; i < 4; i++) 
		if	(m_bScreens[i] != -1) {
			d3dx.DestroyTexture(m_bScreens[i]);
			m_bScreens[i] = -1;
		}
}

void MSinglePlaying::Draw()
{
	if (m_menustate == 1) ProcessGame();

	if (m_bScreens[m_menustate] != -1) {
		d3dx.Draw(m_bScreens[m_menustate], 400, 300, 0, 1.5625);
		m_font->DrawMultiline( 21, 201, 
			lng.Translate(m_data[m_level].text[m_menustate]), 0, 0, 0);
		m_font->DrawMultiline( 20, 200,
			lng.Translate(m_data[m_level].text[m_menustate]), 255,255,255);
	}

	if (m_menustate == 0)
		m_font->DrawText( 400, 400, m_data[m_level].code, 255, 255, 255);

}

void MSinglePlaying::Init(CMainFrame *parent)
{
	GBase::Init(parent);
	g_sb[1].Play();

	if (m_menustate != -1) LoadLevel();
}

int MSinglePlaying::OnKey(int nChar)
{
	switch (nChar) {
	case SDLK_ESCAPE :
		g_sb[1].Play();
		Destroy();
		m_menustate = -1;
		return MENU_SINGLE; 
		break;
	case SDLK_RETURN : 
		switch (m_menustate) {
		case 0: 
			g_sb[1].Play();
			RunGame(); break;
		case 2: 
			m_level++;
			m_menustate = 0;
			return LoadLevel();
		case 3: // Kdyz prohraje, hraje level znova
//			Destroy();
			m_menustate = 0;
			return MENU_DEFAULT; //MENU_SINGLE;
			break;
		}
		break;
	}

	return MENU_DEFAULT;
}

void MSinglePlaying::StartGame(string code, int players)
{
	m_menustate = 0;
	m_players = players;

	// code
	m_level = 0;
	for (int i = 0; i < m_levels; i++)
		if (strcmp(m_data[i].code, code.c_str()) == 0) m_level = i;

	LoadLevel();
}

void MSinglePlaying::RunGame()
{
	m_menustate = 1;
	m_pParent->StartGame(m_data[m_level].file, m_players, false, true, m_data[m_level].bonuslevel); 
}

void MSinglePlaying::ProcessGame()
{
	GGame *g = &m_pParent->m_game;

	if (m_data[m_level].needwon) m_menustate = 3;
	else m_menustate = 2;

	// zda nekdo prezil
	for (int i = 0; i < m_players; i++)
		if (!g->m_bomber[i].m_dead)
			m_menustate = 2;

	// zmacknul escape
	if (!g->m_gameended) m_menustate = 3;

	if (m_menustate == 2) 
	{
		if (m_data[m_level].picturepost == -1) OnKey(SDLK_RETURN);
	}
		
}

#define MAX_BUFF 100
int MSinglePlaying::LoadLevel()
{
	if (m_level == m_levels) return MENU_MAIN;

	Destroy();

	char s[MAX_BUFF];
	snprintf(s, MAX_BUFF, "msingleA%d.jpg", m_data[m_level].picturepre);
	m_bScreens[0] = d3dx.CreateTextureFromFile(s);

	if (m_data[m_level].picturepost != -1) {
		snprintf(s, MAX_BUFF, "msingleB%d.jpg", m_data[m_level].picturepost);
		m_bScreens[2] = d3dx.CreateTextureFromFile(s);
	}
	else {
		m_bScreens[2] = 0;
	}
	
	snprintf(s, MAX_BUFF, "msingleC%d.jpg", m_data[m_level].picturedead);
	m_bScreens[3] = d3dx.CreateTextureFromFile(s);

	if (m_data[m_level].mrchovnik > gmaxmrchovnik) gmaxmrchovnik = m_data[m_level].mrchovnik;
	
	return MENU_DEFAULT;
}
