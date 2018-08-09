#pragma once

typedef struct _CAMERA_AIM
{
	float f1x,f1y,f1z;
	float pos1x,pos1y,pos1z;
	float pos2x,pos2y,pos2z;
	float f2x,f2y,f2z;

} CAMERA_AIM;

void GameAimSyncInit();
void GameStoreLocalPlayerAim();
void GameSetLocalPlayerAim();
CAMERA_AIM * GameGetInternalAim();
void GameStoreRemotePlayerAim(int iPlayer, CAMERA_AIM * caAim);
void GameSetRemotePlayerAim(int iPlayer);
CAMERA_AIM * GameGetRemotePlayerAim(int iPlayer);

void GameSetPlayerCameraMode(uint8_t byteMode, uint8_t bytePlayerID);
uint8_t GameGetPlayerCameraMode(uint8_t bytePlayerID);
uint8_t GameGetLocalPlayerCameraMode();

void GameStoreLocalPlayerCameraExtZoom();
void GameSetLocalPlayerCameraExtZoom();
void GameSetPlayerCameraExtZoom(uint8_t bytePlayerID, float fZoom);
void GameSetRemotePlayerCameraExtZoom(uint8_t bytePlayerID);
float GameGetLocalPlayerCameraExtZoom();
