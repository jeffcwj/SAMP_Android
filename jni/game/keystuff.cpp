#include "main.h"
#include "keystuff.h"

GTA_CONTROLSET GcsLocalPlayerKeys;
GTA_CONTROLSET GcsRemotePlayerKeys[PLAYER_PED_SLOTS];

void GameKeyStatesInit()
{
	memset(&GcsLocalPlayerKeys, 0, sizeof(GTA_CONTROLSET));
	memset(GcsRemotePlayerKeys, 0, sizeof(GcsRemotePlayerKeys));
}

