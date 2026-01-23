// MLANCoopLobby.cpp: implementation of the MLANCoopLobby class.
// LAN Co-op Story Mode - Pre-game lobby for level selection
//////////////////////////////////////////////////////////////////////

#include <cstdio>
#include <cstring>
#include <SDL.h>

#include "stdafx.h"
#include "D3DXApp.h"
#include "MLANCoopLobby.h"
#include "MainFrm.h"
#include "Network.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MLANCoopLobby::MLANCoopLobby()
{
	m_sel = 0;
	m_levels = 0;
	m_level = 0;
	m_localReady = false;
	m_disconnected = false;
	m_blinkTimer = 0;

	LoadLevelData();
}

MLANCoopLobby::~MLANCoopLobby()
{
}

void MLANCoopLobby::LoadLevelData()
{
	// Load single player level data from single.dat (same as MSinglePlaying)
	FILE *f = fopen_search("single.dat", "rt");
	if (f == nullptr) {
		m_levels = 0;
		return;
	}

	fscanf(f, "%d\n", &m_levels);
	if (m_levels > MAX_SINGLE_LEVELS) m_levels = MAX_SINGLE_LEVELS;

	for (int i = 0; i < m_levels; i++) {
		fscanf(f, "%s %d %s %d %d %d %d %d %d %d %d\n",
			m_data[i].file, &m_data[i].bonuslevel, m_data[i].code,
			&m_data[i].picturepre, &m_data[i].picturepost, &m_data[i].picturedead,
			&m_data[i].mrchovnik,
			&m_data[i].text[0], &m_data[i].text[2], &m_data[i].text[3],
			&m_data[i].needwon);
	}

	fclose(f);
}

void MLANCoopLobby::Init(CMainFrame *parent)
{
	GBase::Init(parent);

	m_bMenuBMP = d3dx.CreateTextureFromFile("mmain.jpg");
	g_sb[1].Play();
	m_localReady = false;
	m_disconnected = false;
	m_blinkTimer = 0;
	m_code.clear();

	// If host, send initial level info
	if (g_network.IsHost() && g_network.IsConnected()) {
		SendLevelInfo();
	}
}

void MLANCoopLobby::SendLevelInfo()
{
	if (!g_network.IsHost()) return;

	g_network.SendCoopLevelInfo(
		m_level,
		m_data[m_level].bonuslevel,
		m_data[m_level].needwon,
		m_data[m_level].picturepre,
		m_data[m_level].picturepost,
		m_data[m_level].picturedead,
		m_data[m_level].file,
		m_data[m_level].code
	);
}

void MLANCoopLobby::Move()
{
	g_network.Update();
	m_blinkTimer++;
	if (m_blinkTimer > 60) m_blinkTimer = 0;

	// Check for disconnection
	if (!g_network.IsConnected() && !m_disconnected) {
		m_disconnected = true;
	}

	// Client: Check for level info updates from host
	if (g_network.IsClient() && g_network.HasCoopLevelInfo()) {
		const NetCoopLevelInfoPacket& info = g_network.GetCoopLevelInfo();
		m_level = info.level;
		g_network.ClearCoopLevelInfo();
	}

	// Client: Check if game started
	if (g_network.IsClient() && g_network.HasCoopLevelStart()) {
		// Game will start - handled by returning MENU_LAN_COOP_PLAYING in OnKey
	}
}

void MLANCoopLobby::Draw()
{
	d3dx.Draw(m_bMenuBMP, 400, 300, 0, 1.5625);

	// Title
	m_font->DrawText(400, 80, "CO-OP STORY MODE", 255, 255, 0, HFONT_CENTER);

	// Show connection status
	bool isHost = g_network.IsHost();
	const char* roleText = isHost ? "HOST" : "CLIENT";
	m_font->DrawText(400, 120, roleText, 255, 255, 0, HFONT_CENTER);

	int menuY = 180;
	int menuSel = isHost ? m_sel : -1;

	// Level info display
	char levelText[64];
	snprintf(levelText, sizeof(levelText), "Level %d of %d", m_level + 1, m_levels);
	m_font->DrawText(400, menuY, levelText, 255, 255, 255, HFONT_CENTER);
	menuY += 40;

	// Code display
	if (isHost) {
		char codeDisplay[64];
		if (m_blinkTimer < 30) {
			snprintf(codeDisplay, sizeof(codeDisplay), "Code: %s_", m_code.c_str());
		} else {
			snprintf(codeDisplay, sizeof(codeDisplay), "Code: %s", m_code.c_str());
		}
		m_font->MenuText(400, menuY, codeDisplay, 0, menuSel);
	} else {
		char codeDisplay[64];
		snprintf(codeDisplay, sizeof(codeDisplay), "Code: %s", m_data[m_level].code);
		m_font->DrawText(400, menuY, codeDisplay, 200, 200, 200, HFONT_CENTER);
	}
	menuY += 50;

	// Menu items
	if (isHost) {
		m_font->MenuText(400, menuY, _LNG_LAN_START_, 1, menuSel);
	} else {
		if (m_localReady) {
			m_font->MenuText(400, menuY, _LNG_LAN_READY_, 0, -1);
		} else {
			m_font->MenuText(400, menuY, _LNG_LAN_NOT_READY_, 0, -1);
		}
	}
	menuY += 40;

	m_font->MenuText(400, menuY, _LNG_LAN_BACK_, 2, menuSel);

	// Show ready status
	menuY = 380;
	if (isHost) {
		const char* remoteStatus = g_network.IsRemoteReady() ? "Player 2: READY" : "Player 2: NOT READY";
		m_font->DrawText(400, menuY, remoteStatus, 200, 200, 200, HFONT_CENTER);
	} else {
		const char* status = m_localReady ? "You are READY" : "Press ENTER to toggle READY";
		m_font->DrawText(400, menuY, status, 200, 200, 200, HFONT_CENTER);
	}

	// Show disconnect notification
	if (m_disconnected) {
		m_font->DrawText(400, 450, "Other player disconnected!", 255, 0, 0, HFONT_CENTER);
	}

	// Instructions
	if (isHost) {
		m_font->DrawText(400, 500, "Enter level code or use LEFT/RIGHT to change level", 150, 150, 150, HFONT_CENTER);
	} else {
		m_font->DrawText(400, 500, "Waiting for host to start...", 150, 150, 150, HFONT_CENTER);
	}
}

void MLANCoopLobby::Destroy()
{
	d3dx.DestroyTexture(m_bMenuBMP);
}

int MLANCoopLobby::OnKey(int nChar)
{
	bool isHost = g_network.IsHost();

	// Check for auto-transition on disconnect
	if (nChar == 0 && m_disconnected) {
		m_disconnected = false;
		g_network.Disconnect();
		return MENU_LAN;
	}

	// Client: check if game started
	if (g_network.IsClient() && g_network.HasCoopLevelStart()) {
		// Apply host's gspeed setting
		const NetCoopLevelStartPacket& info = g_network.GetCoopLevelStart();
		gspeed = info.gspeed_x10 / 10.0f;
		g_network.ClearCoopLevelStart();
		return MENU_LAN_COOP_PLAYING;
	}

	// Check for disconnection
	if (!g_network.IsConnected()) {
		g_network.Disconnect();
		return MENU_LAN;
	}

	switch (nChar) {
	case SDLK_ESCAPE:
		g_network.Disconnect();
		return MENU_LAN;

	case SDLK_DOWN:
		if (isHost) {
			if (++m_sel > MAX_COOP_LOBBY_SEL - 1) m_sel = 0;
			g_sb[0].Play();
		}
		break;

	case SDLK_UP:
		if (isHost) {
			if (--m_sel < 0) m_sel = MAX_COOP_LOBBY_SEL - 1;
			g_sb[0].Play();
		}
		break;

	case SDLK_LEFT:
		if (isHost && m_sel == 0) {
			// Change level with arrow keys when on code field
			if (--m_level < 0) m_level = m_levels - 1;
			m_code = m_data[m_level].code;
			SendLevelInfo();
			g_sb[0].Play();
		}
		break;

	case SDLK_RIGHT:
		if (isHost && m_sel == 0) {
			// Change level with arrow keys when on code field
			if (++m_level >= m_levels) m_level = 0;
			m_code = m_data[m_level].code;
			SendLevelInfo();
			g_sb[0].Play();
		}
		break;

	case SDLK_RETURN:
		g_sb[1].Play();
		if (isHost) {
			switch (m_sel) {
			case 0:  // Code field - try to apply code
				{
					// Look for matching code
					for (int i = 0; i < m_levels; i++) {
						if (strcmp(m_data[i].code, m_code.c_str()) == 0) {
							m_level = i;
							SendLevelInfo();
							break;
						}
					}
				}
				break;
			case 1:  // Start game
				if (g_network.IsRemoteReady()) {
					g_network.SendCoopLevelStart(gspeed);
					return MENU_LAN_COOP_PLAYING;
				}
				break;
			case 2:  // Back
				g_network.Disconnect();
				return MENU_LAN;
			}
		} else {
			// Client: toggle ready
			m_localReady = !m_localReady;
			g_network.SendReady(m_localReady);
		}
		break;

	case SDLK_BACKSPACE:
		if (isHost && m_sel == 0 && !m_code.empty()) {
			m_code.pop_back();
			g_sb[0].Play();
		}
		break;

	default:
		// Handle letter/number input for code (host only, when on code field)
		if (isHost && m_sel == 0 && m_code.length() < 6) {
			char c = 0;
			if (nChar >= SDLK_a && nChar <= SDLK_z) {
				c = 'A' + (nChar - SDLK_a);  // Convert to uppercase
			} else if (nChar >= SDLK_0 && nChar <= SDLK_9) {
				c = '0' + (nChar - SDLK_0);
			}

			if (c != 0) {
				m_code += c;
				g_sb[0].Play();
			}
		}
		break;
	}

	return MENU_DEFAULT;
}
