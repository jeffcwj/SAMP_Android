#include "main.h"
#include "game/util.h"

extern CGame 		*pGame;
extern CNetGame		*pNetGame;
extern CChatWindow *pChatWindow;

CRemotePlayer::CRemotePlayer()
{
	m_byteUpdateFromNetwork = UPDATE_TYPE_NONE;
	m_byteState = PLAYER_STATE_NONE;
	m_PlayerID = INVALID_PLAYER_ID;
	m_pPlayerPed = 0;
	m_VehicleID = 0;
	m_dwWaitForEntryExitAnims = GetTickCount();
	m_bGlobalMarkerLoaded = false;
	m_dwGlobalMarker = 0;
}

CRemotePlayer::~CRemotePlayer()
{
	if(m_pPlayerPed)
	{
		m_pPlayerPed->Destroy();
		delete m_pPlayerPed;
		m_pPlayerPed = 0;
	}

	HideGlobalMarker(0);
}

void CRemotePlayer::Process()
{
	CPlayerPool *pPool = pNetGame->GetPlayerPool();
	CLocalPlayer *pLocalPlayer = pPool->GetLocalPlayer();
	MATRIX4X4 matVehicle;
	VECTOR vecMoveSpeed;

	if(IsActive())
	{
		if(m_pPlayerPed)
		{
			if(GetTickCount() > m_dwWaitForEntryExitAnims)
				HandleVehicleEntryExit();

			// ---- ONFOOT NETWORK PROCESSING ----
			if(GetState() == PLAYER_STATE_ONFOOT && m_byteUpdateFromNetwork == UPDATE_TYPE_ONFOOT)
			{
				if(GetTickCount() > m_dwWaitForEntryExitAnims)
				HandleVehicleEntryExit();

				//if (!IsSurfingOrTurretMode())
				//{
					UpdateOnFootPositionAndSpeed(&m_ofSync.vecPos,&m_ofSync.vecMoveSpeed);
					UpdateOnfootTargetPosition();
				//}

				m_pCurrentVehicle = 0;
				m_byteUpdateFromNetwork = UPDATE_TYPE_NONE;
			}

			// ---- DRIVER NETWORK PROCESSING ----
			else if(GetState() == PLAYER_STATE_DRIVER && m_byteUpdateFromNetwork == UPDATE_TYPE_INCAR && m_pPlayerPed->IsInVehicle())
			{
				if(!m_pCurrentVehicle) return;

				// 0.3.7
				CQuaternion::Normalize(&m_icSync.Quat);
				CQuaternion::ConvertQuaternionToMatrix(&matVehicle, &m_icSync.Quat);

				matVehicle.pos.X = m_icSync.vecPos.X;
				matVehicle.pos.Y = m_icSync.vecPos.Y;
				matVehicle.pos.Z = m_icSync.vecPos.Z;

				if(m_pCurrentVehicle->GetModelIndex() == TRAIN_PASSENGER_LOCO ||
					m_pCurrentVehicle->GetModelIndex() == TRAIN_FREIGHT_LOCO ||
					m_pCurrentVehicle->GetModelIndex() == TRAIN_TRAM)
				{
					// TRAIN MATRIX UPDATE
				}
				else
				{
					// GENERIC VEHICLE MATRIX UPDATE
					UpdateInCarMatrixAndSpeed(&matVehicle, &m_icSync.vecPos, &m_icSync.vecMoveSpeed);
					UpdateIncarTargetPosition();
				}

				m_pCurrentVehicle->SetHealth(m_icSync.fCarHealth);
				
				m_byteUpdateFromNetwork = UPDATE_TYPE_NONE;
			}

			// ---- PASSENGER NETWORK PROCESSING ----
			else if(GetState() == PLAYER_STATE_PASSENGER && m_byteUpdateFromNetwork == UPDATE_TYPE_PASSENGER)
			{
				if(!m_pCurrentVehicle) return;
				// UPDATE CURRENT WEAPON

				// FOR INITIALISING PASSENGER DRIVEBY

				m_byteUpdateFromNetwork = UPDATE_TYPE_NONE;
			}

			// ------ PROCESSED FOR ALL FRAMES ----- 
			if(GetState() == PLAYER_STATE_ONFOOT)
			{
				UpdatePedRotation(); // SLERP
				// UpdateHead();
				// ProcessSpecialAction();
				m_pPlayerPed->SetOFKeys(m_ofSync.wKeys, m_ofSync.lrAnalog, m_ofSync.udAnalog);
				// test
				m_pPlayerPed->SetMoveSpeedVector(m_ofSync.vecMoveSpeed);
			}
			else if(GetState() == PLAYER_STATE_DRIVER && m_pPlayerPed->IsInVehicle())
			{
				if(!m_pCurrentVehicle) return;
				UpdateVehicleRotation();
				m_pPlayerPed->SetICKeys(m_icSync.wKeys, m_icSync.lrAnalog, m_icSync.udAnalog);
			}
			else if(GetState() == PLAYER_STATE_PASSENGER)
			{
				m_pPlayerPed->SetICKeys(0, 0, 0);
				m_pPlayerPed->SetOFKeys(0, 0, 0);
			}

			m_pPlayerPed->SetHealth(100.0f);
		}
	}
	else
	{
		if(m_pPlayerPed)
		{
			ResetAllSyncAttributes();
			m_pPlayerPed->Destroy();
			delete m_pPlayerPed;
			m_pPlayerPed = 0;
		}
	}
}

//////////////////////////////////////////////////////
void CRemotePlayer::HandleVehicleEntryExit()
{
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	MATRIX4X4 mat;

	if(!m_pPlayerPed) return;

	if(GetState() == PLAYER_STATE_ONFOOT) 
	{
		if(m_pPlayerPed->IsInVehicle()) 
		{
			m_pPlayerPed->GetMatrix(&mat);
			m_pPlayerPed->RemoveFromVehicleAndPutAt(mat.pos.X,mat.pos.Y,mat.pos.Z);
		}
		return;
	}

	if( GetState() == PLAYER_STATE_DRIVER || GetState() == PLAYER_STATE_PASSENGER )
	{
		if(!m_pPlayerPed->IsInVehicle())
		{
			// if they're onfoot, put them in the vehicle.
			CVehicle *pVehicle = pVehiclePool->GetAt(m_VehicleID);
			if(pVehicle) 
			{
				int iCarID = pVehiclePool->FindGtaIDFromID(m_VehicleID);
				m_pPlayerPed->PutDirectlyInVehicle(iCarID,m_byteSeatID);
			}
		} 
		else 
		{
			// validate the vehicle they're driving is the one they're
			// reporting to be in.
			int iCurrentVehicle = m_pPlayerPed->GetCurrentVehicleID();
			int iReportedVehicle = pVehiclePool->FindGtaIDFromID(m_VehicleID);
			if(iCurrentVehicle != iReportedVehicle)
				m_pPlayerPed->PutDirectlyInVehicle(iReportedVehicle,m_byteSeatID);
		}
	}
}

void CRemotePlayer::UpdatePedRotation()
{
	MATRIX4X4 matEnt;

	if(!m_pPlayerPed) return;

	m_pPlayerPed->GetMatrix(&matEnt);
	//ConvertQuaternionToMatrix(&matEnt, &m_ofSync.Quat);
	CQuaternion::ConvertQuaternionToMatrix(&matEnt, &m_ofSync.Quat);
	m_pPlayerPed->SetMatrix(matEnt);
	// atan2(r21, r11)
	float fZ = atan2(-matEnt.up.X, matEnt.right.X);

	m_pPlayerPed->m_pPed->fRotation1 = fZ;
	m_pPlayerPed->m_pPed->fRotation2 = fZ;
}

// 0.3.7
void CRemotePlayer::UpdateVehicleRotation()
{
	MATRIX4X4 matEnt;
	VECTOR vec = { 0, 0, 0 };
	QUATERNION q;

	if(!m_pCurrentVehicle) return;

	m_pCurrentVehicle->GetTurnSpeedVector(&vec);
	if(vec.X <= 0.02f)
	{
		if(vec.X < -0.02f)
			vec.X = -0.02f;
	}
	else
		vec.X = 0.02f;

	if (vec.Y <= 0.02f)
    {
        if(vec.Y < -0.02f)
          vec.Y = -0.02f;
  	}
      else
        vec.Y = 0.02f;

    if(vec.Z <= 0.02f)
    {
    	if(vec.Z < -0.02f)
          vec.Z = -0.02f;
  	}
  	else
  		vec.Z = 0.02f;

  	m_pCurrentVehicle->SetTurnSpeedVector(vec);

  	m_pCurrentVehicle->GetMatrix(&matEnt);
  	CQuaternion::ConvertMatrixToQuaternion(&q, &matEnt);
  	CQuaternion::Normalize(&q);
  	QUATERNION slerp;
  	CQuaternion::Slerp(&slerp, q, m_IncarQuaternion, 0.75f);
  	CQuaternion::Normalize(&slerp);
  	CQuaternion::ConvertQuaternionToMatrix(&matEnt, &slerp); // unk();
  	m_pCurrentVehicle->SetMatrix(matEnt);
}

void CRemotePlayer::UpdateOnfootTargetPosition()
{
	MATRIX4X4 matEnt;
	VECTOR vec;

	if(!m_pPlayerPed) return;
	m_pPlayerPed->GetMatrix(&matEnt);

	float fx = FloatOffset(m_vecOnfootTargetPos.X, matEnt.pos.X);
	float fy = FloatOffset(m_vecOnfootTargetPos.Y, matEnt.pos.Y);
	float fz = FloatOffset(m_vecOnfootTargetPos.Z, matEnt.pos.Z);

	if((fx > 0.00001f) || (fy > 0.00001f) || (fz > 0.00001f))
	{
		if( fx > 2.0 || fy > 2.0 || fz > 1.0 )
		{
			matEnt.pos.X = m_vecOnfootTargetPos.X;
			matEnt.pos.Y = m_vecOnfootTargetPos.Y;
			matEnt.pos.Z = m_vecOnfootTargetPos.Z;

			m_pPlayerPed->SetMatrix(matEnt);
			return;
		}

		m_pPlayerPed->GetMoveSpeedVector(&vec);

		float fMultiplyAmount = 0.1f;

		if( fx > 0.00001f )
		vec.X += (m_vecOnfootTargetPos.X - matEnt.pos.X) * fMultiplyAmount;
		if( fy > 0.00001f )
		vec.Y += (m_vecOnfootTargetPos.Y - matEnt.pos.Y) * fMultiplyAmount;
		if( fz > 0.00001f )
		vec.Z += (m_vecOnfootTargetPos.Z - matEnt.pos.Z) * fMultiplyAmount;

		m_pPlayerPed->SetMoveSpeedVector(vec);
	}
}

void CRemotePlayer::UpdateOnFootPositionAndSpeed(VECTOR * vecPos, VECTOR * vecMove)
{
    m_vecOnfootTargetPos.X = vecPos->X;
	m_vecOnfootTargetPos.Y = vecPos->Y;
	m_vecOnfootTargetPos.Z = vecPos->Z;
    m_vecOnfootTargetSpeed.X = vecMove->X;
	m_vecOnfootTargetSpeed.Y = vecMove->Y;
	m_vecOnfootTargetSpeed.Z = vecMove->Z;
}

// 0.3.7
void CRemotePlayer::UpdateIncarTargetPosition()
{
	MATRIX4X4 matEnt, mat2;
	VECTOR vec = { 0.0f, 0.0f, 0.0f };
	float ofZ = 0.5;

	if(!m_pCurrentVehicle) return;

	m_pCurrentVehicle->GetMatrix(&matEnt);
	if(/*m_pCurrentVehicle->IsAdded()*/ true)
	{
		m_vecIncarTargetPosOffset.X = FloatOffset(m_vecIncarTargetPos.X, matEnt.pos.X);
		m_vecIncarTargetPosOffset.Y = FloatOffset(m_vecIncarTargetPos.Y, matEnt.pos.Y);
		m_vecIncarTargetPosOffset.Z = FloatOffset(m_vecIncarTargetPos.Z, matEnt.pos.Z);

		if(m_vecIncarTargetPosOffset.X > 0.05f || 
			m_vecIncarTargetPosOffset.Y > 0.05f || 
			m_vecIncarTargetPosOffset.Z > 0.05f)
		{
			/*if (m_pCurrentVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_BOAT ||
				m_pCurrentVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_PLANE ||
				m_pCurrentVehicle->GetVehicleSubtype() == VEHICLE_SUBYYPE_HELI)
					ofZ = 2.0;*/

			if(m_vecIncarTargetPosOffset.X > 8.0f ||
				m_vecIncarTargetPosOffset.Y > 8.0f ||
				m_vecIncarTargetPosOffset.Z > ofZ)
			{
				matEnt.pos.X = m_vecIncarTargetPos.X;
				matEnt.pos.Y = m_vecIncarTargetPos.Y;
				matEnt.pos.Z = m_vecIncarTargetPos.Z;

				memcpy(&mat2, &matEnt, sizeof(MATRIX4X4));
				m_pCurrentVehicle->SetMatrix(mat2);
				m_pCurrentVehicle->SetMoveSpeedVector(m_vecIncarTargetSpeed);
			}
			else
			{
				m_pCurrentVehicle->GetMoveSpeedVector(&vec);
				
				if(m_vecIncarTargetPosOffset.X > 0.05f)
					vec.X += (m_vecIncarTargetPos.X - matEnt.pos.X) * 0.06f;
				if(m_vecIncarTargetPosOffset.Y > 0.05f)
					vec.Y += (m_vecIncarTargetPos.Y - matEnt.pos.Y) * 0.06f;
				if(m_vecIncarTargetPosOffset.Z > 0.05f)
					vec.Z += (m_vecIncarTargetPos.Z - matEnt.pos.Z) * 0.06f;
				if(FloatOffset(vec.X, 0.0f) > 0.01f ||
					FloatOffset(vec.Y, 0.0f) > 0.01f ||
					FloatOffset(vec.Z, 0.0f) > 0.01f)
						m_pCurrentVehicle->SetMoveSpeedVector(vec);
			}
		}
	}
	else
	{
		matEnt.pos.X = m_vecIncarTargetPos.X;
		matEnt.pos.Y = m_vecIncarTargetPos.Y;
		matEnt.pos.Z = m_vecIncarTargetPos.Z;
		memcpy(&mat2, &matEnt, sizeof(MATRIX4X4));
		m_pCurrentVehicle->SetMatrix(mat2);
	}
}

// 0.3.7
void CRemotePlayer::UpdateInCarMatrixAndSpeed(MATRIX4X4 *mat, VECTOR *vecPos, VECTOR *vecMoveSpeed)
{
    //RtQuatConvertFromMatrix(mat, &m_IncarQuaternion);
    CQuaternion::ConvertMatrixToQuaternion(&m_IncarQuaternion, mat);
    CQuaternion::Normalize(&m_IncarQuaternion);

	m_vecIncarTargetPos.X = vecPos->X;
	m_vecIncarTargetPos.Y = vecPos->Y;
	m_vecIncarTargetPos.Z = vecPos->Z;

	m_vecIncarTargetSpeed.X = vecMoveSpeed->X;
	m_vecIncarTargetSpeed.Y = vecMoveSpeed->Y;
	m_vecIncarTargetSpeed.Z = vecMoveSpeed->Z;

	m_pCurrentVehicle->SetMoveSpeedVector(*vecMoveSpeed);
}

void CRemotePlayer::StoreOnFootFullSyncData(ONFOOT_SYNC_DATA *pofSync)
{
	if(GetTickCount() < m_dwWaitForEntryExitAnims) return;

	m_pCurrentVehicle = 0;
	memcpy(&m_ofSync,pofSync,sizeof(ONFOOT_SYNC_DATA));
	m_fReportedHealth = (float)pofSync->byteHealth;
	m_fReportedArmour = (float)pofSync->byteArmour;
	m_byteUpdateFromNetwork = UPDATE_TYPE_ONFOOT;

	SetState(PLAYER_STATE_ONFOOT);
}

void CRemotePlayer::StoreInCarFullSyncData(INCAR_SYNC_DATA *picSync)
{
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();

	if(GetTickCount() < m_dwWaitForEntryExitAnims) return;

	//if(a3 || a3 - *(dword*)(this+457) < 0) return; ???
	// *(dword*)(this+457) = a3; ???

	memcpy(&m_icSync, picSync, sizeof(INCAR_SYNC_DATA));
	m_VehicleID = picSync->VehicleID;
	if(pVehiclePool) m_pCurrentVehicle = pVehiclePool->GetAt(m_VehicleID);

	m_byteSeatID = 0;
	m_fReportedHealth = (float)picSync->bytePlayerHealth;
	m_fReportedArmour = (float)picSync->bytePlayerArmour;
	m_byteUpdateFromNetwork = UPDATE_TYPE_INCAR;

	SetState(PLAYER_STATE_DRIVER);
}

void CRemotePlayer::StorePassengerFullSyncData(PASSENGER_SYNC_DATA *ppsSync)
{
	if(GetTickCount() < m_dwWaitForEntryExitAnims) return;

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();

	memcpy(&m_psSync,ppsSync,sizeof(PASSENGER_SYNC_DATA));
	m_VehicleID = ppsSync->VehicleID;
	m_byteSeatID = ppsSync->byteSeatFlags & 127;
	if (pVehiclePool) m_pCurrentVehicle = pVehiclePool->GetAt(m_VehicleID);
	m_fReportedHealth = (float)ppsSync->bytePlayerHealth;
	m_fReportedArmour = (float)ppsSync->bytePlayerArmour;
	m_byteUpdateFromNetwork = UPDATE_TYPE_PASSENGER;

	SetState(PLAYER_STATE_PASSENGER);
}

bool CRemotePlayer::Spawn(uint8_t byteTeam, int iSkin, VECTOR *vecPos, float fRotation, uint32_t dwColor, uint8_t byteFightingStyle, bool bVisible)
{
	//if(!pGame->IsGameLoaded()) return false;
	
	if(m_pPlayerPed != 0)
	{
		m_pPlayerPed->Destroy();
		delete m_pPlayerPed;
		m_pPlayerPed = 0;
	}

	LOGI("pGame->NewPlayer");
	CPlayerPed *pPlayer = pGame->NewPlayer(iSkin, vecPos->X, vecPos->Y, vecPos->Z, fRotation);

	if(pPlayer)
	{
		if(dwColor!=0) SetPlayerColor(dwColor);
		if(pNetGame->m_iShowPlayerMarkers) pPlayer->ShowMarker(m_PlayerID);

		m_pPlayerPed = pPlayer;
		m_fReportedHealth = 100.0f;
		pPlayer->SetOFKeys(0, 0, 0);
		//pPlayer->SetFightingStyle(byteFightingStyle);
		pPlayer->SetVisible(bVisible);

		SetState(PLAYER_STATE_SPAWNED);

		return true;
	}

	SetState(PLAYER_STATE_NONE);
	return false;
}

void CRemotePlayer::Remove()
{
	if(m_pPlayerPed != 0) 
	{
		m_pPlayerPed->Destroy();
		delete m_pPlayerPed;
		m_pPlayerPed = 0;
	}

	Deactivate();
}

void CRemotePlayer::ResetAllSyncAttributes()
{	
	m_VehicleID = 0;
	memset(&m_ofSync,0,sizeof(ONFOOT_SYNC_DATA));
	memset(&m_icSync,0,sizeof(INCAR_SYNC_DATA));
	m_fReportedHealth = 0.0f;
	m_fReportedArmour = 0.0f;
	m_pCurrentVehicle = 0;
	m_byteSeatID = 0;
	//m_bPassengerDriveByMode = false;
}

void CRemotePlayer::SetID(PLAYERID playerId)
{
	m_PlayerID = playerId; 
}

PLAYERID CRemotePlayer::GetID()
{
	return m_PlayerID;
}

CPlayerPed *CRemotePlayer::GetPlayerPed()
{
	return m_pPlayerPed;
}

void CRemotePlayer::SetState(uint8_t byteState)
{
	if(byteState != m_byteState)
	{
		StateChange(byteState, m_byteState);
		m_byteState = byteState;
	}
}

bool CRemotePlayer::IsActive()
{
	if(m_pPlayerPed && m_byteState != PLAYER_STATE_NONE && m_byteState != PLAYER_STATE_SPECTATING)
		return true;

	return false;
}

void CRemotePlayer::Deactivate() 
{
	m_byteState = PLAYER_STATE_NONE;
};

uint8_t CRemotePlayer::GetState()
{
	return m_byteState;
}

void CRemotePlayer::Say(unsigned char *szText)
{
	char * szPlayerName = pNetGame->GetPlayerPool()->GetPlayerName(m_PlayerID);
	pChatWindow->AddChatMessage(szPlayerName,GetPlayerColorAsRGBA(),(char*)szText);
}

void CRemotePlayer::Privmsg(char *szText)
{
	char szStr[256];
	sprintf(szStr, "PM from %s(%d): %s", pNetGame->GetPlayerPool()->GetPlayerName(m_PlayerID), m_PlayerID, szText);
	pChatWindow->AddClientMessage(0xFFDC181A, szStr);
}

void CRemotePlayer::TeamPrivmsg(char *szText)
{
	char szStr[256];
	sprintf(szStr, "Team PM from %s(%d): %s", pNetGame->GetPlayerPool()->GetPlayerName(m_PlayerID), m_PlayerID, szText);
	pChatWindow->AddClientMessage(0xFFDC181A, szStr);
}

float CRemotePlayer::GetDistanceFromLocalPlayer()
{
	if(!m_pPlayerPed) return 10000.0f; // very far away

	if(GetState() == PLAYER_STATE_DRIVER && m_pCurrentVehicle && m_pPlayerPed->IsInVehicle()) {
		return m_pCurrentVehicle->GetDistanceFromLocalPlayerPed();
	} else {
		return m_pPlayerPed->GetDistanceFromLocalPlayerPed();
	}
}

void CRemotePlayer::SetPlayerColor(uint32_t dwColor)
{
	SetRadarColor(m_PlayerID, dwColor);	
}


uint32_t CRemotePlayer::GetPlayerColorAsRGBA()
{
	return TranslateColorCodeToRGBA(m_PlayerID);
}

uint32_t CRemotePlayer::GetPlayerColorAsARGB()
{
	return (TranslateColorCodeToRGBA(m_PlayerID) >> 8) | 0xFF000000;	
}

void CRemotePlayer::EnterVehicle(VEHICLEID VehicleID, bool bPassenger)
{
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();

	if(m_pPlayerPed && !m_pPlayerPed->IsInVehicle())
	{
		if(bPassenger)
			SetState(PLAYER_STATE_ENTER_VEHICLE_PASSENGER);
		else
			SetState(PLAYER_STATE_ENTER_VEHICLE_DRIVER);

		m_pPlayerPed->SetOFKeys(0, 0, 0);
		m_pPlayerPed->SetICKeys(0, 0, 0);
		if(m_pPlayerPed->GetDistanceFromLocalPlayerPed() < 120.0f) 
		{
			int iGtaVehicleID = pVehiclePool->FindGtaIDFromID(VehicleID);
			if(iGtaVehicleID && iGtaVehicleID != INVALID_VEHICLE_ID)
				m_pPlayerPed->EnterVehicle(iGtaVehicleID,bPassenger);
		}
	}

	m_dwWaitForEntryExitAnims = GetTickCount() + 500;
}

void CRemotePlayer::ExitVehicle()
{
	if(m_pPlayerPed && m_pPlayerPed->IsInVehicle()) 
	{
		SetState(PLAYER_STATE_EXIT_VEHICLE);
		m_pPlayerPed->SetOFKeys(0, 0, 0);
		m_pPlayerPed->SetICKeys(0, 0, 0);
		if(m_pPlayerPed->GetDistanceFromLocalPlayerPed() < 120.0f)
			m_pPlayerPed->ExitCurrentVehicle();
	}

	m_dwWaitForEntryExitAnims = GetTickCount() + 500;
}

// sub_10011030
void CRemotePlayer::SetupGlobalMarker(short x, short y, short z)
{
	m_bGlobalMarkerLoaded = 1;

	if(m_dwGlobalMarker)
	{
		DisableMarker_037(m_dwGlobalMarker);
		m_dwGlobalMarker = 0;
	}

	if(!m_pPlayerPed)
	{		
		m_dwGlobalMarker = pGame->CreateRadarMarkerIcon(0, (float)x, (float)y, (float)z, m_PlayerID);

		m_sGlobalMarkerPos[0] = x;
		m_sGlobalMarkerPos[1] = y;
		m_sGlobalMarkerPos[2] = z;
	}
}

// sub_10010FF0
void CRemotePlayer::HideGlobalMarker(bool bHide)
{
	if(!bHide && m_dwGlobalMarker)
	{
		DisableMarker_037(m_dwGlobalMarker);
		m_dwGlobalMarker = 0;
	}

	m_bGlobalMarkerLoaded = false;
}

// CALLBACK
void CRemotePlayer::StateChange(uint8_t byteNewState, uint8_t byteOldState)
{
	if(byteNewState == PLAYER_STATE_DRIVER && byteOldState == PLAYER_STATE_ONFOOT)
	{
		CPlayerPed *pLocalPlayerPed = pGame->FindPlayerPed();
		CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
		VEHICLEID LocalVehicle = INVALID_VEHICLE_ID;
		MATRIX4X4 mat;

		if(pLocalPlayerPed && pLocalPlayerPed->IsInVehicle() && !pLocalPlayerPed->IsAPassenger() && pVehiclePool)
		{
			LocalVehicle = pVehiclePool->FindIDFromGtaPtr(pLocalPlayerPed->GetGtaVehicle());
			if(LocalVehicle == m_VehicleID)
			{
				pLocalPlayerPed->GetMatrix(&mat);
				pLocalPlayerPed->RemoveFromVehicleAndPutAt(mat.pos.X,mat.pos.Y,mat.pos.Z + 1.0f);
				pGame->DisplayGameText("~r~Car Jacked~w~!",1000,5);
			}
		}
	}
}