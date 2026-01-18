// MLANPlaying.cpp: implementation of the MLANPlaying class.
// LAN multiplayer game wrapper - handles networked deathmatch rounds
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "MLANPlaying.h"
#include "MainFrm.h"
#include "Network.h"
#include "GGame.h"
#include <string>

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MLANPlaying::MLANPlaying()
{
	m_gamerunned = false;
	m_gameend = false;
	m_waitingForNextRound = false;
	m_disconnected = false;
}

MLANPlaying::~MLANPlaying()
{
}

void MLANPlaying::StartGame(string file, bool monsters, int bonuslevel, int victories)
{
	m_monsters = monsters;
	m_victories = victories;
	m_bonuslevel = bonuslevel;
	m_file = file;

	m_score[0] = 0;
	m_score[1] = 0;

	RunGame();
	m_gameend = false;
	m_waitingForNextRound = false;
	m_disconnected = false;
}

void MLANPlaying::Init(CMainFrame *parent)
{
	g_sb[1].Play();

	GBase::Init(parent);

	if (m_gamerunned) ProcessGame();

	m_bDeadBMP = d3dx.CreateTextureFromFile("menudead.bmp");
	m_bDead = d3dx.CreateVirualTexture(m_bDeadBMP, 1, 1, 50, 70);

	if (m_gameend)
		m_bPozadi = d3dx.CreateTextureFromFile("mdeadmatching2.jpg");
	else
		m_bPozadi = d3dx.CreateTextureFromFile("mdeadmatching.jpg");
}

void MLANPlaying::RunGame()
{
	// Set network mode BEFORE starting game so random seed sync works in LoadMap
	m_pParent->m_game.SetNetworkMode(GAME_MODE_LAN);
	// Start the game with 2 players, deathmatch mode
	m_pParent->StartGame(m_file, 2, true, m_monsters, m_bonuslevel);
	m_gamerunned = true;
}

void MLANPlaying::Move()
{
	g_network.Update();

	// Check for disconnection - set flag for auto-transition
	if (!g_network.IsConnected() && !m_disconnected) {
		m_disconnected = true;
		m_gameend = true;
	}

	// Client: check for round end notification from host
	if (g_network.IsClient() && g_network.HasRoundEnded()) {
		const NetRoundEndPacket& info = g_network.GetRoundEndInfo();
		m_score[0] = info.score0;
		m_score[1] = info.score1;
		if (m_score[0] >= m_victories || m_score[1] >= m_victories) {
			m_gameend = true;
		}
		// Force end the client's game to sync with host
		m_pParent->m_game.m_gameended = true;
		m_gamerunned = false;
		m_waitingForNextRound = true;
		g_network.ClearRoundEnded();
	}

	// Client: check for next round signal from host
	if (g_network.IsClient() && g_network.HasNextRound()) {
		g_network.ClearNextRound();
		m_waitingForNextRound = false;
		RunGame();
	}
}

void MLANPlaying::Draw()
{
	d3dx.Draw(m_bPozadi, 400, 300, 0, 1.5625);

	// Draw player scores (similar to MDeadMatching)
	for (int i = 0, y = 270; i < 2; i++, y += 60) {
		if (m_gameend && m_score[i] < m_victories)
			d3dx.Draw(m_bDead, 100, y, 0, 1, 1, 255, i, 2);
		else {
			d3dx.Draw(m_bDead, 100, y, 0, 1, 1, 128, 0, 1);
			d3dx.Draw(m_bDead, 100, y, 0, 1, 1, 255, i);
		}

		for (int j = 0; j < m_score[i]; j++)
			d3dx.Draw(m_bDead, 160 + j * 60, y, 0, 1, 1, 255, 2, 1);
	}

	// Show network info and instructions
	if (!g_network.IsConnected()) {
		m_font->DrawText(400, 500, "Disconnected from other player", 255, 0, 0, HFONT_CENTER);
	} else if (!m_gamerunned && !m_gameend) {
		// Between rounds
		if (g_network.IsHost()) {
			m_font->DrawText(400, 500, "Press ENTER to start next round", 255, 255, 255, HFONT_CENTER);
		} else {
			m_font->DrawText(400, 500, "Waiting for host to start next round...", 255, 255, 0, HFONT_CENTER);
		}
	}
}

void MLANPlaying::ProcessGame()
{
	GGame *g = &m_pParent->m_game;
	if (!g->m_gameended) return;

	// Only host processes round results and sends them to client
	if (g_network.IsHost()) {
		int winnerID = 255;  // 255 = draw

		// Check who won this round
		for (int i = 0; i < 2; i++) {
			if (!g->m_bomber[i].m_dead) {
				winnerID = i;
				if (++m_score[i] == m_victories) {
					m_gameend = true;
				}
				break;
			}
		}

		// Send round end to client
		g_network.SendRoundEnd(winnerID, m_score[0], m_score[1]);
	}

	m_gamerunned = false;
}

void MLANPlaying::Destroy()
{
	if (m_bPozadi != -1) d3dx.DestroyTexture(m_bPozadi);
	m_bPozadi = -1;
	d3dx.DestroyTexture(m_bDeadBMP);
	d3dx.DestroyTexture(m_bDead);
}

int MLANPlaying::OnKey(int nChar)
{
	// Check for auto-transition on disconnect (called with nChar=0 from Move via MMenu)
	if (nChar == 0 && m_disconnected) {
		m_disconnected = false;
		g_network.Disconnect();
		return MENU_LAN;
	}

	// Check for disconnection (manual key press)
	if (!g_network.IsConnected()) {
		g_network.Disconnect();
		return MENU_LAN;
	}

	switch (nChar) {
	case SDLK_ESCAPE:
		g_network.Disconnect();
		g_sb[1].Play();
		return MENU_LAN;
		break;
	case SDLK_RETURN:
		g_sb[1].Play();
		if (m_gameend) {
			g_network.Disconnect();
			return MENU_LAN;
		}
		// Only host can start next round; client waits for signal
		if (g_network.IsHost()) {
			// Host: start next round and notify client
			g_network.SendNextRound();
			RunGame();
		}
		// Client ignores ENTER between rounds (waits for host signal in Move())
		break;
	}

	return MENU_DEFAULT;
}
