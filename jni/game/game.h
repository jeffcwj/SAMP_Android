#pragma once

class CGame
{
private:
	CCamera 		*m_pGameCamera;	
	CPlayerPed 		*m_pGamePlayer;

	VECTOR			m_vecCheckpointPos;
	VECTOR			m_vecCheckpointExtent;
	bool			m_bCheckpointsEnabled;
	uint32_t		m_dwCheckpointMarker;

	VECTOR			m_vecRaceCheckpointPos;
	VECTOR			m_vecRaceCheckpointNext;
	float			m_fRaceCheckpointSize;
	uint8_t			m_byteRaceType;
	bool			m_bRaceCheckpointsEnabled;
	uint32_t		m_dwRaceCheckpointMarker;
	uint32_t		m_dwRaceCheckpointHandle;

public:
	CGame();
	~CGame();

	uint8_t FindFirstFreePlayerPedSlot();
	CPlayerPed *NewPlayer(int iSkin, float fPosX, float fPosY, float fPosZ, float fRotation, uint8_t byteCreateMarker = 1);
	CVehicle *NewVehicle(int iType, float fPosX, float fPosY, float fPosZ, float fRotation, char* szNumberPlate);
	CObject *NewObject(int iModel, float fPosX, float fPosY, float fPosZ, VECTOR vecRot, float fDrawDistance);

	float FindGroundZForCoord(float x, float y, float z);
	void InitGame();
	void StartGame();
	
//发送消息
void SendChat(char* szMessage);
//发送命令
void SendCommand(char *szCommand);
//智能判断
void SendMC(char*mc);

	void RequestModel(int iModelID, int iLoadingStream = 2);
	void LoadRequestedModels();
	uint8_t IsModelLoaded(int iModelID);
	void RemoveModel(int iModelID);
	void SetWorldTime(int iHour, int iMinute);
	void	GetWorldTime(int* iHour, int* iMinute);
	void ToggleThePassingOfTime(bool bOnOff);
	void SetWorldWeather(int iWeatherID);
	void DisplayHUD(bool bDisp);
	void RefreshStreamingAt(float x, float y);
	void ToggleRadar(bool iToggle);
	void DisplayGameText(char *szStr, int iTime, int iType);
	void PlaySound(int iSound, float fX, float fY, float fZ);
	void EnableClock(bool byteClock);
	void DrawGangZone(float *fPos, uint32_t dwColor);
	void EnableZoneNames(bool byteEnable);
	void ToggleCJWalk(bool toggle);

	void UpdateCheckpoints();
	void ToggleCheckpoint(bool bEnabled) { m_bCheckpointsEnabled = bEnabled; };
	void SetCheckpointInformation(VECTOR *pos, VECTOR *extent);

//重力修改
//void SetGravity(float fGravity);
	void MakeRaceCheckpoint();
	void DisableRaceCheckpoint();
	void ToggleRaceCheckpoints(bool bEnabled) { m_bRaceCheckpointsEnabled = bEnabled; };
	void SetRaceCheckpointInformation(uint8_t byteType, VECTOR *pos, VECTOR *next, float fSize);

	uint32_t CreateRadarMarkerIcon(int iMarkerType, float fX, float fY, float fZ, int iColor = 201);
	void DisableMarker(uint32_t dwMarkerID);

	void AddToLocalMoney(int iAmount);
	void ResetLocalMoney();
	int GetLocalMoney();

	CCamera *GetCamera() {	return m_pGameCamera; };

	CPlayerPed  *FindPlayerPed() {
	if(m_pGamePlayer==NULL)	m_pGamePlayer = new CPlayerPed();
	return m_pGamePlayer; };

	uint32_t CreatePickup(int iModel, int iType, float fX, float fY, float fZ, int *unk);
};