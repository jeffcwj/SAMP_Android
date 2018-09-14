#pragma once

class CPlayerPed : public CEntity
{
public:
	CPlayerPed();
	CPlayerPed(uint8_t bytePlayerNumber, int iSkin, float fPosX, float fPosY, float fPosZ, float fRotation = 0.0f);
	~CPlayerPed();

	void SetInitialState();

	void SetOFKeys(uint16_t wKeys, uint16_t lrAnalog, uint16_t udAnalog);
	void SetICKeys(uint16_t wKeys, uint16_t lrAnalog, uint16_t udAnalog);

	uint16_t GetOFKeys(uint16_t *lrAnalog, uint16_t *udAnalog);
	uint16_t GetICKeys(uint16_t *lrAnalog, uint16_t *udAnalog);
	void Destroy();
	void ShowMarker(int iMarkerColorID);
	void HideMarker();
	int GetCurrentVehicleID();
	float GetHealth();
	void  SetHealth(float fHealth);
	float GetArmour();
	void SetArmour(float fArmour);
	bool IsInVehicle();
	uint8_t GetActionTrigger();
	void  SetTargetRotation(float fRotation);
	void ForceTargetRotation(float fRotation);

	void PutDirectlyInVehicle(int iVehicleID, int iSeat);
	void EnterVehicle(int iVehicleID, bool bPassenger);
	void ExitCurrentVehicle();
	void RemoveFromVehicleAndPutAt(float fX, float fY, float fZ);

	VEHICLE_TYPE *GetGtaVehicle();
	PED_TYPE *GetGtaActor() { return m_pPed; };
ENTITY_TYPE * GetGtaContactEntity();
	
	bool IsAPassenger();

	void SetVisible(bool bVisible);

	int GetVehicleSeatID();
	void RestartIfWastedAt(VECTOR *vecRestart, float fRotation);

	void SetModelIndex(unsigned int uiModel);

	void SetInterior(uint8_t byteID);
	bool IsOnGround();

	void TogglePlayerControllable(int iControllable);

	PED_TYPE	*m_pPed;
	uint8_t		m_bytePlayerNumber;

	bool		m_bVisible;

	uint32_t 	m_dwArrow;
};