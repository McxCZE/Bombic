// MLANLobby.cpp: implementation of the MLANLobby class.
// LAN multiplayer - Pre-game lobby
//////////////////////////////////////////////////////////////////////

#include <string>
#include <fstream>
#include <SDL.h>

#include "stdafx.h"
#include "D3DXApp.h"
#include "MLANLobby.h"
#include "MainFrm.h"
#include "Network.h"
#include "GFile.h"
#include "data.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MLANLobby::MLANLobby()
{
	m_sel = 0;
	m_mapID = 0;
	m_monsters = true;
	m_bonuslevel = MAX_BONUS_LEVEL_DEADMATCH;
	m_victories = 3;
	m_files = 0;
	m_localReady = false;
	m_disconnected = false;

	CreateFileList();
}

MLANLobby::~MLANLobby()
{
}

void MLANLobby::Init(CMainFrame *parent)
{
	GBase::Init(parent);

	m_bMenuBMP = d3dx.CreateTextureFromFile("mdead.jpg");
	m_bDeadBMP = d3dx.CreateTextureFromFile("menudead.bmp");
	m_bDead = d3dx.CreateVirualTexture(m_bDeadBMP, 1, 1, 50, 70);

	for (int i = 0; i < 3; i++) {
		char buff[10];
		snprintf(buff, 10, "DM%d.bmp", i);
		m_bDeadMap[i] = d3dx.CreateTextureFromFile(buff);
	}

	g_sb[1].Play();
	m_localReady = false;
	m_disconnected = false;

	// If host, send initial map info
	if (g_network.IsHost() && g_network.IsConnected() && m_files > 0) {
		g_network.SendMapInfo(m_filelist[m_mapID].file.c_str(), m_mapID, m_monsters, m_bonuslevel, m_victories);
	}
}

void MLANLobby::Move()
{
	g_network.Update();

	// Check for disconnection - set flag for auto-transition
	if (!g_network.IsConnected() && !m_disconnected) {
		m_disconnected = true;
	}

	// Client: Check for map info updates from host
	if (g_network.IsClient() && g_network.HasMapInfoUpdate()) {
		const NetMapInfoPacket& info = g_network.GetMapInfo();
		m_mapID = info.mapID;
		m_monsters = info.monsters != 0;
		m_bonuslevel = info.bonuslevel;
		m_victories = info.victories;
		// Find matching map file in our list
		for (int i = 0; i < m_files; i++) {
			if (m_filelist[i].file == info.mapFile) {
				m_mapID = i;
				break;
			}
		}
		g_network.ClearMapInfoUpdate();
	}

	// Client: Check if game started
	if (g_network.IsClient() && g_network.HasGameStarted()) {
		// Game will start - handled by returning MENU_LAN_PLAYING in OnKey
	}
}

void MLANLobby::Draw()
{
	d3dx.Draw(m_bMenuBMP, 400, 300, 0, 1.5625);

	// Show map preview
	if (m_files > 0) {
		int pozadi = m_filelist[m_mapID].pozadi;
		if (pozadi > 2) pozadi = 0;
		d3dx.Draw(m_bDeadMap[pozadi], 400, 150);
	}

	// Show 2 players (always 2 for LAN)
	d3dx.Draw(m_bDead, 350, 190, 0, 1, 1, 128, 0, 1);
	d3dx.Draw(m_bDead, 350, 190, 0, 1, 1, 255, 0);
	d3dx.Draw(m_bDead, 420, 190, 0, 1, 1, 128, 0, 1);
	d3dx.Draw(m_bDead, 420, 190, 0, 1, 1, 255, 1);

	// Show connection status
	bool isHost = g_network.IsHost();
	const char* roleText = isHost ? "HOST" : "CLIENT";
	m_font->DrawText(400, 50, roleText, 255, 255, 0, HFONT_CENTER);

	// Menu items - host can change settings, client can only see
	int menuY = 260;
	int menuSel = isHost ? m_sel : -1;  // Client can't select

	if (isHost) {
		m_font->MenuText(400, menuY, _LNG_LAN_START_, 0, menuSel);
	} else {
		// Client shows ready toggle instead of start
		if (m_localReady) {
			m_font->MenuText(400, menuY, _LNG_LAN_READY_, 0, menuSel);
		} else {
			m_font->MenuText(400, menuY, _LNG_LAN_NOT_READY_, 0, menuSel);
		}
	}
	menuY += 30;

	// Map selection
	if (m_files > 0) {
		m_font->MenuText(400, menuY, m_filelist[m_mapID].file, 1, menuSel);
	}
	menuY += 30;

	m_font->MenuText(400, menuY, _LNG_DEADMATCH_MONSTERS_, 2, menuSel);
	menuY += 30;

	m_font->MenuText(400, menuY, _LNG_DEADMATCH_VICTORIES_, 3, menuSel);
	menuY += 30;

	m_font->MenuText(400, menuY, _LNG_DEADMATCH_BONUSLEVEL_, 4, menuSel);
	menuY += 40;

	m_font->MenuText(400, menuY, _LNG_LAN_BACK_, 5, menuSel);

	// Show ready status
	if (isHost) {
		const char* remoteStatus = g_network.IsRemoteReady() ? "Player 2: READY" : "Player 2: NOT READY";
		m_font->DrawText(400, 480, remoteStatus, 200, 200, 200, HFONT_CENTER);
	} else {
		const char* status = m_localReady ? "You are READY" : "Press ENTER to toggle READY";
		m_font->DrawText(400, 480, status, 200, 200, 200, HFONT_CENTER);
	}

	// Show settings values
	char settingsText[128];
	snprintf(settingsText, sizeof(settingsText), "Monsters: %s  Victories: %d  Bonus: %d",
		m_monsters ? "ON" : "OFF", m_victories, m_bonuslevel);
	m_font->DrawText(400, 510, settingsText, 150, 150, 150, HFONT_CENTER);

	// Show disconnect notification
	if (m_disconnected) {
		m_font->DrawText(400, 550, "Other player disconnected!", 255, 0, 0, HFONT_CENTER);
	}
}

void MLANLobby::Destroy()
{
	d3dx.DestroyTexture(m_bMenuBMP);
	d3dx.DestroyTexture(m_bDeadBMP);
	d3dx.DestroyTexture(m_bDead);
	for (int i = 0; i < 3; i++) {
		d3dx.DestroyTexture(m_bDeadMap[i]);
	}
}

void MLANLobby::CreateFileList()
{
	const char* maps_path = locate_file("deathmaps.txt");
	if (maps_path == nullptr) return;
	ifstream f(maps_path);

	string s;
	while (f && !f.bad() && !f.eof() && f >> s) {
		st_map map;
		GFile gfile;
		gfile.LoadMap(&map, s);
		if (map.deadmach && m_files < MAX_LAN_MAPS) {
			m_filelist[m_files].file = s;
			m_filelist[m_files].pozadi = map.podklad;
			m_files++;
		}
	}
}

int MLANLobby::OnKey(int nChar)
{
	bool isHost = g_network.IsHost();

	// Check for auto-transition on disconnect (called with nChar=0 from Move via MMenu)
	if (nChar == 0 && m_disconnected) {
		m_disconnected = false;
		g_network.Disconnect();
		return MENU_LAN;
	}

	// Client: check if game started
	if (g_network.IsClient() && g_network.HasGameStarted()) {
		// Apply host's gspeed setting for synchronized gameplay
		const NetGameStartPacket& info = g_network.GetGameStartInfo();
		gspeed = info.gspeed_x10 / 10.0f;
		g_network.ClearGameStarted();
		return MENU_LAN_PLAYING;
	}

	// Check for disconnection (manual key press)
	if (!g_network.IsConnected()) {
		g_network.Disconnect();
		return MENU_LAN;
	}

	switch (nChar) {
	case SDLK_ESCAPE:
		g_network.Disconnect();
		return MENU_LAN;
		break;

	case SDLK_DOWN:
		if (isHost) {
			if (++m_sel > MAX_LAN_LOBBY_SEL-1) m_sel = 0;
			g_sb[0].Play();
		}
		break;

	case SDLK_UP:
		if (isHost) {
			if (--m_sel < 0) m_sel = MAX_LAN_LOBBY_SEL-1;
			g_sb[0].Play();
		}
		break;

	case SDLK_RETURN:
	case SDLK_SPACE:
		if (isHost) {
			g_sb[1].Play();
			switch (m_sel) {
			case 0:  // Start game
				if (m_files > 0 && g_network.IsRemoteReady()) {
					g_network.SendGameStart(m_filelist[m_mapID].file.c_str(), m_mapID, m_monsters, m_bonuslevel, m_victories, gspeed);
					return MENU_LAN_PLAYING;
				}
				break;
			case 1:  // Map
				if (++m_mapID >= m_files) m_mapID = 0;
				g_network.SendMapInfo(m_filelist[m_mapID].file.c_str(), m_mapID, m_monsters, m_bonuslevel, m_victories);
				break;
			case 2:  // Monsters
				m_monsters = !m_monsters;
				g_network.SendMapInfo(m_filelist[m_mapID].file.c_str(), m_mapID, m_monsters, m_bonuslevel, m_victories);
				break;
			case 3:  // Victories
				if (++m_victories > 9) m_victories = 1;
				g_network.SendMapInfo(m_filelist[m_mapID].file.c_str(), m_mapID, m_monsters, m_bonuslevel, m_victories);
				break;
			case 4:  // Bonus level
				if (++m_bonuslevel > MAX_BONUS_LEVEL_DEADMATCH) m_bonuslevel = 8;
				g_network.SendMapInfo(m_filelist[m_mapID].file.c_str(), m_mapID, m_monsters, m_bonuslevel, m_victories);
				break;
			case 5:  // Back
				g_network.Disconnect();
				return MENU_LAN;
			}
		} else {
			// Client: toggle ready
			m_localReady = !m_localReady;
			g_network.SendReady(m_localReady);
			g_sb[1].Play();
		}
		break;

	case SDLK_LEFT:
		if (isHost) {
			g_sb[0].Play();
			switch (m_sel) {
			case 1:  // Map
				if (--m_mapID < 0) m_mapID = m_files - 1;
				g_network.SendMapInfo(m_filelist[m_mapID].file.c_str(), m_mapID, m_monsters, m_bonuslevel, m_victories);
				break;
			case 2:  // Monsters
				m_monsters = !m_monsters;
				g_network.SendMapInfo(m_filelist[m_mapID].file.c_str(), m_mapID, m_monsters, m_bonuslevel, m_victories);
				break;
			case 3:  // Victories
				if (--m_victories < 1) m_victories = 9;
				g_network.SendMapInfo(m_filelist[m_mapID].file.c_str(), m_mapID, m_monsters, m_bonuslevel, m_victories);
				break;
			case 4:  // Bonus level
				if (--m_bonuslevel < 8) m_bonuslevel = MAX_BONUS_LEVEL_DEADMATCH;
				g_network.SendMapInfo(m_filelist[m_mapID].file.c_str(), m_mapID, m_monsters, m_bonuslevel, m_victories);
				break;
			}
		}
		break;

	case SDLK_RIGHT:
		if (isHost) {
			g_sb[0].Play();
			switch (m_sel) {
			case 1:  // Map
				if (++m_mapID >= m_files) m_mapID = 0;
				g_network.SendMapInfo(m_filelist[m_mapID].file.c_str(), m_mapID, m_monsters, m_bonuslevel, m_victories);
				break;
			case 2:  // Monsters
				m_monsters = !m_monsters;
				g_network.SendMapInfo(m_filelist[m_mapID].file.c_str(), m_mapID, m_monsters, m_bonuslevel, m_victories);
				break;
			case 3:  // Victories
				if (++m_victories > 9) m_victories = 1;
				g_network.SendMapInfo(m_filelist[m_mapID].file.c_str(), m_mapID, m_monsters, m_bonuslevel, m_victories);
				break;
			case 4:  // Bonus level
				if (++m_bonuslevel > MAX_BONUS_LEVEL_DEADMATCH) m_bonuslevel = 8;
				g_network.SendMapInfo(m_filelist[m_mapID].file.c_str(), m_mapID, m_monsters, m_bonuslevel, m_victories);
				break;
			}
		}
		break;
	}

	return MENU_DEFAULT;
}
