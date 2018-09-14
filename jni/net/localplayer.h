#pragma once

#pragma pack(1)
typedef struct _PLAYER_SPAWN_INFO
{
	uint8_t byteTeam;
	int iSkin;
	uint8_t unk;
	VECTOR vecPos;
	float fRotation;
	int iSpawnWeapons[3];
	int iSpawnWeaponsAmmo[3];
} PLAYER_SPAWN_INFO;

#pragma pack(1)
typedef struct _ONFOOT_SYNC_DATA
{
	uint16_t lrAnalog;
	uint16_t udAnalog;
	uint16_t wKeys;
	VECTOR vecPos;
	QUATERNION Quat;
	uint8_t byteHealth;
	uint8_t byteArmour;
	uint8_t byteCurrentWeapon;
	uint8_t byteSpecialAction;
	VECTOR vecMoveSpeed;
	VECTOR vecSurfOffsets;
	uint16_t wSurfInfo;
	int	iCurrentAnimationID;
} ONFOOT_SYNC_DATA;

#pragma pack(1)
typedef struct _INCAR_SYNC_DATA
{
	VEHICLEID VehicleID;
	uint16_t lrAnalog;
	uint16_t udAnalog;
	uint16_t wKeys;
	QUATERNION Quat;
	VECTOR vecPos;
	VECTOR vecMoveSpeed;
	float fCarHealth;
	uint8_t bytePlayerHealth;
	uint8_t bytePlayerArmour;
	uint8_t byteCurrentWeapon;
	uint8_t byteSirenOn;
	uint8_t byteLandingGearState;
	uint16_t TrailerIDorHydraThrustAngle;
	float fTrainSpeed;
} INCAR_SYNC_DATA;

#pragma pack(1)
typedef struct _PASSENGER_SYNC_DATA
{
	VEHICLEID VehicleID;
	uint8_t byteSeatFlags : 7;
	uint8_t byteDriveBy : 1;
	uint8_t byteCurrentWeapon;
	uint8_t bytePlayerHealth;
	uint8_t bytePlayerArmour;
	uint16_t lrAnalog;
	uint16_t udAnalog;
	uint16_t wKeys;
	VECTOR vecPos;
} PASSENGER_SYNC_DATA;

class CLocalPlayer
{
public:
	CLocalPlayer();
	~CLocalPlayer(){};

	void HandlePassengerEntry();
	void Process();

	void SendOnFootFullSyncData();
	void SendInCarFullSyncData();
	void SendPassengerFullSyncData();
	void ResetAllSyncAttributes();

	int GetOptimumInCarSendRate();
	int GetOptimumOnFootSendRate();

	void RequestClass(int iClass);
	void RequestSpawn();

	void SetSpawnInfo(PLAYER_SPAWN_INFO *pSpawn);

	bool Spawn();
	//发送死亡
void SendWastedNotification();
	CPlayerPed * GetPlayerPed() { return m_pPlayerPed; };

	void SendEnterVehicleNotification(VEHICLEID VehicleID, bool bPassenger);
	void SendExitVehicleNotification(VEHICLEID VehicleID);
	void SetPlayerColor(uint32_t dwColor);
	uint32_t GetPlayerColorAsRGBA();
	uint32_t GetPlayerColorAsARGB();

	void HandleClassSelectionOutcome();

	CPlayerPed			*m_pPlayerPed;
	bool				m_bIsActive;
	bool				m_bIsWasted;

	bool 				m_bWaitingForSpawnRequestReply;

	ONFOOT_SYNC_DATA 	m_ofSync;
	INCAR_SYNC_DATA 	m_icSync;

public:
	PLAYER_SPAWN_INFO 	m_SpawnInfo;
	bool				m_bHasSpawnInfo;
	uint32_t			m_dwLastSendTick;

	bool				m_bIsSpectating;


	VEHICLEID			m_CurrentVehicle;
	VEHICLEID			m_LastVehicle;
};
