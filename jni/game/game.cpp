#include "main.h"
#include "game.h"
#include "util.h"
#include "keystuff.h"
#include "util.h"

void ApplyPreGamePatches();
void ApplyInGamePatches();
void GameInstallHooks();
void InstallSpecialHooks();
void InitScripting();

uint8_t byteUsedPlayerSlots[PLAYER_PED_SLOTS];

CGame::CGame()
{
	m_pGameCamera = new CCamera();
	m_pGamePlayer = 0;
	m_bCheckpointsEnabled = false;
	m_bRaceCheckpointsEnabled = false;
	m_dwRaceCheckpointHandle = 0;

	memset(&byteUsedPlayerSlots[0], 0, PLAYER_PED_SLOTS);
}


CGame::~CGame()
{

}

uint8_t CGame::FindFirstFreePlayerPedSlot()
{
	uint8_t x=2;
	while(x!=PLAYER_PED_SLOTS)
	{
		if(!byteUsedPlayerSlots[x]) return x;
		x++;
	}
	return 0;
}

CPlayerPed *CGame::NewPlayer(int iSkin, float fPosX, float fPosY, float fPosZ, float fRotation, uint8_t byteCreateMarker)
{
	LOGI("CGame::NewPlayer(skin: %d)", iSkin);
	uint8_t bytePlayerNum =FindFirstFreePlayerPedSlot();
	if(!bytePlayerNum) return 0;

	CPlayerPed *pPlayerNew = new CPlayerPed(bytePlayerNum, iSkin, fPosX, fPosY, fPosZ, fRotation);
	if(pPlayerNew && pPlayerNew->m_pPed) byteUsedPlayerSlots[bytePlayerNum]++;

	return pPlayerNew;
}

CVehicle *CGame::NewVehicle(int iType, float fPosX, float fPosY, float fPosZ, float fRotation, char *szNumberPlate)
{
	LOGI("CGame::NewVehicle(type: %d)", iType);
	CVehicle *pVehicleNew = new CVehicle(iType, fPosX, fPosY, fPosZ, fRotation, szNumberPlate, 0);

	return pVehicleNew;
}

CObject *CGame::NewObject(int iModel, float fPosX, float fPosY, float fPosZ, VECTOR vecRot, float fDrawDistance)
{
	CObject *pObjectNew = new CObject(iModel, fPosX, fPosY, fPosZ, vecRot, fDrawDistance);
	return pObjectNew;
}

float CGame::FindGroundZForCoord(float x, float y, float z)
{
	float fGroundZ = 0.0f;
	ScriptCommand(&get_ground_z, x, y, z, &fGroundZ);
	return fGroundZ;
}

void CGame::InitGame()
{
	InstallSpecialHooks();

	GameKeyStatesInit();

	GameResetRadarColors();
	
	ApplyPreGamePatches();
}

void CGame::StartGame()
{
	ApplyInGamePatches();

	GameInstallHooks();

	InitScripting();
}

void CGame::RequestModel(int iModelID, int iLoadingStream)
{
	uint32_t (*RequestModel)(int iModelID, int iLoadingStream);
    *(void **) (&RequestModel) = (void*)(g_libGTASA+0x28EB10+1);
    (*RequestModel)(iModelID, iLoadingStream);
}

void CGame::LoadRequestedModels()
{
	uint32_t (*LoadAllRequestedModels)(bool);
    *(void **) (&LoadAllRequestedModels) = (void*)(g_libGTASA+0x294CB4+1);
    LoadAllRequestedModels(0);
}

uint8_t CGame::IsModelLoaded(int iModelID)
{
	uint32_t StreamingInfo_LoadState = (uint32_t)(0x6706D8+g_libGTASA);
  	uint8_t ret = *(uint8_t*)(StreamingInfo_LoadState+0x14*iModelID+0x10);
	return ret;
}

void CGame::RemoveModel(int iModelID)
{
	void (*RemoveModel)(int iModelID);
    *(void **) (&RemoveModel) = (void*)(g_libGTASA+0x290C4C+1);
    (*RemoveModel)(iModelID);
}

void CGame::SetWorldTime(int iHour, int iMinute)
{
	*(uint8_t*)(g_libGTASA+0x8B18A4) = (uint8_t)iMinute;
	*(uint8_t*)(g_libGTASA+0x8B18A5) = (uint8_t)iHour;
}

void CGame::ToggleThePassingOfTime(bool bOnOff)
{
	uint8_t org[] = { 0x2D, 0xE9 };
	uint8_t ret[] = { 0xF7, 0x46 };

	if(bOnOff)
		WriteMemory(g_libGTASA+0x38C154, (const char*)org, 2);
	else
		WriteMemory(g_libGTASA+0x38C154, (const char*)ret, 2);
}

void CGame::SetWorldWeather(int iWeatherID)
{
	*(uint16_t*)(g_libGTASA+0x9DB990) = iWeatherID;
	*(uint16_t*)(g_libGTASA+0x9DB992) = iWeatherID;
}

void CGame::DisplayHUD(bool bDisp)
{
	if(bDisp)
	{	
		// CTheScripts11bDisplayHud
		*(uint8_t*)(g_libGTASA+0x7165E8) = 1;
		ToggleRadar(1);
	} else {
		*(uint8_t*)(g_libGTASA+0x7165E8) = 0;
		ToggleRadar(0);
	}
}

void CGame::RefreshStreamingAt(float x, float y)
{
	ScriptCommand(&refresh_streaming_at,x,y);
}

void CGame::ToggleRadar(bool iToggle)
{
	*(uint8_t*)(g_libGTASA+0x8EF36B) = (uint8_t)!iToggle;
}

uint16_t szGameTextMessage[256];
void CGame::DisplayGameText(char *szStr, int iTime, int iType)
{
	LOGI("CGame::DisplayGameText (%s)", szStr);

	ScriptCommand(&text_clear_all);
	//strcpy(szGameTextMessage, szStr);
	memset(szGameTextMessage, 0, sizeof(szGameTextMessage));
	CFont::AsciiToGxtChar(szStr, szGameTextMessage);

	uint32_t (*CMessages__AddBigMessage)(uint16_t *text, int time, int type);
    *(void **) (&CMessages__AddBigMessage) = (void*)(g_libGTASA+0x4D18C0+1);
    (*CMessages__AddBigMessage)(szGameTextMessage, iTime, iType);	
}

void CGame::PlaySound(int iSound, float fX, float fY, float fZ)
{
	ScriptCommand(&play_sound, fX, fY, fZ, iSound);
}

void CGame::SetCheckpointInformation(VECTOR *pos, VECTOR *extent)
{
	LOGI("CGame::SetCheckpointInformation (pos %3.f, %3.f, %3.f)", pos->X, pos->Y, pos->Z);

	memcpy(&m_vecCheckpointPos, pos, sizeof(VECTOR));
	memcpy(&m_vecCheckpointExtent, extent, sizeof(VECTOR));
	if(m_dwCheckpointMarker)
	{
		DisableMarker(m_dwCheckpointMarker);
		m_dwCheckpointMarker = 0;

		m_dwCheckpointMarker = CreateRadarMarkerIcon(0, m_vecCheckpointPos.X, m_vecCheckpointPos.Y, m_vecCheckpointPos.Z);
	}
}

uint32_t CGame::CreateRadarMarkerIcon(int iMarkerType, float fX, float fY, float fZ, int iColor)
{
	uint32_t dwMarkerID;
	ScriptCommand(&create_radar_marker_without_sphere, fX, fY, fZ, iMarkerType, &dwMarkerID);
	ScriptCommand(&set_marker_color, dwMarkerID, iColor);
	ScriptCommand(&show_on_radar, dwMarkerID, 3);
	return dwMarkerID;
}

void CGame::DisableMarker(uint32_t dwMarkerID)
{
	ScriptCommand(&disable_marker, dwMarkerID);
}

void CGame::AddToLocalMoney(int iAmount)
{
	ScriptCommand(&add_to_player_money, 0, iAmount);
}

void CGame::ResetLocalMoney()
{
	int iMoney = GetLocalMoney();
	if(!iMoney) return;

	if(iMoney < 0)
		AddToLocalMoney(abs(iMoney));
	else
		AddToLocalMoney(-(iMoney));
}
//获取玩家钱
int CGame::GetLocalMoney()
{
	// допилить
	//return *(int*)(g_libGTASA)
	//这里注释掉，可以解决钱一直为0的问题
//	return 0;
}

uint32_t CGame::CreatePickup(int iModel, int iType, float fX, float fY, float fZ, int* unk)
{
	LOGI("CreatePickup(%d, %d, %4.f, %4.f, %4.f)", iModel, iType, fX, fY, fZ);

	uint32_t hnd;

	if(!IsValidModel(iModel)) iModel = 18631;

	if(!ScriptCommand(&is_model_available, iModel))
	{
		ScriptCommand(&request_model, iModel);
		ScriptCommand(&load_requested_models);
		while(!ScriptCommand(&is_model_available, iModel))
			usleep(1000);
	}

	ScriptCommand(&create_pickup, iModel, iType, fX, fY, fZ, &hnd);

	int lol = 32 * (uint16_t)hnd;
	if(lol) lol /= 32;
	if(unk) *unk = lol;

	return hnd;
}

extern uint8_t bGZ;
void CGame::DrawGangZone(float fPos[], uint32_t dwColor)
{
	void (*CRadar__DrawAreaOnRadar)(float *fPos, uint32_t *dwColor, uint8_t byteMenu);
    *(void **) (&CRadar__DrawAreaOnRadar) = (void*)(g_libGTASA+0x3DE7F8+1);
    (*CRadar__DrawAreaOnRadar)(fPos, &dwColor, bGZ);
}

void CGame::EnableClock(bool bClock)
{
	uint8_t byteClockData[] = {'%', '0', '2', 'd', ':', '%', '0', '2', 'd', 0};

	if(bClock)
	{
		ToggleThePassingOfTime(1);
		WriteMemory(g_libGTASA+0x599504, (const char*)byteClockData, 10);
	}
	else
	{
		ToggleThePassingOfTime(0);
		WriteMemory(g_libGTASA+0x599504, "sa:mp", 6);
	}
}

void CGame::EnableZoneNames(bool bEnable)
{
	ScriptCommand(&enable_zone_names, bEnable);
}

void CGame::ToggleCJWalk(bool toggle)
{
	if(toggle)
		WriteMemory(g_libGTASA+0x45477E, "\xC4\xF8\xDC\x64", 4);
	else
	{
		UnFuck(g_libGTASA+0x45477E);
		NOP(g_libGTASA+0x45477E, 2);
	}
}

void CGame::SetRaceCheckpointInformation(uint8_t byteType, VECTOR *pos, VECTOR *next, float fSize)
{
	memcpy(&m_vecRaceCheckpointPos, pos, sizeof(VECTOR));
	memcpy(&m_vecRaceCheckpointNext, next, sizeof(VECTOR));
	m_fRaceCheckpointSize = fSize;
	m_byteRaceType = byteType;

	if(m_dwRaceCheckpointMarker)
	{
		DisableMarker(m_dwRaceCheckpointMarker);
		m_dwRaceCheckpointMarker = 0;

		LOGI("CreateRadarMarkerIcon#2 %2.f %2.f %2.f", m_vecRaceCheckpointPos.X,
					m_vecRaceCheckpointPos.Y, m_vecRaceCheckpointPos.Z);
		m_dwRaceCheckpointMarker = CreateRadarMarkerIcon(0, m_vecRaceCheckpointPos.X, 
			m_vecRaceCheckpointPos.Y, m_vecRaceCheckpointPos.Z);
	}

	MakeRaceCheckpoint();
}

void CGame::MakeRaceCheckpoint()
{
	DisableRaceCheckpoint();

	ScriptCommand(&create_racing_checkpoint, (int)m_byteRaceType, 
		m_vecRaceCheckpointPos.X, m_vecRaceCheckpointPos.Y, m_vecRaceCheckpointPos.Z,
		m_vecRaceCheckpointNext.X, m_vecRaceCheckpointNext.Y, m_vecRaceCheckpointNext.Z,
		m_fRaceCheckpointSize, &m_dwRaceCheckpointHandle);

	m_bRaceCheckpointsEnabled = true;
}

void CGame::DisableRaceCheckpoint()
{
	if(m_dwRaceCheckpointHandle)
	{
		ScriptCommand(&destroy_racing_checkpoint, m_dwRaceCheckpointHandle);
		m_dwRaceCheckpointHandle = 0;
	}

	m_bRaceCheckpointsEnabled = false;
}

void CGame::UpdateCheckpoints()
{
	if(m_bCheckpointsEnabled)
	{
		CPlayerPed *pPlayerPed = this->FindPlayerPed();
		if(pPlayerPed)
		{
			ScriptCommand(&is_actor_near_point_3d, pPlayerPed->m_dwGTAId,
				m_vecCheckpointPos.X, m_vecCheckpointPos.Y, m_vecCheckpointPos.Z,
				m_vecCheckpointExtent.X, m_vecCheckpointExtent.Y, m_vecCheckpointExtent.Z, 1);

			if(!m_dwCheckpointMarker)
			{
				m_dwCheckpointMarker = CreateRadarMarkerIcon(0, m_vecCheckpointPos.X,
					m_vecCheckpointPos.Y, m_vecCheckpointPos.Z);
			}
		}
	}
	else if(m_dwCheckpointMarker)
	{
		DisableMarker(m_dwCheckpointMarker);
		m_dwCheckpointMarker = 0;
	}

	if(m_bRaceCheckpointsEnabled)
	{
		CPlayerPed *pPlayerPed = this->FindPlayerPed();
		if(pPlayerPed)
		{
			if (!m_dwRaceCheckpointMarker)
			{
				LOGI("CreateRadarMarkerIcon#1 %2.f %2.f %2.f", m_vecRaceCheckpointPos.X,
					m_vecRaceCheckpointPos.Y, m_vecRaceCheckpointPos.Z);
				m_dwRaceCheckpointMarker = CreateRadarMarkerIcon(0, m_vecRaceCheckpointPos.X,
					m_vecRaceCheckpointPos.Y, m_vecRaceCheckpointPos.Z);
			}
		}
	}
	else if(m_dwRaceCheckpointMarker)
	{
		DisableMarker(m_dwRaceCheckpointMarker);
		DisableRaceCheckpoint();
		m_dwRaceCheckpointMarker = 0;
	}
}