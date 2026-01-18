
#include "HFont.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <iostream>
#include "searchfile.h"
#include "HD3DX.h"

using namespace std;

// meausres height of font
int HFont::height()
{
	if (m_height == -1) {
		SDL_Color color = {0,0,0,0};
		SDL_Surface *tmp = TTF_RenderUTF8_Solid(m_font, " ", color);
		if (tmp == nullptr) {
			cerr << "Font rendering error: " << TTF_GetError() << endl;
			return -1;
		}
		m_height = tmp->h;
		SDL_FreeSurface(tmp);
	}
	return m_height;
}

// Font drawing functions
void HFont::DrawText( int x, int y, const std::string text, int r, int g, int b, unsigned int flags)
{
	DrawText(x,y,text.c_str(), r, g, b, flags);
}
    

void HFont::DrawText( int x, int y, const char* text, int r, int g, int b, unsigned int flags)
{
	SDL_Color color = {(Uint8)b, (Uint8)g, (Uint8)r, 255};
	DrawText(x, y, text, color, flags);
}

void HFont::DrawMultiline( int x, int y, const std::string text,
	 	int r, int g, int b, unsigned int flags)
{
	SDL_Color color = {(Uint8)b, (Uint8)g, (Uint8)r, 255};
	DrawMultiline(x, y, text.c_str(), color, flags);
}
		

#define MAX_BUFF 500

void HFont::DrawMultiline(  int x, int y, const char* text, 
			SDL_Color color, unsigned int flags)
{
	const char *c = text;

	// newline couter	
	int   n = 0;
	while (*c != '\0') 
		if (*c++ == '\n') n++;
		
	if (n == 0) {
		DrawText(x, y, text, color, flags);
		return;
	}

	// centering multiline text
	if (flags & HFONT_CENTER_Y) {
		y -= ((n+1)*m_height)/2;
	}


	// Draw separately each line
	char buff[MAX_BUFF];
	c=text;
	while(*c != '\0') {
		text = c;
		while (*c != '\0' && *c != '\n') c++;
				
		if (c-text > MAX_BUFF-1) {
			cerr << "DrawMulitline: String too long." << endl;
		}
		
		memcpy(buff, text, c-text);

		buff[c-text] = '\0';
		
//		cout << "Drawing: x = " << x << " y =  "<< y 
//			<<" text : " << buff << endl;
				
		DrawText(x, y, buff, color, flags&(~HFONT_CENTER_Y));

		y += m_height;
		if (*c == '\0') break;
		c++;
	}
	
	
}


void HFont::DrawText( int x, int y, const char* text, 
			SDL_Color color, unsigned int flags)
{

    	if (!m_font) return;
    	//SDL_Surface *s = TTF_RenderText_Solid(m_font, text, color);
    	SDL_Surface *s = TTF_RenderUTF8_Solid(m_font, text, color);
//	SDL_Surface *s = TTF_RenderText_Blended(m_font, text, color);
	if (!s) return;
	
	SDL_Rect rect;
	rect.w = s->w;
	rect.h = s->h;

	// centering of text
	if (flags & HFONT_CENTER_X) rect.x = x - s->w/2;
	else rect.x = x;
	if (flags & HFONT_CENTER_Y) rect.y = y - s->h/2;
	else	rect.y = y;
	
	
	SDL_BlitSurface(s, NULL, d3dx.m_renderBuffer, &rect);

	SDL_FreeSurface(s);
	
};

// Constructor / destructor
HFont::HFont( const char* file, int ptsize) 
{

	m_font = nullptr;
//    	if(!TTF_WasInit() && TTF_Init()==-1) {
    	if( TTF_Init()==-1) {
		std::cerr << "TTF_Init: " << TTF_GetError() << std::endl;
		return;
	}
	
	m_font = TTF_OpenFont_Search(file, ptsize);
	if(!m_font) {
		return;
	}

	// setin height
	m_height = -1;
	height();
	
};
    
HFont::~HFont() 
{
    	if (m_font != nullptr) {
		TTF_CloseFont(m_font);
		m_font = nullptr;
	}
};


// Special for menu drawing - draws HFONT_CENTER
void HFont::MenuText(int x, int y, const std::string text, int ID, int sel, int /*flags*/)
{
	if (sel == ID)
		DrawText( x, y, text.c_str(), 200, 200, 0,  HFONT_CENTER);
	else
		DrawText( x, y, text.c_str(), 255, 255, 255,  HFONT_CENTER);
}

