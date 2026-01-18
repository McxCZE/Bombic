// GBonus.cpp: implementation of the GBonus class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "GBonus.h"
#include "GBomber.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern float gspeed;

GBonus::GBonus()
{
	m_timer = -1;
	m_shx = 0;
	m_shy = 0;
	m_bView = -1;
	m_timer_MAX = 0;
	m_shouldDelete = false;
}

GBonus::~GBonus()
{

}

void GBonus::Init(int mx, int my, GBonus **self, int bBonusy)
{
	m_mx = mx;
	m_my = my;
	m_self = self;
	m_bBmp = bBonusy;
}

void GBonus::BeforeMove(float* /*speed*/)
{
}

void GBonus::DrawOnGround(int x, int y)
{
	d3dx.Draw(m_bBmp, x, y, 0, 1, 1, 255, m_shx, m_shy);
}

void GBonus::DrawOnPlayer(int /*x*/, int /*y*/)
{
}

void GBonus::Move()
{
	if (m_timer > 0)
		if (--m_timer == 0) {
			End();
			*m_self = nullptr;
			m_shouldDelete = true;  // Mark for deletion by owner
		}
}

void GBonus::Picked(GBomber *bomber, GBonus **self)
{
	m_bomber = bomber;
	m_self   = self;
	m_map    = bomber->m_map;

	m_timer = m_timer_MAX;

	OneTime();
	if (m_onetime) {
		*m_self = nullptr;
		m_shouldDelete = true;  // Mark for deletion by caller
		return;  // Don't access members after marking for deletion
	}

	m_timer = (int)(((float)m_timer)/gspeed);
	m_timer_MAX = (int)(((float)m_timer_MAX)/gspeed);

}

void GBonus::OneTime()
{

}

void GBonus::BeforeInput(bool* /*left*/, bool* /*right*/, bool* /*up*/, bool* /*down*/, bool* /*action*/)
{
}

GBonus* GBonus::GetCopy()
{
	return NULL;
}

void GBonus::End()
{

}

bool GBonus::HitProtect()
{
	return false;
}

void GBonus::AfterPut(int /*bombID*/)
{
}
