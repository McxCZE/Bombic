// Network.h: interface for the Network class.
// P2P LAN multiplayer networking for Bombic
//////////////////////////////////////////////////////////////////////

#ifndef NETWORK_H_
#define NETWORK_H_

#include <SDL_net.h>
#include <string>
#include <cstdint>

#define NET_DEFAULT_PORT 12345
#define NET_MAX_PACKET_SIZE 512
#define NET_PROTOCOL_VERSION 1

// Packet types
enum NetPacketType : uint8_t {
    NET_PACKET_CONNECT = 1,      // Client wants to connect
    NET_PACKET_ACCEPT = 2,       // Host accepts connection
    NET_PACKET_REJECT = 3,       // Host rejects connection
    NET_PACKET_DISCONNECT = 4,   // Player disconnecting
    NET_PACKET_INPUT = 5,        // Player input state
    NET_PACKET_GAME_START = 6,   // Host starting the game
    NET_PACKET_GAME_STATE = 7,   // Game state sync (host -> client)
    NET_PACKET_PING = 8,         // Ping request
    NET_PACKET_PONG = 9,         // Ping response
    NET_PACKET_READY = 10,       // Player ready in lobby
    NET_PACKET_MAP_INFO = 11,    // Map selection info
    NET_PACKET_ROUND_END = 12,   // Round ended, winner info
    NET_PACKET_NEXT_ROUND = 13,  // Start next round
    NET_PACKET_BOMB_PLACED = 14, // Bomb placed (host -> client)
    NET_PACKET_BONUS_SPAWNED = 15, // Bonus spawned (host -> client)
    NET_PACKET_BOMB_KICKED = 16, // Bomb kicked/pushed (host -> client)
    NET_PACKET_BOMB_DETONATE = 17, // Timer bomb detonation (host -> client)
    NET_PACKET_MRCHA_STATE = 18, // Monster state sync (host -> client)
    NET_PACKET_ILLNESS_TRANSFER = 19, // Illness transfer between players (host -> client)
    // Co-op story mode packets
    NET_PACKET_COOP_LEVEL_INFO = 20,   // Host sends level info to client
    NET_PACKET_COOP_LEVEL_START = 21,  // Host signals level gameplay start
    NET_PACKET_COOP_LEVEL_END = 22,    // Host sends level result
    NET_PACKET_COOP_MENU_STATE = 23,   // Host syncs screen state (intro/victory/defeat)
    NET_PACKET_COOP_LEVEL_TRANSITION = 24, // Combined level info + menu state (atomic transition)
    // Bonus synchronization packets (co-op)
    NET_PACKET_BONUS_PICKED = 25,      // Bonus picked up (host -> client)
    NET_PACKET_BONUS_DESTROYED = 26,   // Bonus destroyed by explosion (host -> client)
    // Player hit event (for immediate m_hitting sync)
    NET_PACKET_PLAYER_HIT = 27,        // Player hit event (host -> client)
};

// Bomb type enum for network sync
enum NetBombType : uint8_t {
    NET_BOMB_REGULAR = 0,
    NET_BOMB_MEGA = 1,
    NET_BOMB_NAPALM = 2
};

// Bomb placed packet (host -> client)
struct NetBombPlacedPacket {
    uint8_t type;           // NET_PACKET_BOMB_PLACED
    uint8_t bomberID;       // Who placed (-1 for deadmatch random)
    uint8_t bombType;       // NetBombType
    int8_t x;               // Map X position
    int8_t y;               // Map Y position
    uint8_t dosah;          // Bomb range
    uint8_t isTimerBomb;    // Whether bomber has timer ability (affects bomb timeout)
};

// Bonus spawned packet (host -> client)
struct NetBonusSpawnedPacket {
    uint8_t type;           // NET_PACKET_BONUS_SPAWNED
    int8_t x;               // Map X position
    int8_t y;               // Map Y position
    uint8_t bonusType;      // Bonus type ID (matches the switch case in AddBonus)
};

// Bonus picked up packet (host -> client) - synchronizes bonus pickup
struct NetBonusPickedPacket {
    uint8_t type;           // NET_PACKET_BONUS_PICKED
    int8_t x;               // Map X position where bonus was
    int8_t y;               // Map Y position where bonus was
    uint8_t pickerID;       // Bomber ID who picked up the bonus
};

// Bonus destroyed packet (host -> client) - synchronizes bonus destruction by explosion
struct NetBonusDestroyedPacket {
    uint8_t type;           // NET_PACKET_BONUS_DESTROYED
    int8_t x;               // Map X position
    int8_t y;               // Map Y position
};

// Bomb kicked/pushed packet (host -> client)
struct NetBombKickedPacket {
    uint8_t type;           // NET_PACKET_BOMB_KICKED
    int8_t x;               // Bomb map X position
    int8_t y;               // Bomb map Y position
    uint8_t dir;            // New direction (1=left, 2=right, 3=up, 4=down)
};

// Bomb detonate packet (host -> client) - for timer bombs
struct NetBombDetonatePacket {
    uint8_t type;           // NET_PACKET_BOMB_DETONATE
    uint8_t bomberID;       // Which player's bombs to detonate
};

// Monster (mrcha) state sync packet (host -> client)
struct NetMrchaStatePacket {
    uint8_t type;           // NET_PACKET_MRCHA_STATE
    uint8_t mrchaID;        // Monster index (0-255)
    int16_t mx;             // Map tile X
    int16_t my;             // Map tile Y
    int16_t x;              // Pixel offset X (fixed point * 100)
    int16_t y;              // Pixel offset Y (fixed point * 100)
    uint8_t dir;            // Direction (0-4)
    uint8_t dead;           // Is dead?
    uint8_t lives;          // Remaining lives
    uint8_t hitting;        // Is in hitting state (temporary invulnerability)
};

// Illness transfer packet (host -> client)
struct NetIllnessTransferPacket {
    uint8_t type;           // NET_PACKET_ILLNESS_TRANSFER
    uint8_t fromPlayer;     // Player who has the illness
    uint8_t toPlayer;       // Player receiving the illness
    uint8_t illnessType;    // Type of illness bonus (100+ range)
};

// Player hit event packet (host -> client) - immediate m_hitting sync
struct NetPlayerHitPacket {
    uint8_t type;           // NET_PACKET_PLAYER_HIT
    uint8_t playerID;       // Which player was hit (0 or 1)
    uint8_t hitting;        // 1 = now in hitting state (invulnerable), 0 = no longer hitting
    uint8_t lives;          // Current lives after hit
    uint8_t dead;           // Is dead?
};

// Co-op level info packet (host -> client)
struct NetCoopLevelInfoPacket {
    uint8_t type;           // NET_PACKET_COOP_LEVEL_INFO
    uint8_t level;          // Current level (0-39)
    uint8_t bonuslevel;     // Bonus level setting
    uint8_t needwon;        // Whether monsters must be killed
    uint8_t picturepre;     // Intro picture ID
    uint8_t picturepost;    // Victory picture ID
    uint8_t picturedead;    // Defeat picture ID
    char file[20];          // Map filename
    char code[8];           // Level code
};

// Co-op level start packet (host -> client)
struct NetCoopLevelStartPacket {
    uint8_t type;           // NET_PACKET_COOP_LEVEL_START
    uint8_t gspeed_x10;     // Game speed * 10
};

// Co-op level end packet (host -> client)
struct NetCoopLevelEndPacket {
    uint8_t type;           // NET_PACKET_COOP_LEVEL_END
    uint8_t victory;        // 1 = victory, 0 = defeat
};

// Co-op menu state packet (host -> client)
struct NetCoopMenuStatePacket {
    uint8_t type;           // NET_PACKET_COOP_MENU_STATE
    uint8_t menustate;      // 0=intro, 1=playing, 2=victory, 3=defeat
};

// Co-op level transition packet (host -> client) - combines level info + menu state
// This ensures atomic level transitions without race conditions from separate UDP packets
struct NetCoopLevelTransitionPacket {
    uint8_t type;           // NET_PACKET_COOP_LEVEL_TRANSITION
    uint8_t level;          // Current level (0-39)
    uint8_t bonuslevel;     // Bonus level setting
    uint8_t needwon;        // Whether monsters must be killed
    uint8_t picturepre;     // Intro picture ID
    uint8_t picturepost;    // Victory picture ID
    uint8_t picturedead;    // Defeat picture ID
    uint8_t menustate;      // 0=intro, 1=playing, 2=victory, 3=defeat
    // BUG #40 FIX: Added mrchovnik to sync max monster count between host and client
    // Without this, client may have different gmaxmrchovnik causing monster count desync
    uint8_t mrchovnik;      // Max monsters for this level
    char file[20];          // Map filename
    char code[8];           // Level code
};

// Input state packet (sent each frame)
struct NetInputPacket {
    uint8_t type;           // NET_PACKET_INPUT
    uint8_t playerID;       // Which player (0 or 1)
    uint32_t frame;         // Frame number
    uint8_t left : 1;
    uint8_t right : 1;
    uint8_t up : 1;
    uint8_t down : 1;
    uint8_t action : 1;
    uint8_t reserved : 3;
};

// Connection packet
struct NetConnectPacket {
    uint8_t type;           // NET_PACKET_CONNECT
    uint8_t protocolVersion;
    char playerName[32];
};

// Accept packet
struct NetAcceptPacket {
    uint8_t type;           // NET_PACKET_ACCEPT
    uint8_t assignedPlayerID;
    uint8_t gameMode;       // 0 = deathmatch, 1 = co-op
};

// Game start packet
struct NetGameStartPacket {
    uint8_t type;           // NET_PACKET_GAME_START
    uint8_t mapID;
    uint8_t monsters;
    uint8_t bonuslevel;
    uint8_t victories;
    uint8_t gspeed_x10;     // Game speed * 10 (10-40 for 1.0-4.0)
    char mapFile[64];
};

// Map info packet for lobby
struct NetMapInfoPacket {
    uint8_t type;           // NET_PACKET_MAP_INFO
    uint8_t mapID;
    uint8_t monsters;
    uint8_t bonuslevel;
    uint8_t victories;
    char mapFile[64];
};

// Ready packet
struct NetReadyPacket {
    uint8_t type;           // NET_PACKET_READY
    uint8_t ready;
};

// Round end packet
struct NetRoundEndPacket {
    uint8_t type;           // NET_PACKET_ROUND_END
    uint8_t winnerID;       // Which player won (0 or 1, or 255 for draw)
    uint8_t score0;         // Player 0 score
    uint8_t score1;         // Player 1 score
};

// Next round packet
struct NetNextRoundPacket {
    uint8_t type;           // NET_PACKET_NEXT_ROUND
};

// Game state sync packet (host -> client, periodic position correction)
struct NetGameStatePacket {
    uint8_t type;           // NET_PACKET_GAME_STATE
    // Player 0 position
    int16_t p0_mx;          // Map X tile
    int16_t p0_my;          // Map Y tile
    int16_t p0_x;           // Pixel offset X (fixed point * 100)
    int16_t p0_y;           // Pixel offset Y (fixed point * 100)
    uint8_t p0_dir;         // Direction
    uint8_t p0_dead;        // Is dead?
    // Player 1 position
    int16_t p1_mx;          // Map X tile
    int16_t p1_my;          // Map Y tile
    int16_t p1_x;           // Pixel offset X (fixed point * 100)
    int16_t p1_y;           // Pixel offset Y (fixed point * 100)
    uint8_t p1_dir;         // Direction
    uint8_t p1_dead;        // Is dead?
    // Bomber stats that affect gameplay
    uint8_t p0_bombdosah;   // Player 0 bomb range
    uint8_t p1_bombdosah;   // Player 1 bomb range
    uint8_t p0_bomb;        // Player 0 max bombs
    uint8_t p1_bomb;        // Player 1 max bombs
    // Bomb counters for sync (prevents bombused desync)
    uint8_t p0_bombused;    // Player 0 currently placed bombs
    uint8_t p1_bombused;    // Player 1 currently placed bombs
    // Additional stats for special abilities
    uint8_t p0_megabombs;   // Player 0 mega bombs count
    uint8_t p1_megabombs;   // Player 1 mega bombs count
    uint8_t p0_napalmbombs; // Player 0 napalm bombs count
    uint8_t p1_napalmbombs; // Player 1 napalm bombs count
    uint8_t p0_lives;       // Player 0 lives
    uint8_t p1_lives;       // Player 1 lives
    uint8_t p0_abilities;   // Player 0 abilities (bit flags: kopani, posilani, casovac, hitting)
    uint8_t p1_abilities;   // Player 1 abilities (bit flags: kopani, posilani, casovac, hitting)
    // Score sync
    uint16_t p0_score;      // Player 0 score
    uint16_t p1_score;      // Player 1 score
    // Illness sync
    int8_t p0_illness_type; // Player 0 illness type (-1 = none)
    int8_t p1_illness_type; // Player 1 illness type (-1 = none)
    uint16_t p0_illness_timer; // Player 0 illness remaining timer
    uint16_t p1_illness_timer; // Player 1 illness remaining timer
};

// Network role
enum NetRole {
    NET_ROLE_NONE = 0,
    NET_ROLE_HOST = 1,
    NET_ROLE_CLIENT = 2
};

// Connection state
enum NetState {
    NET_STATE_DISCONNECTED = 0,
    NET_STATE_LISTENING = 1,     // Host waiting for connection
    NET_STATE_CONNECTING = 2,    // Client trying to connect
    NET_STATE_CONNECTED = 3,     // Connected and ready
    NET_STATE_IN_GAME = 4        // In game
};

class Network {
public:
    Network();
    ~Network();

    // Initialize SDL_net
    bool Init();
    void Shutdown();

    // Host a game
    bool StartHost(int port = NET_DEFAULT_PORT);

    // Join a game
    bool StartClient(const char* hostIP, int port = NET_DEFAULT_PORT);

    // Disconnect
    void Disconnect();

    // Update - call each frame to handle network I/O
    void Update();

    // Send input state
    void SendInput(bool left, bool right, bool up, bool down, bool action);

    // Send game start (host only)
    void SendGameStart(const char* mapFile, int mapID, bool monsters, int bonuslevel, int victories, float gspeed);

    // Send map info update (host only, for lobby)
    void SendMapInfo(const char* mapFile, int mapID, bool monsters, int bonuslevel, int victories);

    // Send ready state
    void SendReady(bool ready);

    // Send round end (host only)
    void SendRoundEnd(int winnerID, int score0, int score1);

    // Send next round signal
    void SendNextRound();

    // Send game state (host only, periodic position sync)
    void SendGameState(int p0_mx, int p0_my, float p0_x, float p0_y, int p0_dir, bool p0_dead,
                       int p1_mx, int p1_my, float p1_x, float p1_y, int p1_dir, bool p1_dead,
                       int p0_bombdosah, int p1_bombdosah, int p0_bomb, int p1_bomb,
                       int p0_bombused, int p1_bombused,
                       int p0_megabombs, int p1_megabombs, int p0_napalmbombs, int p1_napalmbombs,
                       int p0_lives, int p1_lives, int p0_abilities, int p1_abilities,
                       int p0_score, int p1_score,
                       int p0_illness_type, int p0_illness_timer,
                       int p1_illness_type, int p1_illness_timer);

    // Get received input (returns true if new input available)
    bool GetRemoteInput(bool& left, bool& right, bool& up, bool& down, bool& action);

    // Send bomb placed (host only)
    void SendBombPlaced(int bomberID, int bombType, int x, int y, int dosah, bool isTimerBomb = false);

    // Send bonus spawned (host only)
    void SendBonusSpawned(int x, int y, int bonusType);

    // Send bomb kicked/pushed (host only)
    void SendBombKicked(int x, int y, int dir);

    // Send timer bomb detonate (host only)
    void SendBombDetonate(int bomberID);

    // Send monster state (host only)
    void SendMrchaState(int mrchaID, int mx, int my, float x, float y, int dir, bool dead, int lives, bool hitting);

    // Send illness transfer (host only)
    void SendIllnessTransfer(int fromPlayer, int toPlayer, int illnessType);

    // Send player hit event (host only) - immediate m_hitting sync
    void SendPlayerHit(int playerID, bool hitting, int lives, bool dead);

    // Game state sync (for client)
    bool HasGameStateUpdate() const { return m_gameStateUpdated; }
    void ClearGameStateUpdate() { m_gameStateUpdated = false; }
    const NetGameStatePacket& GetGameState() const { return m_gameState; }

    // Bomb placed notification (for client)
    bool HasBombPlaced() const { return m_bombPlacedCount > 0; }
    NetBombPlacedPacket PopBombPlaced();

    // Bonus spawned notification (for client)
    bool HasBonusSpawned() const { return m_bonusSpawnedCount > 0; }
    NetBonusSpawnedPacket PopBonusSpawned();

    // Send bonus picked (host only)
    void SendBonusPicked(int x, int y, int pickerID);

    // Send bonus destroyed (host only)
    void SendBonusDestroyed(int x, int y);

    // Bonus picked notification (for client)
    bool HasBonusPicked() const { return m_bonusPickedCount > 0; }
    NetBonusPickedPacket PopBonusPicked();

    // Bonus destroyed notification (for client)
    bool HasBonusDestroyed() const { return m_bonusDestroyedCount > 0; }
    NetBonusDestroyedPacket PopBonusDestroyed();

    // Bomb kicked notification (for client)
    bool HasBombKicked() const { return m_bombKickedCount > 0; }
    NetBombKickedPacket PopBombKicked();

    // Bomb detonate notification (for client)
    bool HasBombDetonate() const { return m_bombDetonateCount > 0; }
    NetBombDetonatePacket PopBombDetonate();

    // Monster state notification (for client)
    bool HasMrchaState() const { return m_mrchaStateCount > 0; }
    NetMrchaStatePacket PopMrchaState();

    // Illness transfer notification (for client)
    bool HasIllnessTransfer() const { return m_illnessTransferCount > 0; }
    NetIllnessTransferPacket PopIllnessTransfer();

    // Player hit notification (for client)
    bool HasPlayerHit() const { return m_playerHitCount > 0; }
    NetPlayerHitPacket PopPlayerHit();

    // Co-op story mode methods (host only)
    void SendCoopLevelInfo(int level, int bonuslevel, int needwon, int picturepre,
                           int picturepost, int picturedead, const char* file, const char* code);
    void SendCoopLevelStart(float gspeed);
    void SendCoopLevelEnd(bool victory);
    void SendCoopMenuState(int menustate);
    // Combined level transition (atomic - prevents race condition from separate UDP packets)
    // BUG #40 FIX: Added mrchovnik parameter for monster count sync
    void SendCoopLevelTransition(int level, int bonuslevel, int needwon, int picturepre,
                                  int picturepost, int picturedead, const char* file,
                                  const char* code, int menustate, int mrchovnik = 0);

    // Co-op story mode notifications (for client)
    bool HasCoopLevelInfo() const { return m_coopLevelInfoUpdated; }
    void ClearCoopLevelInfo() { m_coopLevelInfoUpdated = false; }
    const NetCoopLevelInfoPacket& GetCoopLevelInfo() const { return m_coopLevelInfo; }

    bool HasCoopLevelStart() const { return m_coopLevelStartSignal; }
    void ClearCoopLevelStart() { m_coopLevelStartSignal = false; }
    const NetCoopLevelStartPacket& GetCoopLevelStart() const { return m_coopLevelStartInfo; }

    bool HasCoopLevelEnd() const { return m_coopLevelEndSignal; }
    void ClearCoopLevelEnd() { m_coopLevelEndSignal = false; }
    const NetCoopLevelEndPacket& GetCoopLevelEnd() const { return m_coopLevelEndInfo; }

    bool HasCoopMenuState() const { return m_coopMenuStateUpdated; }
    void ClearCoopMenuState() { m_coopMenuStateUpdated = false; }
    const NetCoopMenuStatePacket& GetCoopMenuState() const { return m_coopMenuState; }

    bool HasCoopLevelTransition() const { return m_coopLevelTransitionUpdated; }
    void ClearCoopLevelTransition() { m_coopLevelTransitionUpdated = false; }
    const NetCoopLevelTransitionPacket& GetCoopLevelTransition() const { return m_coopLevelTransition; }

    // State queries
    NetRole GetRole() const { return m_role; }
    NetState GetState() const { return m_state; }
    bool IsConnected() const { return m_state >= NET_STATE_CONNECTED; }
    bool IsHost() const { return m_role == NET_ROLE_HOST; }
    bool IsClient() const { return m_role == NET_ROLE_CLIENT; }
    int GetLocalPlayerID() const { return m_localPlayerID; }
    int GetRemotePlayerID() const { return m_role == NET_ROLE_HOST ? 1 : 0; }
    bool IsRemoteReady() const { return m_remoteReady; }

    // Game start notification (for client)
    bool HasGameStarted() const { return m_gameStarted; }
    void ClearGameStarted() { m_gameStarted = false; }
    const NetGameStartPacket& GetGameStartInfo() const { return m_gameStartInfo; }

    // Map info (for client lobby)
    bool HasMapInfoUpdate() const { return m_mapInfoUpdated; }
    void ClearMapInfoUpdate() { m_mapInfoUpdated = false; }
    const NetMapInfoPacket& GetMapInfo() const { return m_mapInfo; }

    // Round end notification (for client)
    bool HasRoundEnded() const { return m_roundEnded; }
    void ClearRoundEnded() { m_roundEnded = false; }
    const NetRoundEndPacket& GetRoundEndInfo() const { return m_roundEndInfo; }

    // Next round notification (for client)
    bool HasNextRound() const { return m_nextRoundSignal; }
    void ClearNextRound() { m_nextRoundSignal = false; }

    // Error message
    const char* GetLastError() const { return m_lastError.c_str(); }

private:
    void HandlePacket(UDPpacket* packet);
    void SendPacket(void* data, int len);

    bool m_initialized;
    NetRole m_role;
    NetState m_state;
    int m_localPlayerID;

    UDPsocket m_socket;
    UDPpacket* m_packetIn;
    UDPpacket* m_packetOut;

    IPaddress m_remoteAddr;
    bool m_hasRemoteAddr;

    // Remote input buffer
    bool m_remoteInputAvailable;
    bool m_remoteLeft, m_remoteRight, m_remoteUp, m_remoteDown, m_remoteAction;
    uint32_t m_remoteFrame;

    // Remote ready state
    bool m_remoteReady;

    // Game start info (for client)
    bool m_gameStarted;
    NetGameStartPacket m_gameStartInfo;

    // Map info (for client lobby)
    bool m_mapInfoUpdated;
    NetMapInfoPacket m_mapInfo;

    // Round end info (for client)
    bool m_roundEnded;
    NetRoundEndPacket m_roundEndInfo;

    // Next round signal (for client)
    bool m_nextRoundSignal;

    // Game state sync (for client)
    bool m_gameStateUpdated;
    NetGameStatePacket m_gameState;

    // Bomb placed queue (for client) - circular buffer
    // BUG #66 FIX: Increased from 32 to 64 to handle heavy mega bombing
    // 3 mega bombs can generate 24+ bomb packets, 64 provides safety margin
    static const int MAX_BOMB_QUEUE = 64;
    NetBombPlacedPacket m_bombPlacedQueue[MAX_BOMB_QUEUE];
    int m_bombPlacedCount;
    int m_bombPlacedHead;
    int m_bombPlacedTail;

    // Bonus spawned queue (for client) - circular buffer
    static const int MAX_BONUS_QUEUE = 16;
    NetBonusSpawnedPacket m_bonusSpawnedQueue[MAX_BONUS_QUEUE];
    int m_bonusSpawnedCount;
    int m_bonusSpawnedHead;
    int m_bonusSpawnedTail;

    // Bonus picked queue (for client) - circular buffer
    static const int MAX_BONUS_PICKED_QUEUE = 16;
    NetBonusPickedPacket m_bonusPickedQueue[MAX_BONUS_PICKED_QUEUE];
    int m_bonusPickedCount;
    int m_bonusPickedHead;
    int m_bonusPickedTail;

    // Bonus destroyed queue (for client) - circular buffer
    // BUG #38 FIX: Increased from 16 to 64 to handle mega bomb explosions
    // Mega bombs can generate 8+ bomb placements each destroying multiple bonuses
    // 3 mega bombs near each other could generate 75+ bonus destroyed packets
    static const int MAX_BONUS_DESTROYED_QUEUE = 64;
    NetBonusDestroyedPacket m_bonusDestroyedQueue[MAX_BONUS_DESTROYED_QUEUE];
    int m_bonusDestroyedCount;
    int m_bonusDestroyedHead;
    int m_bonusDestroyedTail;

    // Bomb kicked queue (for client) - circular buffer
    static const int MAX_KICK_QUEUE = 16;
    NetBombKickedPacket m_bombKickedQueue[MAX_KICK_QUEUE];
    int m_bombKickedCount;
    int m_bombKickedHead;
    int m_bombKickedTail;

    // Bomb detonate queue (for client) - circular buffer
    static const int MAX_DETONATE_QUEUE = 8;
    NetBombDetonatePacket m_bombDetonateQueue[MAX_DETONATE_QUEUE];
    int m_bombDetonateCount;
    int m_bombDetonateHead;
    int m_bombDetonateTail;

    // Monster state queue (for client) - circular buffer
    // BUG #41 FIX: Increased from 128 to 512 to prevent overflow with high monster counts
    // Calculation: MAX_GMRCH=256 monsters × sync every 2 frames × latency buffer
    // With 100-300ms latency (3-7 frames), worst case: 256 * 7 / 2 = 896 packets pending
    // 512 provides good coverage for typical scenarios while avoiding excessive memory
    static const int MAX_MRCHA_QUEUE = 512;
    NetMrchaStatePacket m_mrchaStateQueue[MAX_MRCHA_QUEUE];
    int m_mrchaStateCount;
    int m_mrchaStateHead;
    int m_mrchaStateTail;

    // Illness transfer queue (for client) - circular buffer
    static const int MAX_ILLNESS_QUEUE = 8;
    NetIllnessTransferPacket m_illnessTransferQueue[MAX_ILLNESS_QUEUE];
    int m_illnessTransferCount;
    int m_illnessTransferHead;
    int m_illnessTransferTail;

    // Player hit queue (for client) - circular buffer
    static const int MAX_PLAYER_HIT_QUEUE = 8;
    NetPlayerHitPacket m_playerHitQueue[MAX_PLAYER_HIT_QUEUE];
    int m_playerHitCount;
    int m_playerHitHead;
    int m_playerHitTail;

    uint32_t m_localFrame;

    // Heartbeat tracking for disconnect detection
    uint32_t m_lastPingSent;
    uint32_t m_lastPongReceived;
    static const uint32_t HEARTBEAT_INTERVAL_MS = 2000;   // Send ping every 2 seconds
    static const uint32_t HEARTBEAT_TIMEOUT_MS = 10000;   // Consider disconnected after 10 seconds
    // BUG #64 FIX: Connection attempt timeout
    static const uint32_t CONNECT_TIMEOUT_MS = 30000;     // Give up connecting after 30 seconds

    // Co-op story mode data (for client)
    bool m_coopLevelInfoUpdated;
    NetCoopLevelInfoPacket m_coopLevelInfo;

    bool m_coopLevelStartSignal;
    NetCoopLevelStartPacket m_coopLevelStartInfo;

    bool m_coopLevelEndSignal;
    NetCoopLevelEndPacket m_coopLevelEndInfo;

    bool m_coopMenuStateUpdated;
    NetCoopMenuStatePacket m_coopMenuState;

    bool m_coopLevelTransitionUpdated;
    NetCoopLevelTransitionPacket m_coopLevelTransition;

    // BUG #37 FIX: Track last connect time as member variable instead of static
    // Static variable would persist across reconnects causing timing issues
    // NOTE: Placed here to match initialization order in constructor
    uint32_t m_lastConnectTime;
    // BUG #64 FIX: Track when we first started trying to connect for timeout
    uint32_t m_connectStartTime;

    std::string m_lastError;
};

// Global network instance
extern Network g_network;

#endif // NETWORK_H_
