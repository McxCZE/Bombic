# LAN Multiplayer Bugs Analysis and Fixes

## Current Architecture
The LAN implementation uses a simple input-sync model where:
- Both machines run the game simulation independently
- Only player inputs are sent over the network
- Host sends periodic position corrections (every 5 frames)

## Identified Bugs

### Bug 1: Bombs Not Synchronized ✅ FIXED
**Problem:** When a player presses the action button to place a bomb, the bomb is created locally. If the input packet is lost or delayed, the bomb appears at different times on each machine. The game state sync doesn't include bomb positions.

**Symptoms:**
- Bombs appear on one screen but not the other
- Bomb explosions happen at different times
- Player can get killed by "invisible" bombs

**Fix:** Add bomb placement sync. When host detects a bomb placed, send a `NET_PACKET_BOMB_PLACED` with position, bomber ID, and bomb type (regular/mega/napalm).

**Implementation:** Added `NET_PACKET_BOMB_PLACED` packet type in Network.h/cpp. Host sends bomb placement info to client for deadmatch random bombs.

### Bug 2: Bonus Spawning Uses rand() - Desync ✅ FIXED
**Problem:** In `GMap.cpp:407`, bonus type is determined by `rand()%m_bonuslevel`. Even with seeded random at game start, any frame rate difference or timing issue will cause the RNG to desync, resulting in different bonuses appearing on different machines.

**Symptoms:**
- Player picks up "fire" bonus on one machine but sees "bomb" on the other
- Completely different bonuses appearing in different places

**Fix:** Host should be authoritative for bonuses. When a wall is destroyed (by bomb explosion), host determines the bonus type and sends `NET_PACKET_BONUS_SPAWNED` with position and bonus type to client. Client doesn't run its own bonus spawning logic in LAN mode.

**Implementation:** Added `NET_PACKET_BONUS_SPAWNED` packet type. GMap::AddBonus() now skips on client and host sends bonus type. Added GMap::AddBonusByType() for client-side spawning with explicit type.

### Bug 3: Deadmatch Random Bombs Use rand() ✅ FIXED
**Problem:** Line 184 in GGame.cpp:
```cpp
if (rand()%(26-m_game_time/DEADMATCH_MAX_TIME) == 0)
    m_map.AddBomb(-1, rand()%m_map.m_x, rand()%m_map.m_y, 9);
```
This uses `rand()` independently on each machine, causing desync.

**Symptoms:**
- Random "death rain" bombs appear in different places on each machine
- Players get killed by invisible bombs

**Fix:** Only host spawns deadmatch random bombs and sends their positions via `NET_PACKET_BOMB_PLACED`.

**Implementation:** GGame.cpp deadmatch bomb spawning now only runs on host and sends `NET_PACKET_BOMB_PLACED` to client. Client processes bomb queue and spawns bombs at received positions.

### Bug 4: Illness/Bonus Transfer Uses Local Logic ⚠️ PENDING
**Problem:** When players touch and transfer illnesses, this is computed locally on each machine. Timing differences can cause different outcomes.

**Fix:** Host should be authoritative for illness transfer. Send `NET_PACKET_ILLNESS_TRANSFER` when transfer occurs.

### Bug 5: Monster AI Uses rand() ⚠️ PENDING
**Problem:** `GMrchaAI0.cpp:46-47` uses `rand()` for monster direction changes:
```cpp
m_mrcha->m_dir = rand()%4 + 1;
m_tochange = 10 + rand()%5 + rand()%40;
```
This will desync if monsters are enabled in LAN.

**Fix:** For LAN mode with monsters, only host runs monster AI and syncs monster positions to client.

---

## Recommended Fix Priority

### High Priority (Game-Breaking)
1. **Bomb sync** - Add bomb placement/explosion packets
2. **Bonus spawning** - Make host authoritative

### Medium Priority (Noticeable Issues)
3. **Deadmatch random bombs** - Only host spawns
4. **Monster sync** - If monsters are enabled

### Low Priority (Edge Cases)
5. **Illness transfer sync**

---

## Implementation Approach

The simplest robust fix is to make the **host fully authoritative**:

1. Only the host runs game logic that uses `rand()`:
   - Bonus spawning
   - Deadmatch random bombs
   - Monster AI

2. Host sends events to client:
   - `NET_PACKET_BOMB_PLACED` (x, y, type, bomber_id)
   - `NET_PACKET_BOMB_EXPLODED` (bomb_id or x,y)
   - `NET_PACKET_BONUS_SPAWNED` (x, y, bonus_type)
   - `NET_PACKET_BONUS_COLLECTED` (x, y, player_id)

3. Client becomes "dumb" - just renders what host tells it, and sends inputs

This would require significant refactoring but would eliminate all desync issues.

---
