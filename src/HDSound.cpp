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
 *  Sub     : Funkce pro Zvuk
 *
 *  File    : HDSound.cpp
 *  
 *  Projekt : HDXII
 *  
 *  Autor   : Bernard Lidicky [2berny@seznam.cz]
 *
 *  Datum   : 29.12.2000
 *
 */      

#include "stdafx.h"
#include "HDSound.h"
#include <iostream>
#include "searchfile.h"

using namespace std;


HDSound dsound;
extern bool gsoundavailable;

//////////////////////////////////////////////////////////////////////////////////
// NULL-ovaci konstruktor
//////////////////////////////////////////////////////////////////////////////////
HDSound::HDSound()
{
}

//////////////////////////////////////////////////////////////////////////////////
// releasovaci destruktor
//////////////////////////////////////////////////////////////////////////////////
HDSound::~HDSound()
{
	// Melo by byt nekde jinde :-( Driv, nez se zavre SDL_Quit
	Mix_CloseAudio();
}

bool HDSound::Create()
{

	if(SDL_WasInit(SDL_INIT_AUDIO)==0)
		if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0 ) {
			cerr << "SDL audio inicizlization failed: " 
				<<  SDL_GetError() << endl;
			return false;
		}
// open 44.1KHz, signed 16bit, system byte order,
//      stereo audio, using 1024 byte chunks
	if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048)==-1) {
		cerr << "Mix_OpenAudio: " <<  Mix_GetError() << endl;
		return false;
	}

	Mix_AllocateChannels(16);
	
	return true;
	
}




HDSoundBuffer::HDSoundBuffer()
{
	m_sample = nullptr;
	m_sample_channel = -1;
}

HDSoundBuffer::~HDSoundBuffer()
{
	if (m_sample != nullptr) {
		Mix_FreeChunk(m_sample);
		m_sample = nullptr;
	}
}

void HDSoundBuffer::Play(bool loop)
{
	if (!gsoundavailable || !m_sample) return;
	
	if (loop) {
		m_sample_channel = Mix_PlayChannel(-1, m_sample, -1);
	}
	else {
		m_sample_channel = Mix_PlayChannel(-1, m_sample, 0);
	}
}

bool HDSoundBuffer::LoadFromRes(const char *resname)
{
	const char *c = locate_file(resname);
	if (c == nullptr) {
		cerr << "Sound file not found: " << resname << endl;
		return false;
	}
	m_sample = Mix_LoadWAV(c);
	if (m_sample == nullptr) {
		cerr << "Failed to load sound: " << resname << " - " << Mix_GetError() << endl;
		return false;
	}
	m_sample_channel = -1;
	return true;
}

void HDSoundBuffer::SetPan(int pan)
{
}

void HDSoundBuffer::SetFreq(int freq)
{

}

void HDSoundBuffer::SetVolume(int volume)
{

}

void HDSoundBuffer::Stop()
{
	if (m_sample_channel != -1) 
		Mix_HaltChannel(m_sample_channel);
}

