// MMenu.cpp: implementation of the MMenu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DXApp.h"
#include "MMenu.h"
#include "MainFrm.h"


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
}
