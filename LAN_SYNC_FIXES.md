# LAN Multiplayer Synchronization Fixes

This document describes the synchronization issues that were identified and fixed in the LAN multiplayer implementation.

## Architecture Overview

The LAN implementation uses a host-authoritative model:
- The host is the source of truth for game state
- The client receives authoritative updates from the host
- Both machines process inputs and simulate locally, with periodic corrections from host

## Fixed Issues

### 1. Bomb Placement Desync
**Problem:** Both host and client were creating bombs locally based on input, leading to potential position mismatches.

**Fix:** Made bomb placement host-authoritative. Client no longer creates bombs directly - instead, it receives `NET_PACKET_BOMB_PLACED` packets from the host for all bombs (player bombs and deadmatch random bombs).

Files modified:
- `src/GBomber.cpp` - Client returns early from `PutBomb()`
- `src/GGame.cpp` - Client processes all bomb placement packets
- `src/Network.cpp/h` - Already had packet type, extended usage

### 2. Special Bomb Counter Double-Decrement
**Problem:** When client received mega/napalm bomb placement from host and called `AddMegaBomb()`/`AddNapalmBomb()`, it would decrement the special bomb counter even though host already did.

**Fix:** Added `fromNetwork` parameter to these functions. When `true`, the counter decrement is skipped.

Files modified:
- `src/GMap.h` - Added `fromNetwork` parameter
- `src/GMap.cpp` - Check `fromNetwork` before decrementing
- `src/GGame.cpp` - Pass `fromNetwork=true` when processing network packets

### 3. Bomb Kick Synchronization
**Problem:** When a player kicked a bomb, the direction change was not synchronized to the other machine.

**Fix:** Added `NET_PACKET_BOMB_KICKED` packet type. Host sends kick events to client.

Files modified:
- `src/Network.h` - Added packet type and structures
- `src/Network.cpp` - Added send/receive functions
- `src/GBomber.cpp` - Host sends kick packets, client ignores local kicks
- `src/GGame.cpp` - Client processes kick packets

### 4. Timer Bomb Detonation Synchronization
**Problem:** Timer bomb detonation (releasing action button with timer bonus) was not synchronized.

**Fix:** Added `NET_PACKET_BOMB_DETONATE` packet type. Host sends detonation events to client.

Files modified:
- `src/Network.h` - Added packet type and structures
- `src/Network.cpp` - Added send/receive functions
- `src/GBomber.cpp` - Host sends detonation packets
- `src/GGame.cpp` - Client processes detonation packets

### 5. Bomb Throw (Posilani) Synchronization
**Problem:** The "posilani" ability sets bomb direction on placement, but this wasn't sent to client.

**Fix:** Host sends `NET_PACKET_BOMB_KICKED` for thrown bombs.

Files modified:
- `src/GBomber.cpp` - Added kick packet send after setting throw direction

### 6. Extended Game State Sync
**Problem:** Several important player stats weren't being synchronized, including mega bombs, napalm bombs, lives, and abilities.

**Fix:** Extended `NetGameStatePacket` with additional fields and sync them every 5 frames.

Files modified:
- `src/Network.h` - Extended packet structure
- `src/Network.cpp` - Updated `SendGameState()` function
- `src/GGame.cpp` - Host sends extended state, client applies it

### 7. Kicker Illness Random Direction
**Problem:** The kicker illness causes bombs to move in random directions when placed, but this wasn't synchronized.

**Fix:** Host sends `NET_PACKET_BOMB_KICKED` for random directions from kicker illness.

Files modified:
- `src/GBonus_n_kicker.cpp` - Added network sync for random direction

### 8. Monster (Mrcha) State Synchronization
**Problem:** Monster AI uses random movement decisions that could drift between host and client machines, causing monsters to appear in different positions.

**Fix:** Made monster state host-authoritative. Added `NET_PACKET_MRCHA_STATE` packet type. Host sends all monster states (position, direction, death status, lives) to client every 5 frames.

Files modified:
- `src/Network.h` - Added `NetMrchaStatePacket` structure and packet type
- `src/Network.cpp` - Added `SendMrchaState()` and `PopMrchaState()` functions, queue handling
- `src/GGame.cpp` - Host sends monster states, client applies them (updating position maps correctly)

### 9. Illness Transfer Synchronization
**Problem:** Illness transfer between players depends on both players being on the same tile. With position drift between machines, illness could transfer on one machine but not the other.

**Fix:** Made illness transfer host-authoritative. Added `NET_PACKET_ILLNESS_TRANSFER` packet type. Host detects illness transfer events and sends them to client.

Files modified:
- `src/Network.h` - Added `NetIllnessTransferPacket` structure and packet type
- `src/Network.cpp` - Added `SendIllnessTransfer()` and `PopIllnessTransfer()` functions, queue handling
- `src/GBomber.cpp` - Host handles illness transfer and sends notification; client skips local illness logic
- `src/GGame.cpp` - Client processes illness transfer packets

### 10. Game Speed Adjustment Disabled in LAN
**Problem:** The +/- keys adjust game speed (`gspeed`), but this wasn't synchronized between machines, causing massive desync.

**Fix:** Disabled game speed adjustment in LAN mode.

Files modified:
- `src/GGame.cpp` - Speed adjustment keys disabled when `m_networkMode == GAME_MODE_LAN`

### 11. Pause Disabled in LAN
**Problem:** The 'P' key pauses the game locally, but this wasn't synchronized between machines, causing desync.

**Fix:** Disabled pause in LAN mode.

Files modified:
- `src/GGame.cpp` - Pause key disabled when `m_networkMode == GAME_MODE_LAN`

### 12. Megabomb Spawned Bombs Synchronization
**Problem:** When a mega bomb explodes, it spawns 8 additional bombs around it. These bombs were created locally on both machines, which could cause desync if bomb positions drifted.

**Fix:** Made megabomb spawned bombs host-authoritative. Client skips local bomb creation; receives bombs via `NET_PACKET_BOMB_PLACED` from host.

Files modified:
- `src/GMap.cpp` - Modified `ABOMB` macro to skip on client and send packets from host

### 13. AddNemoc Random Illness Type Synchronization
**Problem:** The `AddNemoc()` function (used by nemoc_ostatni bonus) uses `rand()` to pick illness type, which could differ between machines.

**Fix:** Made illness type selection host-authoritative. Client skips local creation; receives via `NET_PACKET_BONUS_SPAWNED`.

Files modified:
- `src/GMap.cpp` - Added LAN mode check to `AddNemoc()`

### 14. Timer Bomb Flag Synchronization
**Problem:** When a player with the timer bonus places a bomb, the bomb's timeout is set to 100000 (essentially infinite). This check happened locally based on `m_casovac`, which might not be synced yet.

**Fix:** Added `isTimerBomb` flag to `NetBombPlacedPacket`. Client applies the flag to set correct timeout.

Files modified:
- `src/Network.h` - Added `isTimerBomb` field to `NetBombPlacedPacket`
- `src/Network.cpp` - Updated `SendBombPlaced()` to include timer flag
- `src/GBomber.cpp` - Pass `m_casovac` when sending bomb placement
- `src/GGame.cpp` - Client applies timer flag to set bomb timeout

## Known Limitations

1. **Frame Timing** - Bomb explosions depend on local frame counting. Small timing differences are possible but mitigated by state sync.

## Testing Recommendations

1. Test with high-latency connections to verify position corrections work properly
2. Test all bonus types to ensure abilities sync correctly
3. Test deadmatch mode with random bombs
4. Test with monsters enabled to verify gameplay is acceptable
5. Test timer bombs and kicker ability
6. Test megabomb explosions
7. Test nemoc_ostatni bonus (infects other players with illness)
