// MDeadMatch.cpp: implementation of the MDeadMatch class.
//
//////////////////////////////////////////////////////////////////////

#include <string>
#include <SDL.h>

#include "stdafx.h"
#include "D3DXApp.h"
#include "MDeadMatch.h"
#include "GFile.h"
#include "MainFrm.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const int bonuslevel[][2] = 
{
	{2, 0}, // smrt
	{1, 0}, // bomba
	{0, 0}, // plamen
	{3, 0}, // botasky
	{4, 0}, // odpalovac
	{0, 1}, // megabomba
	{3, 1}, // kopacky
	{1, 2}, // napalm
	{0, 2}, // posilani
	{2, 1}, // stit
	{1, 1}, // zivot
	{4, 1}, // nemoc ostatni
	{2, 2}, // fireman
};

MDeadMatch::MDeadMatch()
{
	m_anim = 0;
	m_players = 2;
	m_monsters = true;
	m_sel = 0;
	m_victories = 3;
	m_bonuslevel = MAX_BONUS_LEVEL_DEADMATCH; //8-18;


	m_files = 0;
	m_ID = 0;


	CreateFileList();
}

MDeadMatch::~MDeadMatch()
{

}

void MDeadMatch::Init(CMainFrame *parent)
{
	GBase::Init(parent);

	m_bDeadBMP = d3dx.CreateTextureFromFile("menudead.bmp");
	m_bDead = d3dx.CreateVirualTexture(m_bDeadBMP, 1, 1, 50, 70);

	m_bMenuDead = d3dx.CreateTextureFromFile("mdead.jpg");

	for (int i = 0; i < MAX_MAPS; i++) {
		char buff[10];
		snprintf(buff, 10, "DM%d.bmp", i);
		m_bDeadMap[i] = d3dx.CreateTextureFromFile(buff);
	}

	m_bDeadBonusBMP = d3dx.CreateTextureFromFile("bonusy.bmp");
	m_bDeadBonus  = d3dx.CreateVirualTexture(m_bDeadBonusBMP, 1, 1, 50, 50);
}

void MDeadMatch::Draw()
{
	int i;

	d3dx.Draw(m_bMenuDead, 400, 300, 0, 1.5625);

	if (m_filelist[m_ID].pozadi > 2)
		d3dx.Draw(m_bDeadMap[0], 400, 190);
	else
		d3dx.Draw(m_bDeadMap[m_filelist[m_ID].pozadi], 400, 190);
	
/*
	for (i = 0; i < m_players; i++)
		d3dx.Draw(m_bDead, 300+i*60, 190, 0, 0.5, 0.5, 128, 0, 1);
	for (i = 0; i < m_players; i++)
		d3dx.Draw(m_bDead, 300+i*60, 190, 0, 0.5, 0.5, 255, i);

	if (m_monsters) {
		d3dx.Draw(m_bDead, 300, 230, 0, 0.5, 0.5, 128, 1, 1);
		d3dx.Draw(m_bDead, 300, 230, 0, 0.5, 0.5, 255, 4);
	}
	else {
		d3dx.Draw(m_bDead, 550, 230, 0, 0.5, 0.5, 128, 1, 1);
		d3dx.Draw(m_bDead, 550, 230, 0, 0.5, 0.5, 255, 3, 1);
	}

	for (i = 0; i < m_victories; i++)
		d3dx.Draw(m_bDead, 280+i*30, 140, 0, 0.5, 0.5, 255, 2, 1);
*/

#define FACTOR 1

	for (i = 0; i < m_players; i++)
		d3dx.Draw(m_bDead, 285+i*58*FACTOR, 190, 0, FACTOR, FACTOR, 128, 0, 1);
	for (i = 0; i < m_players; i++)
		d3dx.Draw(m_bDead, 285+i*58*FACTOR, 190, 0, FACTOR, FACTOR, 255, i);

	if (m_monsters) {
		d3dx.Draw(m_bDead, 520, 195, 0, FACTOR, FACTOR, 128, 1, 1);
		d3dx.Draw(m_bDead, 520, 195, 0, FACTOR, FACTOR, 255, 4);
	}
	else {
		d3dx.Draw(m_bDead, 520, 195, 0, FACTOR, FACTOR, 128, 1, 1);
		d3dx.Draw(m_bDead, 520, 195, 0, FACTOR, FACTOR, 255, 3, 1);
	}

	for (i = 0; i < m_victories; i++)
		d3dx.Draw(m_bDead, 280+i*30, 140, 0, 0.75, 0.75, 255, 2, 1);


	for (i = 5; i < m_bonuslevel; i++)
		d3dx.Draw(m_bDeadBonus, 277+(i-5)*20, 240, 0, 1, 1, 255, bonuslevel[i-5][0], bonuslevel[i-5][1]);
//	d3dx.Draw(m_bDead, 285+i*30, 240, 0, 0.5, 0.5, 255, 4, 1);


	m_font->MenuText( 400, 290, _LNG_DEADMATCH_START_, 0, m_sel);
	m_font->MenuText( 400, 321, m_filelist[m_ID].file, 1, m_sel);
	m_font->MenuText( 400, 353, _LNG_DEADMATCH_PLAYERS_, 2, m_sel);
	m_font->MenuText( 400, 384, _LNG_DEADMATCH_MONSTERS_, 3, m_sel);
	m_font->MenuText( 400, 415, _LNG_DEADMATCH_VICTORIES_, 4, m_sel);
	m_font->MenuText( 400, 446, _LNG_DEADMATCH_BONUSLEVEL_, 5, m_sel);
	m_font->MenuText( 400, 480, _LNG_DEADMATCH_BACK_, 6, m_sel);
}

void MDeadMatch::Destroy()
{
	d3dx.DestroyTexture(m_bMenuDead);
	d3dx.DestroyTexture(m_bDeadBMP);
	d3dx.DestroyTexture(m_bDead);
	d3dx.DestroyTexture(m_bDeadBonus);
	d3dx.DestroyTexture(m_bDeadBonusBMP);

	for (int i = 0; i < MAX_MAPS; i++)
		d3dx.DestroyTexture(m_bDeadMap[i]);
}

void MDeadMatch::CreateFileList()
{
	const char* maps_path = locate_file("deathmaps.txt");
	if ( maps_path == nullptr) return;
	ifstream f(maps_path);

	string s;
	while (f && !f.bad() && !f.eof() && f >> s)
	{
		st_map map;
		GFile gfile;
		gfile.LoadMap(&map, s);
		if (map.deadmach && m_files < MAX_FILES) {
			m_filelist[m_files].file = s;
			m_filelist[m_files].pozadi = map.podklad;
			m_files++;
		}
	}
		
	
/*
   CFileFind finder;
	GFile     gfile;
	st_map    map;

   BOOL bWorking = finder.FindFile("*.ble");
   while (bWorking)
   {
      bWorking = finder.FindNextFile();
		gfile.LoadMap(&map, finder.GetFileName());
		if (map.deadmach) {
			m_filelist[m_files].file = finder.GetFileName();
			m_filelist[m_files].pozadi = map.podklad;
			m_files++;
		}
   }
   */
}

int MDeadMatch::OnKey(int nChar)
{
	switch (nChar) {
	case SDLK_DOWN   : if (++m_sel > MAX_DEAD_SEL-1) m_sel = 0;  g_sb[0].Play(); break;
	case SDLK_UP : if (--m_sel < 0) m_sel = MAX_DEAD_SEL-1;  g_sb[0].Play(); break;
	case SDLK_RETURN :
	case SDLK_SPACE :
		switch (m_sel) {
		case 0 : return MENU_DEADMATCHING; break;
		case 1 : g_sb[0].Play(); if (++m_ID >= m_files) m_ID = 0; break;
		case 2 : g_sb[0].Play(); if (++m_players > 4) m_players = 2; break;
		case 3 : g_sb[0].Play(); m_monsters = !m_monsters; break;
		case 4 : g_sb[0].Play(); if (++m_victories >  9) m_victories = 1; break;
		case 5 : g_sb[0].Play(); if (++m_bonuslevel > MAX_BONUS_LEVEL_DEADMATCH) m_bonuslevel = 8; break;
		case 6 : return MENU_MAIN; break;
		}
		break;
	case SDLK_LEFT :
		switch (m_sel) {
		case 1 : g_sb[0].Play(); if (--m_ID < 0) m_ID = m_files-1; break;
		case 2 : g_sb[0].Play(); if (--m_players < 2) m_players = 4; break;
		case 3 : g_sb[0].Play(); m_monsters = !m_monsters; break;
		case 4 : g_sb[0].Play(); if (--m_victories < 1) m_victories = 9; break;
		case 5 : g_sb[0].Play(); if (--m_bonuslevel < 8) m_bonuslevel = MAX_BONUS_LEVEL_DEADMATCH; break;
		}
		break;
	case SDLK_RIGHT : 
		switch (m_sel) {
		case 1 : g_sb[0].Play(); if (++m_ID >= m_files) m_ID = 0; break;
		case 2 : g_sb[0].Play(); if (++m_players > 4) m_players = 2; break;
		case 3 : g_sb[0].Play(); m_monsters = !m_monsters; break;
		case 4 : g_sb[0].Play(); if (++m_victories >  9) m_victories = 1; break;
		case 5 : g_sb[0].Play(); if (++m_bonuslevel > MAX_BONUS_LEVEL_DEADMATCH) m_bonuslevel = 8; break;
		}
		break;
	}

	return MENU_DEFAULT;
}
