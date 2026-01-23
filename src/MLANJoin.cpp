// MLANJoin.cpp: implementation of the MLANJoin class.
// LAN multiplayer - Join game screen (enter IP)
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "MLANJoin.h"
#include "MainFrm.h"
#include "Network.h"
#include "MLAN.h"  // For g_coopMode
#include <cstring>
#include <cstdio>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MLANJoin::MLANJoin()
{
	m_connecting = false;
	strncpy(m_hostIP, "192.168.1.", sizeof(m_hostIP));
	m_ipCursor = strlen(m_hostIP);
	m_blinkTimer = 0;
	m_autoTransition = false;
}

MLANJoin::~MLANJoin()
{
}

void MLANJoin::Init(CMainFrame *parent)
{
	GBase::Init(parent);
	m_bMenuBMP = d3dx.CreateTextureFromFile("mmain.jpg");
	g_sb[1].Play();
	m_connecting = false;
	m_blinkTimer = 0;
}

void MLANJoin::Move()
{
	m_blinkTimer++;
	if (m_blinkTimer > 60) m_blinkTimer = 0;

	if (m_connecting) {
		g_network.Update();
		// Auto-transition to lobby when connected
		if (g_network.IsConnected() && !m_autoTransition) {
			m_autoTransition = true;
		}
	}
}

void MLANJoin::Draw()
{
	d3dx.Draw(m_bMenuBMP, 400, 300, 0, 1.5625);

	m_font->DrawText(400, 180, _LNG_LAN_IP_, 255, 255, 255, HFONT_CENTER);

	// IP input field
	char ipDisplay[80];
	if (m_blinkTimer < 30 && !m_connecting) {
		snprintf(ipDisplay, sizeof(ipDisplay), "%s_", m_hostIP);
	} else {
		snprintf(ipDisplay, sizeof(ipDisplay), "%s", m_hostIP);
	}
	m_font->DrawText(400, 230, ipDisplay, 255, 255, 0, HFONT_CENTER);

	if (m_connecting) {
		if (g_network.IsConnected()) {
			m_font->DrawText(400, 310, _LNG_LAN_CONNECTED_, 0, 255, 0, HFONT_CENTER);
		} else if (g_network.GetState() == NET_STATE_DISCONNECTED) {
			m_font->DrawText(400, 310, g_network.GetLastError(), 255, 0, 0, HFONT_CENTER);
			m_connecting = false;
		} else {
			m_font->DrawText(400, 310, _LNG_LAN_CONNECTING_, 255, 255, 0, HFONT_CENTER);
		}
	} else {
		m_font->DrawText(400, 310, "Enter IP and press ENTER", 255, 255, 255, HFONT_CENTER);
	}

	m_font->MenuText(400, 420, _LNG_LAN_BACK_, 0, 0);
}

void MLANJoin::Destroy()
{
	d3dx.DestroyTexture(m_bMenuBMP);
	// Don't disconnect here - we want to keep the connection when going to lobby
}

int MLANJoin::OnKey(int nChar)
{
	// Check for auto-transition (called with nChar=0 from Move via MMenu)
	if (nChar == 0 && m_autoTransition) {
		m_autoTransition = false;
		g_sb[1].Play();
		return g_coopMode ? MENU_LAN_COOP_LOBBY : MENU_LAN_LOBBY;
	}

	// If connected, any key goes to lobby
	if (g_network.IsConnected()) {
		if (nChar == SDLK_RETURN || nChar == SDLK_SPACE) {
			return g_coopMode ? MENU_LAN_COOP_LOBBY : MENU_LAN_LOBBY;
		}
	}

	switch (nChar) {
	case SDLK_ESCAPE:
		g_network.Disconnect();
		m_connecting = false;
		return MENU_LAN;
		break;

	case SDLK_RETURN:
		if (!m_connecting && strlen(m_hostIP) > 0) {
			m_connecting = g_network.StartClient(m_hostIP);
		}
		break;

	case SDLK_BACKSPACE:
		if (!m_connecting && m_ipCursor > 0) {
			m_ipCursor--;
			m_hostIP[m_ipCursor] = '\0';
			g_sb[0].Play();
		}
		break;

	default:
		// Handle number and dot input for IP address
		if (!m_connecting && m_ipCursor < 63) {
			char c = 0;
			if (nChar >= SDLK_0 && nChar <= SDLK_9) {
				c = '0' + (nChar - SDLK_0);
			} else if (nChar == SDLK_PERIOD || nChar == SDLK_KP_PERIOD) {
				c = '.';
			} else if (nChar >= SDLK_KP_0 && nChar <= SDLK_KP_9) {
				c = '0' + (nChar - SDLK_KP_0);
			}

			if (c != 0) {
				m_hostIP[m_ipCursor++] = c;
				m_hostIP[m_ipCursor] = '\0';
				g_sb[0].Play();
			}
		}
		break;
	}
	return MENU_DEFAULT;
}
