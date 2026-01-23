// MLANCoopPlaying.cpp: implementation of the MLANCoopPlaying class.
// LAN Co-op Story Mode - Game session manager
//////////////////////////////////////////////////////////////////////

#include <cstdio>
#include <cstring>
#include <SDL.h>

#include "stdafx.h"
#include "D3DXApp.h"
#include "MLANCoopPlaying.h"
#include "MainFrm.h"
#include "Network.h"
#include "GGame.h"

extern int gmaxmrchovnik;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MLANCoopPlaying::MLANCoopPlaying()
{
	for (int i = 0; i < 4; i++) m_bScreens[i] = -1;
	m_levels = 0;
	m_level = 0;
	m_menustate = -1;
	m_disconnected = false;

	LoadLevelData();
}

MLANCoopPlaying::~MLANCoopPlaying()
{
}

void MLANCoopPlaying::LoadLevelData()
{
	// Load single player level data from single.dat
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

void MLANCoopPlaying::Init(CMainFrame *parent)
{
	GBase::Init(parent);
	g_sb[1].Play();
	m_disconnected = false;

	// If we're resuming (menustate already set), reload level assets
	if (m_menustate != -1) {
		LoadLevel();
	}
}

void MLANCoopPlaying::StartCampaign(int startLevel)
{
	// BUG #63 FIX: Check m_levels > 0 before starting campaign
	// If single.dat is missing or empty, m_levels=0 and game would hang
	if (m_levels <= 0) {
		m_menustate = -1;  // Invalid state - will return to menu
		return;
	}

	m_level = startLevel;
	if (m_level >= m_levels) m_level = 0;
	m_menustate = 0;  // Start with intro screen

	LoadLevel();

	// Send level info to client using atomic transition packet
	// This prevents race condition from separate UDP packets arriving out of order
	if (g_network.IsHost()) {
		g_network.SendCoopLevelTransition(
			m_level,
			m_data[m_level].bonuslevel,
			m_data[m_level].needwon,
			m_data[m_level].picturepre,
			m_data[m_level].picturepost,
			m_data[m_level].picturedead,
			m_data[m_level].file,
			m_data[m_level].code,
			m_menustate,
			m_data[m_level].mrchovnik  // BUG #40 FIX: Include mrchovnik for monster count sync
		);
	}
}

int MLANCoopPlaying::LoadLevel()
{
	if (m_level >= m_levels) {
		// Campaign complete!
		return MENU_LAN;
	}

	// Destroy old textures
	for (int i = 0; i < 4; i++) {
		if (m_bScreens[i] != -1) {
			d3dx.DestroyTexture(m_bScreens[i]);
			m_bScreens[i] = -1;
		}
	}

	char s[100];

	// Load intro screen
	snprintf(s, sizeof(s), "msingleA%d.jpg", m_data[m_level].picturepre);
	m_bScreens[0] = d3dx.CreateTextureFromFile(s);

	// Load victory screen (if exists)
	if (m_data[m_level].picturepost != -1) {
		snprintf(s, sizeof(s), "msingleB%d.jpg", m_data[m_level].picturepost);
		m_bScreens[2] = d3dx.CreateTextureFromFile(s);
	} else {
		m_bScreens[2] = 0;
	}

	// Load defeat screen
	snprintf(s, sizeof(s), "msingleC%d.jpg", m_data[m_level].picturedead);
	m_bScreens[3] = d3dx.CreateTextureFromFile(s);

	// Update max monsters if needed
	if (m_data[m_level].mrchovnik > gmaxmrchovnik) {
		gmaxmrchovnik = m_data[m_level].mrchovnik;
	}

	return MENU_DEFAULT;
}

void MLANCoopPlaying::RunGame()
{
	m_menustate = 1;

	// Set network mode before starting
	m_pParent->m_game.SetNetworkMode(GAME_MODE_LAN);

	// Start the game with 2 players, co-op mode (not deathmatch)
	// Using monsters=true for campaign, deathmatch=false
	m_pParent->StartGame(m_data[m_level].file, 2, false, true, m_data[m_level].bonuslevel);
}

void MLANCoopPlaying::ProcessGame()
{
	GGame *g = &m_pParent->m_game;

	// Only host determines win/lose
	if (!g_network.IsHost()) return;

	// Default: check needwon
	if (m_data[m_level].needwon) {
		m_menustate = 3;  // Defeat by default if needwon is true
	} else {
		m_menustate = 2;  // Victory by default if needwon is false (just survive)
	}

	// Check if at least one player survived
	bool anyAlive = false;
	for (int i = 0; i < 2; i++) {
		if (!g->m_bomber[i].m_dead) {
			anyAlive = true;
			break;
		}
	}

	if (anyAlive) {
		// Players survived - check if we need to kill all monsters
		if (m_data[m_level].needwon) {
			// Check if all monsters are dead
			// Use g->m_mrch (actual monster count) not MAX_GMRCH (array size)
			// Uninitialized slots beyond m_mrch may have m_dead=false
			bool allMonstersDead = true;
			for (int i = 0; i < g->m_mrch; i++) {
				if (!g->m_mrcha[i].m_dead) {
					allMonstersDead = false;
					break;
				}
			}
			if (allMonstersDead) {
				m_menustate = 2;  // Victory
			} else {
				m_menustate = 3;  // Still monsters alive = defeat
			}
		} else {
			m_menustate = 2;  // Victory (just needed to survive)
		}
	} else {
		m_menustate = 3;  // Defeat - all players dead
	}

	// Send result to client
	bool victory = (m_menustate == 2);
	g_network.SendCoopLevelEnd(victory);
	g_network.SendCoopMenuState(m_menustate);

	// If victory screen doesn't exist, auto-advance
	if (m_menustate == 2 && m_data[m_level].picturepost == -1) {
		AdvanceLevel();
	}
}

void MLANCoopPlaying::AdvanceLevel()
{
	m_level++;
	m_menustate = 0;  // Intro screen

	if (m_level >= m_levels) {
		// Campaign complete - will return to menu on next key press
		return;
	}

	LoadLevel();

	// Send new level info to client using atomic transition packet
	// This prevents race condition from separate UDP packets arriving out of order
	if (g_network.IsHost()) {
		g_network.SendCoopLevelTransition(
			m_level,
			m_data[m_level].bonuslevel,
			m_data[m_level].needwon,
			m_data[m_level].picturepre,
			m_data[m_level].picturepost,
			m_data[m_level].picturedead,
			m_data[m_level].file,
			m_data[m_level].code,
			m_menustate,
			m_data[m_level].mrchovnik  // BUG #40 FIX: Include mrchovnik for monster count sync
		);
	}
}

void MLANCoopPlaying::Move()
{
	g_network.Update();

	// Check for disconnection
	if (!g_network.IsConnected() && !m_disconnected) {
		m_disconnected = true;
	}

	// Process game if playing
	if (m_menustate == 1) {
		ProcessGame();
	}

	// Client: Check for level end notification
	if (g_network.IsClient() && g_network.HasCoopLevelEnd()) {
		const NetCoopLevelEndPacket& info = g_network.GetCoopLevelEnd();
		m_menustate = info.victory ? 2 : 3;
		// Force end client's game
		m_pParent->m_game.m_gameended = true;
		g_network.ClearCoopLevelEnd();
	}

	// Client: Check for atomic level transition (preferred - no race condition)
	if (g_network.IsClient() && g_network.HasCoopLevelTransition()) {
		const NetCoopLevelTransitionPacket& trans = g_network.GetCoopLevelTransition();
		int newState = trans.menustate;

		// BUG #48 FIX: Validate menustate before use
		// menustate is used as index into m_bScreens[4] and m_data[].text[4]
		// Invalid values (e.g., 255 from malformed packet) could cause OOB access
		if (newState < 0 || newState > 3) {
			g_network.ClearCoopLevelTransition();
			return;  // Ignore invalid menustate
		}

		// If transitioning to intro for a new level, load it
		if (newState == 0 && m_menustate != 0) {
			// BUG #39 FIX: Validate trans.level before using as array index
			// Malformed network packets could contain invalid level values
			// BUG #42 FIX: Also check against m_levels (actual loaded level count)
			// MAX_SINGLE_LEVELS is array size (40), but single.dat may have fewer levels
			if (trans.level >= MAX_SINGLE_LEVELS || trans.level >= m_levels) {
				g_network.ClearCoopLevelTransition();
				return;  // Ignore invalid level
			}
			m_level = trans.level;
			// Copy level data from transition packet to local cache
			m_data[m_level].bonuslevel = trans.bonuslevel;
			m_data[m_level].needwon = trans.needwon;
			m_data[m_level].picturepre = trans.picturepre;
			m_data[m_level].picturepost = trans.picturepost;
			m_data[m_level].picturedead = trans.picturedead;
			// BUG #40 FIX: Sync mrchovnik from host to ensure same monster count
			m_data[m_level].mrchovnik = trans.mrchovnik;
			// BUG #43 FIX: Validate mrchovnik against MAX_GMRCH to prevent overflow
			// trans.mrchovnik is uint8_t (0-255), MAX_GMRCH is 256
			if (m_data[m_level].mrchovnik >= MAX_GMRCH) {
				m_data[m_level].mrchovnik = MAX_GMRCH - 1;
			}
			// Update gmaxmrchovnik if needed (same logic as LoadLevel on host)
			if (m_data[m_level].mrchovnik > gmaxmrchovnik) {
				gmaxmrchovnik = m_data[m_level].mrchovnik;
			}
			strncpy(m_data[m_level].file, trans.file, sizeof(m_data[m_level].file) - 1);
			m_data[m_level].file[sizeof(m_data[m_level].file) - 1] = '\0';
			strncpy(m_data[m_level].code, trans.code, sizeof(m_data[m_level].code) - 1);
			m_data[m_level].code[sizeof(m_data[m_level].code) - 1] = '\0';
			LoadLevel();
		}

		m_menustate = newState;
		g_network.ClearCoopLevelTransition();
	}

	// Client: Check for menu state updates (legacy fallback)
	if (g_network.IsClient() && g_network.HasCoopMenuState()) {
		const NetCoopMenuStatePacket& info = g_network.GetCoopMenuState();
		int newState = info.menustate;

		// BUG #48 FIX: Validate menustate before use (legacy path)
		// menustate is used as index into m_bScreens[4] and m_data[].text[4]
		if (newState < 0 || newState > 3) {
			g_network.ClearCoopMenuState();
			return;  // Ignore invalid menustate
		}

		// If transitioning to intro for a new level, load it
		if (newState == 0 && m_menustate != 0) {
			// Check for level info update
			if (g_network.HasCoopLevelInfo()) {
				const NetCoopLevelInfoPacket& levelInfo = g_network.GetCoopLevelInfo();
				// BUG #49 FIX: Validate level in legacy fallback code
				// Without this, malformed packets could cause buffer overflow
				if (levelInfo.level >= MAX_SINGLE_LEVELS || levelInfo.level >= m_levels) {
					g_network.ClearCoopLevelInfo();
					g_network.ClearCoopMenuState();
					return;  // Ignore invalid level
				}
				m_level = levelInfo.level;
				g_network.ClearCoopLevelInfo();
				LoadLevel();
			}
		}

		m_menustate = newState;
		g_network.ClearCoopMenuState();
	}

	// Client: Check for level start signal
	if (g_network.IsClient() && g_network.HasCoopLevelStart()) {
		const NetCoopLevelStartPacket& info = g_network.GetCoopLevelStart();
		gspeed = info.gspeed_x10 / 10.0f;
		g_network.ClearCoopLevelStart();
		RunGame();
	}
}

void MLANCoopPlaying::Draw()
{
	// Draw appropriate background based on menu state
	if (m_menustate >= 0 && m_menustate <= 3 && m_bScreens[m_menustate] != -1) {
		d3dx.Draw(m_bScreens[m_menustate], 400, 300, 0, 1.5625);

		// Draw level text
		m_font->DrawMultiline(21, 201,
			lng.Translate(m_data[m_level].text[m_menustate]), 0, 0, 0);
		m_font->DrawMultiline(20, 200,
			lng.Translate(m_data[m_level].text[m_menustate]), 255, 255, 255);
	}

	// Show level code on intro screen
	if (m_menustate == 0) {
		m_font->DrawText(400, 400, m_data[m_level].code, 255, 255, 255);
	}

	// Show network info
	if (!g_network.IsConnected()) {
		m_font->DrawText(400, 550, "Disconnected!", 255, 0, 0, HFONT_CENTER);
	} else if (m_menustate == 0) {
		if (g_network.IsHost()) {
			m_font->DrawText(400, 550, "Press ENTER to start level", 255, 255, 255, HFONT_CENTER);
		} else {
			m_font->DrawText(400, 550, "Waiting for host...", 255, 255, 0, HFONT_CENTER);
		}
	} else if (m_menustate == 2 || m_menustate == 3) {
		if (g_network.IsHost()) {
			m_font->DrawText(400, 550, "Press ENTER to continue", 255, 255, 255, HFONT_CENTER);
		} else {
			m_font->DrawText(400, 550, "Waiting for host...", 255, 255, 0, HFONT_CENTER);
		}
	}

	// Campaign complete message
	if (m_level >= m_levels && m_menustate == 2) {
		m_font->DrawText(400, 300, "CAMPAIGN COMPLETE!", 255, 255, 0, HFONT_CENTER);
	}
}

void MLANCoopPlaying::Destroy()
{
	for (int i = 0; i < 4; i++) {
		if (m_bScreens[i] != -1) {
			d3dx.DestroyTexture(m_bScreens[i]);
			m_bScreens[i] = -1;
		}
	}
}

int MLANCoopPlaying::OnKey(int nChar)
{
	bool isHost = g_network.IsHost();

	// Check for auto-transition on disconnect
	if (nChar == 0 && m_disconnected) {
		m_disconnected = false;
		g_network.Disconnect();
		Destroy();
		m_menustate = -1;
		return MENU_LAN;
	}

	// Check for disconnection
	if (!g_network.IsConnected()) {
		g_network.Disconnect();
		Destroy();
		m_menustate = -1;
		return MENU_LAN;
	}

	switch (nChar) {
	case SDLK_ESCAPE:
		g_network.Disconnect();
		g_sb[1].Play();
		Destroy();
		m_menustate = -1;
		return MENU_LAN;

	case SDLK_RETURN:
		g_sb[1].Play();
		switch (m_menustate) {
		case 0:  // Intro - start game
			if (isHost) {
				// Host starts the game and notifies client
				g_network.SendCoopLevelStart(gspeed);
				RunGame();
			}
			// Client waits for signal in Move()
			break;

		case 2:  // Victory - advance to next level
			if (isHost) {
				if (m_level + 1 >= m_levels) {
					// Campaign complete
					g_network.Disconnect();
					Destroy();
					m_menustate = -1;
					return MENU_MAIN;  // Return to main menu
				}
				AdvanceLevel();
			}
			// Client follows host's state
			break;

		case 3:  // Defeat - retry level
			if (isHost) {
				m_menustate = 0;  // Back to intro
				g_network.SendCoopMenuState(m_menustate);
			}
			// Client follows host's state
			break;
		}
		break;
	}

	return MENU_DEFAULT;
}
