#include "main.h"
#include "imgui.h"
#include "RenderWare/RenderWare.h"
#include "gui/renderware_imgui.h"

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CChatWindow *pChatWindow;
//进入服务器
void ServerJoin(RPCParameters *rpcParams)
{
	LOGI("RPC_SERVERJOIN");
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	char szPlayerName[MAX_PLAYER_NAME+1];
	PLAYERID playerId;
	uint8_t byteNameLen=0;
	int unk;
	uint8_t bIsNPC;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

	bsData.Read(playerId);
	bsData.Read(unk);
	bsData.Read(bIsNPC);
	bsData.Read(byteNameLen);
	bsData.Read(szPlayerName,byteNameLen);
	szPlayerName[byteNameLen] = '\0';

	pPlayerPool->New(playerId, szPlayerName);
}
//退出服务器
void ServerQuit(RPCParameters *rpcParams)
{
	LOGI("RPC_SERVERQUIT");
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	PLAYERID playerId;
	uint8_t byteReason;

	bsData.Read(playerId);
	bsData.Read(byteReason);

	pPlayerPool->Delete(playerId,byteReason);
}

extern int iNetModeNormalOnfootSendRate;
extern int iNetModeNormalIncarSendRate;
extern int iNetModeFiringSendRate;
extern int iNetModeSendMultiplier;

void InitGame(RPCParameters *rpcParams)
{
	LOGI("RPC_INITGAME");
//初始化游戏
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsInitGame(Data,(iBitLength/8)+1,false);
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	CLocalPlayer *pLocalPlayer = 0;

	if(pPlayerPool) pLocalPlayer = pPlayerPool->GetLocalPlayer();

	PLAYERID MyPlayerID;
	bool bLanMode, bStuntBonus;
	uint8_t byteVehicleModels[212];

	bsInitGame.ReadCompressed(pNetGame->m_bZoneNames);
	bsInitGame.ReadCompressed(pNetGame->m_bUseCJWalk);
	bsInitGame.ReadCompressed(pNetGame->m_bAllowWeapons);
	bsInitGame.ReadCompressed(pNetGame->m_bLimitGlobalChatRadius);
	bsInitGame.Read(pNetGame->m_fGlobalChatRadius);
	bsInitGame.ReadCompressed(bStuntBonus);
	bsInitGame.Read(pNetGame->m_fNameTagDrawDistance);
	bsInitGame.ReadCompressed(pNetGame->m_bDisableEnterExits);
	bsInitGame.ReadCompressed(pNetGame->m_bNameTagLOS);
	bsInitGame.ReadCompressed(pNetGame->m_bManualVehicleEngineAndLight); //
	bsInitGame.Read(pNetGame->m_iSpawnsAvailable);
	bsInitGame.Read(MyPlayerID);
	bsInitGame.ReadCompressed(pNetGame->m_bShowPlayerTags);
	bsInitGame.Read(pNetGame->m_iShowPlayerMarkers);
	bsInitGame.Read(pNetGame->m_byteWorldTime);
	bsInitGame.Read(pNetGame->m_byteWeather);
	bsInitGame.Read(pNetGame->m_fGravity);
	bsInitGame.ReadCompressed(bLanMode);
	bsInitGame.Read(pNetGame->m_iDeathDropMoney);
	bsInitGame.ReadCompressed(pNetGame->m_bInstagib);

	// Server's send rate restrictions
	bsInitGame.Read(iNetModeNormalOnfootSendRate);
	bsInitGame.Read(iNetModeNormalIncarSendRate);
	bsInitGame.Read(iNetModeFiringSendRate);
	bsInitGame.Read(iNetModeSendMultiplier);

	bsInitGame.Read(pNetGame->m_bLagCompensation);
	uint8_t unk;
	bsInitGame.Read(unk);
	bsInitGame.Read(unk);
	bsInitGame.Read(unk);

	uint8_t byteStrLen;
	bsInitGame.Read(byteStrLen);
	if(byteStrLen)
	{
		memset(pNetGame->m_szHostName, 0, sizeof(pNetGame->m_szHostName));
		bsInitGame.Read(pNetGame->m_szHostName, byteStrLen);
	}
	pNetGame->m_szHostName[byteStrLen] = '\0';

	// допилить

	bsInitGame.Read((char*)&byteVehicleModels[0], 212);
	//pGame->SetRequiredVehicleModels(byteVehicleModels);

	if(pPlayerPool) pPlayerPool->SetLocalPlayerID(MyPlayerID);
	//pGame->EnableStuntBonus(bStuntBonus);
	//if(bLanMode) pNetGame->SetLanMode(true);

	//pNetGame->InitGameLogic();

	// set the gravity now
	//pGame->SetGravity(pNetGame->m_fGravity);

	//if(pNetGame->m_bDisableEnterExits)
	//	pGame->ToggleEnterExits(true);

	pGame->ToggleCJWalk(pNetGame->m_bUseCJWalk);

	pNetGame->SetGameState(GAMESTATE_CONNECTED);
	//if(pLocalPlayer) pLocalPlayer->HandleClassSelection();

	// ============== TEST ====================
	CPlayerPed *pPed = pLocalPlayer->m_pPlayerPed;
	if(pPed)
	{
		pPed->SetInitialState();
		pPed->SetHealth(100.0f);
	}
	pLocalPlayer->RequestClass(0);
	pLocalPlayer->RequestSpawn();
	pLocalPlayer->m_bWaitingForSpawnRequestReply = true;
	// ========================================
	//修正乱码
	char *szString=ImGuiPlus::utf8_to_gbk("已连接至");
	pChatWindow->AddDebugMessage("%s [B9C9BF]%.64s",szString, pNetGame->m_szHostName);
}

void Chat(RPCParameters *rpcParams)
{
//聊天消息处理
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	PLAYERID playerId;
	uint8_t byteTextLen;
	char szText[256];
	memset(szText, 0, 256);

	bsData.Read(playerId);
	bsData.Read(byteTextLen);
	bsData.Read((char*)szText, byteTextLen);

	szText[byteTextLen] = '\0';
	
	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (playerId  == pPlayerPool->GetLocalPlayerID())
	{
		pChatWindow->AddChatMessage(pPlayerPool->GetLocalPlayerName(),
			pPlayerPool->GetLocalPlayer()->GetPlayerColorAsRGBA(), (char*)szText);
	} else {
		CRemotePlayer *pRemotePlayer = pNetGame->GetPlayerPool()->GetAt(playerId);
		if(pRemotePlayer) {
			pRemotePlayer->Say((unsigned char *)szText);
		}
	}
}

void ToggleClock(RPCParameters *rpcParams)
{
unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	uint8_t byteClock;
	bsData.Read(byteClock);
	pGame->EnableClock(byteClock);	
	if (byteClock)
	{
		pNetGame->m_byteHoldTime = 0;
	}
	else
	{
		pNetGame->m_byteHoldTime = 1;
		pGame->GetWorldTime((int*)&pNetGame->m_byteWorldTime, (int*)&pNetGame->m_byteWorldMinute);
	}
}

void ClientMessage(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	uint32_t dwStrLen;
	uint32_t dwColor;

	bsData.Read(dwColor);
	bsData.Read(dwStrLen);
	char *szMsg = (char*)malloc(dwStrLen+1);
	bsData.Read(szMsg, dwStrLen);
	szMsg[dwStrLen] = 0;
	pChatWindow->AddClientMessage(dwColor, szMsg);

	free(szMsg);
}

void RequestClass(RPCParameters *rpcParams)
{
	LOGI("RPC_REQUESTCLASS");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
    int iBitLength = rpcParams->numberOfBitsOfData;

    RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
    uint8_t byteRequestOutcome = 0;
    PLAYER_SPAWN_INFO SpawnInfo;

    CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
    CLocalPlayer *pPlayer = 0;

    if (pPlayerPool) pPlayer = pPlayerPool->GetLocalPlayer();

    bsData.Read(byteRequestOutcome);
    bsData.Read((char*)&SpawnInfo, sizeof(PLAYER_SPAWN_INFO));

    if(pPlayer && byteRequestOutcome)
    {
        pPlayer->SetSpawnInfo(&SpawnInfo);
        pPlayer->HandleClassSelectionOutcome();
    }
}

void RequestSpawn(RPCParameters *rpcParams)
{
	LOGI("RPC_REQUESTSPAWN");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint8_t byteRequestOutcome=0;
	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	bsData.Read(byteRequestOutcome);

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	CLocalPlayer *pPlayer = NULL;

	if (pPlayerPool) pPlayer = pPlayerPool->GetLocalPlayer();

		if (pPlayer) 
		{ 
		if (byteRequestOutcome == 2 || (byteRequestOutcome && pPlayer->m_bWaitingForSpawnRequestReply)) 
		{
			pPlayer->Spawn();
		}
		else 
		{
			pPlayer->m_bWaitingForSpawnRequestReply = false;
		}
	}
}

void WorldPlayerAdd(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	CRemotePlayer *pRemotePlayer;
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

	PLAYERID playerId;
    uint8_t byteFightingStyle=4;
    uint8_t byteTeam = 0;
    int iSkin = 0;
    VECTOR vecPos;
    float fRotation = 0;
    uint32_t dwColor = 0;
    bool bVisible;

	bsData.Read(playerId);
    bsData.Read(byteTeam);
    bsData.Read(iSkin);
    bsData.Read(vecPos.X);
    bsData.Read(vecPos.Y);
    bsData.Read(vecPos.Z);
    bsData.Read(fRotation);
    bsData.Read(dwColor);
    bsData.Read(byteFightingStyle);
    bsData.Read(bVisible);

    if(pPlayerPool)
    {
    	pRemotePlayer = pPlayerPool->GetAt(playerId);
    	if(pRemotePlayer) pRemotePlayer->Spawn(byteTeam, iSkin, &vecPos, fRotation, dwColor, byteFightingStyle, bVisible);
    }

}

void WorldPlayerRemove(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

	PLAYERID playerId=0;
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	bsData.Read(playerId);

	if(pPlayerPool) 
	{
		CRemotePlayer *pRemotePlayer = pPlayerPool->GetAt(playerId);
		if(pRemotePlayer) pRemotePlayer->Remove();
	}
}

void WorldVehicleAdd(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	if(!pVehiclePool) return;

	NEW_VEHICLE NewVehicle;

	bsData.Read((char *)&NewVehicle,sizeof(NEW_VEHICLE));

	if(NewVehicle.iVehicleType < 400 || NewVehicle.iVehicleType > 611) return;

//	LOGI("ID %d | vehType %d | vecPos | rot %f | color1 %d | color 2 %d | heath %f",NewVehicle.VehicleId, NewVehicle.iVehicleType, NewVehicle.fRotation,
       //   NewVehicle.aColor1, NewVehicle.aColor2, NewVehicle.fHealth);

	pVehiclePool->New(&NewVehicle);
}

void WorldVehicleRemove(RPCParameters *rpcParams)
{

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();

	if(!pVehiclePool) return;

	VEHICLEID VehicleID;
	VEHICLEID MyVehicleID;

	bsData.Read(VehicleID);
	if(pPlayerPed)
	{
		MyVehicleID = pVehiclePool->FindIDFromGtaPtr(pPlayerPed->GetGtaVehicle());

		if(MyVehicleID == VehicleID)
		{
			MATRIX4X4 mat;
			pPlayerPed->GetMatrix(&mat);
			pPlayerPed->RemoveFromVehicleAndPutAt(mat.pos.X,mat.pos.Y,mat.pos.Z);
		}
		pVehiclePool->Delete(VehicleID);
	}
}

void SetCheckpoint(RPCParameters *rpcParams)
{
	LOGI("RPC_SETCHECKPOINT");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	float fX, fY, fZ, fSize;

	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);
	bsData.Read(fSize);

	VECTOR Pos, Extent;

	Pos.X = fX;
	Pos.Y = fY;
	Pos.Z = fZ;
	Extent.X = fSize;
	Extent.Y = fSize;
	Extent.Z = fSize;

	pGame->SetCheckpointInformation(&Pos, &Extent);
	pGame->ToggleCheckpoint(true);
}

void DisableCheckpoint(RPCParameters *rpcParams)
{
	LOGI("RPC_DISABLECHECKPOINT");

	pGame->ToggleCheckpoint(false);
}

void GameModeRestart(RPCParameters *rpcParams)
{
	LOGI("RPC_GAMEMODERESTART");
	pNetGame->ShutdownForGameModeRestart();
}

void WorldTime(RPCParameters *rpcParams)
{
	LOGI("RPC_WORLDTIME");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	uint8_t byteWorldTime;
	bsData.Read(byteWorldTime);
	pNetGame->m_byteWorldTime = byteWorldTime;
}

void Weather(RPCParameters *rpcParams)
{
	LOGI("RPC_WEATHER");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	uint8_t byteWeather;
	bsData.Read(byteWeather);
	pNetGame->m_byteWeather = byteWeather;
}

void EnterVehicle(RPCParameters *rpcParams)
{
	LOGI("RPC_ENTERVEHICLE");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	PLAYERID playerId;
	VEHICLEID VehicleID = 0;
	uint8_t bytePassenger = 0;
	bool bPassenger = false;

	bsData.Read(playerId);
	bsData.Read(VehicleID);
	bsData.Read(bytePassenger);

	if(bytePassenger) bPassenger = true;

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

	if (pPlayerPool)
	{
		CRemotePlayer *pRemotePlayer = pPlayerPool->GetAt(playerId);
		if(pRemotePlayer)
			pRemotePlayer->EnterVehicle(VehicleID, bPassenger);
	}
}

void ExitVehicle(RPCParameters *rpcParams)
{
	LOGI("RPC_EXITVEHICLE");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	PLAYERID playerId;
	VEHICLEID VehicleID = 0;

	bsData.Read(playerId);
	bsData.Read(VehicleID);

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

	if (pPlayerPool)
	{
		CRemotePlayer *pRemotePlayer = pPlayerPool->GetAt(playerId);
		if(pRemotePlayer)
			pRemotePlayer->ExitVehicle();
	}
}

void DialogBox(RPCParameters *rpcParams)
{
	LOGI("RPC_DIALOGBOX");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

	uint16_t wDialogID;

	bsData.Read(wDialogID);

	if(wDialogID == 1)
	{
		RakNet::BitStream bsSend;
		bsSend.Write((uint16_t)1); // wDialogID
		bsSend.Write((uint8_t)1); // bButtonID
		bsSend.Write((uint16_t)0); // wListBoxItem
		bsSend.Write((uint8_t)strlen("123123")); // respLen
		bsSend.Write("123123", (uint8_t)strlen("123123"));
		pNetGame->GetRakClient()->RPC(&RPC_DialogResponse, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, 0);
	}

	LOGI("wDialogID = %d", wDialogID);
}

void Pickup(RPCParameters *rpcParams)
{
	LOGI("RPC_PICKUP");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

	PICKUP Pickup;
	int iIndex;

	bsData.Read(iIndex);
	bsData.Read((char*)&Pickup, sizeof (PICKUP));

	LOGI("Pickup(%d): %d %d %f %f %f", iIndex, Pickup.iModel, Pickup.iType, Pickup.fX, Pickup.fY, Pickup.fZ);

	CPickupPool *pPickupPool = pNetGame->GetPickupPool();
	if (pPickupPool) pPickupPool->New(&Pickup, iIndex);
}

void DestroyPickup(RPCParameters *rpcParams)
{
	LOGI("RPC_DESTROYPICKUP");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

	int iIndex;
	bsData.Read(iIndex);

	CPickupPool *pPickupPool = pNetGame->GetPickupPool();
	if (pPickupPool) pPickupPool->Destroy(iIndex);
}

void SetRaceCheckpoint(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	float fX, fY, fZ;
	uint8_t byteType;
	VECTOR Pos, Next;

	bsData.Read(byteType);
	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);
	Pos.X = fX;
	Pos.Y = fY;
	Pos.Z = fZ;

	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);
	Next.X = fX;
	Next.Y = fY;
	Next.Z = fZ;

	bsData.Read(fX); // Float:size

	pGame->SetRaceCheckpointInformation(byteType, &Pos, &Next, fX);
	pGame->ToggleRaceCheckpoints(true);
}

void DisableRaceCheckpoint(RPCParameters *rpcParams)
{
	pGame->ToggleRaceCheckpoints(false);
}

void RegisterRPCs(RakClientInterface *pRakClient)
{
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_InitGame, InitGame);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_Chat, Chat);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ClientMessage, ClientMessage);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_RequestClass, RequestClass);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_RequestSpawn, RequestSpawn);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ServerJoin, ServerJoin);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ServerQuit, ServerQuit);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerAdd, WorldPlayerAdd);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerRemove, WorldPlayerRemove);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldVehicleAdd, WorldVehicleAdd);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldVehicleRemove, WorldVehicleRemove);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetCheckpoint, SetCheckpoint);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_DisableCheckpoint, DisableCheckpoint);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_GameModeRestart, GameModeRestart);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldTime, WorldTime);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_Weather, Weather);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_EnterVehicle, EnterVehicle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ExitVehicle, ExitVehicle);
	
	//解锁
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrDialogBox, DialogBox);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_Pickup, Pickup);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_DestroyPickup, DestroyPickup);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ToggleClock, ToggleClock); // 31
	
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetRaceCheckpoint, SetRaceCheckpoint);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_DisableRaceCheckpoint, DisableRaceCheckpoint);
}

void UnRegisterRPCs(RakClientInterface * pRakClient)
{
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_InitGame);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_Chat);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ClientMessage);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_RequestClass);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_RequestSpawn);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ServerJoin);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ServerQuit);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldPlayerAdd);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldPlayerRemove);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldVehicleAdd);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldVehicleRemove);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_SetCheckpoint);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_DisableCheckpoint);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_GameModeRestart);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldTime);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_Weather);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_EnterVehicle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ExitVehicle);
	
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrDialogBox);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_Pickup);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_DestroyPickup);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ToggleClock); // 31
	
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_SetRaceCheckpoint);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_DisableRaceCheckpoint);
}