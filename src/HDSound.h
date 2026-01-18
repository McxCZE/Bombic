/*  Hippo Games - 2000 (C) - http://www.hippo.nipax.cz/
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
 *  Sub     : Funkce pro DirectSound
 *
 *  File    : HDSound.h
 *  
 *  Projekt : HDXII
 *  
 *  Autor   : Bernard Lidicky [berny@prog.cz]
 *
 *  Datum   : 29.12.2000
 *
 */      

#ifndef HDSOUND_HEADER
#define HDSOUND_HEADER

#include <SDL_mixer.h>

class HDSound  
{
public:
	bool Create();

	HDSound();
	virtual ~HDSound();

private:
};

extern HDSound dsound;

class HDSoundBuffer
{
public:
	void Stop();
	void SetVolume(int volume);
	void SetFreq(int freq);
	void SetPan(int pan);
	bool LoadFromRes(const char* resname);
	HDSoundBuffer();
	~HDSoundBuffer();

	void Play(bool loop = false);

private:
	Mix_Chunk *m_sample;
	int 	   m_sample_channel;
};

#endif // HDSOUND_H_

