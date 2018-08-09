#pragma once

#define INVALID_VEHICLE_ID			0xFFFF

#pragma pack(1)
typedef struct _NEW_VEHICLE
{
	VEHICLEID 	VehicleId;
	int		    iVehicleType;
	VECTOR	  	vecPos;
	float	    fRotation;
	char	    aColor1;
	char	    aColor2;
	float	    fHealth;
	uint8_t	  	byteInterior;
	uint8_t	  	byteDoorsLocked;
	uint32_t	dwDoorDamageStatus;
	uint32_t  	dwPanelDamageStatus;
	uint8_t	  	byteLightDamageStatus;
} NEW_VEHICLE;

class CVehiclePool
{
public:
	CVehiclePool();
	~CVehiclePool();

	bool New(NEW_VEHICLE *pNewVehicle);
	bool Delete(VEHICLEID VehicleID);

	CVehicle* GetAt(VEHICLEID VehicleID) 
	{
		if(VehicleID >= MAX_VEHICLES || !m_bVehicleSlotState[VehicleID])
			return 0;
		return m_pVehicles[VehicleID];
	};

	bool GetSlotState(VEHICLEID VehicleID) 
	{
		if(VehicleID >= MAX_VEHICLES)
			return false;
		return m_bVehicleSlotState[VehicleID];
	};

	VEHICLEID FindIDFromGtaPtr(VEHICLE_TYPE *pGtaVehicle);
	int FindGtaIDFromID(int iID);

	void Process();

	int FindNearestToLocalPlayerPed();

private:
	bool 			m_bVehicleSlotState[MAX_VEHICLES];
	CVehicle 		*m_pVehicles[MAX_VEHICLES];
	VEHICLE_TYPE	*m_pGTAVehicles[MAX_VEHICLES];
	bool 			m_bIsActive[MAX_VEHICLES];
	bool 			m_bIsWasted[MAX_VEHICLES];
};