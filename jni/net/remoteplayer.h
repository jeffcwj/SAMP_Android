#pragma once

#define PLAYER_STATE_NONE						0
#define PLAYER_STATE_ONFOOT						1
#define PLAYER_STATE_DRIVER						2
#define PLAYER_STATE_PASSENGER					3
#define PLAYER_STATE_EXIT_VEHICLE				4
#define PLAYER_STATE_ENTER_VEHICLE_DRIVER		5
#define PLAYER_STATE_ENTER_VEHICLE_PASSENGER	6
#define PLAYER_STATE_WASTED						7
#define PLAYER_STATE_SPAWNED					8
#define PLAYER_STATE_SPECTATING					9

#define UPDATE_TYPE_NONE		0
#define UPDATE_TYPE_ONFOOT		1
#define UPDATE_TYPE_INCAR		2
#define UPDATE_TYPE_PASSENGER	3

class CRemotePlayer
{
public:
	CRemotePlayer();
	~CRemotePlayer();

	void UpdatePedRotation();
	void UpdateVehicleRotation();

	void StateChange(uint8_t byteNewState, uint8_t byteOldState);
	void SetState(uint8_t byteState);
	uint8_t GetState();

	void Process();

	void HandleVehicleEntryExit();

	void Say(unsigned char * szText);
	void Privmsg(char* szText);
	void TeamPrivmsg(char* szText);

	void SetID(PLAYERID playerId);
	PLAYERID GetID();
	CPlayerPed *GetPlayerPed();
	bool IsActive();
	void Deactivate();

	void UpdateIncarTargetPosition();
	void UpdateOnfootTargetPosition();

	void UpdateOnFootPositionAndSpeed(VECTOR * vecPos, VECTOR *vecMoveSpeed);
	void UpdateInCarMatrixAndSpeed(MATRIX4X4 *mat, VECTOR *vecPos, VECTOR *vecMove);

	void StoreOnFootFullSyncData(ONFOOT_SYNC_DATA *pofSync);
	void StoreInCarFullSyncData(INCAR_SYNC_DATA *picSync);
	void StorePassengerFullSyncData(PASSENGER_SYNC_DATA *ppsSync);

	bool Spawn(uint8_t byteTeam, int iSkin, VECTOR *vecPos, float fRotation, uint32_t dwColor, uint8_t byteFightingStyle = 4, bool bVisible = true);
	void Remove();

	void ResetAllSyncAttributes();

	float GetDistanceFromLocalPlayer();

	void SetPlayerColor(uint32_t dwColor);
	uint32_t GetPlayerColorAsRGBA();
	uint32_t GetPlayerColorAsARGB();

	float GetReportedHealth() { return m_fReportedHealth; };

	void EnterVehicle(VEHICLEID VehicleID, bool bPassenger);
	void ExitVehicle();

	// 0.3.7 sub_10011030
	void SetupGlobalMarker(short x, short y, short z);
	// 0.3.7 sub_10010FF0
	void HideGlobalMarker(bool bHide);

	// 0.3.7
	bool 			m_bGlobalMarkerLoaded;
	uint16_t		m_sGlobalMarkerPos[3];
	uint32_t 		m_dwGlobalMarker;

	CPlayerPed		*m_pPlayerPed;
	CVehicle		*m_pCurrentVehicle;

	PLAYERID 		m_PlayerID;
	uint8_t			m_byteUpdateFromNetwork;
	uint8_t			m_byteState;
	VEHICLEID		m_VehicleID;
	uint8_t			m_byteSeatID;

	ONFOOT_SYNC_DATA		m_ofSync;
	INCAR_SYNC_DATA			m_icSync;
	PASSENGER_SYNC_DATA		m_psSync;

	VECTOR			m_vecOnfootTargetPos;
	VECTOR			m_vecOnfootTargetSpeed;
	VECTOR			m_vecIncarTargetPos;
	VECTOR			m_vecIncarTargetSpeed;
	VECTOR 			m_vecIncarTargetPosOffset; // 0.3.7 [+83]
	QUATERNION		m_IncarQuaternion; // 0.3.7 [+95]

	float 			m_fReportedHealth;
	float 			m_fReportedArmour;
	uint32_t		m_dwWaitForEntryExitAnims;	
};
