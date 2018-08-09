#include "main.h"
#include "util.h"

extern CGame *pGame;
extern CNetGame *pNetGame;

// допилить
CVehicle::CVehicle(int iType, float fPosX, float fPosY, float fPosZ, 
					float fRotation, char* szNumberPlate, bool bKeepModelLoaded)
{
	uint32_t dwRetID = 0;

	m_pVehicle = 0;
	m_dwGTAId = 0;

	if( (iType != TRAIN_PASSENGER_LOCO) &&
		(iType != TRAIN_FREIGHT_LOCO) &&
		(iType != TRAIN_PASSENGER) &&
		(iType != TRAIN_FREIGHT) &&
		(iType != TRAIN_TRAM))
	{
		// NORMAL VEHICLE

		if(!pGame->IsModelLoaded(iType))
		{
			pGame->RequestModel(iType);
			pGame->LoadRequestedModels();
		}

		ScriptCommand(&create_car,iType,fPosX,fPosY,fPosZ+0.1f,&dwRetID);
		ScriptCommand(&set_car_z_angle,dwRetID,fRotation);
		//ScriptCommand(&car_gas_tank_explosion,dwRetID,0);
		//ScriptCommand(&set_car_hydraulics,dwRetID,0);
		//ScriptCommand(&toggle_car_tires_vulnerable,dwRetID,0);

		m_pVehicle = GamePool_Vehicle_GetAt(dwRetID);
		m_pEntity = (ENTITY_TYPE *)m_pVehicle; 
		m_dwGTAId = dwRetID;
	}
	else if( (iType == TRAIN_PASSENGER_LOCO) || 
		(iType == TRAIN_FREIGHT_LOCO) ||
		(iType == TRAIN_TRAM))
	{
		// TRAIN LOCOMOTIVES
	}
	else if((iType == TRAIN_PASSENGER) ||
			(iType == TRAIN_FREIGHT) )
	{
		// ???
	}

	m_bIsInvulnerable = false;
	m_dwMarkerID = 0;
}

CVehicle::~CVehicle()
{
	m_pVehicle = GamePool_Vehicle_GetAt(m_dwGTAId);

	if(m_pVehicle)
	{
		if(m_dwMarkerID) 
		{
			ScriptCommand(&disable_marker, m_dwMarkerID);
			m_dwMarkerID = 0;
		}

		RemoveEveryoneFromVehicle();

		if(m_pVehicle->entity.nModelIndex == TRAIN_PASSENGER_LOCO ||
			m_pVehicle->entity.nModelIndex == TRAIN_FREIGHT_LOCO)
		{
			//ScriptCommand(&destroy_train,m_dwGTAId);
		}
		else
		{
			int nModelIndex = m_pVehicle->entity.nModelIndex;
			ScriptCommand(&destroy_car, m_dwGTAId);
			if(!GetModelReferenceCount(nModelIndex))
			{
				LOGI("~CVehicle: RemoveModel(%d)", nModelIndex);
				pGame->RemoveModel(nModelIndex);
				LOGI("true");
			}
		}
	}
}

bool CVehicle::IsOccupied()
{
	if(m_pVehicle)
	{
		if(m_pVehicle->pDriver) return true;

		if(m_pVehicle->pPassengers[0]) return true;
		if(m_pVehicle->pPassengers[1]) return true;
		if(m_pVehicle->pPassengers[2]) return true;
		if(m_pVehicle->pPassengers[3]) return true;
		if(m_pVehicle->pPassengers[4]) return true;
		if(m_pVehicle->pPassengers[5]) return true;
		if(m_pVehicle->pPassengers[6]) return true;
	}
	return false;
}

void CVehicle::SetInvulnerable(bool bInv)
{
	if(!m_pVehicle) return;
	if(!GamePool_Vehicle_GetAt(m_dwGTAId)) return;

	if(bInv)
	{
		ScriptCommand(&set_car_immunities,m_dwGTAId,1,1,1,1,1);
		ScriptCommand(&toggle_car_tires_vulnerable,m_dwGTAId,0);

		m_bIsInvulnerable = true;
	}
	else
	{	
		ScriptCommand(&set_car_immunities,m_dwGTAId,0,0,0,0,0);
		//if (pNetGame && pNetGame->m_bTirePopping)
			ScriptCommand(&toggle_car_tires_vulnerable,m_dwGTAId,1);

		m_bIsInvulnerable = false;
	}
}

float CVehicle::GetHealth()
{	
	if(m_pVehicle) return m_pVehicle->fHealth;
	else return 0.0f;
}

void CVehicle::SetHealth(float fHealth)
{
	if(m_pVehicle)
		m_pVehicle->fHealth = fHealth;
}

bool CVehicle::IsDriverLocalPlayer()
{
	if(m_pVehicle)
	{
		if((PED_TYPE*)m_pVehicle->pDriver == GamePool_FindPlayerPed())
			return true;
	}

	return false;
}

void CVehicle::ProcessMarkers()
{
	if(!m_pVehicle) return;

	if(GetDistanceFromLocalPlayerPed() < CSCANNER_DISTANCE && !IsOccupied())
	{
		// SHOW
		if(!m_dwMarkerID)
		{
			ScriptCommand(&tie_marker_to_car, m_dwGTAId, 1, 2, &m_dwMarkerID);
			ScriptCommand(&set_marker_color,m_dwMarkerID,500);
		}
	}
	else if(IsOccupied() || GetDistanceFromLocalPlayerPed() >= CSCANNER_DISTANCE)
	{
		// REMOVE
		if(m_dwMarkerID)
		{
			ScriptCommand(&disable_marker, m_dwMarkerID);
			m_dwMarkerID = 0;
		}
	}
}

void CVehicle::RemoveEveryoneFromVehicle()
{
	if(!m_pVehicle) return;
	if(!GamePool_Vehicle_GetAt(m_dwGTAId)) return;

	float fPosX = m_pVehicle->entity.mat->pos.X;
	float fPosY = m_pVehicle->entity.mat->pos.Y;
	float fPosZ = m_pVehicle->entity.mat->pos.Z;

	int iPlayerID = 0;
	if(m_pVehicle->pDriver)
	{
		iPlayerID = GamePool_Ped_GetIndex(m_pVehicle->pDriver);
		ScriptCommand( &remove_actor_from_car_and_put_at, iPlayerID, fPosX, fPosY, fPosZ + 2 );
	}

	// REMOVE PASSENGER
	for (int i = 0; i < 7; i++)
    {
		if (m_pVehicle->pPassengers[i] != 0)
        {
			iPlayerID = GamePool_Ped_GetIndex( m_pVehicle->pPassengers[i] );
			ScriptCommand( &remove_actor_from_car_and_put_at, iPlayerID, fPosX, fPosY, fPosZ + 2 );
        }
	}
}