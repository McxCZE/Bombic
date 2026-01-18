/*  Hippo Games - 2001 (C) - http://www.prog.cz/hippo/
 *    __________________________________________________
 *   /\     __  __    _____   ______   ______   ______  \
 *   \ \   /\ \|\ \  /\_  _\ /\  __ \ /\  __ \ /\  __ \  \
 *    \ \  \ \ \_\ \ \//\ \/ \ \ \_\ \\ \ \_\ \\ \ \ \ \  \
 *     \ \  \ \  __ \  \ \ \  \ \  __/ \ \  __/ \ \ \ \ \  \
 *      \ \  \ \ \/\ \  \_\ \_ \ \ \/   \ \ \/   \ \ \_\ \  \
 *       \ \  \ \_\ \_\ /\____\ \ \_\    \ \_\    \ \_____\  \
 *        \ \  \/_/\/_/ \/____/  \/_/     \/_/     \/_____/   \
 *         \ \_________________________________________________\
 *          \/_________________________________________________/
 *                           
 * 
 *  Souboru : HD3DX.cpp
 *  
 *  Projekt : HD3DX
 *  
 *  Autor   : Bernard Lidicky [berny@prog.cz]
 *
 *  Datum   : 13.10.2001
 *
 */                 

#ifndef _HIPPO_DIRECT3DX_INCLUDED_
#define _HIPPO_DIRECT3DX_INCLUDED_



#define MAX_HD3DX_TEXTURES 1024

#define TEXTURE_NONE      0
#define TEXTURE_REAL      1
#define TEXTURE_VIRTUAL   2

#include <SDL.h>
#include <string>
 
typedef struct {
	int                type;   // typ textury
	SDL_Surface       *surf;   // pointer na texturu
//	D3DXIMAGE_INFO     info;   // informace o obrazku
//	HMODULE            module; // modul resource
	std::string        name;   // jmeno souboru/resource - slouzi pri reloadu
	long              usage;  // ty uziti - muze byt RENDER_TARGET
	SDL_Rect           rect;   // obdelnik textury
	int              virtID; // ID pro virualni textury
} HD3DX_Texture;



class HD3DX  
{
public:	
	void DestroyTextures();
	bool NeedRestore();

	HD3DX();
	virtual ~HD3DX();

	bool Create(int w, int h, int bpp, bool windowed);
	void Resize(int x, int y, bool windowed);   // zmeni velikost device (obrazovky)
	void Destroy();              // zrusi
	bool Restore();              // po alt+tabu obnovi


	void BeginScene(bool clear = false); // pred zacatkem kresleni - clear == 10ms!!
	void Draw(int sprite, int x, int y, float angle = 0, float sx = 1, float sy = 1, int alpha = 255, int shx = 0, int shy = 0);
	void Draw(int sprite, float x, float y, float angle = 0, float sx = 1, float sy = 1, int alpha = 255, int shx = 0, int shy = 0);
	void EndScene();   // na konci kresleni

	void ErrorMessage(const std::string mess);  // zobrazi (vetsinou chybovou) hlasku

	int CreateTexture(int w, int h, int ID = -1);
//	int CreateTextureFromRes(HMODULE modul, LPCSTR resname, DWORD usage = 0, int ID = -1);
	int CreateTextureFromFile(const std::string file, int ID = -1);
	int CreateVirualTexture(int FromID, int left, int top, int w, int h);

	void ReloadTexture(int ID);   // znovu nacte texturu
	void DestroyTexture(int ID);  // znici texturu ID

//	D3DFORMAT GetFormat(int bpp); // vrati pixelovy format pro danou barevnou hloubku

/*
   LPDIRECT3D8        m_pD3D;               // hlavni D3D objekt
   LPDIRECT3DDEVICE8  m_dev;                // rendrovaci device
	LPD3DXSPRITE       m_sprite;

   D3DCAPS8              m_d3dCaps;            // Caps pro m_dev
   D3DSURFACE_DESC       m_d3dsdBackBuffer;    // Surface desc of the backbuffer
	D3DPRESENT_PARAMETERS m_d3dppPresentParam;  // Presentation Parameter
*/

	HD3DX_Texture m_text[MAX_HD3DX_TEXTURES]; // pole textur

	int  m_h;        // vyska m_dev
	int  m_w;        // sirka m_dev
	int  m_bpp;      // barevna hloubka m_dev
	bool m_windowed; // zba aplikace bezi v okne nebo ve fullscreenu

	SDL_Window *m_window;
	SDL_Surface *m_screen;      // window surface (may change size)
	SDL_Surface *m_renderBuffer; // internal 800x600 render target

	bool m_valid;

	void HandleResize();  // handle window resize

private:
	int FindFreeText(); // najde v m_text volnou polozku
};

extern HD3DX d3dx; // globalni deklarace

#endif // #ifndef _HIPPO_DIRECT3DX_INCLUDED_
