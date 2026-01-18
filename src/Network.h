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
};

// Game start packet
struct NetGameStartPacket {
    uint8_t type;           // NET_PACKET_GAME_START
    uint8_t mapID;
    uint8_t monsters;
    uint8_t bonuslevel;
    uint8_t victories;
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
    void SendGameStart(const char* mapFile, int mapID, bool monsters, int bonuslevel, int victories);

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
                       int p0_bombdosah, int p1_bombdosah, int p0_bomb, int p1_bomb);

    // Get received input (returns true if new input available)
    bool GetRemoteInput(bool& left, bool& right, bool& up, bool& down, bool& action);

    // Game state sync (for client)
    bool HasGameStateUpdate() const { return m_gameStateUpdated; }
    void ClearGameStateUpdate() { m_gameStateUpdated = false; }
    const NetGameStatePacket& GetGameState() const { return m_gameState; }

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

    uint32_t m_localFrame;

    std::string m_lastError;
};

// Global network instance
extern Network g_network;

#endif // NETWORK_H_
