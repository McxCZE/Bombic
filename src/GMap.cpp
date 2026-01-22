// GMap.cpp: implementation of the GMap class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <algorithm>

#include "stdafx.h"
#include "D3DXApp.h"
#include "GMap.h"
#include "GGame.h"

#include "GBombaMega.h"
#include "GBombaNapalm.h"

#include "GBonus.h"
#include "GBonus_fire.h"
#include "GBonus_bomb.h"
#include "GBonus_speed.h"
#include "GBonus_n_slow.h"
#include "GBonus_n_dement.h"
#include "GBonus_n_nobomb.h"
#include "GBonus_n_berserk.h"
#include "GBonus_n_stop.h"
#include "GBonus_n_kicker.h"
#include "GBonus_timer.h"
#include "GBonus_megabomb.h"
#include "GBonus_live.h"
#include "GBonus_kicker.h"
#include "GBonus_shield.h"
#include "GBonus_nemoc_ostatni.h"
#include "GBonus_posilani.h"
#include "GBonus_napalmbomb.h"
#include "GBonus_fireman.h"

#ifdef HAVE_SDL2_NET
#include "Network.h"
#endif

// Bonus type IDs for network sync
#define BONUS_TYPE_NEMOC          0  // 0 = illness (uses AddNemoc)
#define BONUS_TYPE_FIRE           1  // 1-3 = fire
#define BONUS_TYPE_BOMB           4  // 4-7 = bomb
#define BONUS_TYPE_SPEED          8
#define BONUS_TYPE_TIMER          9
#define BONUS_TYPE_MEGABOMB      10
#define BONUS_TYPE_KICKER        11
#define BONUS_TYPE_NAPALMBOMB    12
#define BONUS_TYPE_POSILANI      13
#define BONUS_TYPE_SHIELD        14
#define BONUS_TYPE_LIVE          15
#define BONUS_TYPE_NEMOC_OSTATNI 16
#define BONUS_TYPE_FIREMAN       17
// Nemoc (illness) subtypes: 100+
#define BONUS_TYPE_N_SLOW        100
#define BONUS_TYPE_N_BERSERK     101
#define BONUS_TYPE_N_NOBOMB      102
#define BONUS_TYPE_N_DEMENT      103
#define BONUS_TYPE_N_STOP        104
#define BONUS_TYPE_N_KICKER      105



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GMap::GMap()
{
	m_explodeanim = 0;
	for (int i = 0; i < MAX_X; i++)
		for (int j = 0; j < MAX_Y; j++)
				m_bonusmap[i][j] = nullptr;

	for (int i = 0; i < MAX_BOMBS; i++)
		m_bomba[i] = nullptr;

	m_bonuslevel = 3;
}

GMap::~GMap()
{

}

extern bool  gexplode_predict_visible; // zda je viditelna predikce exploze

void GMap::Draw(int x, int y, bool walk, int bx, int by)
{
	// podklad
	if (m_map[x][y].FireIn() == walk)
		m_map[x][y].Draw(bx, by);

	// bomby, vybuchy a bonusy
	if (!walk) {
		if (m_bmap[x][y].bomba != nullptr) m_bmap[x][y].bomba->Draw(x, y, bx, by);

		int naplam_stop = (NAPALM_TIME) - (int)((float)(NAPALM_TIME - NAPALM_STOP)/gspeed);

		if (m_bmap[x][y].cas > naplam_stop) {
		 	d3dx.Draw(m_bExplode, bx, by, 0, 1, 1, 255, (m_bmap[x][y].cas%22)/3, 6);
		}

		if (m_bmap[x][y].cas < naplam_stop && m_bmap[x][y].cas > EXPLOTIME) m_bmap[x][y].cas = 0;
			
		if ((m_bmap[x][y].cas > 0 && m_bmap[x][y].cas <= EXPLOTIME) ||
			 (m_bmap[x][y].cas > NAPALM_TIME-EXPLOTIME)) 
		 	d3dx.Draw(m_bExplode, bx, by, 0, 1, 1, 255, m_bmap[x][y].vybuch, m_explodeanim);

		if (m_bonusmap[x][y])
			m_bonusmap[x][y]->DrawOnGround(bx, by);
	}

	if (m_bmap[x][y].danger && gexplode_predict_visible && m_bmap[x][y].cas == 0)
		d3dx.Draw(m_bExplode, bx, by, 0, 1, 1, 20, 7, m_explodeanim);
}

void GMap::Init(GGame *game)
{
	m_game = game;
	m_bExplodeBMP = d3dx.CreateTextureFromFile("vybuch.bmp");
	m_bExplode = d3dx.CreateVirualTexture(m_bExplodeBMP, 1, 1, 50, 50);
	m_bBonusBMP = d3dx.CreateTextureFromFile("bonusy.bmp");
	m_bBonus = d3dx.CreateVirualTexture(m_bBonusBMP, 1, 1, 50, 50);

}

#define MAX_BUFF 60
void GMap::LoadMap(st_map *map)
{
	int i, j;

	char buff[MAX_BUFF];
	snprintf(buff, MAX_BUFF, "mapa%d.bmp", map->podklad);
	m_bMapBMP = d3dx.CreateTextureFromFile(buff);
	m_bMap = d3dx.CreateVirualTexture(m_bMapBMP, 1, 1, 50, 100);

	m_x = map->x;
	m_y = map->y;

	for (i = 0; i < MAX_X; i++)
		for (j = 0; j < MAX_Y; j++) {
			m_bmap[i][j].bomba = nullptr;
			m_bmap[i][j].cas = 0;
			m_bmap[i][j].vybuch = 0;
			m_bmap[i][j].barva = 0;
			m_mmap[i][j] = 0;
			m_map[i][j].Init(m_game, i, j, m_bMap, map->map[i][j]);
			m_bonusmap[i][j] = nullptr;
			m_bmap[i][j].danger = false;
		}
	for (i = 0; i < MAX_BOMBS; i++)
		SAFE_DELETE(m_bomba[i]);

}

bool GMap::IsFree(int x, int y) const
{
	return (m_map[x][y].WalkIn() && m_bmap[x][y].bomba == nullptr);
}

bool GMap::IsFlyFree(int x, int y) const
{
	return (m_map[x][y].FireIn() && m_bmap[x][y].bomba == nullptr);
}

bool GMap::IsFlyFreeNoBombs(int x, int y) const
{
	return (m_map[x][y].FireIn());
}

// Comparator for sorting bombs - moving bombs first, then by bombtime
static bool bombCompare(const GBomba* o1, const GBomba* o2)
{
	// nullptr goes to the end
	if (o1 == nullptr) return false;
	if (o2 == nullptr) return true;

	// Moving bombs have priority
	if (o1->m_moveing && !o2->m_moveing) return true;
	if (!o1->m_moveing && o2->m_moveing) return false;

	// Both moving or both not moving - sort by bombtime
	if (o1->m_moveing && o2->m_moveing) {
		return o1->m_bombtime < o2->m_bombtime;
	}
	return false;
}



void GMap::Move()
{
	int i,j;

	for (i = 0; i < MAX_X; i++)
		for (j = 0; j < MAX_Y; j++)
			m_bmap[i][j].danger = false;


	for (i = 0; i < MAX_BOMBS; i++)
		if (m_bomba[i] != nullptr) m_bomba[i]->Move();

	for (i = 0; i < MAX_X; i++)
		for (j = 0; j < MAX_Y; j++) {
			m_map[i][j].Move();

			if (m_bmap[i][j].cas) {
				m_bmap[i][j].cas--;
				if (m_bmap[i][j].bomba != nullptr)
						m_bmap[i][j].bomba->m_bombtime = 0;
				m_bmap[i][j].danger = true;
			}
		}

	std::sort(&m_bomba[0], &m_bomba[MAX_BOMBS], bombCompare);

	for (i = 0; i < MAX_BOMBS; i++)
		if (m_bomba[i] != nullptr) m_bomba[i]->SetDanger();


	if (++m_explodeanim > MAX_EXPLODEANIM) m_explodeanim = 0;
}

int GMap::AddBomb(int bomberID, int x, int y, int dosah, bool fromNetwork)
{
	int i;

	if (x < 1 || y < 1 || y >= MAX_Y || x >= MAX_X) return -1;

	if (m_bmap[x][y].bomba != nullptr) return -1;
	if (!m_map[x][y].WalkIn()) return -1;

	for (i = 0; i < MAX_BOMBS; i++)
		if (m_bomba[i] == nullptr) break;

	if (i >= MAX_BOMBS) return -1;

	m_bomba[i] = new GBomba;

	m_bomba[i]->Init(m_game, x, y, m_game->m_bBomba, m_game->m_bBomba_s, bomberID, dosah, fromNetwork);

	// Only increment m_bombused if not from network (host already did it)
	if (bomberID != -1 && !fromNetwork) m_game->m_bomber[bomberID].m_bombused++;

	m_bmap[x][y].bomba = m_bomba[i];

	return i;
}

int GMap::AddMegaBomb(int bomberID, int x, int y, int dosah, bool fromNetwork)
{
	int i;

	if (m_bmap[x][y].bomba != nullptr) return -1;
	if (!m_map[x][y].WalkIn()) return -1;

	for (i = 0; i < MAX_BOMBS; i++)
		if (m_bomba[i] == nullptr) break;

	if (i >= MAX_BOMBS) return -1;

	m_bomba[i] = (new GBombaMega);

	m_bomba[i]->Init(m_game, x, y, m_game->m_bBomba, m_game->m_bBomba_s, bomberID, dosah, fromNetwork);
	// Only decrement megabombs counter if not from network (host already did it)
	if (!fromNetwork) {
		m_game->m_bomber[bomberID].m_megabombs--;
	}

	m_bmap[x][y].bomba = m_bomba[i];

	return i;
}

int GMap::AddNapalmBomb(int bomberID, int x, int y, int dosah, bool fromNetwork)
{
	int i;

	if (m_bmap[x][y].bomba != nullptr) return -1;
	if (!m_map[x][y].WalkIn()) return -1;

	for (i = 0; i < MAX_BOMBS; i++)
		if (m_bomba[i] == nullptr) break;

	if (i >= MAX_BOMBS) return -1;

	m_bomba[i] = (new GBombaNapalm);

	m_bomba[i]->Init(m_game, x, y, m_game->m_bBomba, m_game->m_bBomba_s, bomberID, dosah, fromNetwork);
	// Only decrement napalmbombs counter if not from network (host already did it)
	if (!fromNetwork) {
		m_game->m_bomber[bomberID].m_napalmbombs--;
	}

	m_bmap[x][y].bomba = m_bomba[i];

	return i;
}


void GMap::BombExpolode(GBomba *bomb, int time)
{
	if (bomb == nullptr) return;

	int x = bomb->m_mx, 
		 y = bomb->m_my,
		 b = bomb->m_bomberID,
		 d = bomb->m_dosah;
	int i;

	m_bmap[x][y].bomba = nullptr;
	for (i = 0; i < MAX_BOMBS; i++) {
		if (m_bomba[i] == bomb) {
			delete m_bomba[i];
			m_bomba[i] = nullptr;
			break;
		}
	}

	FireMap(x, y, 3, b, time);

	bool p = true;  // vybuch doprava
	for (i = 1; i < d; i++)
		if (!FireMap(x+i, y, 1, b, time)) { p = false; break; }
	if (p) FireMap(x+d,y, 2, b, time);
	

	p = true; // vybuch doleva 
	for (i = 1; i < d; i++)
		if (!FireMap(x-i, y, 1, b, time)) { p = false; break; }
	if (p) FireMap(x-d,y, 0, b, time);

	p = true; // vybuch nahoru 
	for (i = 1; i < d; i++)
		if (!FireMap(x, y-i, 5, b, time)) { p = false; break; }
	if (p) FireMap(x,y-d, 4, b, time);


	p = true; // vybuch dolu
	for (i = 1; i < d; i++)
		if (!FireMap(x, y+i, 5, b, time)) { p = false; break; }
	if (p) FireMap(x,y+d, 6, b, time);
}

void GMap::BombNapalmExpolode(GBomba *bomb)
{
	BombExpolode(bomb, NAPALM_TIME);

	m_game->m_view.StartTres(EXPLOTIME);
}


// Helper macro for mega bomb explosion
// In LAN mode, client skips local bomb creation (receives from host)
#ifdef HAVE_SDL2_NET
#define ABOMB(bx, by) \
	if (m_game->m_networkMode != GAME_MODE_LAN || g_network.IsHost()) { \
		if ((k = AddBomb(b, bx, by, d)) != -1) { \
			m_bomba[k]->m_bombtime = 0; \
			if (m_game->m_networkMode == GAME_MODE_LAN && g_network.IsHost()) { \
				g_network.SendBombPlaced(b, NET_BOMB_REGULAR, bx, by, d); \
			} \
		} \
	}
#else
#define ABOMB(bx, by) if ((k = AddBomb(b, bx, by, d)) != -1) m_bomba[k]->m_bombtime = 0;
#endif

void GMap::BombMegaExpolode(GBomba *bomb)
{
	if (bomb == nullptr) return;

	int x = bomb->m_mx,
		 y = bomb->m_my,
		 b = bomb->m_bomberID,
		 d = bomb->m_dosah,
		 k;

	ABOMB(x+1, y+1);
	ABOMB(x+1, y);
	ABOMB(x+1, y-1);
	ABOMB(x, y+1);
	ABOMB(x, y-1);
	ABOMB(x-1, y+1);
	ABOMB(x-1, y);
	ABOMB(x-1, y-1);

	BombExpolode(bomb);

	m_game->m_view.StartTres(EXPLOTIME);
}

#undef ABOMB


bool GMap::FireMap(int x, int y, int bmp, int b, int explotime)
{
	if (explotime != NAPALM_TIME) explotime = (int)((float)explotime / gspeed);
	else explotime = (NAPALM_TIME - EXPLOTIME) + (int)((float)EXPLOTIME / gspeed);

	if (explotime < 2) explotime = 2;

	if (x < 0 || y < 0 || x >= MAX_X || y >= MAX_Y) return false;
	if (m_map[x][y].Hit()) {
		m_bmap[x][y].barva = b; 
		
		if (m_bmap[x][y].cas == 0 || m_bmap[x][y].vybuch == bmp)
			m_bmap[x][y].vybuch = bmp;
		else {
			if (m_bmap[x][y].vybuch < 3 && bmp < 3) m_bmap[x][y].vybuch = 1;
			else if (m_bmap[x][y].vybuch > 3 && bmp > 3) m_bmap[x][y].vybuch = 5;
				else m_bmap[x][y].vybuch = 3;
		}		
				

		if (m_bmap[x][y].cas < explotime)
			m_bmap[x][y].cas = explotime;
		if (m_bonusmap[x][y]) {
#ifdef HAVE_SDL2_NET
			// In LAN mode, only host can destroy bonuses
			// Client waits for host notification
			if (m_game->m_networkMode == GAME_MODE_LAN && g_network.IsClient()) {
				// Client: do NOT delete bonus - wait for host notification
			} else {
				// Host or local mode: delete bonus and notify client
				if (m_game->m_networkMode == GAME_MODE_LAN && g_network.IsHost()) {
					g_network.SendBonusDestroyed(x, y);
				}
				delete m_bonusmap[x][y];
				m_bonusmap[x][y] = nullptr;
			}
#else
			delete m_bonusmap[x][y];
			m_bonusmap[x][y] = nullptr;
#endif
		}
		return true;
	} 

	return false;
}


void GMap::Destroy()
{
	d3dx.DestroyTexture(m_bMapBMP);
	d3dx.DestroyTexture(m_bMap);
	d3dx.DestroyTexture(m_bExplodeBMP);
	d3dx.DestroyTexture(m_bExplode);
	d3dx.DestroyTexture(m_bBonusBMP);
	d3dx.DestroyTexture(m_bBonus);

	// Clean up bonuses
	for (int i = 0; i < MAX_X; i++)
		for (int j = 0; j < MAX_Y; j++)
			if (m_bonusmap[i][j]) {
				delete m_bonusmap[i][j];
				m_bonusmap[i][j] = nullptr;
			}

	// Clean up bombs
	for (int i = 0; i < MAX_BOMBS; i++)
		SAFE_DELETE(m_bomba[i]);
}

void GMap::AddBonus(int mx, int my)
{
#ifdef HAVE_SDL2_NET
	// In LAN mode, only host decides bonus type
	if (m_game->m_networkMode == GAME_MODE_LAN) {
		if (g_network.IsClient()) {
			// Client: do nothing, wait for host to send bonus info
			return;
		}
		// Host: decide bonus type and send to client
	}
#endif

	int bonusType;
	int r = rand() % m_bonuslevel;

	switch (r) {
	case 0:
		// Illness - determine specific type
		{
			int nemocType = rand() % 6;
			bonusType = BONUS_TYPE_N_SLOW + nemocType;  // 100-105
		}
		break;
	case  1:
	case  2:
	case  3: bonusType = BONUS_TYPE_FIRE;       break;
	case  4:
	case  5:
	case  6:
	case  7: bonusType = BONUS_TYPE_BOMB;       break;
	case  8: bonusType = BONUS_TYPE_SPEED;      break;
	case  9: bonusType = BONUS_TYPE_TIMER;      break;
	case 10: bonusType = BONUS_TYPE_MEGABOMB;   break;
	case 11: bonusType = BONUS_TYPE_KICKER;     break;
	case 12: bonusType = BONUS_TYPE_NAPALMBOMB; break;
	case 13: bonusType = BONUS_TYPE_POSILANI;   break;
	case 14: bonusType = BONUS_TYPE_SHIELD;     break;
	case 15: bonusType = BONUS_TYPE_LIVE;       break;
	case 16: bonusType = BONUS_TYPE_NEMOC_OSTATNI; break;
	case 17: bonusType = BONUS_TYPE_FIREMAN;    break;
	default: bonusType = BONUS_TYPE_FIRE;       break;
	}

#ifdef HAVE_SDL2_NET
	// Host sends bonus info to client
	if (m_game->m_networkMode == GAME_MODE_LAN && g_network.IsHost()) {
		g_network.SendBonusSpawned(mx, my, bonusType);
	}
#endif

	// Actually spawn the bonus
	AddBonusByType(mx, my, bonusType);
}

void GMap::AddBonusByType(int mx, int my, int bonusType)
{
	// Bounds check to prevent buffer overflow
	if (mx < 0 || mx >= MAX_X || my < 0 || my >= MAX_Y) {
		return;
	}

	GBonus *b = nullptr;

	// Check for illness types (100+)
	if (bonusType >= BONUS_TYPE_N_SLOW) {
		switch (bonusType) {
		case BONUS_TYPE_N_SLOW:    b = (GBonus*) new GBonus_n_slow;    break;
		case BONUS_TYPE_N_BERSERK: b = (GBonus*) new GBonus_n_berserk; break;
		case BONUS_TYPE_N_NOBOMB:  b = (GBonus*) new GBonus_n_nobomb;  break;
		case BONUS_TYPE_N_DEMENT:  b = (GBonus*) new GBonus_n_dement;  break;
		case BONUS_TYPE_N_STOP:    b = (GBonus*) new GBonus_n_stop;    break;
		case BONUS_TYPE_N_KICKER:  b = (GBonus*) new GBonus_n_kicker;  break;
		default:                   b = (GBonus*) new GBonus_n_slow;    break;
		}
	} else {
		switch (bonusType) {
		case BONUS_TYPE_FIRE:          b = (GBonus*) new GBonus_fire;       break;
		case BONUS_TYPE_BOMB:          b = (GBonus*) new GBonus_bomb;       break;
		case BONUS_TYPE_SPEED:         b = (GBonus*) new GBonus_speed;      break;
		case BONUS_TYPE_TIMER:         b = (GBonus*) new GBonus_timer;      break;
		case BONUS_TYPE_MEGABOMB:      b = (GBonus*) new GBonus_megabomb;   break;
		case BONUS_TYPE_KICKER:        b = (GBonus*) new GBonus_kicker;     break;
		case BONUS_TYPE_NAPALMBOMB:    b = (GBonus*) new GBonus_napalmbomb; break;
		case BONUS_TYPE_POSILANI:      b = (GBonus*) new GBonus_posilani;   break;
		case BONUS_TYPE_SHIELD:        b = (GBonus*) new GBonus_shield;     break;
		case BONUS_TYPE_LIVE:          b = (GBonus*) new GBonus_live;       break;
		case BONUS_TYPE_NEMOC_OSTATNI: b = (GBonus*) new GBonus_nemoc_ostatni(m_game); break;
		case BONUS_TYPE_FIREMAN:       b = (GBonus*) new GBonus_fireman;    break;
		default:                       b = (GBonus*) new GBonus_fire;       break;
		}
	}

	if (b) {
		// Delete existing bonus to prevent memory leak
		if (m_bonusmap[mx][my] != nullptr) {
			delete m_bonusmap[mx][my];
		}
		m_bonusmap[mx][my] = b;
		b->Init(mx, my, &m_bonusmap[mx][my], m_bBonus);
	}
}

void GMap::AddNemoc(int mx, int my)
{
#ifdef HAVE_SDL2_NET
	// In LAN mode, only host decides illness type
	if (m_game->m_networkMode == GAME_MODE_LAN) {
		if (g_network.IsClient()) {
			// Client: do nothing, wait for host to send bonus info
			return;
		}
		// Host: pick random illness type and send to client
		int nemocType = rand() % 6;
		int bonusType = BONUS_TYPE_N_SLOW + nemocType;
		g_network.SendBonusSpawned(mx, my, bonusType);
		AddBonusByType(mx, my, bonusType);
		return;
	}
#endif
	// Local mode: just pick random illness
	int nemocType = rand() % 6;
	AddBonusByType(mx, my, BONUS_TYPE_N_SLOW + nemocType);
}


void GMap::SetBombDanger(int x, int y, int d)
{
	if (x < 1 || y < 1 || x > MAX_X - 1 || y > MAX_Y - 1) return;

	if (!(m_bmap[x][y].bomba != nullptr || m_map[x][y].WalkIn())) return;

	m_bmap[x][y].danger = true;

	int i;

	// vybuch doprava
	for (i = x+1; i < x+d+1; i++)
		if (IsFlyFreeNoBombs(i, y)) m_bmap[i][y].danger = true; 
		else break;
	

	// vybuch doleva 
	for (i = x-1; i > x-d-1; i--)
		if (IsFlyFreeNoBombs(i, y)) m_bmap[i][y].danger = true; 
		else break;

	// vybuch nahoru 
	for (i = y+1; i < y+d+1; i++)
		if (IsFlyFreeNoBombs(x, i)) m_bmap[x][i].danger = true; 
		else break;


	// vybuch dolu
	for (i = y-1; i > y-d-1; i--)
		if (IsFlyFreeNoBombs(x, i)) m_bmap[x][i].danger = true; 
		else break;
}
