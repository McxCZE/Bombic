# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
./build.sh          # Build (checks deps, runs cmake + make)
./install.sh        # Install to /usr/local/bin/bombic
bombic              # Run after install
```

**Manual build:**
```bash
mkdir -p build && cd build
cmake ..
make -j$(nproc)
```

**Run from build directory:**
```bash
./build/src/bombic
```

**Command-line options:**
```
-p X, --players X   Set player count (1-4)
-f, --fullscreen    Fullscreen mode
-m MAP.ble          Load specific map
-d, --deathmatch    Deathmatch mode
```

## Dependencies

SDL2, SDL2_image, SDL2_ttf, SDL2_mixer, SDL2_net (optional - enables LAN multiplayer)

## Architecture

**State Machine + Scene System:**
```
CMainFrame (Main Controller)
├── GGame (Active Game Engine)
├── MMenu (Menu System - State Router)
└── GBase* (Active Scene/Mode)
```

**Object Lifecycle:** `Init()` → `Move()` (logic) → `Draw()` (render) → `OnKey()` (input) → `Destroy()`

**Main Loop (D3DXApp.cpp):** 25fps cap, calls `pFrame->OnTimer()` which delegates to active scene's `Move()` and `Draw()`.

### Key Components

| Component | Files | Purpose |
|-----------|-------|---------|
| Main Entry | `D3DXApp.cpp` | SDL init, main loop, CLI parsing |
| Controller | `MainFrm.cpp/h` | Scene delegation |
| Game Logic | `GGame.cpp/h` | Bomber/monster management |
| Map | `GMap.cpp/h` | 50x50 tile grid, bomb physics, collisions |
| Player | `GBomber.cpp/h` | Movement, bomb placement, abilities |
| Graphics | `HD3DX.cpp/h` | SDL2 sprite rendering |
| Network | `Network.cpp/h` | P2P LAN multiplayer |

### Scene Types (M*.cpp/h)

- `MMain` - Main menu
- `MSingle/MSinglePlaying` - Single player (40 levels)
- `MDeadMatch/MDeadMatching` - Local deathmatch
- `MLAN/MLANHost/MLANJoin/MLANLobby/MLANPlaying` - LAN multiplayer (conditional: `HAVE_SDL2_NET`)

### Bonus System (GBonus_*.cpp/h)

15+ power-up types: bomb count, fire range, speed, shield, mega bomb, napalm bomb, kicker, punch, timer control, lives, illness effects.

### Monster System (GMrcha*.cpp/h)

15 monster types with 8 AI variants (GMrchaAI0-5, AIBlesk, AIBoss). Each has speed, animation, lives, score, AI type.

### Bomb Types

- `GBomba` - Regular bomb
- `GBombaMega` - Large blast radius
- `GBombaNapalm` - Persistent fire (1030 frames)

## Network Architecture

Host-authoritative P2P model (2 players max). Conditional compilation via `HAVE_SDL2_NET`.

**Key globals:** `g_network` (Network instance)

**Sync strategy:**
- Host authoritative for: bombs, monsters, bonuses, explosions
- Clients send input per frame
- Host broadcasts periodic game state

**Packet types (36):** CONNECT, ACCEPT, INPUT, GAME_STATE, BOMB_PLACED, BONUS_SPAWNED, MRCHA_STATE, etc.

## Important Globals

- `gspeed` - Game speed multiplier (1.0-4.0, default 1.5)
- `gmaxmrchovnik` - Max monsters on screen
- `g_config` - Persistent configuration
- `gfullscreen`, `glangfile`, `gsoundavailable`

## Configuration

Stored at `~/.config/bombic/bombic.conf` (INI format). Managed by `Config.cpp/h`.

## Data Files

```
data/
├── maps/          # Multiplayer maps (*.ble)
├── singlemaps/    # Campaign maps
├── samples/       # Sound effects (*.wav)
├── font/          # TTF fonts
├── *.txt          # Translations (cz-utf8.txt, en.txt, de-utf8.txt, pl-utf8.txt)
├── bomber.dat     # Character definitions
└── single.dat     # Campaign level data
```

## Build System

CMake 3.16+, C++11. Strict warnings: `-Wall -Wextra -Wpedantic`.

Network files only compiled when SDL2_net found:
- Network.cpp, MLAN.cpp, MLANHost.cpp, MLANJoin.cpp, MLANLobby.cpp, MLANPlaying.cpp

Config header generated: `cmake/config.h.in` → `config.h`
