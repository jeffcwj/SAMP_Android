#include "main.h"
#include "../game/util.h"

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CChatWindow *pChatWindow;
#define AUTH_BS "1031CA8429843C9B8C178B65F3C73602578440D17F8"

int iVehiclePoolProcessFlag = 0;
int iPickupPoolProcessFlag = 0;
//最后一个参数用于获取设置的玩家名
CNetGame::CNetGame(char *szHostOrIp, int iPort, char* szPlayerName, char *szPass,char* pName)
{
	strcpy(m_szHostName, "San Andreas Multiplayer");
	strncpy(m_szHostOrIp, szHostOrIp, sizeof(m_szHostOrIp));
	m_iPort = iPort;

	m_pPlayerPool = new CPlayerPool();
	m_pPlayerPool->SetLocalPlayerName(szPlayerName);

	m_pVehiclePool 	= new CVehiclePool();
	m_pObjectPool	= new CObjectPool();
	m_pPickupPool	= new CPickupPool();
	m_pGangZonePool = new CGangZonePool();

	m_pRakClient = RakNetworkFactory::GetRakClientInterface();

	RegisterRPCs(m_pRakClient);
	RegisterScriptRPCs(m_pRakClient);
	m_pRakClient->SetPassword(szPass);

	m_pRakClient->SetMTUSize(1492);

	m_dwLastConnectAttempt = GetTickCount();
	m_iGameState = GAMESTATE_WAIT_CONNECT;

	m_byteWorldTime = 12;
	m_byteWorldMinute = 0;
	m_byteWeather = 10;
	m_byteHoldTime = 1;

	for(int i = 0; i < 32; i++) m_dwMapIcon[i] = 0;

	pGame->EnableClock(false);
	pGame->EnableZoneNames(false);

	if(pChatWindow) 
		pChatWindow->AddDebugMessageNoGBK("{FFFFFF}SA-MP {B9C9BF}0.3.7.{FF00FF}47 {FFFFFF}Started");
		pChatWindow->AddDebugMessageNoGBK("{66ccff}版本号:v1808.6");
//		pChatWindow->AddDebugMessageNoGBK("{66ccff}");
		pChatWindow->AddDebugMessageNoGBK("{66ccff}by qq1198");
		pChatWindow->AddDebugMessageNoGBK("%s",pName);
		printf("By qq1198");
		printf("qq1198075593");
}

CNetGame::~CNetGame()
{

}

void CNetGame::ShutdownForGameModeRestart()
{
	m_byteWorldTime = 12;
	m_byteWorldMinute = 0;
	m_byteWeather = 10;
	m_byteHoldTime = 1;
	pGame->EnableClock(1);

	for (PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++)
	{
		CRemotePlayer *pPlayer = m_pPlayerPool->GetAt(playerId);
		if(pPlayer)
		{
			pPlayer->ResetAllSyncAttributes();
		}
	}

	CLocalPlayer *pLocalPlayer = m_pPlayerPool->GetLocalPlayer();
	if(pLocalPlayer)
	{
		pLocalPlayer->ResetAllSyncAttributes();
	}

	m_iGameState = GAMESTATE_RESTARTING;

	m_pPlayerPool->DeactivateAll();
	m_pPlayerPool->Process();

	ResetVehiclePool();

	pGame->EnableZoneNames(false);

	GameResetRadarColors();
}

void CNetGame::Process()
{
	UpdateNetwork();
		pGame->SetWorldTime(m_byteWorldTime, m_byteWorldMinute);

	if(m_byteHoldTime) 
		pGame->SetWorldTime(m_byteWorldTime, m_byteWorldMinute);

	pGame->SetWorldWeather(m_byteWeather);

	if(GetGameState() == GAMESTATE_CONNECTED)
	{
		if(m_pPlayerPool) m_pPlayerPool->Process();

		if(m_pVehiclePool && iVehiclePoolProcessFlag > 5)
		{
			m_pVehiclePool->Process();
			iVehiclePoolProcessFlag = 0;
		}
		else
			iVehiclePoolProcessFlag++;

		if(m_pPickupPool && iPickupPoolProcessFlag > 5)
		{
			m_pPickupPool->Process();
			iPickupPoolProcessFlag = 0;
		}
		else
			++iPickupPoolProcessFlag;

	}
	else
	{
		CPlayerPed *pPlayer = pGame->FindPlayerPed();
		CCamera *pCamera = pGame->GetCamera();

		if(pPlayer && pCamera)
		{
			if(pPlayer->IsInVehicle())
				pPlayer->RemoveFromVehicleAndPutAt(1133.05f, -2038.40f, 69.10f);
			else
				pPlayer->TeleportTo(1133.05f, -2038.40f, 69.10f);

			pCamera->SetPosition(1093.0f, -2036.0f, 90.0f, 0.0f, 0.0f, 0.0f);
            pCamera->LookAtPoint(384.0f, -1557.0f, 20.0f, 2);
			pGame->DisplayHUD(false);
		}
	}

	if(GetGameState() == GAMESTATE_WAIT_CONNECT && (GetTickCount() - m_dwLastConnectAttempt) > 3000)
	{
		LOGI("Connecting to %s:%d...", m_szHostOrIp, m_iPort);
		if (pChatWindow) pChatWindow->AddDebugMessageNoGBK("正在连接至 %s:%d...", m_szHostOrIp, m_iPort);
		m_pRakClient->Connect(m_szHostOrIp, m_iPort, 0, 0, 5);
		m_dwLastConnectAttempt = GetTickCount();
	}
}

void CNetGame::UpdateNetwork()
{
	unsigned char packetIdentifier;
	Packet *pkt;

	while(pkt = m_pRakClient->Receive())
	{
		if ( ( unsigned char ) pkt->data[ 0 ] == ID_TIMESTAMP )
		{
			if ( pkt->length > sizeof( unsigned char ) + sizeof( unsigned int ) )
				packetIdentifier = ( unsigned char ) pkt->data[ sizeof( unsigned char ) + sizeof( unsigned int ) ];
			else
				return;
		}
		else
			packetIdentifier = ( unsigned char ) pkt->data[ 0 ];

		switch(packetIdentifier)
		{

			case ID_DISCONNECTION_NOTIFICATION:
			Packet_DisconnectionNotification(pkt);
			break;

			case ID_CONNECTION_BANNED:
			Packet_ConnectionBanned(pkt);
			break;

			case ID_CONNECTION_ATTEMPT_FAILED:
			Packet_ConnectAttemptFailed(pkt);
			break;

			case ID_NO_FREE_INCOMING_CONNECTIONS:
			Packet_NoFreeIncomingConnections(pkt);
			break;

			case ID_CONNECTION_LOST:
			Packet_ConnectionLost(pkt);
			break;

			case ID_INVALID_PASSWORD:
			Packet_InvalidPassword(pkt);

			case ID_AUTH_KEY:
			Packet_AuthKey(pkt);
			break;

			case ID_CONNECTION_REQUEST_ACCEPTED:
			Packet_ConnectionSucceeded(pkt);
			break;

			case ID_PLAYER_SYNC:
			Packet_PlayerSync(pkt);
			break;

			case ID_VEHICLE_SYNC:
			Packet_VehicleSync(pkt);
			break;

			case ID_PASSENGER_SYNC:
			Packet_PassengerSync(pkt);
			break;

			// 0.3.7
			case ID_MARKERS_SYNC:
			Packet_MarkerSync(pkt);
			break;
		}

		m_pRakClient->DeallocatePacket(pkt);
	}
}

void CNetGame::Packet_PlayerSync(Packet *pkt)
{
	CRemotePlayer * pPlayer;
	RakNet::BitStream bsPlayerSync((unsigned char *)pkt->data, pkt->length, false);
	ONFOOT_SYNC_DATA ofSync;
	uint8_t bytePacketID=0;
	PLAYERID playerId;

	bool bHasLR,bHasUD;
	bool bHasVehicleSurfingInfo;

	memset(&ofSync, 0, sizeof(ONFOOT_SYNC_DATA));

	bsPlayerSync.Read(bytePacketID);
	bsPlayerSync.Read(playerId);

	// LEFT/RIGHT KEYS
	bsPlayerSync.Read(bHasLR);
	if(bHasLR) bsPlayerSync.Read(ofSync.lrAnalog);

	// UP/DOWN KEYS
	bsPlayerSync.Read(bHasUD);
	if(bHasUD) bsPlayerSync.Read(ofSync.udAnalog);

	// GENERAL KEYS
	bsPlayerSync.Read(ofSync.wKeys);

	// VECTOR POS
	bsPlayerSync.Read((char*)&ofSync.vecPos,sizeof(VECTOR));

	// QUATERNION
	// ------  что за дерьмо ------
	float tw, tx, ty, tz;
	bsPlayerSync.ReadNormQuat(tw, tx, ty, tz);
	ofSync.Quat.W = tw;
	ofSync.Quat.X = tx;
	ofSync.Quat.Y = ty;
	ofSync.Quat.Z = tz;
	// ----------------------------

	// HEALTH/ARMOUR (COMPRESSED INTO 1 BYTE)
	uint8_t byteHealthArmour;
	uint8_t byteArmTemp=0,byteHlTemp=0;

	bsPlayerSync.Read(byteHealthArmour);
	byteArmTemp = (byteHealthArmour & 0x0F);
	byteHlTemp = (byteHealthArmour >> 4);

	if(byteArmTemp == 0xF) ofSync.byteArmour = 100;
	else if(byteArmTemp == 0) ofSync.byteArmour = 0;
	else ofSync.byteArmour = byteArmTemp * 7;

	if(byteHlTemp == 0xF) ofSync.byteHealth = 100;
	else if(byteHlTemp == 0) ofSync.byteHealth = 0;
	else ofSync.byteHealth = byteHlTemp * 7;

    // CURRENT WEAPON
    bsPlayerSync.Read(ofSync.byteCurrentWeapon);
    // SPECIAL ACTION
    bsPlayerSync.Read(ofSync.byteSpecialAction);
    
    // READ MOVESPEED VECTORS
    // чому это говно не хочет работать?
    bsPlayerSync.ReadVector(tx, ty, tz);
    ofSync.vecMoveSpeed.X = tx;
    ofSync.vecMoveSpeed.Y = ty;
    ofSync.vecMoveSpeed.Z = tz;
    // ---------------------

    bsPlayerSync.Read(bHasVehicleSurfingInfo);

    if (bHasVehicleSurfingInfo) 
    {
        bsPlayerSync.Read(ofSync.wSurfInfo);
        bsPlayerSync.Read(ofSync.vecSurfOffsets.X);
        bsPlayerSync.Read(ofSync.vecSurfOffsets.Y);
        bsPlayerSync.Read(ofSync.vecSurfOffsets.Z);
    } 
    else
    	ofSync.wSurfInfo = INVALID_VEHICLE_ID;

    if(m_pPlayerPool)
    {
    	pPlayer = m_pPlayerPool->GetAt(playerId);
    	if(pPlayer)
    		pPlayer->StoreOnFootFullSyncData(&ofSync);
    }
}

void CNetGame::Packet_VehicleSync(Packet *p)
{
	CRemotePlayer *pPlayer;
	RakNet::BitStream bsSync((unsigned char *)p->data, p->length, false);
	uint8_t	bytePacketID = 0;
	PLAYERID playerId;
	INCAR_SYNC_DATA	icSync;

	if(GetGameState() != GAMESTATE_CONNECTED) return;

	memset(&icSync, 0, sizeof(INCAR_SYNC_DATA)); // 60

	bsSync.Read(bytePacketID);
	bsSync.Read(playerId);
	bsSync.Read(icSync.VehicleID);

	// KEYS
	bsSync.Read(icSync.lrAnalog);
	bsSync.Read(icSync.udAnalog);
	bsSync.Read(icSync.wKeys);

	// QUATERNION
	bsSync.ReadNormQuat(icSync.Quat.W,
		icSync.Quat.X,
		icSync.Quat.Y,
		icSync.Quat.Z);

	// POSITION
	bsSync.Read((char*)&icSync.vecPos, sizeof(VECTOR));

	// SPEED
	bsSync.ReadVector(icSync.vecMoveSpeed.X,
						icSync.vecMoveSpeed.Y,
						icSync.vecMoveSpeed.Z);

	// VEHICLE HEALTH
	uint16_t wTempVehicleHealth;
	bsSync.Read(wTempVehicleHealth);
	icSync.fCarHealth = (float)wTempVehicleHealth;

	// HEALTH/ARMOUR (COMPRESSED INTO 1 BYTE)
	uint8_t byteHealthArmour;
	uint8_t byteArmTemp=0,byteHlTemp=0;

	bsSync.Read(byteHealthArmour);
	byteArmTemp = (byteHealthArmour & 0x0F);
	byteHlTemp = (byteHealthArmour >> 4);

	if(byteArmTemp == 0xF) icSync.bytePlayerArmour = 100;
	else if(byteArmTemp == 0) icSync.bytePlayerArmour = 0;
	else icSync.bytePlayerArmour = byteArmTemp * 7;

	if(byteHlTemp == 0xF) icSync.bytePlayerHealth = 100;
	else if(byteHlTemp == 0) icSync.bytePlayerHealth = 0;
	else icSync.bytePlayerHealth = byteHlTemp * 7;

	// CURRENT WEAPON
	uint8_t byteTempWeapon;
	bsSync.Read(byteTempWeapon);
	icSync.byteCurrentWeapon ^= (byteTempWeapon ^ icSync.byteCurrentWeapon) & 0x3F;

	if(m_pPlayerPool)
	{
		pPlayer = m_pPlayerPool->GetAt(playerId);
		if(pPlayer)
			pPlayer->StoreInCarFullSyncData(&icSync);
	}
}

void CNetGame::Packet_PassengerSync(Packet *p)
{
	CRemotePlayer * pPlayer;
	RakNet::BitStream bsPassengerSync((unsigned char *)p->data, p->length, false);

	uint8_t	bytePacketID = 0;
	PLAYERID	playerId;
	PASSENGER_SYNC_DATA psSync;

	if(GetGameState() != GAMESTATE_CONNECTED) return;

	bsPassengerSync.Read(bytePacketID);
	bsPassengerSync.Read(playerId);
	bsPassengerSync.Read((char*)&psSync,sizeof(PASSENGER_SYNC_DATA));

	if (m_pPlayerPool) 
	{
		pPlayer = m_pPlayerPool->GetAt(playerId);
		if(pPlayer) 
			pPlayer->StorePassengerFullSyncData(&psSync);
	}
}

// 0.3.7
void CNetGame::Packet_MarkerSync(Packet *p)
{
	RakNet::BitStream bsMarkerSync((unsigned char *)p->data, p->length, false);
	int			iNumberOfPlayers = 0;
	PLAYERID	playerId;
	short		sPos[3];
	bool		bIsPlayerActive;
	uint8_t 	unk0 = 0;
	CRemotePlayer *pPlayer;

	bsMarkerSync.Read(unk0);
	bsMarkerSync.Read(iNumberOfPlayers);

	if(iNumberOfPlayers)
	{
		for(int i = 0; i < iNumberOfPlayers; i++)
		{
			bsMarkerSync.Read(playerId);
			bsMarkerSync.ReadCompressed(bIsPlayerActive);

			if(bIsPlayerActive)
			{
				bsMarkerSync.Read(sPos[0]); // x
				bsMarkerSync.Read(sPos[1]); // y
				bsMarkerSync.Read(sPos[2]); // z
			}

			if(playerId < MAX_PLAYERS && m_pPlayerPool->GetSlotState(playerId))
			{
				pPlayer = m_pPlayerPool->GetAt(playerId);
				if(pPlayer)
				{
					if(bIsPlayerActive)
						pPlayer->SetupGlobalMarker(sPos[0], sPos[1], sPos[2]);
					else
						pPlayer->HideGlobalMarker(0);
				}
			}

			//LOGI("MARKER_SYNC(unk: %d | iNumberOfPlayers: %d)", unk0, iNumberOfPlayers);
			//LOGI("MARKER_SYNC(playerId: %d | bIsPlayerActive: %d)", playerId, bIsPlayerActive);
			//LOGI("MARKER_SYNC(posX: %d | posY: %d | posZ: %d)", sPos[0], sPos[1], sPos[2]);
		}
	}
}

void gen_auth_key(char buf[260], char* auth_in);
void CNetGame::Packet_AuthKey(Packet *pkt)
{
	RakNet::BitStream bsAuth((unsigned char *)pkt->data, pkt->length, false);

	uint8_t byteAuthLen;
	char szAuth[260];

	bsAuth.IgnoreBits(8);
	bsAuth.Read(byteAuthLen);
	bsAuth.Read(szAuth, byteAuthLen);
	szAuth[byteAuthLen] = '\0';

	char szAuthKey[260];
 	gen_auth_key(szAuthKey, szAuth);

 	RakNet::BitStream bsKey;
 	uint8_t byteAuthKeyLen = (uint8_t)strlen(szAuthKey);
 	bsKey.Write((uint8_t)ID_AUTH_KEY);
 	bsKey.Write((uint8_t)byteAuthKeyLen);
 	bsKey.Write(szAuthKey, byteAuthKeyLen);

 	m_pRakClient->Send(&bsKey, SYSTEM_PRIORITY, RELIABLE, NULL);

 	//LOGI("[AUTH] %s -> %s", szAuth, szAuthKey);
}

void CNetGame::Packet_ConnectionSucceeded(Packet *pkt)
{
	m_iGameState = GAMESTATE_AWAIT_JOIN;
	LOGI("CNetGame::Packet_ConnectionSucceeded");
	if(pChatWindow)
		pChatWindow->AddDebugMessageNoGBK("已连接. 正在加入游戏...");
	RakNet::BitStream bsSuccAuth((unsigned char *)pkt->data, pkt->length, false);
	PLAYERID MyPlayerID;
	unsigned int uiChallenge;

	bsSuccAuth.IgnoreBits(8); // ID_CONNECTION_REQUEST_ACCEPTED
	bsSuccAuth.IgnoreBits(32); // binaryAddress
	bsSuccAuth.IgnoreBits(16); // port

	bsSuccAuth.Read(MyPlayerID);
	bsSuccAuth.Read(uiChallenge);

	if (m_pPlayerPool) m_pPlayerPool->SetLocalPlayerID(MyPlayerID);

	int iVersion = NETGAME_VERSION;
	uint8_t byteMod = 0x01;
	unsigned int uiClientChallengeResponse = uiChallenge ^ iVersion;

	uint8_t byteAuthBSLen;
	byteAuthBSLen = (uint8_t)strlen(AUTH_BS);
	uint8_t byteNameLen = (uint8_t)strlen(m_pPlayerPool->GetLocalPlayerName());

	RakNet::BitStream bsSend;
	bsSend.Write(iVersion);
	bsSend.Write(byteMod);
	bsSend.Write(byteNameLen);
	bsSend.Write(m_pPlayerPool->GetLocalPlayerName(), byteNameLen);
	bsSend.Write(uiClientChallengeResponse);
	bsSend.Write(byteAuthBSLen);
	bsSend.Write(AUTH_BS, byteAuthBSLen);
	bsSend.Write((uint8_t)5);
	bsSend.Write("0.3.7", (uint8_t)5);
	m_pRakClient->RPC(&RPC_ClientJoin, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);
}

void CNetGame::Packet_ConnectionBanned(Packet *packet)
{
	LOGI("You are banned from this server.");
	if(pChatWindow)
		pChatWindow->AddDebugMessageNoGBK("你已被封禁至这个服务器");
}

void CNetGame::Packet_NoFreeIncomingConnections(Packet *packet)
{
	LOGI("The server is full. Retrying...");
	if(pChatWindow)
		pChatWindow->AddDebugMessageNoGBK("这个服务器已满，正在重试...");
	SetGameState(GAMESTATE_WAIT_CONNECT);
}

void CNetGame::Packet_DisconnectionNotification(Packet *packet)
{
	LOGI("Server closed the connection.");
	if(pChatWindow)
		pChatWindow->AddDebugMessageNoGBK("服务器已关闭连接.");
	m_pRakClient->Disconnect(0);
}

void CNetGame::Packet_ConnectionLost(Packet *packet)
{
	LOGI("Lost connection to the server. Reconnecting..");
	if(pChatWindow)
		pChatWindow->AddDebugMessageNoGBK("与服务器丢失连接，正在重新连接...");
	ShutdownForGameModeRestart();
	SetGameState(GAMESTATE_WAIT_CONNECT);
}

void CNetGame::Packet_InvalidPassword(Packet *packet)
{
	LOGI("Wrong server password.");
	if(pChatWindow)
		pChatWindow->AddDebugMessageNoGBK("服务器密码错误!");
	m_pRakClient->Disconnect(0);
}

void CNetGame::Packet_ConnectAttemptFailed(Packet *packet)
{
	LOGI("The server didn't respond. Retrying..");
	if(pChatWindow)
		pChatWindow->AddDebugMessageNoGBK("与服务器失去响应，正在重试...");
	SetGameState(GAMESTATE_WAIT_CONNECT);
}

void CNetGame::ResetVehiclePool()
{
	LOGI("CNetGame::ResetVehiclePool");

	if(m_pVehiclePool)
		delete m_pVehiclePool;

	m_pVehiclePool = new CVehiclePool();
}

void CNetGame::SetMapIcon(uint8_t byteIndex, float fX, float fY, float fZ, uint8_t byteIcon, uint32_t dwColor)
{
	if(byteIndex >= 32) return;
	if(m_dwMapIcon[byteIndex] != 0) DisableMapIcon(byteIndex);
	m_dwMapIcon[byteIndex] = pGame->CreateRadarMarkerIcon(byteIcon, fX, fY, fZ, dwColor);
}

void CNetGame::DisableMapIcon(uint8_t byteIndex)
{
	if(byteIndex >= 32) return;
	ScriptCommand(&disable_marker, m_dwMapIcon[byteIndex]);
	m_dwMapIcon[byteIndex] = 0;
}