#pragma once
void SetRadarColor(uint8_t nIndex, uint32_t dwColor);

PED_TYPE * GamePool_Ped_GetAt(int iID);
int GamePool_Ped_GetIndex(PED_TYPE *pActor);
PED_TYPE *GamePool_FindPlayerPed();

bool IsValidModel(int iModelID);
uint16_t GetModelReferenceCount(int nModelIndex);

VEHICLE_TYPE *GamePool_Vehicle_GetAt(int iID);
uint32_t GamePool_Vehicle_GetIndex(VEHICLE_TYPE *pVehicle);

ENTITY_TYPE *GamePool_Object_GetAt(int iID);

void SetPlayerPedPtrRecord(uint8_t bytePlayer, uint32_t dwPedPtr);
uint8_t FindPlayerNumFromPedPtr(uint32_t dwPedPtr);

void DisableMarker_037(uint32_t uid);

void GameResetRadarColors();

float DegToRad(float fDegrees);
float FloatOffset(float f1, float f2);

void GameResetRadarColors();
uint32_t TranslateColorCodeToRGBA(uint32_t iCode);
void SetRadarColor(int nIndex, uint32_t dwColor);

//void ConvertMatrixToQuaternion(PQUATERNION q, PMATRIX4X4 m);
//void ConvertQuaternionToMatrix(PMATRIX4X4 m, PQUATERNION q);

//void RtQuatConvertFromMatrix(PMATRIX4X4 m, PQUATERNION q);
