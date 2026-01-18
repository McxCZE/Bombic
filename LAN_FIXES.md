# LAN Multiplayer Fixes

## Implemented Fixes

### Bug 1: Game end synchronization (CHYBY.md #1)
- **Problem**: When one player dies, the round ends only for them, not synchronized with the other player
- **Solution implemented**:
  1. Host is now authoritative for game end - only host can trigger round end
  2. When host's `EndGame()` returns true, it sends `NET_PACKET_ROUND_END` to client
  3. Client immediately ends its game when receiving this packet
  4. Random seed is now synchronized between both machines for deterministic random events

- **Remaining issue**: Map desync during gameplay (different destroyed walls) may still occur. This is because:
  - Input arrives at different frames on each machine due to network latency
  - Bombs are placed at different frames -> explode at different times
  - This causes different walls to be destroyed on each machine

  To fully fix this would require implementing one of:
  - **Lockstep simulation**: Both machines wait for each other's input before advancing each frame
  - **Input delay**: Buffer inputs and apply them after a fixed delay on both machines
  - **Rollback netcode**: Run ahead with predictions and rollback when actual input arrives

  These are significant architectural changes beyond simple bug fixes.

### Bug 2: Host disconnect (CHYBY.md #2)
- **Problem**: When host disconnects, client's game doesn't end
- **Solution implemented**:
  1. Added `m_disconnected` flag to track disconnection
  2. Auto-transition back to LAN menu when disconnection is detected
  3. Visual notification "Other player disconnected!" shown to user

### Additional fixes
- Client auto-enters lobby when connected (no extra ENTER press needed)
- LAN player always uses arrow keys (player on their own PC doesn't need WASD)
- Next round synchronization: only host can start next round, client waits for signal

## Files Modified
- `src/Network.h` - Added `NET_PACKET_ROUND_END`, `NET_PACKET_NEXT_ROUND` packets and round state tracking
- `src/Network.cpp` - Implemented `SendRoundEnd()`, `SendNextRound()` methods and packet handling
- `src/GGame.cpp` - Host-authoritative game end, random seed sync, client checks for round end packet
- `src/MLANPlaying.cpp/.h` - Round sync logic, disconnect handling, next round sync
- `src/MLANLobby.cpp/.h` - Disconnect auto-transition
- `src/MLANHost.cpp/.h` - Auto-transition to lobby on connection
- `src/MLANJoin.cpp/.h` - Auto-transition to lobby on connection
- `src/MMenu.cpp` - Auto-transition check in Move()

## Testing Required
The fixes need to be tested on two actual machines or using two instances with different ports. Test cases:
1. Host and client connect -> should auto-enter lobby
2. Host starts game -> both should see same initial state
3. One player dies -> both should see round end simultaneously
4. Host presses ENTER -> next round starts for both
5. Host quits (ESC or close) -> client returns to menu with notification
