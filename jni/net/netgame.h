#pragma once

#define NETGAME_VERSION 4057

#define GAMESTATE_NONE			0
#define GAMESTATE_CONNECTING	1
#define GAMESTATE_CONNECTED		2
#define GAMESTATE_AWAIT_JOIN	3
#define GAMESTATE_DISCONNECTED  4
#define GAMESTATE_RESTARTING	5
#define GAMESTATE_WAIT_CONNECT	6

#define NETMODE_NORMAL_ONFOOT_SENDRATE	30
#define NETMODE_NORMAL_INCAR_SENDRATE	30
#define NETMODE_FIRING_SENDRATE			30
#define NETMODE_SEND_MULTIPLIER			2

class CNetGame
{
public:
	CNetGame(char* szHostOrIp, int iPort, char* szPlayerName, char* szPass,char*pName);
	~CNetGame();

	inline int GetGameState() { return m_iGameState; };
	inline void SetGameState(int iGameState) { m_iGameState = iGameState; };

	CPlayerPool 		*GetPlayerPool() 	{ return m_pPlayerPool; 	};
	CVehiclePool 		*GetVehiclePool() 	{ return m_pVehiclePool; 	};
	CObjectPool			*GetObjectPool()	{ return m_pObjectPool;		};
	CPickupPool			*GetPickupPool()	{ return m_pPickupPool;		};
	CGangZonePool 		*GetGangZonePool() 	{ return m_pGangZonePool; 	};
	
	RakClientInterface 	*GetRakClient() 	{ return m_pRakClient; 		};

	void Process();

	void ResetVehiclePool();
	void ShutdownForGameModeRestart();

	void SetMapIcon(uint8_t byteIndex, float fX, float fY, float fZ, uint8_t byteIcon, uint32_t dwCOlor);
	void DisableMapIcon(uint8_t byteIndex);

	int			m_iSpawnsAvailable;
	int			m_iShowPlayerMarkers;
	bool		m_bShowPlayerTags;
	uint8_t		m_byteWorldTime;
	uint8_t		m_byteWorldMinute;
	uint8_t		m_byteWeather;
	float		m_WorldBounds[4];
	bool		m_bAllowWeapons;
	bool		m_bNameTagLOS;
	float		m_fGravity;
	int			m_iDeathDropMoney;
	uint8_t 	m_byteHoldTime;
	bool		m_bInstagib;
	bool		m_bZoneNames;
	bool		m_bLimitGlobalChatRadius;
	bool		m_bUseCJWalk;
	float		m_fGlobalChatRadius;
	float		m_fNameTagDrawDistance;
	bool		m_bDisableEnterExits;
	uint32_t	m_dwMapIcon[32];

	// new
	bool		m_bManualVehicleEngineAndLight;
	uint8_t		m_bLagCompensation;

	char 		m_szHostName[0xFF];
	char 		m_szHostOrIp[0x80];
	int 		m_iPort;

private:
	RakClientInterface *m_pRakClient;

	CPlayerPool			*m_pPlayerPool;
	CVehiclePool		*m_pVehiclePool;
	CObjectPool			*m_pObjectPool;
	CPickupPool			*m_pPickupPool;
	CGangZonePool		*m_pGangZonePool;

	int					m_iGameState;
	uint32_t			m_dwLastConnectAttempt;

	void UpdateNetwork();

	// Packet handlers
	void Packet_AuthKey(Packet *pkt);\
	void Packet_MarkerSync(Packet *pkt);
	void Packet_PlayerSync(Packet *pkt);
	void Packet_VehicleSync(Packet *pkt);
	void Packet_PassengerSync(Packet *pkt);
	void Packet_ConnectionSucceeded(Packet *pkt);
	// ----
	void Packet_ConnectionBanned(Packet* packet);
	void Packet_NoFreeIncomingConnections(Packet* packet);
	void Packet_DisconnectionNotification(Packet* packet);
	void Packet_ConnectionLost(Packet* packet);
	void Packet_InvalidPassword(Packet* packet);
	void Packet_ConnectAttemptFailed(Packet* packet);
};