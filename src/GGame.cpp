// GGame.cpp: implementation of the GGame class.
//
//////////////////////////////////////////////////////////////////////

#include <string>
#include <stdio.h>
#include <cstdlib>
#include "stdafx.h"
#include "D3DXApp.h"
#include "GGame.h"
#include "MainFrm.h"
#include "GFile.h"
#ifdef HAVE_SDL2_NET
#include "Network.h"
#endif


using namespace std;

// Ovladani bombicu (SDL2 scancodes)
const int controls[MAX_BOMBERS][5] = {
	{SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_SPACE},
	{SDL_SCANCODE_A, SDL_SCANCODE_D, SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_LCTRL},
	{SDL_SCANCODE_J, SDL_SCANCODE_L, SDL_SCANCODE_I, SDL_SCANCODE_K, SDL_SCANCODE_RCTRL},
	{SDL_SCANCODE_KP_4, SDL_SCANCODE_KP_6, SDL_SCANCODE_KP_8, SDL_SCANCODE_KP_5, SDL_SCANCODE_KP_0},
};

// startovni policka na dead match
const int stratpoints[MAX_BOMBERS][2] = {
	{1,2}, {14, 10}, {1, 10}, {14, 2},
};
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GGame::GGame()
{
	m_networkMode = GAME_MODE_LOCAL;
}

GGame::~GGame()
{

}

#define MAX_BUFF 50
void GGame::Init(CMainFrame *parent)
{
	GBase::Init(parent);

	int i;
	char buff[MAX_BUFF];


	// Grafika
	for (i = 0; i < MAX_BOMBERS; i++) {
		snprintf(buff, MAX_BUFF, "bomber%d.bmp", i);
		m_bBomberBMP[i] = d3dx.CreateTextureFromFile(buff);
		m_bBomber[i] = d3dx.CreateVirualTexture(m_bBomberBMP[i], 1, 1, P_bomber.b_x, P_bomber.b_y);
	}
	m_bBomberBMP_s = d3dx.CreateTextureFromFile("bomber_s.bmp");
	m_bBomber_s = d3dx.CreateVirualTexture(m_bBomberBMP_s, 1, 1, P_bomber.b_x, P_bomber.b_y);

	m_bBombaBMP = d3dx.CreateTextureFromFile("vybuch.bmp");
	m_bBomba = d3dx.CreateVirualTexture(m_bBombaBMP, 1, 154, 50, 50);

	m_bBombaBMP_s = d3dx.CreateTextureFromFile("vybuch_s.bmp");
	m_bBomba_s = d3dx.CreateVirualTexture(m_bBombaBMP, 1, 154, 50, 50);


	for (i = 0; i < MAX_MRCH; i++) {
		snprintf(buff, MAX_BUFF, "mrcha%d.bmp", i);
		m_bMrchaBMP[i] = d3dx.CreateTextureFromFile(buff);
		m_bMrcha[i] = d3dx.CreateVirualTexture(m_bMrchaBMP[i], 1, 1, P_mrcha[i].b_x, P_mrcha[i].b_y);
		
		snprintf(buff, MAX_BUFF, "mrcha%d_s.bmp", i);
		m_bMrchaBMP_s[i] = d3dx.CreateTextureFromFile(buff);
		m_bMrcha_s[i] = d3dx.CreateVirualTexture(m_bMrchaBMP_s[i], 1, 1, P_mrcha[i].b_x, P_mrcha[i].b_y);
	}

	m_map.Init(this);
	m_view.Init(m_pParent, this);
	m_show_mrtvol = false;
	m_show_game_speed = 0;
}

void GGame::Draw()
{
	m_view.UpdateView();
	m_view.Draw();

	if (m_paused)
		m_font->DrawText( 400, 300,  _LNG_PAUSED_, 255, 255, 255,  HFONT_CENTER);	

	// kresleni score (pocet mrtvol)
	if (m_show_mrtvol) {
		char s[50];
		snprintf(s, 50, "%s %d", lng.Translate(41).c_str(), m_mrtvol);
		m_font->DrawText(400, 580, s, 255, 255, 255, HFONT_CENTER);
	}

	if (dinput.m_key[SDL_SCANCODE_EQUALS] || dinput.m_key[SDL_SCANCODE_MINUS]) {
		m_show_game_speed = 50;
	}
	if ( --m_show_game_speed > 0) {
		char s[50];
		snprintf(s, 50, "%s %.1f", lng.Translate(77).c_str(), gspeed);
		m_font->DrawText(400, 50, s, 255, 255, 255, HFONT_CENTER);
	}
}

#define B(x) (x != 0)
extern bool gdeadmatch_bombkill; // zda se po urcite dobe ukoncuje dedmatch nasilne


void GGame::Move()
{
	if (m_paused) return;

	int i;
	dinput.Update();

#ifdef HAVE_SDL2_NET
	// LAN mode: update network
	if (m_networkMode == GAME_MODE_LAN) {
		g_network.Update();

		// Client: check if host ended the round
		if (g_network.IsClient() && g_network.HasRoundEnded()) {
			// Host ended the game - end client's game too
			m_gameended = true;
			m_pParent->StartMenu();
			return;  // Don't process any more this frame
		}
	}
#endif

	// Zmeny ve hre - posuny
	for (i = 0; i < m_bombers; i++) {
#ifdef HAVE_SDL2_NET
		if (m_networkMode == GAME_MODE_LAN) {
			// LAN mode: local player uses keyboard, remote player uses network input
			int localPlayerID = g_network.GetLocalPlayerID();
			int remotePlayerID = g_network.GetRemotePlayerID();

			if (i == localPlayerID) {
				// Local player - use keyboard (always arrow keys in LAN mode since player is alone)
				// and send input over network
				bool left = B(dinput.m_key[controls[0][0]]);
				bool right = B(dinput.m_key[controls[0][1]]);
				bool up = B(dinput.m_key[controls[0][2]]);
				bool down = B(dinput.m_key[controls[0][3]]);
				bool action = B(dinput.m_key[controls[0][4]]);

				m_bomber[i].Input(left, right, up, down, action);
				g_network.SendInput(left, right, up, down, action);
			} else if (i == remotePlayerID) {
				// Remote player - use network input
				bool left, right, up, down, action;
				g_network.GetRemoteInput(left, right, up, down, action);
				m_bomber[i].Input(left, right, up, down, action);
			}
		} else
#endif
		{
			// Local mode - all players use keyboard
			m_bomber[i].Input( B(dinput.m_key[controls[i][0]]), B(dinput.m_key[controls[i][1]]),
				B(dinput.m_key[controls[i][2]]), B(dinput.m_key[controls[i][3]]), B(dinput.m_key[controls[i][4]]));
		}
		m_bomber[i].Move();
	}

	for (i = 0; i < m_mrch; i++) {
		m_mrcha[i].Automove();
		m_mrcha[i].Move();
	}

	m_map.Move();

#define DEADMATCH_MAX_TIME 2000

	// ukoncovani deadmatch
	if (++m_game_time > DEADMATCH_MAX_TIME && m_deadmatch && gdeadmatch_bombkill) 
		if (DEADMATCH_MAX_TIME*25 <=  m_game_time || rand()%(26-m_game_time/DEADMATCH_MAX_TIME) == 0) m_map.AddBomb(-1, rand()%m_map.m_x, rand()%m_map.m_y, 9);

	// Zasahy
	for (i = 0; i < m_mrch; i++) {
		m_mrcha[i].Hit();
	}

	for (i = 0; i < m_bombers; i++) {
		m_bomber[i].Hit();
	}

#ifdef HAVE_SDL2_NET
	// In LAN mode, only the host can end the game
	// Client waits for round end packet from host
	if (m_networkMode == GAME_MODE_LAN) {
		if (g_network.IsHost()) {
			// Host: send periodic game state sync to client (every 5 frames)
			if (m_game_time % 5 == 0) {
				g_network.SendGameState(
					m_bomber[0].m_mx, m_bomber[0].m_my, m_bomber[0].m_x, m_bomber[0].m_y, m_bomber[0].m_dir, m_bomber[0].m_dead,
					m_bomber[1].m_mx, m_bomber[1].m_my, m_bomber[1].m_x, m_bomber[1].m_y, m_bomber[1].m_dir, m_bomber[1].m_dead,
					m_bomber[0].m_bombdosah, m_bomber[1].m_bombdosah, m_bomber[0].m_bomb, m_bomber[1].m_bomb
				);
			}
			if (EndGame()) {
				if (--m_gameendig <= 0) {
					m_gameended = true;
					m_pParent->StartMenu();
				}
			}
		} else {
			// Client: apply game state corrections from host
			if (g_network.HasGameStateUpdate()) {
				const NetGameStatePacket& state = g_network.GetGameState();
				// Apply position corrections for both players
				m_bomber[0].m_mx = state.p0_mx;
				m_bomber[0].m_my = state.p0_my;
				m_bomber[0].m_x = state.p0_x / 100.0f;
				m_bomber[0].m_y = state.p0_y / 100.0f;
				m_bomber[0].m_dir = state.p0_dir;
				m_bomber[0].m_dead = state.p0_dead;
				m_bomber[1].m_mx = state.p1_mx;
				m_bomber[1].m_my = state.p1_my;
				m_bomber[1].m_x = state.p1_x / 100.0f;
				m_bomber[1].m_y = state.p1_y / 100.0f;
				m_bomber[1].m_dir = state.p1_dir;
				m_bomber[1].m_dead = state.p1_dead;
				// Sync gameplay-affecting stats
				m_bomber[0].m_bombdosah = state.p0_bombdosah;
				m_bomber[1].m_bombdosah = state.p1_bombdosah;
				m_bomber[0].m_bomb = state.p0_bomb;
				m_bomber[1].m_bomb = state.p1_bomb;
				g_network.ClearGameStateUpdate();
			}
		}
		// Client: game ends when m_gameended is set by MLANPlaying upon receiving round end packet
	} else
#endif
	{
		// Local mode: normal end game check
		if (EndGame())
			if (--m_gameendig <= 0) {
				m_gameended = true;
				m_pParent->StartMenu();
			}
	}

	// dame hraci moznost upravovat rychlost hry pri hre
	if (dinput.m_key[SDL_SCANCODE_EQUALS]) if ((gspeed += 0.05f) > 4) gspeed = 4;
	if (dinput.m_key[SDL_SCANCODE_MINUS]) if ((gspeed -= 0.05f) < 1) gspeed = 1;
}

void GGame::Destroy()
{
	// Grafika
	int i;

	for (i = 0; i < MAX_BOMBERS; i++) {
		d3dx.DestroyTexture(m_bBomberBMP[i]);
		d3dx.DestroyTexture(m_bBomber[i]);
	}
	d3dx.DestroyTexture(m_bBomberBMP_s);
	d3dx.DestroyTexture(m_bBomber_s);

	d3dx.DestroyTexture(m_bBombaBMP);
	d3dx.DestroyTexture(m_bBomba);
	d3dx.DestroyTexture(m_bBombaBMP_s);
	d3dx.DestroyTexture(m_bBomba_s);

	for (i = 0; i < MAX_MRCH; i++) {
		d3dx.DestroyTexture(m_bMrchaBMP[i]);
		d3dx.DestroyTexture(m_bMrcha[i]);
		d3dx.DestroyTexture(m_bMrchaBMP_s[i]);
		d3dx.DestroyTexture(m_bMrcha_s[i]);
	}

	m_map.Destroy();
	m_view.Destroy();
}

int GGame::OnKey(int nChar)
{
	switch (nChar) {
	case SDLK_ESCAPE :
		m_pParent->StartMenu();
		break;
//	case VK_PAUSE:
	case SDLK_p:
		m_paused = !m_paused;
		break;
	case SDLK_m:
		m_show_mrtvol = !m_show_mrtvol;
		break;
	}
	return MENU_DEFAULT;
}

void GGame::LoadMap(const string file, int players, bool deadmatch, bool monsters, int bonuslevel)
{
	st_map map;
	int i, j;

	m_deadmatch = deadmatch;

#ifdef HAVE_SDL2_NET
	// In LAN mode, use deterministic random seed for synchronized simulation
	// Both host and client will generate the same random sequences
	if (m_networkMode == GAME_MODE_LAN) {
		// Use a simple hash of the filename as seed
		unsigned int seed = 12345;
		for (size_t k = 0; k < file.length(); k++) {
			seed = seed * 31 + (unsigned char)file[k];
		}
		srand(seed);
	}
#endif

	GFile::LoadMap(&map, file);

	m_map.LoadMap(&map);
	m_view.LoadMap(&map);

	for (i = 0; i < players; i++) {
		if (deadmatch == false) m_bomber[i].Init(this, 1, 2, m_bBomber[i], m_bBomber_s, i);
		else m_bomber[i].Init(this, stratpoints[i][0], stratpoints[i][1], m_bBomber[i], m_bBomber_s, i);
	}
	m_bombers = players;

	m_mrch = 0;

	if (monsters) {
		for (i = 0; i < MAX_X; i++) 
			for (j = 0; j < MAX_Y; j++) 
				if (map.mapmrch[i][j] > -1) {
					AddMrcha(i, j, map.mapmrch[i][j]);
				}
	}

	m_map.m_bonuslevel = bonuslevel;
	m_gameendig = 100;
	m_gameended = false;
	m_game_time = 0;
	m_paused = false;
	m_mrtvol = 0;
}

bool GGame::EndGame()
{
	int b = 0, m = 0; // pocitame pocet zivych mrch a bombicu

	for (int i = 0; i < m_bombers; i++)
		if (m_bomber[i].m_dead == false) b++;

	for (int i = 0; i < m_mrch; i++)
		if (m_mrcha[i].m_dead == false) m++;

	if (m_deadmatch && b < 2) return true;
	if (!m_deadmatch && (b == 0 || m == 0)) return true;

	return false;
}

int GGame::AddMrcha(int x, int y, int mrchaID)
{
	int i;
	for (i = 0; i < m_mrch; i++)
		if (m_mrcha[i].m_dead) break;

	if (i >= MAX_GMRCH) return -1;
	m_mrcha[i].Init(this, x, y, m_bMrcha[mrchaID], m_bMrcha_s[mrchaID], mrchaID);
	if (i == m_mrch) m_mrch++;
	return i;
}
