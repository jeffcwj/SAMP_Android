#include "main.h"
#include "game/util.h"
#include "game/keystuff.h"
#include "imgui.h"
#include "RenderWare/RenderWare.h"
#include "gui/renderware_imgui.h"

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CChatWindow *pChatWindow;

bool bFirstSpawn = true;

int iNetModeNormalOnfootSendRate	= NETMODE_NORMAL_ONFOOT_SENDRATE;
int iNetModeNormalIncarSendRate		= NETMODE_NORMAL_INCAR_SENDRATE;
int iNetModeFiringSendRate			= NETMODE_FIRING_SENDRATE;
int iNetModeSendMultiplier			= NETMODE_SEND_MULTIPLIER;

CLocalPlayer::CLocalPlayer()
{
	m_bHasSpawnInfo = false;
	m_pPlayerPed = pGame->FindPlayerPed();
	m_bIsActive = false;
	m_bIsWasted = false;
	m_dwLastSendTick = GetTickCount();
	m_bWaitingForSpawnRequestReply = false;

	m_bIsSpectating = false;
}

// допилить
void CLocalPlayer::ResetAllSyncAttributes()
{
/*	m_byteCurInterior = 0;
	m_LastVehicle = INVALID_VEHICLE_ID;
	m_bInRCMode = FALSE;
	m_DamageVehicleUpdating = INVALID_VEHICLE_ID;
	*/
}

void CLocalPlayer::Process()
{
	if(m_bIsActive && m_pPlayerPed)
	{

		uint32_t dwThisTick = GetTickCount();

		// SERVER CHECKPOINTS UPDATE
		pGame->UpdateCheckpoints();

		// DRIVER CONDITIONS
		if(m_pPlayerPed->IsInVehicle() && !m_pPlayerPed->IsAPassenger())
		{
			CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
			CVehicle *pVehicle;
			if (pVehiclePool)
				m_CurrentVehicle = pVehiclePool->FindIDFromGtaPtr(m_pPlayerPed->GetGtaVehicle());
			pVehicle = pVehiclePool->GetAt(m_CurrentVehicle);

			// updated to 0.3.7
			if ((dwThisTick - m_dwLastSendTick) > GetOptimumInCarSendRate())
			{
				m_dwLastSendTick = GetTickCount();
				SendInCarFullSyncData();
			}
		}
		// ONFOOT CONDITIONS
		else if(m_pPlayerPed->GetActionTrigger() == ACTION_NORMAL || m_pPlayerPed->GetActionTrigger() == ACTION_SCOPE)
		{
			HandlePassengerEntry();

			if(m_CurrentVehicle != INVALID_VEHICLE_ID)
			{	
				m_LastVehicle = m_CurrentVehicle;
				m_CurrentVehicle = INVALID_VEHICLE_ID;
			}


			// updated to 0.3.7
			if((dwThisTick - m_dwLastSendTick) > GetOptimumOnFootSendRate())
			{
				m_dwLastSendTick = GetTickCount();
				SendOnFootFullSyncData();
			}
		}
		// PASSENGER CONDITIONS
		else if(m_pPlayerPed->IsInVehicle() && m_pPlayerPed->IsAPassenger())
		{
			if((dwThisTick - m_dwLastSendTick) > GetOptimumInCarSendRate())
			{
				m_dwLastSendTick = GetTickCount();
				SendPassengerFullSyncData();
			}
		}
	}
}

void CLocalPlayer::HandlePassengerEntry()
{
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();

	int (*IsDoubleTapped)(int widget, bool v2, bool v3);
    *(void **) (&IsDoubleTapped) = (void*)(g_libGTASA+0x270818+1);
    int isTapped = (*IsDoubleTapped)(0, 1, 0);

	if(isTapped)
	{
		VEHICLEID ClosestVehicleID = pVehiclePool->FindNearestToLocalPlayerPed();
		if(ClosestVehicleID < MAX_VEHICLES && pVehiclePool->GetSlotState(ClosestVehicleID)) 
		{
			CVehicle *pVehicle = pVehiclePool->GetAt(ClosestVehicleID);
			if(pVehicle->GetDistanceFromLocalPlayerPed() < 4.0f)
			{
				m_pPlayerPed->EnterVehicle(pVehicle->m_dwGTAId, true);
				SendEnterVehicleNotification(ClosestVehicleID, true);
			}
		}
	}
}

void CLocalPlayer::SendOnFootFullSyncData()
{
	RakNet::BitStream bsPlayerSync;
	MATRIX4X4 matPlayer;
	VECTOR vecMoveSpeed;
	QUATERNION q;
	uint16_t lrAnalog, udAnalog;
	uint16_t wKeys = m_pPlayerPed->GetOFKeys(&lrAnalog, &udAnalog);

	ONFOOT_SYNC_DATA ofSync;

	m_pPlayerPed->GetMatrix(&matPlayer);
	m_pPlayerPed->GetMoveSpeedVector(&vecMoveSpeed);

	ofSync.lrAnalog = lrAnalog;
	ofSync.udAnalog = udAnalog;
	ofSync.wKeys = wKeys;
	
	ofSync.vecPos.X = matPlayer.pos.X;
	ofSync.vecPos.Y = matPlayer.pos.Y;
	ofSync.vecPos.Z = matPlayer.pos.Z;

	// vrem
	CQuaternion::ConvertMatrixToQuaternion(&q, &matPlayer);
	ofSync.Quat.X = q.X;
	ofSync.Quat.Y = q.Y;
	ofSync.Quat.Z = q.Z;
	ofSync.Quat.W = q.W;
	// -------
	ofSync.byteHealth = (uint8_t)m_pPlayerPed->GetHealth();
	ofSync.byteArmour = (uint8_t)0;

	ofSync.byteCurrentWeapon = 0;
	ofSync.byteSpecialAction = 0;

	ofSync.vecMoveSpeed.X = vecMoveSpeed.X;
	ofSync.vecMoveSpeed.Y = vecMoveSpeed.Y;
	ofSync.vecMoveSpeed.Z = vecMoveSpeed.Z;

	ofSync.vecSurfOffsets.X = 0.0f;
	ofSync.vecSurfOffsets.Y = 0.0f;
	ofSync.vecSurfOffsets.Z = 0.0f;
	ofSync.wSurfInfo = 0;

	ofSync.iCurrentAnimationID = 0;

	bsPlayerSync.Write((uint8_t)ID_PLAYER_SYNC);
	bsPlayerSync.Write((char*)&ofSync, sizeof(ONFOOT_SYNC_DATA));
	pNetGame->GetRakClient()->Send(&bsPlayerSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

void CLocalPlayer::SendInCarFullSyncData()
{
	//LOGI("SendInCarFullSyncData");

	RakNet::BitStream bsVehicleSync;
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();

	INCAR_SYNC_DATA icSync;

	MATRIX4X4 matPlayer;
	VECTOR vecMoveSpeed;
	QUATERNION q;

	uint16_t lrAnalog, udAnalog;
	uint16_t wKeys = m_pPlayerPed->GetICKeys(&lrAnalog, &udAnalog);
	CVehicle *pGameVehicle = 0;

	memset(&icSync, 0, sizeof(INCAR_SYNC_DATA));

	if(m_pPlayerPed)
	{
		icSync.VehicleID = pVehiclePool->FindIDFromGtaPtr(m_pPlayerPed->GetGtaVehicle());

		if(icSync.VehicleID == INVALID_VEHICLE_ID) return;

		icSync.lrAnalog = lrAnalog;
		icSync.udAnalog = udAnalog;
		icSync.wKeys = wKeys;

		// if(!pVehiclePool->??) return;

		pGameVehicle = pVehiclePool->GetAt(icSync.VehicleID);
		if(!pGameVehicle) return;

		pGameVehicle->GetMatrix(&matPlayer);
		pGameVehicle->GetMoveSpeedVector(&vecMoveSpeed);
		CQuaternion::ConvertMatrixToQuaternion(&q, &matPlayer);

		icSync.Quat.W = q.W;
		icSync.Quat.X = q.X;
		icSync.Quat.Y = q.Y;
		icSync.Quat.Z = q.Z;

		/*
		if(	FloatOffset(q.W, m_icSync.Quat.W) < 0.00001 &&
			FloatOffset(q.X, m_icSync.Quat.X) < 0.00001 &&
			FloatOffset(q.Y, m_icSync.Quat.Y) < 0.00001 &&
			FloatOffset(q.Z, m_icSync.Quat.Z) < 0.00001)
		{
			icSync.Quat.W = m_icSync.Quat.W;
			icSync.Quat.X = m_icSync.Quat.X;
			icSync.Quat.Y = m_icSync.Quat.Y;
			icSync.Quat.Z = m_icSync.Quat.Z;
		}*/

		// POSITION
		icSync.vecPos.X = matPlayer.pos.X;
		icSync.vecPos.Y = matPlayer.pos.Y;
		icSync.vecPos.Z = matPlayer.pos.Z;

		// MOVE SPEED
		icSync.vecMoveSpeed.X = vecMoveSpeed.X;
		icSync.vecMoveSpeed.Y = vecMoveSpeed.Y;
		icSync.vecMoveSpeed.Z = vecMoveSpeed.Z;

		// HEALTH / ARMOUR
		icSync.fCarHealth = pGameVehicle->GetHealth();
		icSync.bytePlayerHealth = (uint8_t)m_pPlayerPed->GetHealth();
		icSync.bytePlayerArmour = 0;//(uint8_t)m_pPlayerPed->GetArmour();

		/*
		if(sub_10003920(INCAR_SYNC_DATA* icSync, ptr_to_gettickcount, INCAR_SYNC_DATA* m_icSync, INCAR_SYNC_DATA* icSync, sizeof(INCAR_SYNC_DATA)))
		{
			bsVehicleSync.Write((uint8_t)ID_VEHICLE_SYNC);
			bsVehicleSync.Write((char*)&icSync, sizeof(INCAR_SYNC_DATA));
			memcpy(m_icSync, icSync, (sizeof(INCAR_SYNC_DATA)-3)); ???

			this->230 = xx;
			this->232 = (BYTE2)xx;
		}
		*/

		// допилить
		icSync.byteCurrentWeapon = 0;
		icSync.byteSirenOn = 0;
		icSync.byteLandingGearState = 0;
		icSync.TrailerIDorHydraThrustAngle = 0;
		icSync.fTrainSpeed = 0.0f;

		bsVehicleSync.Write((uint8_t)ID_VEHICLE_SYNC);
		bsVehicleSync.Write((char*)&icSync, sizeof(INCAR_SYNC_DATA));
		pNetGame->GetRakClient()->Send(&bsVehicleSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
	}
}
//发送死亡到服务器
//------------------------------------------------------------------------
void CLocalPlayer::SendWastedNotification()
{
	RakNet::BitStream bsPlayerDeath;
	uint8_t byteDeathReason;
byteDeathReason =55;
	bsPlayerDeath.Write(byteDeathReason);
	pNetGame->GetRakClient()->RPC(&RPC_Death, &bsPlayerDeath, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, NULL);
}
///---------------------------------------------------------------------------------
	void CLocalPlayer::SendPassengerFullSyncData()
{
	RakNet::BitStream bsPassengerSync;
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();

	uint16_t lrAnalog = 0, udAnalog = 0;
	uint16_t wKeys = 0;
	PASSENGER_SYNC_DATA psSync;
	MATRIX4X4 mat;

	psSync.VehicleID = pVehiclePool->FindIDFromGtaPtr(m_pPlayerPed->GetGtaVehicle());

	if(psSync.VehicleID == INVALID_VEHICLE_ID) return;

	psSync.lrAnalog = lrAnalog;
	psSync.udAnalog = udAnalog;
	psSync.wKeys = wKeys;
	psSync.bytePlayerHealth = (uint8_t)m_pPlayerPed->GetHealth();
	psSync.bytePlayerArmour = (uint8_t)m_pPlayerPed->GetArmour();

	psSync.byteSeatFlags = m_pPlayerPed->GetVehicleSeatID();
	psSync.byteDriveBy = 0;//m_bPassengerDriveByMode; допилить

	psSync.byteCurrentWeapon = 0;//m_pPlayerPed->GetCurrentWeapon(); допилить

	m_pPlayerPed->GetMatrix(&mat);
	psSync.vecPos.X = mat.pos.X;
	psSync.vecPos.Y = mat.pos.Y;
	psSync.vecPos.Z = mat.pos.Z;

	bsPassengerSync.Write((uint8_t)ID_PASSENGER_SYNC);
	bsPassengerSync.Write((char*)&psSync,sizeof(PASSENGER_SYNC_DATA));
	pNetGame->GetRakClient()->Send(&bsPassengerSync,HIGH_PRIORITY,UNRELIABLE_SEQUENCED,0);

	//if(m_bPassengerDriveByMode) SendAimSyncData(); допилить
}

// допилить
int CLocalPlayer::GetOptimumInCarSendRate()
{
	return 40;
}

// допилить
int CLocalPlayer::GetOptimumOnFootSendRate()
{
	return 40;
}

void CLocalPlayer::RequestClass(int iClass)
{
    RakNet::BitStream bsSpawnRequest;
    bsSpawnRequest.Write(iClass);
    pNetGame->GetRakClient()->RPC(&RPC_RequestClass, &bsSpawnRequest, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, 0);
}

//请求生成玩家
void CLocalPlayer::RequestSpawn()
{
	 RakNet::BitStream bsSpawnRequest;
	 pNetGame->GetRakClient()->RPC(&RPC_RequestSpawn, &bsSpawnRequest, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, 0);
}
//设置玩家生成信息
void CLocalPlayer::SetSpawnInfo(PLAYER_SPAWN_INFO *pSpawn)
{
	memcpy(&m_SpawnInfo, pSpawn, sizeof(PLAYER_SPAWN_INFO));
	m_bHasSpawnInfo = true;
}
//生成玩家
bool CLocalPlayer::Spawn()
{
	if(!m_bHasSpawnInfo) return false;

	CCamera *pGameCamera;
	pGameCamera->Restore();
	pGameCamera->SetBehindPlayer();
	//显示HUD
	pGame->DisplayHUD(true);

	if(!bFirstSpawn)
		m_pPlayerPed->SetInitialState();
	else
		bFirstSpawn = false;

	pGame->RefreshStreamingAt(m_SpawnInfo.vecPos.X,m_SpawnInfo.vecPos.Y);

	// допилить
	m_pPlayerPed->RestartIfWastedAt(&m_SpawnInfo.vecPos, m_SpawnInfo.fRotation);
	m_pPlayerPed->SetModelIndex(m_SpawnInfo.iSkin);
	//m_pPlayerPed->ClearAllWeapons();
	//m_pPlayerPed->ResetDamageEntity();

	m_pPlayerPed->TeleportTo(m_SpawnInfo.vecPos.X, m_SpawnInfo.vecPos.Y,(m_SpawnInfo.vecPos.Z + 1.0f));
	m_pPlayerPed->SetTargetRotation(m_SpawnInfo.fRotation);

	m_bIsWasted = false;
	m_bIsActive = true;
	m_bWaitingForSpawnRequestReply = false;

	RakNet::BitStream bsSendSpawn;
	pNetGame->GetRakClient()->RPC(&RPC_Spawn,&bsSendSpawn,HIGH_PRIORITY,RELIABLE_SEQUENCED,0,false,UNASSIGNED_NETWORK_ID,0);

	return true;
}
//进入车的时候触发
void CLocalPlayer::SendEnterVehicleNotification(VEHICLEID VehicleID, bool bPassenger)
{
/*char * n=ImGuiPlus::utf8_to_gbk("你好，这是手机端发送的消息，它已经做出来了!");
pGame->SendChat(n);
*/
	RakNet::BitStream bsSend;
	uint8_t bytePassenger = 0;

	if(bPassenger) 
		bytePassenger = 1;

	bsSend.Write(VehicleID);
	bsSend.Write(bytePassenger);

	pNetGame->GetRakClient()->RPC(&RPC_EnterVehicle,&bsSend,HIGH_PRIORITY,RELIABLE_SEQUENCED,0,false,UNASSIGNED_NETWORK_ID,0);

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);

	/* Допилить
	if (pVehicle && pVehicle->IsATrainPart())
	{
		DWORD dwVehicle = pVehicle->m_dwGTAId;
		ScriptCommand(&camera_on_vehicle, dwVehicle, 3, 2);
		dwEnterVehTimeElasped = GetTickCount();
	}
	*/
}
//下车时触发
// допилить
void CLocalPlayer::SendExitVehicleNotification(VEHICLEID VehicleID)
{
	RakNet::BitStream bsSend;

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
	
	if(pVehicle)
	{ 
		if (!m_pPlayerPed->IsAPassenger())
			m_LastVehicle = VehicleID;

		//if ( pVehicle->IsATrainPart() )
		//	pGame->GetCamera()->SetBehindPlayer();

		if(/*!pVehicle->IsRCVehicle()*/true)
		{
			bsSend.Write(VehicleID);
			pNetGame->GetRakClient()->RPC(&RPC_ExitVehicle,&bsSend, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, 0);
		}
	}
}

void CLocalPlayer::SetPlayerColor(uint32_t dwColor)
{
	SetRadarColor(pNetGame->GetPlayerPool()->GetLocalPlayerID(),dwColor);
}

uint32_t CLocalPlayer::GetPlayerColorAsRGBA()
{
	return TranslateColorCodeToRGBA(pNetGame->GetPlayerPool()->GetLocalPlayerID());
}

uint32_t CLocalPlayer::GetPlayerColorAsARGB()
{
	return (TranslateColorCodeToRGBA(pNetGame->GetPlayerPool()->GetLocalPlayerID()) >> 8) | 0xFF000000;
}

void CLocalPlayer::HandleClassSelectionOutcome()
{
	// допилить
}
