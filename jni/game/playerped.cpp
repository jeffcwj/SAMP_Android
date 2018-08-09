#include "main.h"
#include "util.h"
#include "keystuff.h"

extern CGame *pGame;
extern CNetGame *pNetGame;

extern uint8_t byteUsedPlayerSlots[PLAYER_PED_SLOTS];

CPlayerPed::CPlayerPed()
{
	m_dwGTAId = 1;
	m_pPed = GamePool_FindPlayerPed();
	m_pEntity = (ENTITY_TYPE*)GamePool_FindPlayerPed();

	m_bytePlayerNumber = 0;
	SetPlayerPedPtrRecord(m_bytePlayerNumber,(uint32_t)m_pPed);
	//ScriptCommand(&set_actor_weapon_droppable,m_dwGTAId,1);
	//ScriptCommand(&set_actor_can_be_decapitated,m_dwGTAId,0);

	m_dwArrow = 0;
}

CPlayerPed::CPlayerPed(uint8_t bytePlayerNumber, int iSkin, float fX, float fY, float fZ, float fRotation)
{
	uint32_t dwPlayerActorID=0;
	int iPlayerNum = bytePlayerNumber;

	m_pPed=0;
	m_dwGTAId=0;

	// CREATE PLAYER
	ScriptCommand(&create_player, &iPlayerNum, fX, fY, fZ, &dwPlayerActorID);
	ScriptCommand(&create_actor_from_player,&iPlayerNum,&dwPlayerActorID);

	m_dwGTAId = dwPlayerActorID;
	m_pPed = GamePool_Ped_GetAt(m_dwGTAId);
	m_pEntity = (ENTITY_TYPE *)m_pPed;

	m_bytePlayerNumber = bytePlayerNumber;
	SetPlayerPedPtrRecord(m_bytePlayerNumber,(uint32_t)m_pPed);
	//ScriptCommand(&set_actor_weapon_droppable,m_dwGTAId,1);
	ScriptCommand(&set_actor_immunities,m_dwGTAId,0,0,1,0,0);
	//ScriptCommand(&set_actor_can_be_decapitated,m_dwGTAId,0);

	m_dwArrow = 0;

	SetModelIndex(iSkin);
	SetTargetRotation(fRotation);
	memset(&GcsRemotePlayerKeys[m_bytePlayerNumber],0,sizeof(GTA_CONTROLSET));
}

CPlayerPed::~CPlayerPed()
{

}

void CPlayerPed::Destroy()
{
	LOGI("CPlayerPed::Destroy(%d)", m_bytePlayerNumber);

	memset(&GcsRemotePlayerKeys[m_bytePlayerNumber],0,sizeof(GTA_CONTROLSET));
 	SetPlayerPedPtrRecord(m_bytePlayerNumber,0);
	byteUsedPlayerSlots[m_bytePlayerNumber] = 0;

	if (!m_pPed || !GamePool_Ped_GetAt(m_dwGTAId) || m_pPed->entity.vtable == (g_libGTASA+0x5C7358))
	{
		m_pPed = 0;
		m_pEntity = 0;
		m_dwGTAId = 0;
		return;
	}

	if (IN_VEHICLE(m_pPed)) 
        RemoveFromVehicleAndPutAt(100.0f, 100.0f, 10.0f);

	uint32_t (*DestroyPlayerPed)(PED_TYPE*);
    *(void **) (&DestroyPlayerPed) = (void*)(**(void***)m_pPed); //(void*)(g_libGTASA+0x454294+1);
    (*DestroyPlayerPed)(m_pPed);

	m_pPed = 0;
    m_pEntity = 0;
}
void CPlayerPed::SetInitialState()
{
	LOGI("CPlayerPed::SetInitialState");
	uint32_t (*CPlayerPed__SetInitialState)(PED_TYPE*, bool);
	*(void **) (&CPlayerPed__SetInitialState) = (void*)(g_libGTASA+0x458D1C+1);
	(CPlayerPed__SetInitialState)(m_pPed, 0);
}

void CPlayerPed::SetOFKeys(uint16_t wKeys, uint16_t lrAnalog, uint16_t udAnalog)
{
	GTA_CONTROLSET *pPlayerKeys = &GcsRemotePlayerKeys[m_bytePlayerNumber];

	// LEFT/RIGHT
	pPlayerKeys->wWalkLR = lrAnalog;
	// UP/DOWN
	pPlayerKeys->wWalkUD = udAnalog;

	// SECONDARY FIRE
	wKeys >>= 1; // 1

	// CROUCHING TOGGLE
	if(wKeys & 1)
		pPlayerKeys->bDuckJustDown = 1;
	else
		pPlayerKeys->bDuckJustDown = 0;
	wKeys >>= 1; // 2

	// FIRING
	if(wKeys & 1)
		pPlayerKeys->bMeleeAttackJustDown = 1;
	else
		pPlayerKeys->bMeleeAttackJustDown = 0;
	wKeys >>= 1; // 3

	// SPRINT
	if(wKeys & 1)
		pPlayerKeys->bSprintJustDown = 1;
	else
		pPlayerKeys->bSprintJustDown = 0;
	wKeys >>= 1; // 4

	// SECONDARY ONFOOT ATTACK
	wKeys >>= 1; // 5

	// JUMP
	if(wKeys & 1)
		pPlayerKeys->bJumpJustDown = 1;
	else
		pPlayerKeys->bJumpJustDown = 0;
	wKeys >>= 1; // 6

	// INCAR LOOK RIGHT
	wKeys >>= 1; // 7

	// INCAR HANDBRAKE / ONFOOT TARGET
	wKeys >>= 1; // 8

	// INCAR LOOK LEFT
	wKeys >>= 1; // 9

	// SUBMISSION
	wKeys >>= 1; // 10

	// WALKING
	wKeys >>= 1; // 11

	// ANALOG2 D/U
	wKeys >>= 2; // 12 - 13
}

void CPlayerPed::SetICKeys(uint16_t wKeys, uint16_t lrAnalog, uint16_t udAnalog)
{
	GTA_CONTROLSET *pPlayerKeys = &GcsRemotePlayerKeys[m_bytePlayerNumber];

	// LEFT/RIGHT
	pPlayerKeys->wSteeringLR = lrAnalog;
	// UP/DOWN
	pPlayerKeys->wSteeringUD = udAnalog;

	//  0x1 1
	wKeys >>= 1;

	//  0x2 10
	wKeys >>= 1;

	// 0x4 	100
	wKeys >>= 1;

	// 0x8 	1000
	if(wKeys & 1)
		pPlayerKeys->wAccelerate = 0xFF;
	else
		pPlayerKeys->wAccelerate = 0;
	wKeys >>= 1;

	// 0x10 10000
	wKeys >>= 1;

	// 0x20 100000
	if(wKeys & 1)
		pPlayerKeys->wBrake = 0xFF;
	else
		pPlayerKeys->wBrake = 0;
	wKeys >>= 1;

	// 0x40 1000000
	wKeys >>= 1;

	// 0x80 10000000
	wKeys >>= 1;

	// 0x100 10000000
	wKeys >>= 1;

	// 0x200 100000000
	wKeys >>= 1;

	// 0x400 1000000000
	wKeys >>= 1;

	// 0x800 1000000000
	wKeys >>= 1;

	// 0x1000 1000000000
	wKeys >>= 1;

	// 0x4000 100000000000
}

uint16_t CPlayerPed::GetOFKeys(uint16_t *lrAnalog, uint16_t *udAnalog)
{
	uint16_t wRet = 0;

	*lrAnalog = GcsLocalPlayerKeys.wWalkLR;
	*udAnalog = GcsLocalPlayerKeys.wWalkUD;

	// analog2 L
	wRet <<= 1;
	
	// analog2 R
	wRet <<= 1;
	
	// analog2 D
	wRet <<= 1;
	
	// analog2 U
	wRet <<= 1;

	// walking
	wRet <<= 1;
	
	// submission
	wRet <<= 1;
	
	// incar look left
	wRet <<= 1;
	
	// incar handbrake / target
	wRet <<= 1;
	
	// incar look right
	wRet <<= 1;
	
	// jump
	if(GcsLocalPlayerKeys.bJumpJustDown) wRet |= 1;
	wRet <<= 1;
	
	// secondary onfoot attack
	wRet <<= 1;
	
	// sprint
	if(GcsLocalPlayerKeys.bSprintJustDown) wRet |= 1;
	wRet <<= 1;

	// fire
	wRet <<= 1;

	// crouch
	if(GcsLocalPlayerKeys.bDuckJustDown) wRet |= 1;
	wRet <<= 1;
	
	// secondary fire

	return wRet;
}

uint16_t CPlayerPed::GetICKeys(uint16_t *lrAnalog, uint16_t *udAnalog)
{
	unsigned short wRet =  0;

	*lrAnalog = GcsLocalPlayerKeys.wSteeringLR;
	*udAnalog = GcsLocalPlayerKeys.wSteeringUD;

	// 0x4000 100000000000
	wRet <<= 1;
	
	// 0x2000 10000000000
	wRet <<= 1;
	
	// 0x1000 1000000000
	wRet <<= 1;
	
	// 0x800 1000000000
	wRet <<= 1;

	// 0x400 1000000000
	wRet <<= 1;
	
	// 0x200 100000000
	wRet <<= 1;
	
	// 0x100 10000000
	wRet <<= 1;
	
	// 0x80 10000000
	wRet <<= 1;
	
	// 0x40 1000000
	wRet <<= 1;
	
	// 0x20 100000
	if(GcsLocalPlayerKeys.wBrake) wRet |= 1;
	wRet <<= 1;
	
	// 0x10 10000
	wRet <<= 1;
	
	// 0x8 	1000
	if(GcsLocalPlayerKeys.wAccelerate) wRet |= 1;
	wRet <<= 1;

	// 0x4 	100
	wRet <<= 1;

	//  0x2 10
	wRet <<= 1;
	
	//  0x1 1

	return wRet;
}

int CPlayerPed::GetCurrentVehicleID()
{
	if(!m_pPed) return 0;

	VEHICLE_TYPE *pVehicle = (VEHICLE_TYPE *)m_pPed->pVehicle;
	return GamePool_Vehicle_GetIndex(pVehicle);
}

void CPlayerPed::ShowMarker(int iMarkerColorID)
{	
	if (m_dwArrow) HideMarker();
	ScriptCommand(&create_arrow_above_actor, m_dwGTAId, &m_dwArrow);
	ScriptCommand(&set_marker_color, m_dwArrow, iMarkerColorID);
	ScriptCommand(&show_on_radar2, m_dwArrow, 2);
}

void CPlayerPed::HideMarker()
{	
	if (m_dwArrow) ScriptCommand(&disable_marker, m_dwArrow);
	m_dwArrow = 0;
}

float CPlayerPed::GetHealth()
{	
	if(!m_pPed) return 0.0f;
	return m_pPed->fHealth;
}
//设置血量
void CPlayerPed::SetHealth(float fHealth)
{
	if(!m_pPed) return;
	//+30.0是用来修复服务器返回的血量显示不准的问题
	m_pPed->fHealth = fHealth+30.0;
}

float CPlayerPed::GetArmour()
{
	if(!m_pPed) return 0.0f;
	return m_pPed->fArmour;
}

void CPlayerPed::SetArmour(float fArmour)
{
	if(!m_pPed) return;
	m_pPed->fArmour = fArmour;
}

uint8_t CPlayerPed::GetActionTrigger()
{
	return (uint8_t)m_pPed->dwAction;
}

bool CPlayerPed::IsInVehicle()
{
	if(!m_pPed) return false;

	if(IN_VEHICLE(m_pPed)) {
		return true;
	}
	return false;
}

void CPlayerPed::SetTargetRotation(float fRotation)
{
	if(!m_pPed) return;
	if(!GamePool_Ped_GetAt(m_dwGTAId)) return;

	m_pPed->fRotation2 = DegToRad(fRotation);

	if(!IsOnGround()) 
	{
		m_pPed->fRotation1 = DegToRad(fRotation);
		ScriptCommand(&set_actor_z_angle,m_dwGTAId,fRotation);
	}
}

void CPlayerPed::ForceTargetRotation(float fRotation)
{
	if(!m_pPed) return;
	if(!GamePool_Ped_GetAt(m_dwGTAId)) return;

	m_pPed->fRotation1 = DegToRad(fRotation);
	m_pPed->fRotation2 = DegToRad(fRotation);

	ScriptCommand(&set_actor_z_angle, m_dwGTAId, fRotation);
}

bool CPlayerPed::IsAPassenger()
{
	if( m_pPed->pVehicle && IN_VEHICLE(m_pPed) )
	{
		VEHICLE_TYPE * pVehicle = (VEHICLE_TYPE *)m_pPed->pVehicle;

		if( pVehicle->pDriver != m_pPed || 
			pVehicle->entity.nModelIndex == TRAIN_PASSENGER ||
			pVehicle->entity.nModelIndex == TRAIN_FREIGHT ) 
			return true;
		else 
			return false;
	}

	return false;
}

VEHICLE_TYPE* CPlayerPed::GetGtaVehicle()
{
	return (VEHICLE_TYPE*)m_pPed->pVehicle;
}

void CPlayerPed::SetVisible(bool bVisible)
{
	m_bVisible = bVisible;

	// допилить
	/*
	if(bVisible)
		m_pPed->dwUnkFlags &= ~(1<<3); //sets 3rd bit to 0
	else
		m_pPed->dwUnkFlags |= (1<<3); // sets 3rd bit to 1
	*/
}

int CPlayerPed::GetVehicleSeatID()
{
	VEHICLE_TYPE *pVehicle;

	if( GetActionTrigger() == ACTION_INCAR && (pVehicle = (VEHICLE_TYPE *)m_pPed->pVehicle) != 0 )
	{
		if(pVehicle->pDriver == m_pPed) return 0;
		if(pVehicle->pPassengers[0] == m_pPed) return 1;
		if(pVehicle->pPassengers[1] == m_pPed) return 2;
		if(pVehicle->pPassengers[2] == m_pPed) return 3;
		if(pVehicle->pPassengers[3] == m_pPed) return 4;
		if(pVehicle->pPassengers[4] == m_pPed) return 5;
		if(pVehicle->pPassengers[5] == m_pPed) return 6;
		if(pVehicle->pPassengers[6] == m_pPed) return 7;
	}

	return -1;
}

void CPlayerPed::PutDirectlyInVehicle(int iVehicleID, int iSeat)
{
	if(!m_pPed) return;
    if(!GamePool_Vehicle_GetAt(iVehicleID)) return;
    if(!GamePool_Ped_GetAt(m_dwGTAId)) return;

    VEHICLE_TYPE *pVehicle = GamePool_Vehicle_GetAt(iVehicleID);

    if(pVehicle->fHealth == 0.0f) return;

    // CPlaceable
    if(pVehicle->entity.vtable == (g_libGTASA+0x5C7358)) return;

    if(iSeat == 0)
    {
        if(pVehicle->pDriver && IN_VEHICLE(pVehicle->pDriver)) return;
        ScriptCommand(&put_actor_in_car,m_dwGTAId,iVehicleID);
    }
    else
    {
        iSeat--;
        ScriptCommand(&put_actor_in_car2,m_dwGTAId,iVehicleID,iSeat);
    }
    if(m_pPed == GamePool_FindPlayerPed() && IN_VEHICLE(m_pPed)) {
        pGame->GetCamera()->SetBehindPlayer();
    }

    // допилить
    /* TRAIN
    if(pNetGame)
    {
        CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
        VEHICLEID TrainVehicleId = pVehiclePool->FindIDFromGtaPtr(pVehicle);
        if(TrainVehicleId == INVALID_VEHICLE_ID || TrainVehicleId > MAX_VEHICLES) return;

        CVehicle *pTrain = pVehiclePool->GetAt(TrainVehicleId);

        if(pTrain && pTrain->IsATrainPart() && this == pNetGame->GetPlayerPool()->GetLocalPlayer()->m_pPlayerPed)
            ScriptCommand(&camera_on_vehicle, pTrain->m_dwGTAId, 3, 2);
    }*/
}

void CPlayerPed::EnterVehicle(int iVehicleID, bool bPassenger)
{
	LOGI("CPlayerPed::EnterVehicle(id %d, pass %d)", iVehicleID, bPassenger);

	if(!m_pPed) return;
	VEHICLE_TYPE *ThisVehicleType;
	if((ThisVehicleType = GamePool_Vehicle_GetAt(iVehicleID)) == NULL) return;
	if(!GamePool_Ped_GetAt(m_dwGTAId)) return;

	if(bPassenger)
	{
		if(ThisVehicleType->entity.nModelIndex == TRAIN_PASSENGER && (m_pPed == GamePool_FindPlayerPed()))
			return;// допилить
		else
			ScriptCommand(&send_actor_to_car_passenger,m_dwGTAId,iVehicleID,3000,-1);
	}
	else
		ScriptCommand(&send_actor_to_car_driverseat,m_dwGTAId,iVehicleID,3000);
}

void CPlayerPed::ExitCurrentVehicle()
{
	if(!m_pPed) return;
	if(!GamePool_Ped_GetAt(m_dwGTAId)) return;
	if(IN_VEHICLE(m_pPed))
		ScriptCommand(&make_actor_leave_car,m_dwGTAId,GetCurrentVehicleID());
}

void CPlayerPed::RemoveFromVehicleAndPutAt(float fX, float fY, float fZ)
{
	if(!GamePool_Ped_GetAt(m_dwGTAId)) return;
	if(m_pPed && IN_VEHICLE(m_pPed))
		ScriptCommand(&remove_actor_from_car_and_put_at,m_dwGTAId,fX,fY,fZ);
}

void CPlayerPed::RestartIfWastedAt(VECTOR *vecRestart, float fRotation)
{	
	ScriptCommand(&restart_if_wasted_at, vecRestart->X,vecRestart->Y,vecRestart->Z,fRotation,0);
}

void CPlayerPed::SetModelIndex(unsigned int uiModel)
{
	// допилить
	if(uiModel > 299) uiModel = 0;

	if(!GamePool_Ped_GetAt(m_dwGTAId)) return;
	if(!IsValidModel(uiModel))
		uiModel = 0;

	if(m_pPed)
	{
		// CClothes__RebuildPlayer
		//UnFuck(g_libGTASA+0x3F1030);
		//memcpy((void*)(g_libGTASA+0x3F1030), "\x4F\xF0\x00\x00\xF7\x46", 6);
		WriteMemory(g_libGTASA+0x3F1030, "\x70\x47", 2);

		CEntity::SetModelIndex(uiModel);

		// reset the Ped Audio Attributes
		LOGI("CAEPedSpeechAudioEntity__Initialize");
		void (*CAEPedSpeechAudioEntity__Initialize)(uintptr_t pedSAInterface, uintptr_t entity);
		*(void **) (&CAEPedSpeechAudioEntity__Initialize) = (void*)(g_libGTASA+0x34B2A8+1);
		(CAEPedSpeechAudioEntity__Initialize)((uintptr_t)((char*)m_pPed+660), (uintptr_t)m_pPed);
	}
}

void CPlayerPed::SetInterior(uint8_t byteID)
{
	if(!m_pPed) return;

	ScriptCommand(&select_interior,byteID);
	ScriptCommand(&link_actor_to_interior,m_dwGTAId,byteID);

	MATRIX4X4 mat;
	GetMatrix(&mat);
	ScriptCommand(&refresh_streaming_at,mat.pos.X,mat.pos.Y);
}

bool CPlayerPed::IsOnGround()
{
	if(m_pPed && (m_pPed->dwStateFlags & 3)) 
		return true;

	return false;
}

void CPlayerPed::TogglePlayerControllable(int iControllable)
{
	MATRIX4X4 mat;

	if(!GamePool_Ped_GetAt(m_dwGTAId)) return;

	if(!iControllable)
	{
		ScriptCommand(&toggle_player_controllable,m_bytePlayerNumber,0);
		ScriptCommand(&lock_actor,m_dwGTAId,1);
	}
	else
	{
		ScriptCommand(&toggle_player_controllable,m_bytePlayerNumber,1);
		ScriptCommand(&lock_actor,m_dwGTAId,0);
		if(!IsInVehicle())
		{
			GetMatrix(&mat);
			TeleportTo(mat.pos.X,mat.pos.Y,mat.pos.Z);
		}
	}
}