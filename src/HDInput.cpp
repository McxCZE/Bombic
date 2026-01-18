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
 *  File    : HDInput.cpp
 *  
 *  Projekt : HDXII
 *  
 *  Autor   : Bernard Lidicky [berny@prog.cz]
 *
 *  Datum   : 29.12.2000
 *
 */      

#include "stdafx.h"
#include "HDInput.h"


HDInput dinput;


//////////////////////////////////////////////////////////////////////////////////
// NULL-ovaci konstruktor
//////////////////////////////////////////////////////////////////////////////////
HDInput::HDInput()
{
}

//////////////////////////////////////////////////////////////////////////////////
// Destruktor - releasuje to, co je potreba
//////////////////////////////////////////////////////////////////////////////////
HDInput::~HDInput()
{
}

//////////////////////////////////////////////////////////////////////////////////
// Hlavni tvorici fce. Oba parametry mohou byt NULL - automaticke vyplneni
// parametru. Vytvori obsluhu pro klavesnici i mys
//////////////////////////////////////////////////////////////////////////////////
bool HDInput::Create()
{
	Update();
	
	return true;
}

//////////////////////////////////////////////////////////////////////////////////
// Aktualizace vstupnich dat. Melo by se volat vzdy pred pouzitim dat.
//////////////////////////////////////////////////////////////////////////////////
void HDInput::Update()
{
	// aktulizace klavesnice
	m_key = SDL_GetKeyboardState(NULL);

	Uint32 buttons;

	buttons = SDL_GetMouseState(&m_x, &m_y);
	m_z = 0;
	if ( buttons & SDL_BUTTON_LMASK ) m_b[0] = 1;
	else m_b[0] = 0;

}

