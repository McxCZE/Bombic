// MLANHost.cpp: implementation of the MLANHost class.
// LAN multiplayer - Host waiting for connection screen
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "MLANHost.h"
#include "MainFrm.h"
#include "Network.h"
#include <cstring>
#include <cstdio>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ifaddrs.h>
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MLANHost::MLANHost()
{
	m_hostStarted = false;
	m_localIP[0] = '\0';
	m_autoTransition = false;
}

MLANHost::~MLANHost()
{
}

// Helper function to get local IP address
static void GetLocalIP(char* buffer, size_t bufsize)
{
	strncpy(buffer, "Unknown", bufsize);
	buffer[bufsize-1] = '\0';

#ifndef _WIN32
	struct ifaddrs *ifaddr, *ifa;
	if (getifaddrs(&ifaddr) == -1) {
		return;
	}

	for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == nullptr) continue;

		// Only IPv4
		if (ifa->ifa_addr->sa_family == AF_INET) {
			struct sockaddr_in *addr = (struct sockaddr_in*)ifa->ifa_addr;
			char *ip = inet_ntoa(addr->sin_addr);

			// Skip loopback
			if (strcmp(ip, "127.0.0.1") != 0) {
				strncpy(buffer, ip, bufsize - 1);
				buffer[bufsize - 1] = '\0';
				break;
			}
		}
	}

	freeifaddrs(ifaddr);
#else
	char hostname[256];
	if (gethostname(hostname, sizeof(hostname)) == 0) {
		struct hostent *host = gethostbyname(hostname);
		if (host) {
			struct in_addr *addr = (struct in_addr*)host->h_addr_list[0];
			strncpy(buffer, inet_ntoa(*addr), bufsize);
			buffer[bufsize-1] = '\0';
		}
	}
#endif
}

void MLANHost::Init(CMainFrame *parent)
{
	GBase::Init(parent);
	m_bMenuBMP = d3dx.CreateTextureFromFile("mmain.jpg");
	g_sb[1].Play();

	// Get local IP
	GetLocalIP(m_localIP, sizeof(m_localIP));

	// Start hosting
	m_hostStarted = g_network.StartHost();
	if (!m_hostStarted) {
		// Failed to start host, error will be shown
	}
}

void MLANHost::Move()
{
	if (m_hostStarted) {
		g_network.Update();
		// Auto-transition to lobby when player connects
		if (g_network.IsConnected() && !m_autoTransition) {
			m_autoTransition = true;
		}
	}
}

void MLANHost::Draw()
{
	d3dx.Draw(m_bMenuBMP, 400, 300, 0, 1.5625);

	char ipText[128];
	snprintf(ipText, sizeof(ipText), "%s: %s:%d", _LNG_LAN_IP_.c_str(), m_localIP, NET_DEFAULT_PORT);
	m_font->DrawText(400, 200, ipText, 255, 255, 255, HFONT_CENTER);

	if (m_hostStarted) {
		if (g_network.IsConnected()) {
			m_font->DrawText(400, 280, _LNG_LAN_CONNECTED_, 0, 255, 0, HFONT_CENTER);
			m_font->DrawText(400, 340, "Press ENTER to continue", 255, 255, 255, HFONT_CENTER);
		} else {
			m_font->DrawText(400, 280, _LNG_LAN_WAITING_, 255, 255, 0, HFONT_CENTER);
		}
	} else {
		m_font->DrawText(400, 280, g_network.GetLastError(), 255, 0, 0, HFONT_CENTER);
	}

	m_font->MenuText(400, 420, _LNG_LAN_BACK_, 0, 0);
}

void MLANHost::Destroy()
{
	d3dx.DestroyTexture(m_bMenuBMP);
	// Don't disconnect here - we want to keep the connection when going to lobby
}

int MLANHost::OnKey(int nChar)
{
	// Check for auto-transition (called with nChar=0 from Move via MMenu)
	if (nChar == 0 && m_autoTransition) {
		m_autoTransition = false;
		g_sb[1].Play();
		return MENU_LAN_LOBBY;
	}

	switch (nChar) {
	case SDLK_ESCAPE:
		g_network.Disconnect();
		return MENU_LAN;
		break;
	case SDLK_RETURN:
		if (g_network.IsConnected()) {
			return MENU_LAN_LOBBY;
		}
		break;
	}
	return MENU_DEFAULT;
}
