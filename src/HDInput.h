/*  Hippo Games - 2000 (C) - http://www.prog.cz/hippo/
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
 *  Sub     : Funkce pro DirectInput
 *
 *  File    : HDInput.h
 *  
 *  Projekt : HDXII
 *  
 *  Autor   : Bernard Lidicky [berny@prog.cz]
 *
 *  Datum   : 29.12.2000
 *
 */      

#ifndef HDINPUT_HEADER
#define HDINPUT_HEADER

#include <SDL.h>


class HDInput  
{
public:



	void Update();
	bool Create();
	HDInput();
	virtual ~HDInput();

	// Mys  - x, y, z, buttonky
	int m_x;
	int m_y;
	int m_z;
	int m_b[4];

	const Uint8 *m_key; // buffer klavesnice
};

extern HDInput dinput;

#endif // ifndef HDINPUT_HEADER
