#pragma once

class CVehicle : public CEntity
{
public:

	CVehicle(int iType, float fPosX, float fPosY, float fPosZ, float fRotation = 0.0f, char* szNumberPlate = 0, bool bKeepModelLoaded = false);
	virtual ~CVehicle();

	float GetHealth();
	void SetHealth(float fHealth);
	void  SetColor(int iColor1, int iColor2);
	bool IsDriverLocalPlayer();

	void SetInvulnerable(bool bInv);
	void ProcessMarkers();

	bool IsOccupied();
	void    Recreate();
	void RemoveEveryoneFromVehicle();

	VEHICLE_TYPE	*m_pVehicle;
	bool			m_bIsInvulnerable;
	int 			m_dwMarkerID;
	
	bool		m_bDoorsLocked; // Vehicle is enterable TRUE/FALSE
};