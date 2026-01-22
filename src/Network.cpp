// Network.cpp: implementation of the Network class.
// P2P LAN multiplayer networking for Bombic
//////////////////////////////////////////////////////////////////////

#include "Network.h"
#include "MLAN.h"  // For g_coopMode
#include <cstring>
#include <cstdio>
#include <cstdarg>

// Simple logging for network issues - can be expanded to file logging if needed
static void NetLog(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "[Network] ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
}

// Global network instance
Network g_network;

Network::Network()
    : m_initialized(false)
    , m_role(NET_ROLE_NONE)
    , m_state(NET_STATE_DISCONNECTED)
    , m_localPlayerID(0)
    , m_socket(nullptr)
    , m_packetIn(nullptr)
    , m_packetOut(nullptr)
    , m_hasRemoteAddr(false)
    , m_remoteInputAvailable(false)
    , m_remoteLeft(false), m_remoteRight(false)
    , m_remoteUp(false), m_remoteDown(false), m_remoteAction(false)
    , m_remoteFrame(0)
    , m_remoteReady(false)
    , m_gameStarted(false)
    , m_mapInfoUpdated(false)
    , m_roundEnded(false)
    , m_nextRoundSignal(false)
    , m_gameStateUpdated(false)
    , m_bombPlacedCount(0)
    , m_bombPlacedHead(0)
    , m_bombPlacedTail(0)
    , m_bonusSpawnedCount(0)
    , m_bonusSpawnedHead(0)
    , m_bonusSpawnedTail(0)
    , m_bonusPickedCount(0)
    , m_bonusPickedHead(0)
    , m_bonusPickedTail(0)
    , m_bonusDestroyedCount(0)
    , m_bonusDestroyedHead(0)
    , m_bonusDestroyedTail(0)
    , m_bombKickedCount(0)
    , m_bombKickedHead(0)
    , m_bombKickedTail(0)
    , m_bombDetonateCount(0)
    , m_bombDetonateHead(0)
    , m_bombDetonateTail(0)
    , m_mrchaStateCount(0)
    , m_mrchaStateHead(0)
    , m_mrchaStateTail(0)
    , m_illnessTransferCount(0)
    , m_illnessTransferHead(0)
    , m_illnessTransferTail(0)
    , m_playerHitCount(0)
    , m_playerHitHead(0)
    , m_playerHitTail(0)
    , m_localFrame(0)
    , m_lastPingSent(0)
    , m_lastPongReceived(0)
    , m_coopLevelInfoUpdated(false)
    , m_coopLevelStartSignal(false)
    , m_coopLevelEndSignal(false)
    , m_coopMenuStateUpdated(false)
    , m_coopLevelTransitionUpdated(false)
{
    memset(&m_remoteAddr, 0, sizeof(m_remoteAddr));
    memset(&m_gameStartInfo, 0, sizeof(m_gameStartInfo));
    memset(&m_mapInfo, 0, sizeof(m_mapInfo));
    memset(&m_roundEndInfo, 0, sizeof(m_roundEndInfo));
    memset(&m_gameState, 0, sizeof(m_gameState));
    memset(m_bombPlacedQueue, 0, sizeof(m_bombPlacedQueue));
    memset(m_bonusSpawnedQueue, 0, sizeof(m_bonusSpawnedQueue));
    memset(m_bonusPickedQueue, 0, sizeof(m_bonusPickedQueue));
    memset(m_bonusDestroyedQueue, 0, sizeof(m_bonusDestroyedQueue));
    memset(m_bombKickedQueue, 0, sizeof(m_bombKickedQueue));
    memset(m_bombDetonateQueue, 0, sizeof(m_bombDetonateQueue));
    memset(m_mrchaStateQueue, 0, sizeof(m_mrchaStateQueue));
    memset(m_illnessTransferQueue, 0, sizeof(m_illnessTransferQueue));
    memset(m_playerHitQueue, 0, sizeof(m_playerHitQueue));
    memset(&m_coopLevelInfo, 0, sizeof(m_coopLevelInfo));
    memset(&m_coopLevelStartInfo, 0, sizeof(m_coopLevelStartInfo));
    memset(&m_coopLevelEndInfo, 0, sizeof(m_coopLevelEndInfo));
    memset(&m_coopMenuState, 0, sizeof(m_coopMenuState));
    memset(&m_coopLevelTransition, 0, sizeof(m_coopLevelTransition));
}

Network::~Network()
{
    Shutdown();
}

bool Network::Init()
{
    if (m_initialized) return true;

    if (SDLNet_Init() < 0) {
        m_lastError = "SDLNet_Init failed: ";
        m_lastError += SDLNet_GetError();
        return false;
    }

    m_packetIn = SDLNet_AllocPacket(NET_MAX_PACKET_SIZE);
    m_packetOut = SDLNet_AllocPacket(NET_MAX_PACKET_SIZE);

    if (!m_packetIn || !m_packetOut) {
        m_lastError = "Failed to allocate network packets";
        SDLNet_Quit();
        return false;
    }

    m_initialized = true;
    return true;
}

void Network::Shutdown()
{
    Disconnect();

    if (m_packetIn) {
        SDLNet_FreePacket(m_packetIn);
        m_packetIn = nullptr;
    }
    if (m_packetOut) {
        SDLNet_FreePacket(m_packetOut);
        m_packetOut = nullptr;
    }

    if (m_initialized) {
        SDLNet_Quit();
        m_initialized = false;
    }
}

bool Network::StartHost(int port)
{
    if (!m_initialized) {
        if (!Init()) return false;
    }

    Disconnect();

    m_socket = SDLNet_UDP_Open(port);
    if (!m_socket) {
        m_lastError = "Failed to open UDP port: ";
        m_lastError += SDLNet_GetError();
        return false;
    }

    m_role = NET_ROLE_HOST;
    m_state = NET_STATE_LISTENING;
    m_localPlayerID = 0;  // Host is always player 0
    m_hasRemoteAddr = false;
    m_remoteReady = false;
    m_gameStarted = false;
    m_localFrame = 0;

    return true;
}

bool Network::StartClient(const char* hostIP, int port)
{
    if (!m_initialized) {
        if (!Init()) return false;
    }

    Disconnect();

    // Open a socket on any available port
    m_socket = SDLNet_UDP_Open(0);
    if (!m_socket) {
        m_lastError = "Failed to open UDP socket: ";
        m_lastError += SDLNet_GetError();
        return false;
    }

    // Resolve host address
    if (SDLNet_ResolveHost(&m_remoteAddr, hostIP, port) < 0) {
        m_lastError = "Failed to resolve host: ";
        m_lastError += SDLNet_GetError();
        SDLNet_UDP_Close(m_socket);
        m_socket = nullptr;
        return false;
    }

    m_hasRemoteAddr = true;
    m_role = NET_ROLE_CLIENT;
    m_state = NET_STATE_CONNECTING;
    m_remoteReady = false;
    m_gameStarted = false;
    m_localFrame = 0;

    // Send connection request
    NetConnectPacket packet;
    packet.type = NET_PACKET_CONNECT;
    packet.protocolVersion = NET_PROTOCOL_VERSION;
    strncpy(packet.playerName, "Player", sizeof(packet.playerName) - 1);
    packet.playerName[sizeof(packet.playerName) - 1] = '\0';

    SendPacket(&packet, sizeof(packet));

    return true;
}

void Network::Disconnect()
{
    if (m_socket && m_hasRemoteAddr && m_state >= NET_STATE_CONNECTED) {
        uint8_t packet = NET_PACKET_DISCONNECT;
        SendPacket(&packet, 1);
    }

    if (m_socket) {
        SDLNet_UDP_Close(m_socket);
        m_socket = nullptr;
    }

    m_role = NET_ROLE_NONE;
    m_state = NET_STATE_DISCONNECTED;
    m_hasRemoteAddr = false;
    m_remoteInputAvailable = false;
    m_remoteReady = false;
    m_gameStarted = false;
    m_mapInfoUpdated = false;
}

void Network::Update()
{
    if (!m_socket) return;

    // Receive packets
    while (SDLNet_UDP_Recv(m_socket, m_packetIn) > 0) {
        HandlePacket(m_packetIn);
    }

    Uint32 now = SDL_GetTicks();

    // If client is connecting, resend connection request periodically
    if (m_state == NET_STATE_CONNECTING) {
        static Uint32 lastConnectTime = 0;
        if (now - lastConnectTime > 1000) {  // Resend every second
            lastConnectTime = now;
            NetConnectPacket packet;
            packet.type = NET_PACKET_CONNECT;
            packet.protocolVersion = NET_PROTOCOL_VERSION;
            strncpy(packet.playerName, "Player", sizeof(packet.playerName) - 1);
            packet.playerName[sizeof(packet.playerName) - 1] = '\0';
            SendPacket(&packet, sizeof(packet));
        }
    }

    // Heartbeat mechanism for IN_GAME and CONNECTED states
    // Keeps connection alive and detects disconnection
    if (m_state >= NET_STATE_CONNECTED && m_hasRemoteAddr) {
        // Send periodic pings
        if (now - m_lastPingSent > HEARTBEAT_INTERVAL_MS) {
            uint8_t ping = NET_PACKET_PING;
            SendPacket(&ping, 1);
            m_lastPingSent = now;
        }

        // Check for timeout (disconnection)
        if (m_lastPongReceived > 0 && now - m_lastPongReceived > HEARTBEAT_TIMEOUT_MS) {
            NetLog("Heartbeat timeout - remote player may have disconnected");
            m_lastError = "Connection timeout - remote player disconnected";
            m_state = NET_STATE_DISCONNECTED;
            m_hasRemoteAddr = false;
            m_remoteInputAvailable = false;
        }
    }
}

void Network::HandlePacket(UDPpacket* packet)
{
    if (packet->len < 1) return;

    uint8_t packetType = packet->data[0];

    switch (packetType) {
    case NET_PACKET_CONNECT:
        if (m_role == NET_ROLE_HOST && m_state == NET_STATE_LISTENING) {
            if (packet->len >= (int)sizeof(NetConnectPacket)) {
                NetConnectPacket* connectPkt = (NetConnectPacket*)packet->data;
                if (connectPkt->protocolVersion == NET_PROTOCOL_VERSION) {
                    // Accept connection
                    m_remoteAddr = packet->address;
                    m_hasRemoteAddr = true;

                    NetAcceptPacket acceptPkt;
                    acceptPkt.type = NET_PACKET_ACCEPT;
                    acceptPkt.assignedPlayerID = 1;  // Client is player 1
                    acceptPkt.gameMode = g_coopMode ? 1 : 0;  // Send game mode to client
                    SendPacket(&acceptPkt, sizeof(acceptPkt));

                    m_state = NET_STATE_CONNECTED;
                    m_remoteReady = false;
                    // Initialize heartbeat tracking
                    m_lastPingSent = SDL_GetTicks();
                    m_lastPongReceived = SDL_GetTicks();
                } else {
                    // Reject - wrong protocol version
                    uint8_t rejectPkt = NET_PACKET_REJECT;
                    m_remoteAddr = packet->address;
                    m_hasRemoteAddr = true;
                    SendPacket(&rejectPkt, 1);
                    m_hasRemoteAddr = false;
                }
            }
        }
        break;

    case NET_PACKET_ACCEPT:
        if (m_role == NET_ROLE_CLIENT && m_state == NET_STATE_CONNECTING) {
            if (packet->len >= (int)sizeof(NetAcceptPacket)) {
                NetAcceptPacket* acceptPkt = (NetAcceptPacket*)packet->data;
                m_localPlayerID = acceptPkt->assignedPlayerID;
                g_coopMode = (acceptPkt->gameMode == 1);  // Set game mode from host
                m_state = NET_STATE_CONNECTED;
                m_remoteReady = false;
                // Initialize heartbeat tracking
                m_lastPingSent = SDL_GetTicks();
                m_lastPongReceived = SDL_GetTicks();
            }
        }
        break;

    case NET_PACKET_REJECT:
        if (m_role == NET_ROLE_CLIENT && m_state == NET_STATE_CONNECTING) {
            m_lastError = "Connection rejected by host";
            Disconnect();
        }
        break;

    case NET_PACKET_DISCONNECT:
        if (m_state >= NET_STATE_CONNECTED) {
            m_lastError = "Remote player disconnected";
            m_state = NET_STATE_DISCONNECTED;
            m_hasRemoteAddr = false;
            m_remoteInputAvailable = false;
        }
        break;

    case NET_PACKET_INPUT:
        if (m_state >= NET_STATE_CONNECTED && packet->len >= (int)sizeof(NetInputPacket)) {
            NetInputPacket* inputPkt = (NetInputPacket*)packet->data;
            // Only accept input from the remote player
            if ((m_role == NET_ROLE_HOST && inputPkt->playerID == 1) ||
                (m_role == NET_ROLE_CLIENT && inputPkt->playerID == 0)) {
                m_remoteLeft = inputPkt->left;
                m_remoteRight = inputPkt->right;
                m_remoteUp = inputPkt->up;
                m_remoteDown = inputPkt->down;
                m_remoteAction = inputPkt->action;
                m_remoteFrame = inputPkt->frame;
                m_remoteInputAvailable = true;
            }
        }
        break;

    case NET_PACKET_GAME_START:
        if (m_role == NET_ROLE_CLIENT && m_state == NET_STATE_CONNECTED) {
            if (packet->len >= (int)sizeof(NetGameStartPacket)) {
                memcpy(&m_gameStartInfo, packet->data, sizeof(NetGameStartPacket));
                m_gameStarted = true;
                m_state = NET_STATE_IN_GAME;
            }
        }
        break;

    case NET_PACKET_MAP_INFO:
        if (m_role == NET_ROLE_CLIENT && m_state == NET_STATE_CONNECTED) {
            if (packet->len >= (int)sizeof(NetMapInfoPacket)) {
                memcpy(&m_mapInfo, packet->data, sizeof(NetMapInfoPacket));
                m_mapInfoUpdated = true;
            }
        }
        break;

    case NET_PACKET_READY:
        if (m_state == NET_STATE_CONNECTED && packet->len >= (int)sizeof(NetReadyPacket)) {
            NetReadyPacket* readyPkt = (NetReadyPacket*)packet->data;
            m_remoteReady = readyPkt->ready != 0;
        }
        break;

    case NET_PACKET_PING:
        {
            uint8_t pong = NET_PACKET_PONG;
            if (m_hasRemoteAddr) {
                IPaddress savedAddr = m_remoteAddr;
                m_remoteAddr = packet->address;
                SendPacket(&pong, 1);
                m_remoteAddr = savedAddr;
            }
        }
        break;

    case NET_PACKET_PONG:
        // Track last pong time for heartbeat timeout detection
        m_lastPongReceived = SDL_GetTicks();
        break;

    case NET_PACKET_ROUND_END:
        if (m_role == NET_ROLE_CLIENT && m_state == NET_STATE_IN_GAME) {
            if (packet->len >= (int)sizeof(NetRoundEndPacket)) {
                memcpy(&m_roundEndInfo, packet->data, sizeof(NetRoundEndPacket));
                m_roundEnded = true;
            }
        }
        break;

    case NET_PACKET_NEXT_ROUND:
        if (m_role == NET_ROLE_CLIENT && m_state == NET_STATE_IN_GAME) {
            m_nextRoundSignal = true;
        }
        break;

    case NET_PACKET_GAME_STATE:
        if (m_role == NET_ROLE_CLIENT && m_state == NET_STATE_IN_GAME) {
            if (packet->len >= (int)sizeof(NetGameStatePacket)) {
                memcpy(&m_gameState, packet->data, sizeof(NetGameStatePacket));
                m_gameStateUpdated = true;
            }
        }
        break;

    case NET_PACKET_BOMB_PLACED:
        if (m_role == NET_ROLE_CLIENT && m_state == NET_STATE_IN_GAME) {
            if (packet->len >= (int)sizeof(NetBombPlacedPacket)) {
                if (m_bombPlacedCount < MAX_BOMB_QUEUE) {
                    memcpy(&m_bombPlacedQueue[m_bombPlacedTail], packet->data, sizeof(NetBombPlacedPacket));
                    m_bombPlacedTail = (m_bombPlacedTail + 1) % MAX_BOMB_QUEUE;
                    m_bombPlacedCount++;
                } else {
                    NetLog("WARNING: Bomb placed queue full, dropping packet");
                }
            }
        }
        break;

    case NET_PACKET_BONUS_SPAWNED:
        if (m_role == NET_ROLE_CLIENT && m_state == NET_STATE_IN_GAME) {
            if (packet->len >= (int)sizeof(NetBonusSpawnedPacket)) {
                if (m_bonusSpawnedCount < MAX_BONUS_QUEUE) {
                    memcpy(&m_bonusSpawnedQueue[m_bonusSpawnedTail], packet->data, sizeof(NetBonusSpawnedPacket));
                    m_bonusSpawnedTail = (m_bonusSpawnedTail + 1) % MAX_BONUS_QUEUE;
                    m_bonusSpawnedCount++;
                } else {
                    NetLog("WARNING: Bonus spawned queue full, dropping packet");
                }
            }
        }
        break;

    case NET_PACKET_BONUS_PICKED:
        if (m_role == NET_ROLE_CLIENT && m_state == NET_STATE_IN_GAME) {
            if (packet->len >= (int)sizeof(NetBonusPickedPacket)) {
                if (m_bonusPickedCount < MAX_BONUS_PICKED_QUEUE) {
                    memcpy(&m_bonusPickedQueue[m_bonusPickedTail], packet->data, sizeof(NetBonusPickedPacket));
                    m_bonusPickedTail = (m_bonusPickedTail + 1) % MAX_BONUS_PICKED_QUEUE;
                    m_bonusPickedCount++;
                } else {
                    NetLog("WARNING: Bonus picked queue full, dropping packet");
                }
            }
        }
        break;

    case NET_PACKET_BONUS_DESTROYED:
        if (m_role == NET_ROLE_CLIENT && m_state == NET_STATE_IN_GAME) {
            if (packet->len >= (int)sizeof(NetBonusDestroyedPacket)) {
                if (m_bonusDestroyedCount < MAX_BONUS_DESTROYED_QUEUE) {
                    memcpy(&m_bonusDestroyedQueue[m_bonusDestroyedTail], packet->data, sizeof(NetBonusDestroyedPacket));
                    m_bonusDestroyedTail = (m_bonusDestroyedTail + 1) % MAX_BONUS_DESTROYED_QUEUE;
                    m_bonusDestroyedCount++;
                } else {
                    NetLog("WARNING: Bonus destroyed queue full, dropping packet");
                }
            }
        }
        break;

    case NET_PACKET_BOMB_KICKED:
        if (m_role == NET_ROLE_CLIENT && m_state == NET_STATE_IN_GAME) {
            if (packet->len >= (int)sizeof(NetBombKickedPacket)) {
                if (m_bombKickedCount < MAX_KICK_QUEUE) {
                    memcpy(&m_bombKickedQueue[m_bombKickedTail], packet->data, sizeof(NetBombKickedPacket));
                    m_bombKickedTail = (m_bombKickedTail + 1) % MAX_KICK_QUEUE;
                    m_bombKickedCount++;
                } else {
                    NetLog("WARNING: Bomb kicked queue full, dropping packet");
                }
            }
        }
        break;

    case NET_PACKET_BOMB_DETONATE:
        if (m_role == NET_ROLE_CLIENT && m_state == NET_STATE_IN_GAME) {
            if (packet->len >= (int)sizeof(NetBombDetonatePacket)) {
                if (m_bombDetonateCount < MAX_DETONATE_QUEUE) {
                    memcpy(&m_bombDetonateQueue[m_bombDetonateTail], packet->data, sizeof(NetBombDetonatePacket));
                    m_bombDetonateTail = (m_bombDetonateTail + 1) % MAX_DETONATE_QUEUE;
                    m_bombDetonateCount++;
                } else {
                    NetLog("WARNING: Bomb detonate queue full, dropping packet");
                }
            }
        }
        break;

    case NET_PACKET_MRCHA_STATE:
        if (m_role == NET_ROLE_CLIENT && m_state == NET_STATE_IN_GAME) {
            if (packet->len >= (int)sizeof(NetMrchaStatePacket)) {
                if (m_mrchaStateCount < MAX_MRCHA_QUEUE) {
                    memcpy(&m_mrchaStateQueue[m_mrchaStateTail], packet->data, sizeof(NetMrchaStatePacket));
                    m_mrchaStateTail = (m_mrchaStateTail + 1) % MAX_MRCHA_QUEUE;
                    m_mrchaStateCount++;
                } else {
                    NetLog("WARNING: Mrcha state queue full, dropping packet");
                }
            }
        }
        break;

    case NET_PACKET_ILLNESS_TRANSFER:
        if (m_role == NET_ROLE_CLIENT && m_state == NET_STATE_IN_GAME) {
            if (packet->len >= (int)sizeof(NetIllnessTransferPacket)) {
                if (m_illnessTransferCount < MAX_ILLNESS_QUEUE) {
                    memcpy(&m_illnessTransferQueue[m_illnessTransferTail], packet->data, sizeof(NetIllnessTransferPacket));
                    m_illnessTransferTail = (m_illnessTransferTail + 1) % MAX_ILLNESS_QUEUE;
                    m_illnessTransferCount++;
                } else {
                    NetLog("WARNING: Illness transfer queue full, dropping packet");
                }
            }
        }
        break;

    case NET_PACKET_PLAYER_HIT:
        if (m_role == NET_ROLE_CLIENT && m_state == NET_STATE_IN_GAME) {
            if (packet->len >= (int)sizeof(NetPlayerHitPacket)) {
                if (m_playerHitCount < MAX_PLAYER_HIT_QUEUE) {
                    memcpy(&m_playerHitQueue[m_playerHitTail], packet->data, sizeof(NetPlayerHitPacket));
                    m_playerHitTail = (m_playerHitTail + 1) % MAX_PLAYER_HIT_QUEUE;
                    m_playerHitCount++;
                } else {
                    NetLog("WARNING: Player hit queue full, dropping packet");
                }
            }
        }
        break;

    // Co-op story mode packets
    case NET_PACKET_COOP_LEVEL_INFO:
        if (m_role == NET_ROLE_CLIENT && m_state >= NET_STATE_CONNECTED) {
            if (packet->len >= (int)sizeof(NetCoopLevelInfoPacket)) {
                memcpy(&m_coopLevelInfo, packet->data, sizeof(NetCoopLevelInfoPacket));
                m_coopLevelInfoUpdated = true;
            }
        }
        break;

    case NET_PACKET_COOP_LEVEL_START:
        if (m_role == NET_ROLE_CLIENT && m_state >= NET_STATE_CONNECTED) {
            if (packet->len >= (int)sizeof(NetCoopLevelStartPacket)) {
                memcpy(&m_coopLevelStartInfo, packet->data, sizeof(NetCoopLevelStartPacket));
                m_coopLevelStartSignal = true;
                m_state = NET_STATE_IN_GAME;
            }
        }
        break;

    case NET_PACKET_COOP_LEVEL_END:
        if (m_role == NET_ROLE_CLIENT && m_state == NET_STATE_IN_GAME) {
            if (packet->len >= (int)sizeof(NetCoopLevelEndPacket)) {
                memcpy(&m_coopLevelEndInfo, packet->data, sizeof(NetCoopLevelEndPacket));
                m_coopLevelEndSignal = true;
            }
        }
        break;

    case NET_PACKET_COOP_MENU_STATE:
        if (m_role == NET_ROLE_CLIENT && m_state >= NET_STATE_CONNECTED) {
            if (packet->len >= (int)sizeof(NetCoopMenuStatePacket)) {
                memcpy(&m_coopMenuState, packet->data, sizeof(NetCoopMenuStatePacket));
                m_coopMenuStateUpdated = true;
            }
        }
        break;

    case NET_PACKET_COOP_LEVEL_TRANSITION:
        if (m_role == NET_ROLE_CLIENT && m_state >= NET_STATE_CONNECTED) {
            if (packet->len >= (int)sizeof(NetCoopLevelTransitionPacket)) {
                memcpy(&m_coopLevelTransition, packet->data, sizeof(NetCoopLevelTransitionPacket));
                m_coopLevelTransitionUpdated = true;
            }
        }
        break;
    }
}

void Network::SendPacket(void* data, int len)
{
    if (!m_socket || !m_hasRemoteAddr || len > NET_MAX_PACKET_SIZE) return;

    memcpy(m_packetOut->data, data, len);
    m_packetOut->len = len;
    m_packetOut->address = m_remoteAddr;

    SDLNet_UDP_Send(m_socket, -1, m_packetOut);
}

void Network::SendInput(bool left, bool right, bool up, bool down, bool action)
{
    if (m_state < NET_STATE_CONNECTED) return;

    NetInputPacket packet;
    packet.type = NET_PACKET_INPUT;
    packet.playerID = m_localPlayerID;
    packet.frame = m_localFrame++;
    packet.left = left ? 1 : 0;
    packet.right = right ? 1 : 0;
    packet.up = up ? 1 : 0;
    packet.down = down ? 1 : 0;
    packet.action = action ? 1 : 0;
    packet.reserved = 0;

    SendPacket(&packet, sizeof(packet));
}

void Network::SendGameStart(const char* mapFile, int mapID, bool monsters, int bonuslevel, int victories, float gspeed)
{
    if (m_role != NET_ROLE_HOST || m_state < NET_STATE_CONNECTED) return;

    NetGameStartPacket packet;
    packet.type = NET_PACKET_GAME_START;
    packet.mapID = (uint8_t)mapID;
    packet.monsters = monsters ? 1 : 0;
    packet.bonuslevel = (uint8_t)bonuslevel;
    packet.victories = (uint8_t)victories;
    packet.gspeed_x10 = (uint8_t)(gspeed * 10.0f);  // Store as 10-40 for 1.0-4.0
    strncpy(packet.mapFile, mapFile, sizeof(packet.mapFile) - 1);
    packet.mapFile[sizeof(packet.mapFile) - 1] = '\0';

    SendPacket(&packet, sizeof(packet));

    m_state = NET_STATE_IN_GAME;
}

void Network::SendMapInfo(const char* mapFile, int mapID, bool monsters, int bonuslevel, int victories)
{
    if (m_role != NET_ROLE_HOST || m_state < NET_STATE_CONNECTED) return;

    NetMapInfoPacket packet;
    packet.type = NET_PACKET_MAP_INFO;
    packet.mapID = (uint8_t)mapID;
    packet.monsters = monsters ? 1 : 0;
    packet.bonuslevel = (uint8_t)bonuslevel;
    packet.victories = (uint8_t)victories;
    strncpy(packet.mapFile, mapFile, sizeof(packet.mapFile) - 1);
    packet.mapFile[sizeof(packet.mapFile) - 1] = '\0';

    SendPacket(&packet, sizeof(packet));
}

void Network::SendReady(bool ready)
{
    if (m_state < NET_STATE_CONNECTED) return;

    NetReadyPacket packet;
    packet.type = NET_PACKET_READY;
    packet.ready = ready ? 1 : 0;

    SendPacket(&packet, sizeof(packet));
}

bool Network::GetRemoteInput(bool& left, bool& right, bool& up, bool& down, bool& action)
{
    // Always return the last known input state, even if no new packet arrived
    // This prevents "moonwalking" caused by UDP packet loss
    left = m_remoteLeft;
    right = m_remoteRight;
    up = m_remoteUp;
    down = m_remoteDown;
    action = m_remoteAction;

    // Return true if we've ever received input (connection is alive)
    return m_remoteInputAvailable;
}

void Network::SendRoundEnd(int winnerID, int score0, int score1)
{
    if (m_role != NET_ROLE_HOST || m_state < NET_STATE_IN_GAME) return;

    NetRoundEndPacket packet;
    packet.type = NET_PACKET_ROUND_END;
    packet.winnerID = (uint8_t)winnerID;
    packet.score0 = (uint8_t)score0;
    packet.score1 = (uint8_t)score1;

    SendPacket(&packet, sizeof(packet));
}

void Network::SendNextRound()
{
    if (m_state < NET_STATE_IN_GAME) return;

    NetNextRoundPacket packet;
    packet.type = NET_PACKET_NEXT_ROUND;

    SendPacket(&packet, sizeof(packet));
}

void Network::SendGameState(int p0_mx, int p0_my, float p0_x, float p0_y, int p0_dir, bool p0_dead,
                            int p1_mx, int p1_my, float p1_x, float p1_y, int p1_dir, bool p1_dead,
                            int p0_bombdosah, int p1_bombdosah, int p0_bomb, int p1_bomb,
                            int p0_megabombs, int p1_megabombs, int p0_napalmbombs, int p1_napalmbombs,
                            int p0_lives, int p1_lives, int p0_abilities, int p1_abilities,
                            int p0_score, int p1_score,
                            int p0_illness_type, int p0_illness_timer,
                            int p1_illness_type, int p1_illness_timer)
{
    if (m_role != NET_ROLE_HOST || m_state < NET_STATE_IN_GAME) return;

    NetGameStatePacket packet;
    packet.type = NET_PACKET_GAME_STATE;
    packet.p0_mx = (int16_t)p0_mx;
    packet.p0_my = (int16_t)p0_my;
    packet.p0_x = (int16_t)(p0_x * 100);  // Convert float to fixed point
    packet.p0_y = (int16_t)(p0_y * 100);
    packet.p0_dir = (uint8_t)p0_dir;
    packet.p0_dead = p0_dead ? 1 : 0;
    packet.p1_mx = (int16_t)p1_mx;
    packet.p1_my = (int16_t)p1_my;
    packet.p1_x = (int16_t)(p1_x * 100);
    packet.p1_y = (int16_t)(p1_y * 100);
    packet.p1_dir = (uint8_t)p1_dir;
    packet.p1_dead = p1_dead ? 1 : 0;
    packet.p0_bombdosah = (uint8_t)p0_bombdosah;
    packet.p1_bombdosah = (uint8_t)p1_bombdosah;
    packet.p0_bomb = (uint8_t)p0_bomb;
    packet.p1_bomb = (uint8_t)p1_bomb;
    packet.p0_megabombs = (uint8_t)p0_megabombs;
    packet.p1_megabombs = (uint8_t)p1_megabombs;
    packet.p0_napalmbombs = (uint8_t)p0_napalmbombs;
    packet.p1_napalmbombs = (uint8_t)p1_napalmbombs;
    packet.p0_lives = (uint8_t)p0_lives;
    packet.p1_lives = (uint8_t)p1_lives;
    packet.p0_abilities = (uint8_t)p0_abilities;
    packet.p1_abilities = (uint8_t)p1_abilities;
    packet.p0_score = (uint16_t)p0_score;
    packet.p1_score = (uint16_t)p1_score;
    packet.p0_illness_type = (int8_t)p0_illness_type;
    packet.p1_illness_type = (int8_t)p1_illness_type;
    packet.p0_illness_timer = (uint16_t)p0_illness_timer;
    packet.p1_illness_timer = (uint16_t)p1_illness_timer;

    SendPacket(&packet, sizeof(packet));
}

void Network::SendBombPlaced(int bomberID, int bombType, int x, int y, int dosah, bool isTimerBomb)
{
    if (m_role != NET_ROLE_HOST || m_state < NET_STATE_IN_GAME) return;

    NetBombPlacedPacket packet;
    packet.type = NET_PACKET_BOMB_PLACED;
    packet.bomberID = (uint8_t)bomberID;
    packet.bombType = (uint8_t)bombType;
    packet.x = (int8_t)x;
    packet.y = (int8_t)y;
    packet.dosah = (uint8_t)dosah;
    packet.isTimerBomb = isTimerBomb ? 1 : 0;

    SendPacket(&packet, sizeof(packet));
}

void Network::SendBonusSpawned(int x, int y, int bonusType)
{
    if (m_role != NET_ROLE_HOST || m_state < NET_STATE_IN_GAME) return;

    NetBonusSpawnedPacket packet;
    packet.type = NET_PACKET_BONUS_SPAWNED;
    packet.x = (int8_t)x;
    packet.y = (int8_t)y;
    packet.bonusType = (uint8_t)bonusType;

    SendPacket(&packet, sizeof(packet));
}

void Network::SendBonusPicked(int x, int y, int pickerID)
{
    if (m_role != NET_ROLE_HOST || m_state < NET_STATE_IN_GAME) return;

    NetBonusPickedPacket packet;
    packet.type = NET_PACKET_BONUS_PICKED;
    packet.x = (int8_t)x;
    packet.y = (int8_t)y;
    packet.pickerID = (uint8_t)pickerID;

    SendPacket(&packet, sizeof(packet));
}

void Network::SendBonusDestroyed(int x, int y)
{
    if (m_role != NET_ROLE_HOST || m_state < NET_STATE_IN_GAME) return;

    NetBonusDestroyedPacket packet;
    packet.type = NET_PACKET_BONUS_DESTROYED;
    packet.x = (int8_t)x;
    packet.y = (int8_t)y;

    SendPacket(&packet, sizeof(packet));
}

NetBombPlacedPacket Network::PopBombPlaced()
{
    NetBombPlacedPacket result = {};
    if (m_bombPlacedCount > 0) {
        result = m_bombPlacedQueue[m_bombPlacedHead];
        m_bombPlacedHead = (m_bombPlacedHead + 1) % MAX_BOMB_QUEUE;
        m_bombPlacedCount--;
    }
    return result;
}

NetBonusSpawnedPacket Network::PopBonusSpawned()
{
    NetBonusSpawnedPacket result = {};
    if (m_bonusSpawnedCount > 0) {
        result = m_bonusSpawnedQueue[m_bonusSpawnedHead];
        m_bonusSpawnedHead = (m_bonusSpawnedHead + 1) % MAX_BONUS_QUEUE;
        m_bonusSpawnedCount--;
    }
    return result;
}

NetBonusPickedPacket Network::PopBonusPicked()
{
    NetBonusPickedPacket result = {};
    if (m_bonusPickedCount > 0) {
        result = m_bonusPickedQueue[m_bonusPickedHead];
        m_bonusPickedHead = (m_bonusPickedHead + 1) % MAX_BONUS_PICKED_QUEUE;
        m_bonusPickedCount--;
    }
    return result;
}

NetBonusDestroyedPacket Network::PopBonusDestroyed()
{
    NetBonusDestroyedPacket result = {};
    if (m_bonusDestroyedCount > 0) {
        result = m_bonusDestroyedQueue[m_bonusDestroyedHead];
        m_bonusDestroyedHead = (m_bonusDestroyedHead + 1) % MAX_BONUS_DESTROYED_QUEUE;
        m_bonusDestroyedCount--;
    }
    return result;
}

void Network::SendBombKicked(int x, int y, int dir)
{
    if (m_role != NET_ROLE_HOST || m_state < NET_STATE_IN_GAME) return;

    NetBombKickedPacket packet;
    packet.type = NET_PACKET_BOMB_KICKED;
    packet.x = (int8_t)x;
    packet.y = (int8_t)y;
    packet.dir = (uint8_t)dir;

    SendPacket(&packet, sizeof(packet));
}

void Network::SendBombDetonate(int bomberID)
{
    if (m_role != NET_ROLE_HOST || m_state < NET_STATE_IN_GAME) return;

    NetBombDetonatePacket packet;
    packet.type = NET_PACKET_BOMB_DETONATE;
    packet.bomberID = (uint8_t)bomberID;

    SendPacket(&packet, sizeof(packet));
}

NetBombKickedPacket Network::PopBombKicked()
{
    NetBombKickedPacket result = {};
    if (m_bombKickedCount > 0) {
        result = m_bombKickedQueue[m_bombKickedHead];
        m_bombKickedHead = (m_bombKickedHead + 1) % MAX_KICK_QUEUE;
        m_bombKickedCount--;
    }
    return result;
}

NetBombDetonatePacket Network::PopBombDetonate()
{
    NetBombDetonatePacket result = {};
    if (m_bombDetonateCount > 0) {
        result = m_bombDetonateQueue[m_bombDetonateHead];
        m_bombDetonateHead = (m_bombDetonateHead + 1) % MAX_DETONATE_QUEUE;
        m_bombDetonateCount--;
    }
    return result;
}

void Network::SendMrchaState(int mrchaID, int mx, int my, float x, float y, int dir, bool dead, int lives, bool hitting)
{
    if (m_role != NET_ROLE_HOST || m_state < NET_STATE_IN_GAME) return;

    NetMrchaStatePacket packet;
    packet.type = NET_PACKET_MRCHA_STATE;
    packet.mrchaID = (uint8_t)mrchaID;
    packet.mx = (int16_t)mx;
    packet.my = (int16_t)my;
    packet.x = (int16_t)(x * 100);  // Convert float to fixed point
    packet.y = (int16_t)(y * 100);
    packet.dir = (uint8_t)dir;
    packet.dead = dead ? 1 : 0;
    packet.lives = (uint8_t)lives;
    packet.hitting = hitting ? 1 : 0;

    SendPacket(&packet, sizeof(packet));
}

NetMrchaStatePacket Network::PopMrchaState()
{
    NetMrchaStatePacket result = {};
    if (m_mrchaStateCount > 0) {
        result = m_mrchaStateQueue[m_mrchaStateHead];
        m_mrchaStateHead = (m_mrchaStateHead + 1) % MAX_MRCHA_QUEUE;
        m_mrchaStateCount--;
    }
    return result;
}

void Network::SendIllnessTransfer(int fromPlayer, int toPlayer, int illnessType)
{
    if (m_role != NET_ROLE_HOST || m_state < NET_STATE_IN_GAME) return;

    NetIllnessTransferPacket packet;
    packet.type = NET_PACKET_ILLNESS_TRANSFER;
    packet.fromPlayer = (uint8_t)fromPlayer;
    packet.toPlayer = (uint8_t)toPlayer;
    packet.illnessType = (uint8_t)illnessType;

    SendPacket(&packet, sizeof(packet));
}

NetIllnessTransferPacket Network::PopIllnessTransfer()
{
    NetIllnessTransferPacket result = {};
    if (m_illnessTransferCount > 0) {
        result = m_illnessTransferQueue[m_illnessTransferHead];
        m_illnessTransferHead = (m_illnessTransferHead + 1) % MAX_ILLNESS_QUEUE;
        m_illnessTransferCount--;
    }
    return result;
}

void Network::SendPlayerHit(int playerID, bool hitting, int lives, bool dead)
{
    if (m_role != NET_ROLE_HOST || m_state < NET_STATE_IN_GAME) return;

    NetPlayerHitPacket packet;
    packet.type = NET_PACKET_PLAYER_HIT;
    packet.playerID = (uint8_t)playerID;
    packet.hitting = hitting ? 1 : 0;
    packet.lives = (uint8_t)lives;
    packet.dead = dead ? 1 : 0;

    SendPacket(&packet, sizeof(packet));
}

NetPlayerHitPacket Network::PopPlayerHit()
{
    NetPlayerHitPacket result = {};
    if (m_playerHitCount > 0) {
        result = m_playerHitQueue[m_playerHitHead];
        m_playerHitHead = (m_playerHitHead + 1) % MAX_PLAYER_HIT_QUEUE;
        m_playerHitCount--;
    }
    return result;
}

// Co-op story mode methods

void Network::SendCoopLevelInfo(int level, int bonuslevel, int needwon, int picturepre,
                                 int picturepost, int picturedead, const char* file, const char* code)
{
    if (m_role != NET_ROLE_HOST || m_state < NET_STATE_CONNECTED) return;

    NetCoopLevelInfoPacket packet;
    packet.type = NET_PACKET_COOP_LEVEL_INFO;
    packet.level = (uint8_t)level;
    packet.bonuslevel = (uint8_t)bonuslevel;
    packet.needwon = (uint8_t)needwon;
    packet.picturepre = (uint8_t)picturepre;
    packet.picturepost = (uint8_t)picturepost;
    packet.picturedead = (uint8_t)picturedead;
    strncpy(packet.file, file, sizeof(packet.file) - 1);
    packet.file[sizeof(packet.file) - 1] = '\0';
    strncpy(packet.code, code, sizeof(packet.code) - 1);
    packet.code[sizeof(packet.code) - 1] = '\0';

    SendPacket(&packet, sizeof(packet));
}

void Network::SendCoopLevelStart(float gspeed)
{
    if (m_role != NET_ROLE_HOST || m_state < NET_STATE_CONNECTED) return;

    NetCoopLevelStartPacket packet;
    packet.type = NET_PACKET_COOP_LEVEL_START;
    packet.gspeed_x10 = (uint8_t)(gspeed * 10.0f);

    // Send multiple times to improve reliability over UDP
    // This critical packet starts level gameplay - packet loss would cause desync
    for (int i = 0; i < 3; i++) {
        SendPacket(&packet, sizeof(packet));
    }

    m_state = NET_STATE_IN_GAME;
}

void Network::SendCoopLevelEnd(bool victory)
{
    if (m_role != NET_ROLE_HOST || m_state < NET_STATE_IN_GAME) return;

    NetCoopLevelEndPacket packet;
    packet.type = NET_PACKET_COOP_LEVEL_END;
    packet.victory = victory ? 1 : 0;

    // Send multiple times to improve reliability over UDP
    // This critical packet determines level end sync - packet loss would cause desync
    for (int i = 0; i < 3; i++) {
        SendPacket(&packet, sizeof(packet));
    }
}

void Network::SendCoopMenuState(int menustate)
{
    if (m_role != NET_ROLE_HOST || m_state < NET_STATE_CONNECTED) return;

    NetCoopMenuStatePacket packet;
    packet.type = NET_PACKET_COOP_MENU_STATE;
    packet.menustate = (uint8_t)menustate;

    SendPacket(&packet, sizeof(packet));
}

void Network::SendCoopLevelTransition(int level, int bonuslevel, int needwon, int picturepre,
                                       int picturepost, int picturedead, const char* file,
                                       const char* code, int menustate)
{
    if (m_role != NET_ROLE_HOST || m_state < NET_STATE_CONNECTED) return;

    NetCoopLevelTransitionPacket packet;
    packet.type = NET_PACKET_COOP_LEVEL_TRANSITION;
    packet.level = (uint8_t)level;
    packet.bonuslevel = (uint8_t)bonuslevel;
    packet.needwon = (uint8_t)needwon;
    packet.picturepre = (uint8_t)picturepre;
    packet.picturepost = (uint8_t)picturepost;
    packet.picturedead = (uint8_t)picturedead;
    packet.menustate = (uint8_t)menustate;
    strncpy(packet.file, file, sizeof(packet.file) - 1);
    packet.file[sizeof(packet.file) - 1] = '\0';
    strncpy(packet.code, code, sizeof(packet.code) - 1);
    packet.code[sizeof(packet.code) - 1] = '\0';

    // Send multiple times to improve reliability over UDP
    // This critical packet handles level transitions - packet loss would cause desync
    for (int i = 0; i < 3; i++) {
        SendPacket(&packet, sizeof(packet));
    }
}
