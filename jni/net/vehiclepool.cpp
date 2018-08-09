#include "main.h"
#include "game/util.h"

extern CGame *pGame;
extern CNetGame *pNetGame;

CVehiclePool::CVehiclePool()
{
	for(VEHICLEID VehicleID = 0; VehicleID < MAX_VEHICLES; VehicleID++) 
	{
		m_bVehicleSlotState[VehicleID] = false;
		m_pVehicles[VehicleID] = 0;
		m_pGTAVehicles[VehicleID] = 0;
	}
}

CVehiclePool::~CVehiclePool()
{
	for(VEHICLEID VehicleID = 0; VehicleID < MAX_VEHICLES; VehicleID++) 
	{
		Delete(VehicleID);
	}
}

bool CVehiclePool::New(NEW_VEHICLE *pNewVehicle)
{
	if(m_pVehicles[pNewVehicle->VehicleId] != 0)
	{
		LOGI("Warning: vehicle %u was not deleted", pNewVehicle->VehicleId);
		Delete(pNewVehicle->VehicleId);
	}

	m_pVehicles[pNewVehicle->VehicleId] = pGame->NewVehicle(pNewVehicle->iVehicleType, 
															pNewVehicle->vecPos.X,pNewVehicle->vecPos.Y,pNewVehicle->vecPos.Z, 
															pNewVehicle->fRotation, NULL);

	if(m_pVehicles[pNewVehicle->VehicleId])
	{
		m_pGTAVehicles[pNewVehicle->VehicleId] = m_pVehicles[pNewVehicle->VehicleId]->m_pVehicle;
		m_bVehicleSlotState[pNewVehicle->VehicleId] = true;

		m_bIsActive[pNewVehicle->VehicleId] = true;
		m_bIsWasted[pNewVehicle->VehicleId] = false;

		return true;
	}

	return false;
}

bool CVehiclePool::Delete(VEHICLEID VehicleID)
{
	if(!GetSlotState(VehicleID) || !m_pVehicles[VehicleID])
		return false;

	m_bVehicleSlotState[VehicleID] = false;
	delete m_pVehicles[VehicleID];
	m_pVehicles[VehicleID] = 0;
	m_pGTAVehicles[VehicleID] = 0;
	
	return true;
}

VEHICLEID CVehiclePool::FindIDFromGtaPtr(VEHICLE_TYPE* pGtaVehicle)
{
	int x = 1;

	while(x != MAX_VEHICLES)
	{
		if(pGtaVehicle == m_pGTAVehicles[x]) return x;
		x++;
	}

	return INVALID_VEHICLE_ID;
}

int CVehiclePool::FindGtaIDFromID(int iID)
{
	if(m_pGTAVehicles[iID])
		return GamePool_Vehicle_GetIndex(m_pGTAVehicles[iID]);
	else
		return INVALID_VEHICLE_ID;
}

void CVehiclePool::Process()
{
	CVehicle *pVehicle;

	for(VEHICLEID x = 0; x != MAX_VEHICLES; x++)
	{
		if(GetSlotState(x) == true)
		{
			pVehicle = m_pVehicles[x];

			if(m_bIsActive[x])
			{
				if(pVehicle->IsDriverLocalPlayer())
					pVehicle->SetInvulnerable(false);
				else
					pVehicle->SetInvulnerable(true);
			}

			if(pVehicle->m_pVehicle != m_pGTAVehicles[x])
				m_pGTAVehicles[x] = pVehicle->m_pVehicle;

			pVehicle->ProcessMarkers();
		}
	}
}

int CVehiclePool::FindNearestToLocalPlayerPed()
{
	float fLeastDistance = 10000.0f;
	float fThisDistance;
	VEHICLEID ClosestSoFar = INVALID_VEHICLE_ID;

	VEHICLEID x = 0;
	while(x < MAX_VEHICLES)
	{
		if(GetSlotState(x) && m_bIsActive[x])
		{
			fThisDistance = m_pVehicles[x]->GetDistanceFromLocalPlayerPed();
			if(fThisDistance < fLeastDistance) 
			{
				fLeastDistance = fThisDistance;
				ClosestSoFar = x;
			}
		}
		x++;
	}
	
	return ClosestSoFar;
}