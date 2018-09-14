#include "main.h"

extern CGame 		*pGame;
extern CNetGame		*pNetGame;
extern CChatWindow *pChatWindow;
CPlayerPool::CPlayerPool()
{
	m_pLocalPlayer = new CLocalPlayer();
//玩家池 这个文件中包含了，玩家的操作
	// допилить
	for(PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++)
	{
		m_bPlayerSlotState[playerId] = false;
		m_pPlayers[playerId] = 0;
	}
}

CPlayerPool::~CPlayerPool()
{
	LOGI("CPlayerPool::~CPlayerPool destructor");

	delete m_pLocalPlayer;

	for(PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++)
		Delete(playerId, 0);
}
//设置自己名字
void CPlayerPool::SetLocalPlayerName(char *szName)
{
	strcpy(m_szLocalPlayerName, szName);
}
//获取自己名字
char *CPlayerPool::GetLocalPlayerName()
{
	return m_szLocalPlayerName;
}
//获取玩家名字
char *CPlayerPool::GetPlayerName(PLAYERID playerId)
{
	return m_szPlayerNames[playerId];
}
//设置玩家名字
void CPlayerPool::SetPlayerName(PLAYERID playerId, char *szName)
{
	strcpy(m_szPlayerNames[playerId], szName);
}

CLocalPlayer *CPlayerPool::GetLocalPlayer()
{
	return m_pLocalPlayer;
}

///////////////
CRemotePlayer *CPlayerPool::GetAt(PLAYERID playerId)
{
	if(playerId > MAX_PLAYERS)
		return 0;

	return m_pPlayers[playerId];
}

bool CPlayerPool::GetSlotState(PLAYERID playerId)
{
	if(playerId > MAX_PLAYERS)
		return false;

	return m_bPlayerSlotState[playerId];
}
//设置自己ID
void CPlayerPool::SetLocalPlayerID(PLAYERID MyPlayerID)
{
	strcpy(m_szPlayerNames[MyPlayerID], m_szLocalPlayerName);
	m_LocalPlayerID = MyPlayerID;
}
//获取自己ID
PLAYERID CPlayerPool::GetLocalPlayerID()
{
	return m_LocalPlayerID;
}
//生成玩家
bool CPlayerPool::New(PLAYERID playerId, char* szPlayerName)
{
	m_pPlayers[playerId] = new CRemotePlayer();

	if(m_pPlayers[playerId])
	{
		strcpy(m_szPlayerNames[playerId], szPlayerName);
		m_pPlayers[playerId]->SetID(playerId);
		m_bPlayerSlotState[playerId] = true;
		return true;
	}
	else
		return false;
}
//删除玩家 (玩家退出或其他情况)
bool CPlayerPool::Delete(PLAYERID playerId, uint8_t byteReason)
{
	if(!GetSlotState(playerId) || !m_pPlayers[playerId])
		return false;

	m_bPlayerSlotState[playerId] = false;
	delete m_pPlayers[playerId];
	m_pPlayers[playerId] = 0;

	return true;
}

bool CPlayerPool::Process()
{
	for(PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++)
	{
		if(m_bPlayerSlotState[playerId] == true)
		{
			m_pPlayers[playerId]->Process();
		}
	}

	m_pLocalPlayer->Process();

	return true;
}


PLAYERID CPlayerPool::FindRemotePlayerIDFromGtaPtr(PED_TYPE *pActor)
{
	CPlayerPed *pPlayerPed;

	for(PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++)
	{
		if(m_bPlayerSlotState[playerId])
		{
			pPlayerPed = m_pPlayers[playerId]->GetPlayerPed();

			if(pPlayerPed)
			{
				PED_TYPE *pTestActor = pPlayerPed->GetGtaActor();
				if(pTestActor && (pActor == pTestActor))
					return m_pPlayers[playerId]->GetID();
			}
		}
	}

	return INVALID_PLAYER_ID;
}

void CPlayerPool::DeactivateAll()
{
	LOGI("CPlayerPool::DeactivateAll");
	m_pLocalPlayer->m_bIsActive = false;
	//m_pLocalPlayer->m_iSelectedClass = 0;

	for(PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++) 
	{
		if(m_bPlayerSlotState[playerId])
			m_pPlayers[playerId]->Deactivate();
	}
}