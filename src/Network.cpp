// Network.cpp: implementation of the Network class.
// P2P LAN multiplayer networking for Bombic
//////////////////////////////////////////////////////////////////////

#include "Network.h"
#include <cstring>
#include <cstdio>

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
    , m_localFrame(0)
{
    memset(&m_remoteAddr, 0, sizeof(m_remoteAddr));
    memset(&m_gameStartInfo, 0, sizeof(m_gameStartInfo));
    memset(&m_mapInfo, 0, sizeof(m_mapInfo));
    memset(&m_roundEndInfo, 0, sizeof(m_roundEndInfo));
    memset(&m_gameState, 0, sizeof(m_gameState));
    memset(m_bombPlacedQueue, 0, sizeof(m_bombPlacedQueue));
    memset(m_bonusSpawnedQueue, 0, sizeof(m_bonusSpawnedQueue));
    memset(m_bombKickedQueue, 0, sizeof(m_bombKickedQueue));
    memset(m_bombDetonateQueue, 0, sizeof(m_bombDetonateQueue));
    memset(m_mrchaStateQueue, 0, sizeof(m_mrchaStateQueue));
    memset(m_illnessTransferQueue, 0, sizeof(m_illnessTransferQueue));
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

    // If client is connecting, resend connection request periodically
    if (m_state == NET_STATE_CONNECTING) {
        static Uint32 lastConnectTime = 0;
        Uint32 now = SDL_GetTicks();
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
                    SendPacket(&acceptPkt, sizeof(acceptPkt));

                    m_state = NET_STATE_CONNECTED;
                    m_remoteReady = false;
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
                m_state = NET_STATE_CONNECTED;
                m_remoteReady = false;
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
        // Could track latency here
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
            if (packet->len >= (int)sizeof(NetBombPlacedPacket) && m_bombPlacedCount < MAX_BOMB_QUEUE) {
                memcpy(&m_bombPlacedQueue[m_bombPlacedTail], packet->data, sizeof(NetBombPlacedPacket));
                m_bombPlacedTail = (m_bombPlacedTail + 1) % MAX_BOMB_QUEUE;
                m_bombPlacedCount++;
            }
        }
        break;

    case NET_PACKET_BONUS_SPAWNED:
        if (m_role == NET_ROLE_CLIENT && m_state == NET_STATE_IN_GAME) {
            if (packet->len >= (int)sizeof(NetBonusSpawnedPacket) && m_bonusSpawnedCount < MAX_BONUS_QUEUE) {
                memcpy(&m_bonusSpawnedQueue[m_bonusSpawnedTail], packet->data, sizeof(NetBonusSpawnedPacket));
                m_bonusSpawnedTail = (m_bonusSpawnedTail + 1) % MAX_BONUS_QUEUE;
                m_bonusSpawnedCount++;
            }
        }
        break;

    case NET_PACKET_BOMB_KICKED:
        if (m_role == NET_ROLE_CLIENT && m_state == NET_STATE_IN_GAME) {
            if (packet->len >= (int)sizeof(NetBombKickedPacket) && m_bombKickedCount < MAX_KICK_QUEUE) {
                memcpy(&m_bombKickedQueue[m_bombKickedTail], packet->data, sizeof(NetBombKickedPacket));
                m_bombKickedTail = (m_bombKickedTail + 1) % MAX_KICK_QUEUE;
                m_bombKickedCount++;
            }
        }
        break;

    case NET_PACKET_BOMB_DETONATE:
        if (m_role == NET_ROLE_CLIENT && m_state == NET_STATE_IN_GAME) {
            if (packet->len >= (int)sizeof(NetBombDetonatePacket) && m_bombDetonateCount < MAX_DETONATE_QUEUE) {
                memcpy(&m_bombDetonateQueue[m_bombDetonateTail], packet->data, sizeof(NetBombDetonatePacket));
                m_bombDetonateTail = (m_bombDetonateTail + 1) % MAX_DETONATE_QUEUE;
                m_bombDetonateCount++;
            }
        }
        break;

    case NET_PACKET_MRCHA_STATE:
        if (m_role == NET_ROLE_CLIENT && m_state == NET_STATE_IN_GAME) {
            if (packet->len >= (int)sizeof(NetMrchaStatePacket) && m_mrchaStateCount < MAX_MRCHA_QUEUE) {
                memcpy(&m_mrchaStateQueue[m_mrchaStateTail], packet->data, sizeof(NetMrchaStatePacket));
                m_mrchaStateTail = (m_mrchaStateTail + 1) % MAX_MRCHA_QUEUE;
                m_mrchaStateCount++;
            }
        }
        break;

    case NET_PACKET_ILLNESS_TRANSFER:
        if (m_role == NET_ROLE_CLIENT && m_state == NET_STATE_IN_GAME) {
            if (packet->len >= (int)sizeof(NetIllnessTransferPacket) && m_illnessTransferCount < MAX_ILLNESS_QUEUE) {
                memcpy(&m_illnessTransferQueue[m_illnessTransferTail], packet->data, sizeof(NetIllnessTransferPacket));
                m_illnessTransferTail = (m_illnessTransferTail + 1) % MAX_ILLNESS_QUEUE;
                m_illnessTransferCount++;
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

void Network::SendGameStart(const char* mapFile, int mapID, bool monsters, int bonuslevel, int victories)
{
    if (m_role != NET_ROLE_HOST || m_state < NET_STATE_CONNECTED) return;

    NetGameStartPacket packet;
    packet.type = NET_PACKET_GAME_START;
    packet.mapID = (uint8_t)mapID;
    packet.monsters = monsters ? 1 : 0;
    packet.bonuslevel = (uint8_t)bonuslevel;
    packet.victories = (uint8_t)victories;
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
                            int p0_lives, int p1_lives, int p0_abilities, int p1_abilities)
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

void Network::SendMrchaState(int mrchaID, int mx, int my, float x, float y, int dir, bool dead, int lives)
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
