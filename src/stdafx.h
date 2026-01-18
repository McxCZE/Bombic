#ifndef _STDAFX_H_
#define _STDAFX_H_

// Modern C++ headers
#include <memory>
#include <string>
#include <cstdint>

#include <SDL.h>
#include "HD3DX.h"
#include "HDSound.h"
#include "HDInput.h"

// Sound effect IDs
enum SoundEffect : int {
    SND_MENU_CHNG         = 0,
    SND_MENU_SEL          = 1,
    SND_GAME_EXPLODE      = 2,
    SND_GAME_MEGA_EXPLODE = 3,
    SND_GAME_BOMBPUT      = 4,
    SND_GAME_BONUS        = 5,
    SND_GAME_TIME_BONUS   = 6,
    SND_GAME_DEAD_BOMBER  = 7,
    SND_GAME_DEAD_MRCHA   = 8
};

#define gfx_modules gfx_module

extern HDSoundBuffer g_sb[25];

// Safe delete macro - uses nullptr for modern C++
#define SAFE_DELETE(x) { if (x != nullptr) { delete x; x = nullptr; } }


#endif
