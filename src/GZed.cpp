// GZed.cpp: implementation of the GZed class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "GZed.h"
#include "GGame.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GZed::GZed()
{
	m_params = P_zed[0];
}

GZed::~GZed()
{

}

bool GZed::WalkIn() const
{
	return m_params.walk;
}

void GZed::Init(GGame *game, int x, int y, int bBmp, int type)
{
	GObject::Init(game, x, y, bBmp, 0);
	m_params = P_zed[type];
	m_anim = 0;
	m_destroiig = false;
}

void GZed::Draw(int bx, int by)
{
	if (!m_destroiig)
		d3dx.Draw(m_bBmp, bx, by-25, 0, 1, 1, 255, m_params.sx+m_anim/m_params.anim_speed, m_params.sy);
	else
		d3dx.Draw(m_bBmp, bx, by-25, 0, 1, 1, 255, m_params.sx+m_anim/m_params.animsbum_speed, m_params.sy);
}

bool GZed::Hit()
{
	if (m_params.destructable == false) return (m_params.walk || m_params.firein);
	if (m_params.walk == true) return true;
	if (m_destroiig) return (m_params.walk || m_params.firein);
	m_anim = 0;
	m_destroiig = true;
	if (rand()%9 > 6-m_game->m_bombers || m_game->m_deadmatch) m_game->m_map.AddBonus(m_mx, m_my); // pridani bonusu po rozbite zdi
	return false;
}

void GZed::Move()
{
	if (!m_destroiig) {
		if (++m_anim > m_params.anims*m_params.anim_speed) m_anim = 0;
	}
	else if (m_anim < m_params.animbum*m_params.animsbum_speed) m_anim++;
		  else m_params.walk = true;
	      
}

bool GZed::FireIn() const
{
	return (m_params.firein || m_params.walk);
}
