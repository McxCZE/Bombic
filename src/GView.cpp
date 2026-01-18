// GView.cpp: implementation of the GView class.
//
//////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <cstring>
#include "stdafx.h"
#include "D3DXApp.h"
#include "GView.h"
#include "GGame.h"	// Added by ClassView
#include "GBonus.h"
#include "GObject.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GView::GView()
{
	m_game = nullptr;
	m_x = 0;
	m_y = 0;
	m_sx = 0;
	m_sy = 0;
}

GView::~GView()
{

}

void GView::Init(CMainFrame *parent, GGame *game)
{
	GBase::Init(parent);

	m_game = game;

	m_bBombic = d3dx.CreateVirualTexture(m_game->m_bBombaBMP, 1, 358, 118, 68);
	m_bBonus  = d3dx.CreateVirualTexture(m_game->m_bBombaBMP, 1, 427,  24, 24);
}

void GView::LoadMap(st_map *map)
{

}


typedef struct {
	int x;
	int y;
} POINT;
const POINT bomberpos[4] = 
{ {59, 34}, {741, 566}, {59, 566}, {741, 34}};

// Compare objects by Y position for depth sorting
static bool objectCompare(const GObject* o1, const GObject* o2)
{
	return o1->m_y < o2->m_y;
}


#define MAX_BUFF  50

void GView::Draw()
{
	int i,j, k;
	char s[50];

	if (m_tres > 0) {
		// Use SDL_GetTicks for cosmetic shake instead of rand()
		// This prevents desyncing the game simulation RNG in LAN mode
		Uint32 t = SDL_GetTicks();
		m_sx += (int)((t * 7) % 10) - 5;
		m_sy += (int)((t * 13) % 10) - 5;
	}

	// priprava bomberu a mrch na kresleni
	GObject *object[MAX_BOMBERS+MAX_GMRCH];
	int      objects = 0;

	for (k = 0; k < m_game->m_bombers; k++)
		object[objects++] = &m_game->m_bomber[k];

	for (k = 0; k < m_game->m_mrch; k++) 
		object[objects++] = &m_game->m_mrcha[k];

	std::sort(object, object + objects, objectCompare);

	std::memset(m_obmap, 0, sizeof(m_obmap));

	for (k = 0; k < objects; k++) {
		int max = m_obmap[object[k]->m_mx][object[k]->m_my].max;
		if (max < MAX_OBMAP) {
			m_obmap[object[k]->m_mx][object[k]->m_my].objects[max] = object[k];
			m_obmap[object[k]->m_mx][object[k]->m_my].max++;
		}
	}

	// Kresleni mapy
	for ( j = m_by; j < m_by+14; j++)
		if (j < m_game->m_map.m_y)
			for (i = m_bx; i < m_bx+18; i++) 
				m_game->m_map.Draw(i, j, true, i*50+25+m_sx, j*50+25+m_sy);

// kresleni stinu
	for ( j = m_by; j < m_by+14; j++) 
		if (j < m_game->m_map.m_y)
			for (i = m_bx; i < m_bx+18; i++)
				for (k = 0; k < m_obmap[i][j].max; k++)
					m_obmap[i][j].objects[k]->Draw(i, j, true, i*50+25+m_sx, j*50+25+m_sy);

// kresleni zbytku
	for ( j = m_by; j < m_by+14; j++) {
		if (j < m_game->m_map.m_y)
			for (i = m_bx; i < m_bx+18; i++) {
				m_game->m_map.Draw(i, j, false, i*50+25+m_sx, j*50+25+m_sy);
				for (k = 0; k < m_obmap[i][j].max; k++)
					m_obmap[i][j].objects[k]->Draw(i, j, false, i*50+25+m_sx, j*50+25+m_sy);
			}
	}


// kresleni bombicu
//	CString s;
	for ( i = 0; i < m_game->m_bombers; i++) {
		d3dx.Draw(m_bBombic, bomberpos[i].x, bomberpos[i].y, 0, 1, 1, 255, i);
		// zivoty
		if (m_game->m_deadmatch) {
			snprintf(s, MAX_BUFF, "%dX", m_game->m_bomber[i].m_lives);
			m_font->DrawText((bomberpos[i].x-41), (bomberpos[i].y-21), s,
				200, 200, 200, HFONT_CENTER);
		}
		// plamen
		snprintf(s, MAX_BUFF, "%dX", m_game->m_bomber[i].m_bombdosah);
		m_font->DrawText((bomberpos[i].x+40), (bomberpos[i].y-19),  s, 
				200, 200, 200, HFONT_CENTER);
		// bomby
		if (m_game->m_bomber[i].m_napalmbombs) {
			snprintf(s, MAX_BUFF, "%dX", m_game->m_bomber[i].m_napalmbombs);
			d3dx.Draw(m_bBonus, (bomberpos[i].x+14), (bomberpos[i].y-1), 0, 1, 1, 255, 1);
		} 
		else 
			if (m_game->m_bomber[i].m_megabombs) {
				snprintf(s, MAX_BUFF, "%dX", m_game->m_bomber[i].m_megabombs);
				d3dx.Draw(m_bBonus, bomberpos[i].x+14, bomberpos[i].y-1,
				          0, 1, 1, 255, 0);
			}
			else {
				snprintf(s, MAX_BUFF, "%dX",
				   m_game->m_bomber[i].m_bomb-m_game->m_bomber[i].m_bombused);
			}
		m_font->DrawText((bomberpos[i].x+40), (bomberpos[i].y-1), s,
				200, 200, 200, HFONT_CENTER);
		// casovy bonus
		if (m_game->m_bomber[i].m_bonus) {
			float scale;
			
//			snprintf(s, MAX_BUFF, "%d%%",(m_game->m_bomber[i].m_bonus->m_timer*100)/m_game->m_bomber[i].m_bonus->m_timer_MAX);
			scale = (float)m_game->m_bomber[i].m_bonus->m_timer/(float)m_game->m_bomber[i].m_bonus->m_timer_MAX;
			snprintf(s, MAX_BUFF, "%.0f%%", scale*100);


			m_font->DrawText((bomberpos[i].x+40), (bomberpos[i].y+19), s, 
				200, 200, 200, HFONT_CENTER);

			if (m_game->m_bomber[i].m_bonus->m_bView != -1)
				d3dx.Draw(m_bBonus, bomberpos[i].x+14, bomberpos[i].y+19, 0, 1, 1, 255, m_game->m_bomber[i].m_bonus->m_bView);

		}
		if (m_game->m_bomber[i].m_kopani)
			d3dx.Draw(m_bBonus, bomberpos[i].x-41, bomberpos[i].y+16, 0, 1, 1, 255, 6);
		if (m_game->m_bomber[i].m_posilani)
			d3dx.Draw(m_bBonus, bomberpos[i].x-10, bomberpos[i].y+16, 0, 1, 1, 255, 5);
	}
}

void GView::UpdateView()
{
	int cnt = 0, x = 0, y = 0;
	for (int i = 0; i < m_game->m_bombers; i++)
		if (!m_game->m_bomber[i].m_dead) {
			x += (int)m_game->m_bomber[i].m_x + m_game->m_bomber[i].m_mx*50;
			y += (int)m_game->m_bomber[i].m_y + m_game->m_bomber[i].m_my*50;
			cnt++;
		}

	if (cnt > 0) {
		m_x = x/cnt;
		m_y = y/cnt;
	}

	if (m_x < 400) m_x = 400;
	if (m_y < 300) m_y = 300;
	if (m_x > m_game->m_map.m_x*50 - 400) m_x = m_game->m_map.m_x*50 - 400;
	if (m_y > m_game->m_map.m_y*50 - 300) m_y = m_game->m_map.m_y*50 - 300;

	m_bx = (m_x-400)/50;
	m_by = (m_y-300)/50;

	if (m_bx < 0) m_bx = 0;
	if (m_by < 0) m_by = 0;

	m_sx = -(m_bx*50 + m_x - 50*(m_x/50));
	m_sy = -(m_by*50 + m_y - 50*(m_y/50));

	if (m_tres > 0) m_tres--;
}

void GView::StartTres(int doba)
{
	m_tres = doba;
}

void GView::Destroy()
{
	d3dx.DestroyTexture(m_bBombic);
	d3dx.DestroyTexture(m_bBonus);
}
