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
#include "GBonus.h"
#ifdef HAVE_SDL2_NET
#include "Network.h"
// Illness bonus types for network sync
#include "GBonus_n_slow.h"
#include "GBonus_n_berserk.h"
#include "GBonus_n_nobomb.h"
#include "GBonus_n_dement.h"
#include "GBonus_n_stop.h"
#include "GBonus_n_kicker.h"
#define BONUS_TYPE_N_SLOW        100
#define BONUS_TYPE_N_BERSERK     101
#define BONUS_TYPE_N_NOBOMB      102
#define BONUS_TYPE_N_DEMENT      103
#define BONUS_TYPE_N_STOP        104
#define BONUS_TYPE_N_KICKER      105
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

		// Client: check if host ended the round (deathmatch) or level (co-op)
		if (g_network.IsClient()) {
			// Deathmatch: check HasRoundEnded
			if (g_network.HasRoundEnded()) {
				// Host ended the game - end client's game too
				m_gameended = true;
				m_pParent->StartMenu();
				return;  // Don't process any more this frame
			}
			// Co-op: check HasCoopLevelEnd
			if (g_network.HasCoopLevelEnd()) {
				// Host ended the level - end client's game too
				// Note: Don't clear the signal here - MLANCoopPlaying::Move() will read it
				m_gameended = true;
				m_pParent->StartMenu();
				return;  // Don't process any more this frame
			}
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

#ifdef HAVE_SDL2_NET
	// In LAN mode, client skips monster AI to avoid rand() desync
	// Host sends monster positions every 5 frames
	if (m_networkMode == GAME_MODE_LAN && g_network.IsClient()) {
		// Client: only run movement (positions will be corrected from host)
		for (i = 0; i < m_mrch; i++) {
			m_mrcha[i].Move();
		}
	} else
#endif
	{
		for (i = 0; i < m_mrch; i++) {
			m_mrcha[i].Automove();
			m_mrcha[i].Move();
		}
	}

	m_map.Move();

#define DEADMATCH_MAX_TIME 2000

	// ukoncovani deadmatch - random bombs falling from sky
#ifdef HAVE_SDL2_NET
	if (m_networkMode == GAME_MODE_LAN) {
		// In LAN mode, only host spawns random bombs and syncs to client
		if (g_network.IsHost()) {
			if (++m_game_time > DEADMATCH_MAX_TIME && m_deadmatch && gdeadmatch_bombkill) {
				if (DEADMATCH_MAX_TIME*25 <= m_game_time || rand()%(26-m_game_time/DEADMATCH_MAX_TIME) == 0) {
					int bx = rand() % m_map.m_x;
					int by = rand() % m_map.m_y;
					if (m_map.AddBomb(-1, bx, by, 9) != -1) {
						g_network.SendBombPlaced(-1, NET_BOMB_REGULAR, bx, by, 9);
					}
				}
			}
		} else {
			// Client just increments game time
			++m_game_time;
		}
	} else
#endif
	{
		// Local mode
		if (++m_game_time > DEADMATCH_MAX_TIME && m_deadmatch && gdeadmatch_bombkill)
			if (DEADMATCH_MAX_TIME*25 <= m_game_time || rand()%(26-m_game_time/DEADMATCH_MAX_TIME) == 0)
				m_map.AddBomb(-1, rand()%m_map.m_x, rand()%m_map.m_y, 9);
	}

	// Zasahy
#ifdef HAVE_SDL2_NET
	// In LAN mode, only the host processes hits
	// Client receives m_lives and m_dead from GameState packets
	if (m_networkMode == GAME_MODE_LAN && g_network.IsClient()) {
		// Client: skip hit detection, authoritative state comes from host
	} else
#endif
	{
		for (i = 0; i < m_mrch; i++) {
			m_mrcha[i].Hit();
		}

		for (i = 0; i < m_bombers; i++) {
			m_bomber[i].Hit();
		}
	}

#ifdef HAVE_SDL2_NET
	// In LAN mode, only the host can end the game
	// Client waits for round end packet from host
	if (m_networkMode == GAME_MODE_LAN) {
		if (g_network.IsHost()) {
			// Host: send periodic player state sync to client (every 2 frames)
			// Reduced from 5 to 2 to minimize m_hitting desync window
			// m_hitting provides temporary invulnerability after being hit
			if (m_game_time % 2 == 0) {
				// Encode abilities as bit flags (including m_hitting)
				int p0_abilities = (m_bomber[0].m_kopani ? 1 : 0) |
				                   (m_bomber[0].m_posilani ? 2 : 0) |
				                   (m_bomber[0].m_casovac ? 4 : 0) |
				                   (m_bomber[0].m_hitting ? 8 : 0);
				int p1_abilities = (m_bomber[1].m_kopani ? 1 : 0) |
				                   (m_bomber[1].m_posilani ? 2 : 0) |
				                   (m_bomber[1].m_casovac ? 4 : 0) |
				                   (m_bomber[1].m_hitting ? 8 : 0);
				// Get illness info (type 100-105 for illnesses, -1 for none)
				int p0_illness_type = -1, p0_illness_timer = 0;
				int p1_illness_type = -1, p1_illness_timer = 0;
				if (m_bomber[0].m_bonus && m_bomber[0].m_bonus->m_illness) {
					p0_illness_type = m_bomber[0].m_bonus->m_type;
					p0_illness_timer = m_bomber[0].m_bonus->m_timer;
				}
				if (m_bomber[1].m_bonus && m_bomber[1].m_bonus->m_illness) {
					p1_illness_type = m_bomber[1].m_bonus->m_type;
					p1_illness_timer = m_bomber[1].m_bonus->m_timer;
				}
				g_network.SendGameState(
					m_bomber[0].m_mx, m_bomber[0].m_my, m_bomber[0].m_x, m_bomber[0].m_y, m_bomber[0].m_dir, m_bomber[0].m_dead,
					m_bomber[1].m_mx, m_bomber[1].m_my, m_bomber[1].m_x, m_bomber[1].m_y, m_bomber[1].m_dir, m_bomber[1].m_dead,
					m_bomber[0].m_bombdosah, m_bomber[1].m_bombdosah, m_bomber[0].m_bomb, m_bomber[1].m_bomb,
					m_bomber[0].m_bombused, m_bomber[1].m_bombused,
					m_bomber[0].m_megabombs, m_bomber[1].m_megabombs, m_bomber[0].m_napalmbombs, m_bomber[1].m_napalmbombs,
					m_bomber[0].m_lives, m_bomber[1].m_lives, p0_abilities, p1_abilities,
					m_bomber[0].m_score, m_bomber[1].m_score,
					p0_illness_type, p0_illness_timer, p1_illness_type, p1_illness_timer
				);
			}

			// Host: send monster states to client (every 2 frames)
			// Increased frequency from 4 to 2 frames for smoother movement and faster hitting sync
			if (m_game_time % 2 == 0) {
				for (int mi = 0; mi < m_mrch; mi++) {
					g_network.SendMrchaState(mi, m_mrcha[mi].m_mx, m_mrcha[mi].m_my,
						m_mrcha[mi].m_x, m_mrcha[mi].m_y, m_mrcha[mi].m_dir,
						m_mrcha[mi].m_dead, m_mrcha[mi].m_lives, m_mrcha[mi].m_hitting);
				}
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
				// Sync bomb used counters (prevents bombused desync)
				m_bomber[0].m_bombused = state.p0_bombused;
				m_bomber[1].m_bombused = state.p1_bombused;
				// Sync special bomb counts
				m_bomber[0].m_megabombs = state.p0_megabombs;
				m_bomber[1].m_megabombs = state.p1_megabombs;
				m_bomber[0].m_napalmbombs = state.p0_napalmbombs;
				m_bomber[1].m_napalmbombs = state.p1_napalmbombs;
				// Sync lives
				m_bomber[0].m_lives = state.p0_lives;
				m_bomber[1].m_lives = state.p1_lives;
				// Sync abilities from bit flags (including m_hitting)
				m_bomber[0].m_kopani = (state.p0_abilities & 1) != 0;
				m_bomber[0].m_posilani = (state.p0_abilities & 2) != 0;
				m_bomber[0].m_casovac = (state.p0_abilities & 4) != 0;
				m_bomber[0].m_hitting = (state.p0_abilities & 8) != 0;
				m_bomber[1].m_kopani = (state.p1_abilities & 1) != 0;
				m_bomber[1].m_posilani = (state.p1_abilities & 2) != 0;
				m_bomber[1].m_casovac = (state.p1_abilities & 4) != 0;
				m_bomber[1].m_hitting = (state.p1_abilities & 8) != 0;
				// Sync score
				m_bomber[0].m_score = state.p0_score;
				m_bomber[1].m_score = state.p1_score;
				// Sync illnesses
				SyncPlayerIllness(0, state.p0_illness_type, state.p0_illness_timer);
				SyncPlayerIllness(1, state.p1_illness_type, state.p1_illness_timer);
				g_network.ClearGameStateUpdate();
			}

			// Client: process bonus spawns from host
			while (g_network.HasBonusSpawned()) {
				NetBonusSpawnedPacket bonus = g_network.PopBonusSpawned();
				m_map.AddBonusByType(bonus.x, bonus.y, bonus.bonusType);
			}

			// Client: process bomb placements from host (all bombs - host is authoritative)
			while (g_network.HasBombPlaced()) {
				NetBombPlacedPacket bomb = g_network.PopBombPlaced();
				int bomberID = (bomb.bomberID == 255) ? -1 : (int)bomb.bomberID;

				// Create the appropriate bomb type
				// Pass fromNetwork=true to avoid double-decrementing special bomb counters
				int bombIdx = -1;
				switch (bomb.bombType) {
				case NET_BOMB_NAPALM:
					if (bomberID >= 0 && bomberID < m_bombers) {
						bombIdx = m_map.AddNapalmBomb(bomberID, bomb.x, bomb.y, bomb.dosah, true);
					}
					break;
				case NET_BOMB_MEGA:
					if (bomberID >= 0 && bomberID < m_bombers) {
						bombIdx = m_map.AddMegaBomb(bomberID, bomb.x, bomb.y, bomb.dosah, true);
					}
					break;
				case NET_BOMB_REGULAR:
				default:
					bombIdx = m_map.AddBomb(bomberID, bomb.x, bomb.y, bomb.dosah, true);
					break;
				}

				// If bomb was created and it's a timer bomb, set the long timeout
				if (bombIdx != -1 && bomb.isTimerBomb && m_map.m_bomba[bombIdx] != nullptr) {
					m_map.m_bomba[bombIdx]->m_bombtime = 100000;
				}

				// Play bomb placement sound if successful
				if (bombIdx != -1 && bomberID >= 0) {
					g_sb[SND_GAME_BOMBPUT].Play(false);
				}
			}

			// Client: process bomb kick events from host
			while (g_network.HasBombKicked()) {
				NetBombKickedPacket kick = g_network.PopBombKicked();
				// Set the bomb's direction at the specified position
				// Validate direction (1=left, 2=right, 3=up, 4=down)
				if (kick.x >= 0 && kick.x < MAX_X && kick.y >= 0 && kick.y < MAX_Y) {
					if (m_map.m_bmap[kick.x][kick.y].bomba != nullptr) {
						if (kick.dir >= 1 && kick.dir <= 4) {
							m_map.m_bmap[kick.x][kick.y].bomba->m_dir = kick.dir;
						}
					}
				}
			}

			// Client: process timer bomb detonation events from host
			while (g_network.HasBombDetonate()) {
				NetBombDetonatePacket det = g_network.PopBombDetonate();
				int bomberID = det.bomberID;
				// Detonate all bombs belonging to this player
				for (int i = 0; i < MAX_BOMBS; i++) {
					if (m_map.m_bomba[i] != nullptr && m_map.m_bomba[i]->m_bomberID == bomberID) {
						m_map.m_bomba[i]->m_bombtime = 1;
					}
				}
			}

			// Client: process monster state updates from host
			while (g_network.HasMrchaState()) {
				NetMrchaStatePacket state = g_network.PopMrchaState();
				int mrchaID = state.mrchaID;
				// BUG #61 FIX: Validate against both MAX_GMRCH (array size) and m_mrch (initialized count)
				// MAX_GMRCH prevents buffer overflow, m_mrch prevents accessing uninitialized monsters
				// Without m_mrch check, could create "ghost" monsters above actual spawn count
				if (mrchaID >= 0 && mrchaID < MAX_GMRCH && mrchaID < m_mrch) {
					// Save old position and death state for m_mmap update
					int oldMx = m_mrcha[mrchaID].m_mx;
					int oldMy = m_mrcha[mrchaID].m_my;
					bool wasAlive = !m_mrcha[mrchaID].m_dead;

					// Apply new state
					m_mrcha[mrchaID].m_mx = state.mx;
					m_mrcha[mrchaID].m_my = state.my;
					m_mrcha[mrchaID].m_x = state.x / 100.0f;
					m_mrcha[mrchaID].m_y = state.y / 100.0f;
					m_mrcha[mrchaID].m_dir = state.dir;
					m_mrcha[mrchaID].m_lives = state.lives;
					m_mrcha[mrchaID].m_hitting = (state.hitting != 0);

					bool isNowAlive = !state.dead;

					// Update m_mmap based on position/alive state changes
					// Bounds check to prevent buffer overflow
					// Remove from old position if was alive there
					if (wasAlive && oldMx >= 0 && oldMx < MAX_X && oldMy >= 0 && oldMy < MAX_Y) {
						m_map.m_mmap[oldMx][oldMy]--;
						// Sanity check: m_mmap should never go negative
						if (m_map.m_mmap[oldMx][oldMy] < 0) {
							m_map.m_mmap[oldMx][oldMy] = 0;
						}
					}
					// Add to new position if now alive
					// BUG #57 FIX: Add overflow protection to prevent m_mmap from exceeding 255
					// Packet reordering could cause multiple increments for same monster
					if (isNowAlive && state.mx >= 0 && state.mx < MAX_X && state.my >= 0 && state.my < MAX_Y) {
						if (m_map.m_mmap[state.mx][state.my] < 255) {
							m_map.m_mmap[state.mx][state.my]++;
						}
					}

					// Handle death state transition for animation
					if (state.dead && !m_mrcha[mrchaID].m_dead) {
						m_mrcha[mrchaID].m_dead = true;
						m_mrcha[mrchaID].m_anim = 0;
					} else {
						m_mrcha[mrchaID].m_dead = state.dead;
					}
				}
			}

			// Client: process illness transfers from host
			// Note: This is now handled via SyncPlayerIllness from GameState packets
			// We just clear the queue to prevent stale packets accumulating
			while (g_network.HasIllnessTransfer()) {
				g_network.PopIllnessTransfer();
				// Illness will be applied via GameState sync which has the authoritative timer
			}

			// Client: process player hit events from host
			// BUG #62 FIX: Don't sync m_hitting from PlayerHit - use GameState only
			// Having two sources (GameState + PlayerHit) without ordering guarantee
			// causes desync when packets arrive in wrong order
			// GameState syncs m_hitting every 2 frames, which is sufficient
			while (g_network.HasPlayerHit()) {
				NetPlayerHitPacket hit = g_network.PopPlayerHit();
				int pid = hit.playerID;
				if (pid >= 0 && pid < m_bombers) {
					// BUG #51 FIX: Don't sync m_lives from PlayerHit packets
					// Lives are already synced via GameState packets every 2 frames
					// BUG #62 FIX: Don't sync m_hitting here - use GameState only
					// Only handle death state transition which needs immediate update
					if (hit.dead && !m_bomber[pid].m_dead) {
						m_bomber[pid].m_dead = true;
						m_bomber[pid].m_anim = 0;
						g_sb[SND_GAME_DEAD_BOMBER].Play(false);
					}
				}
			}

			// Client: process bonus picked notifications from host
			while (g_network.HasBonusPicked()) {
				NetBonusPickedPacket picked = g_network.PopBonusPicked();
				int bx = picked.x;
				int by = picked.y;
				int pickerID = picked.pickerID;
				// Remove bonus from map (host already applied the bonus to the bomber)
				if (bx >= 0 && bx < MAX_X && by >= 0 && by < MAX_Y) {
					if (m_map.m_bonusmap[bx][by] != nullptr) {
						// Bonus effects are synced via GameState packets
						// Just delete the bonus visual on client
						delete m_map.m_bonusmap[bx][by];
						m_map.m_bonusmap[bx][by] = nullptr;
						// Play sound effect
						g_sb[SND_GAME_BONUS].Play(false);
					}
				}
				(void)pickerID;  // Suppress unused warning
			}

			// Client: process bonus destroyed notifications from host
			while (g_network.HasBonusDestroyed()) {
				NetBonusDestroyedPacket destroyed = g_network.PopBonusDestroyed();
				int bx = destroyed.x;
				int by = destroyed.y;
				// Remove bonus from map
				if (bx >= 0 && bx < MAX_X && by >= 0 && by < MAX_Y) {
					if (m_map.m_bonusmap[bx][by] != nullptr) {
						delete m_map.m_bonusmap[bx][by];
						m_map.m_bonusmap[bx][by] = nullptr;
					}
				}
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
	// Note: Disabled in LAN mode to prevent desync
#ifdef HAVE_SDL2_NET
	if (m_networkMode != GAME_MODE_LAN)
#endif
	{
		if (dinput.m_key[SDL_SCANCODE_EQUALS]) if ((gspeed += 0.05f) > 4) gspeed = 4;
		if (dinput.m_key[SDL_SCANCODE_MINUS]) if ((gspeed -= 0.05f) < 1) gspeed = 1;
	}
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
		// Pause disabled in LAN mode to prevent desync
#ifdef HAVE_SDL2_NET
		if (m_networkMode != GAME_MODE_LAN)
#endif
		{
			m_paused = !m_paused;
		}
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

#ifdef HAVE_SDL2_NET
void GGame::SyncPlayerIllness(int playerID, int illnessType, int illnessTimer)
{
	if (playerID < 0 || playerID >= m_bombers) return;
	GBomber& bomber = m_bomber[playerID];

	// Check current illness state
	int currentType = -1;
	if (bomber.m_bonus && bomber.m_bonus->m_illness) {
		currentType = bomber.m_bonus->m_type;
	}

	// If illness type changed
	if (illnessType != currentType) {
		// Remove current illness if any
		if (bomber.m_bonus && bomber.m_bonus->m_illness) {
			bomber.m_bonus->End();
			delete bomber.m_bonus;
			bomber.m_bonus = nullptr;
		}

		// Create new illness if needed
		if (illnessType >= BONUS_TYPE_N_SLOW && illnessType <= BONUS_TYPE_N_KICKER) {
			GBonus* newBonus = nullptr;
			switch (illnessType) {
			case BONUS_TYPE_N_SLOW:    newBonus = new GBonus_n_slow;    break;
			case BONUS_TYPE_N_BERSERK: newBonus = new GBonus_n_berserk; break;
			case BONUS_TYPE_N_NOBOMB:  newBonus = new GBonus_n_nobomb;  break;
			case BONUS_TYPE_N_DEMENT:  newBonus = new GBonus_n_dement;  break;
			case BONUS_TYPE_N_STOP:    newBonus = new GBonus_n_stop;    break;
			case BONUS_TYPE_N_KICKER:  newBonus = new GBonus_n_kicker;  break;
			}
			if (newBonus) {
				newBonus->m_type = illnessType;
				newBonus->m_illness = true;
				newBonus->m_onetime = false;
				newBonus->m_timer = illnessTimer;
				newBonus->m_timer_MAX = illnessTimer;
				newBonus->m_bomber = &bomber;
				newBonus->m_self = &bomber.m_bonus;
				bomber.m_bonus = newBonus;
			}
		}
	} else if (bomber.m_bonus && bomber.m_bonus->m_illness) {
		// Same illness type, just update timer
		bomber.m_bonus->m_timer = illnessTimer;
	}
}
#endif
