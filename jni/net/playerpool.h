#pragma once

#define INVALID_PLAYER_ID 0xFFFF

class CPlayerPool
{
public:
	CPlayerPool();
	~CPlayerPool();

	void DeactivateAll();

	bool Process();
	void SetLocalPlayerName(char* szName);
	char *GetLocalPlayerName();
	char *GetPlayerName(PLAYERID plaeyrId);
	void SetPlayerName(PLAYERID plaeyrId, char *szName);

	CLocalPlayer *GetLocalPlayer();
	PLAYERID FindRemotePlayerIDFromGtaPtr(PED_TYPE *pActor);

	bool New(PLAYERID plaeyrId, char* szPlayerName);
	bool Delete(PLAYERID plaeyrId, uint8_t byteReason);

	CRemotePlayer *GetAt(PLAYERID plaeyrId);

	bool GetSlotState(PLAYERID plaeyrId);

	void SetLocalPlayerID(PLAYERID MyPlayerId);

	PLAYERID GetLocalPlayerID();

private:
	bool			m_bPlayerSlotState[MAX_PLAYERS];
	CLocalPlayer	*m_pLocalPlayer;
	PLAYERID 		m_LocalPlayerID;
	CRemotePlayer	*m_pPlayers[MAX_PLAYERS];
	char			m_szLocalPlayerName[MAX_PLAYER_NAME+1];
	char			m_szPlayerNames[MAX_PLAYERS][MAX_PLAYER_NAME+1];
};