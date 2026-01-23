// MMenu.cpp: implementation of the MMenu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "MMenu.h"
#include "MainFrm.h"
#ifdef HAVE_SDL2_NET
#include "Network.h"
#endif


#define D_SIZE    (float)0.01
#define D_MAXSIZE 1.5
#define D_MINSIZE 0.8

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MMenu::MMenu()
{
	m_actual = nullptr;
}

MMenu::~MMenu()
{

}

void MMenu::Init(CMainFrame *parent)
{
	GBase::Init(parent);

	m_bMenuBMP = d3dx.CreateTextureFromFile("menu.bmp");
	m_bLogo    = d3dx.CreateTextureFromFile("menu2.bmp");
	m_bHLogo    = d3dx.CreateTextureFromFile("menu3.bmp");

	if (m_actual == nullptr) {

		m_Mmain.Init(parent);
		m_actual = &m_Mmain;

	} else {
		m_actual->Init(parent);
	}

	m_size = 1;
	m_dsize = D_SIZE;
}

void MMenu::Move()
{
	if ((m_size += m_dsize) > D_MAXSIZE) m_dsize = -m_dsize;
	else if (m_size < D_MINSIZE) m_dsize = -m_dsize;

	m_actual->Move();

	// Check for auto-transitions (e.g., LAN connection established)
	// Call OnKey with 0 to check for pending transitions
	OnKey(0);
}

int MMenu::OnKey(int nChar)
{
	switch (m_actual->OnKey(nChar)) {
		case MENU_MAIN         :
			m_actual->Destroy();
			m_Mmain.Init(m_pParent);
			m_actual = &m_Mmain; 
			break;
		case MENU_DEADMATCH    : 
			m_actual->Destroy();
			m_Mdeadmatch.Init(m_pParent);
			m_actual = &m_Mdeadmatch; 
			break;
		case MENU_SINGLE       : 
			m_actual->Destroy();
			m_Msingle.Init(m_pParent);
			m_actual = &m_Msingle; 
			break;
		case MENU_MRCHOVNIK    :
			m_actual->Destroy();
			m_Mmrchovnik.Init(m_pParent);
			m_actual = &m_Mmrchovnik; 
			break;
 		case MENU_END          : 
			return MENU_END; 
			break;
		case MENU_DEADMATCHING : 
			m_actual->Destroy();
			m_Mdeadmatching.Init(m_pParent);
			m_actual = &m_Mdeadmatching; 
			m_Mdeadmatching.StartGame(m_Mdeadmatch.m_filelist[m_Mdeadmatch.m_ID].file,
				            m_Mdeadmatch.m_players, m_Mdeadmatch.m_monsters,
								m_Mdeadmatch.m_bonuslevel, m_Mdeadmatch.m_victories);
			break;
		case MENU_SINGLEPLAYING: 
			m_actual->Destroy();
			m_Msingleplaying.Init(m_pParent);
			m_actual = &m_Msingleplaying; 
			m_Msingleplaying.StartGame(m_Msingle.m_code, m_Msingle.m_players);
			break;
		case MENU_SETTING    :
			m_actual->Destroy();
			m_Msetting.Init(m_pParent);
			m_actual = &m_Msetting; 
			break;
		case MENU_HELP    :
			m_actual->Destroy();
			m_Mhelp.Init(m_pParent);
			m_actual = &m_Mhelp; 
			break;
		case MENU_CREDITS    :
			m_actual->Destroy();
			m_Mcredits.Init(m_pParent);
			m_actual = &m_Mcredits;
			break;
#ifdef HAVE_SDL2_NET
		case MENU_LAN        :
			m_actual->Destroy();
			m_Mlan.Init(m_pParent);
			m_actual = &m_Mlan;
			break;
		case MENU_LAN_HOST   :
			m_actual->Destroy();
			m_MlanHost.Init(m_pParent);
			m_actual = &m_MlanHost;
			break;
		case MENU_LAN_JOIN   :
			m_actual->Destroy();
			m_MlanJoin.Init(m_pParent);
			m_actual = &m_MlanJoin;
			break;
		case MENU_LAN_LOBBY  :
			m_actual->Destroy();
			m_MlanLobby.Init(m_pParent);
			m_actual = &m_MlanLobby;
			break;
		case MENU_LAN_PLAYING:
			m_actual->Destroy();
			m_MlanPlaying.Init(m_pParent);
			m_actual = &m_MlanPlaying;
			if (g_network.IsHost()) {
				// Host uses lobby settings
				m_MlanPlaying.StartGame(
					m_MlanLobby.m_filelist[m_MlanLobby.m_mapID].file,
					m_MlanLobby.m_monsters,
					m_MlanLobby.m_bonuslevel,
					m_MlanLobby.m_victories);
			} else {
				// Client uses settings from network packet
				const NetGameStartPacket& info = g_network.GetGameStartInfo();
				m_MlanPlaying.StartGame(
					info.mapFile,
					info.monsters != 0,
					info.bonuslevel,
					info.victories);
			}
			break;
		case MENU_LAN_COOP_LOBBY:
			m_actual->Destroy();
			m_MlanCoopLobby.Init(m_pParent);
			m_actual = &m_MlanCoopLobby;
			break;
		case MENU_LAN_COOP_PLAYING:
			m_actual->Destroy();
			m_MlanCoopPlaying.Init(m_pParent);
			m_actual = &m_MlanCoopPlaying;
			// Both host and client use StartCampaign with level from lobby
			// (lobby already synced level from host to client via CoopLevelInfo packets)
			m_MlanCoopPlaying.StartCampaign(m_MlanCoopLobby.GetSelectedLevel());
			break;
#endif
		case MENU_DEFAULT      :
			return MENU_DEFAULT;
			break;
	}

	return MENU_DEFAULT;
}

void MMenu::Draw()
{
	d3dx.BeginScene(true);
	m_actual->Draw();

/* single 
	d3dx.Draw(m_bMenuBMP, 110, 46, 0);
	d3dx.Draw(m_bLogo,    700, 553, 0);
	d3dx.Draw(m_bHLogo,   80, 545, 0);
	m_font->DrawTextScaled( 80, 580,  0.9f, 0.5f, 0.5f, 0xffffffff, "www.hippo.nipax.cz", D3DFONT_FILTERED | D3DFONT_CENTERED);
*/
	d3dx.Draw(m_bMenuBMP, 110, 46, 0);
	d3dx.Draw(m_bHLogo,   700, 545, 0);
	
	m_font->DrawText( 700, 580, "hippo.nipax.cz", 255, 255, 255,  HFONT_CENTER);

}

void MMenu::Destroy()
{
	d3dx.DestroyTexture(m_bMenuBMP);
	d3dx.DestroyTexture(m_bLogo);
	d3dx.DestroyTexture(m_bHLogo);

	m_Msingle.Destroy();
	m_Mdeadmatching.Destroy();
	m_Mdeadmatch.Destroy();
	m_Mmain.Destroy();
	m_Mmrchovnik.Destroy();
	m_Msingleplaying.Destroy();
	m_Msetting.Destroy();
	m_Mhelp.Destroy();
	m_Mcredits.Destroy();
#ifdef HAVE_SDL2_NET
	m_Mlan.Destroy();
	m_MlanHost.Destroy();
	m_MlanJoin.Destroy();
	m_MlanLobby.Destroy();
	m_MlanPlaying.Destroy();
	m_MlanCoopLobby.Destroy();
	m_MlanCoopPlaying.Destroy();
#endif
}
