#ifndef HFONT_H
#define HFONT_H


#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <iostream>
#include "searchfile.h"

#define HFONT_CENTER_X 0x00001
#define HFONT_CENTER_Y 0x00002
#define HFONT_CENTER   0x00003


//TODO: This is EXTREMLY bad implementation
class HFont
{

public:
    // Font drawing functions
	void DrawText( int x, int y, const std::string text, 
			int r, int g, int b, unsigned int flags = 0);
			
	void DrawText( int x, int y, const char* text, 
			int r, int g, int b, unsigned int flags = 0); 
			
	void DrawText( int x, int y, const char* text, 
			SDL_Color color, unsigned int flags = 0);

	void DrawMultiline(  int x, int y, const char* text, 
			SDL_Color color, unsigned int flags = 0);

	void DrawMultiline( int x, int y, const std::string text,
			 int r, int g, int b, unsigned int flags = 0);
	// Constructor / destructor
	HFont( const char* file, int ptsize); 
	~HFont();

	// Special for menu drawing - draws HFONT_CENTER
	// ID - id of text in menu
	// sel - currently selected id - changes color of text
	void MenuText(int x, int y, const std::string text, 
		int ID, int sel, int flags = HFONT_CENTER);

	// returns height of bitmap with font
	int height();
private:
	TTF_Font *m_font;
	int m_height;
};



#endif


