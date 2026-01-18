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
 *  Datum   : 15.8.2001
 *
 */                 

#include "config.h"
#include "searchfile.h"
#include <iostream>
#include "stdafx.h"
#include "HD3DX.h"

#include <SDL_ttf.h>
#include <SDL_image.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

using namespace std;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HD3DX::HD3DX()
{
	for (int i = 0; i < MAX_HD3DX_TEXTURES; i++) {
		m_text[i].surf = nullptr;
		m_text[i].usage = 0;
		m_text[i].name = "";
		m_text[i].type = TEXTURE_NONE;
	}

	m_window = nullptr;
	m_screen = nullptr;
	m_renderBuffer = nullptr;
	m_valid = false;
}

///////////////////////////////////////////////////////////////////////////////////////

HD3DX::~HD3DX()
{

}


///////////////////////////////////////////////////////////////////////////////////////

bool HD3DX::Create(int w, int h, int bpp, bool windowed)
{

	// SDL video inicializaton
	if(SDL_WasInit(SDL_INIT_VIDEO)==0)
		if ( SDL_InitSubSystem(SDL_INIT_VIDEO) < 0 ) {
        	cerr << "SDL video inicizlization failed: "
			<<  SDL_GetError() << endl;
		return false;
		}

	Uint32 flags = SDL_WINDOW_SHOWN;
	if (!windowed) flags |= SDL_WINDOW_FULLSCREEN;
	else flags |= SDL_WINDOW_RESIZABLE;

	m_window = SDL_CreateWindow("Bombic",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		w, h, flags);

	if (m_window == nullptr) {
		cerr << "Window creation failed: " << SDL_GetError() << endl;
		return false;
	}

	m_screen = SDL_GetWindowSurface(m_window);
	if (m_screen == nullptr) {
		cerr << "Failed to get window surface: " << SDL_GetError() << endl;
		return false;
	}

	// Create internal render buffer at fixed game resolution
	m_renderBuffer = SDL_CreateRGBSurface(0, w, h, 32,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	if (m_renderBuffer == nullptr) {
		cerr << "Failed to create render buffer: " << SDL_GetError() << endl;
		return false;
	}

	m_bpp = bpp;
	m_windowed = windowed;
	m_w = w;
	m_h = h;

	m_valid = true;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

void HD3DX::Destroy()
{
	DestroyTextures();

	if (m_renderBuffer != nullptr) {
		SDL_FreeSurface(m_renderBuffer);
		m_renderBuffer = nullptr;
	}

	if (m_window != nullptr) {
		SDL_DestroyWindow(m_window);
		m_window = nullptr;
		m_screen = nullptr; // freed with window
	}
}

///////////////////////////////////////////////////////////////////////////////////////

int HD3DX::CreateTextureFromFile(string file, int ID)
{
	if (ID == -1) ID = FindFreeText();

	if (ID == 0) {
		cerr << "HD3DX: No free texture slots available for " << file << endl;
		return 0;
	}

	// load from file
	SDL_Surface *loaded = IMG_Load_Search(file.c_str());

	if (loaded == nullptr) {
		cerr << "HD3DX: Failed to load texture: " << file << " - " << IMG_GetError() << endl;
		return 0;
	}

	// Convert to screen format for proper color key and alpha support
	m_text[ID].surf = SDL_ConvertSurfaceFormat(loaded, SDL_PIXELFORMAT_ARGB8888, 0);
	SDL_FreeSurface(loaded);

	if (m_text[ID].surf == nullptr) {
		cerr << "HD3DX: Failed to convert surface format for " << file << " - " << SDL_GetError() << endl;
		return 0;
	}

	// transparent color - pink
	if ( SDL_SetColorKey(m_text[ID].surf, SDL_TRUE,
		 SDL_MapRGB(m_text[ID].surf->format, 255, 0, 255)) < 0 ) {
		cerr << "HD3DX: Failed to set color key for " << file << " - " << SDL_GetError() << endl;
		return 0;
	}

	m_text[ID].name = file;

	m_text[ID].virtID = 0;
	m_text[ID].type = TEXTURE_REAL;
	m_text[ID].rect.x = 0;
	m_text[ID].rect.y = 0;
	m_text[ID].rect.w = m_text[ID].surf->w;
	m_text[ID].rect.h = m_text[ID].surf->h;

	return ID;
}

///////////////////////////////////////////////////////////////////////////////////////

void HD3DX::DestroyTexture(int ID)
{
	if (ID <= 0) return;
	if (m_text[ID].surf != nullptr) {
		SDL_FreeSurface(m_text[ID].surf);
		m_text[ID].surf = nullptr;
	}
	m_text[ID].name = "";
	m_text[ID].type = TEXTURE_NONE;
}

///////////////////////////////////////////////////////////////////////////////////////

int HD3DX::FindFreeText()
{
	int i;
	for (i = 1; i < MAX_HD3DX_TEXTURES; i++) 
		if (m_text[i].type == TEXTURE_NONE) break;
	if (i == MAX_HD3DX_TEXTURES) return 0;
	return i;
}

///////////////////////////////////////////////////////////////////////////////////////

void HD3DX::BeginScene(bool clear)
{
	if (m_renderBuffer == nullptr) return;

	// all to black - clear the render buffer
	if (clear)
		SDL_FillRect(m_renderBuffer, NULL, SDL_MapRGB(m_renderBuffer->format, 0, 0, 0));
}

///////////////////////////////////////////////////////////////////////////////////////

void HD3DX::EndScene()
{
	if (m_screen == nullptr || m_window == nullptr || m_renderBuffer == nullptr) return;

	// Calculate scaled size maintaining aspect ratio (4:3)
	int winW = m_screen->w;
	int winH = m_screen->h;
	float gameAspect = (float)m_w / (float)m_h;  // 800/600 = 4:3
	float winAspect = (float)winW / (float)winH;

	SDL_Rect destRect;
	if (winAspect > gameAspect) {
		// Window is wider - pillarbox (black bars on sides)
		destRect.h = winH;
		destRect.w = (int)(winH * gameAspect);
		destRect.x = (winW - destRect.w) / 2;
		destRect.y = 0;
	} else {
		// Window is taller - letterbox (black bars top/bottom)
		destRect.w = winW;
		destRect.h = (int)(winW / gameAspect);
		destRect.x = 0;
		destRect.y = (winH - destRect.h) / 2;
	}

	// Clear screen (black bars)
	SDL_FillRect(m_screen, NULL, SDL_MapRGB(m_screen->format, 0, 0, 0));

	// Scale render buffer to window surface with aspect ratio
	SDL_BlitScaled(m_renderBuffer, NULL, m_screen, &destRect);

	// update screen
	SDL_UpdateWindowSurface(m_window);
}

void HD3DX::HandleResize()
{
	if (m_window == nullptr) return;

	// Get new window surface after resize
	m_screen = SDL_GetWindowSurface(m_window);
}

void HD3DX::Draw(int sprite, float x, float y, float angle, float sx, float sy, int alpha, int shx, int shy)
{
	Draw(sprite, (int)x, (int)y, angle, sx, sy, alpha, shx, shy);
}

///////////////////////////////////////////////////////////////////////////////////////

void HD3DX::Draw(int sprite, int x, int y, float /*angle*/, float /*sx*/, float /*sy*/, int alpha, int shx, int shy)
{
	if (m_text[sprite].type == TEXTURE_NONE) {
		return;
	}

	SDL_Rect rect_dst, rect_src;
	int   fsprite = sprite;

	if (m_text[sprite].type == TEXTURE_VIRTUAL)
		fsprite = m_text[sprite].virtID;

	if (m_text[fsprite].surf == nullptr) return;


	// DST
	rect_dst.w  = m_text[sprite].rect.w;
	rect_dst.h  = m_text[sprite].rect.h;
	rect_dst.x  = x - rect_dst.w/2;
	rect_dst.y  = y - rect_dst.h/2;

	rect_src.x = m_text[sprite].rect.x + (m_text[sprite].rect.w+1) * shx;
	rect_src.y = m_text[sprite].rect.y + (m_text[sprite].rect.h+1) * shy;
	rect_src.h = m_text[sprite].rect.h;
	rect_src.w  = m_text[sprite].rect.w;

	SDL_SetSurfaceAlphaMod(m_text[fsprite].surf, alpha);
	SDL_SetSurfaceBlendMode(m_text[fsprite].surf, SDL_BLENDMODE_BLEND);
	SDL_BlitSurface(m_text[fsprite].surf, &rect_src, m_renderBuffer, &rect_dst);
}

///////////////////////////////////////////////////////////////////////////////////////

void HD3DX::Resize(int sx, int sy, bool windowed)
{
	// Not implemented for SDL2 - window resizing handled elsewhere
	(void)sx; (void)sy; (void)windowed;
}


///////////////////////////////////////////////////////////////////////////////////////

int HD3DX::CreateVirualTexture(int FromID, int left, int top, int w, int h)
{
	int ID = FindFreeText();
	if (ID == 0) return 0;

	m_text[ID].surf = nullptr;

	m_text[ID].name = "";
	m_text[ID].usage = 0;
	m_text[ID].type = TEXTURE_VIRTUAL;
	m_text[ID].virtID = FromID;

	m_text[ID].rect.x = left;
	m_text[ID].rect.y = top;
	m_text[ID].rect.w = w;
	m_text[ID].rect.h = h;

	return ID;
}

bool HD3DX::NeedRestore()
{
	return false;
}

void HD3DX::DestroyTextures()
{
	for (int i = 1; i < MAX_HD3DX_TEXTURES; i++) 
		DestroyTexture(i);
}
