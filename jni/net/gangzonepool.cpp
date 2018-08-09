#include "main.h"

extern CGame *pGame;

CGangZonePool::CGangZonePool()
{
	for(uint16_t wZone = 0; wZone < MAX_GANG_ZONES; wZone++)
	{
		m_pGangZone[wZone] = 0;
		m_bSlotState[wZone] = false; 
	}
}

CGangZonePool::~CGangZonePool()
{
	for(uint16_t wZone = 0; wZone = MAX_GANG_ZONES; wZone++)
	{
		if(m_pGangZone[wZone])
		{
			delete m_pGangZone[wZone];
			m_pGangZone[wZone] = 0;
		}
	}
}

void CGangZonePool::New(uint16_t wZone, float fMinX, float fMinY, float fMaxX, float fMaxY, uint32_t dwColor)
{
	if(m_pGangZone[wZone])
	{
		delete m_pGangZone[wZone];
		m_pGangZone[wZone] = 0;
		m_bSlotState[wZone] = 0;
	}

	GANG_ZONE *pGangZone = (GANG_ZONE*)malloc(sizeof(GANG_ZONE));
	if(pGangZone)
	{
		pGangZone->fPos[0] = fMinX;
		pGangZone->fPos[1] = fMaxY;
		pGangZone->fPos[2] = fMaxX;
		pGangZone->fPos[3] = fMinY;
		pGangZone->dwColor = dwColor;
		pGangZone->dwAltColor = dwColor;
		m_pGangZone[wZone] = pGangZone;
		m_bSlotState[wZone] = true;
	}
}

void CGangZonePool::Flash(uint16_t wZone, uint32_t dwColor)
{
	if(m_pGangZone[wZone])
		m_pGangZone[wZone]->dwAltColor = dwColor;
}

void CGangZonePool::StopFlash(uint16_t wZone)
{
	if(m_pGangZone[wZone])
		m_pGangZone[wZone]->dwAltColor = m_pGangZone[wZone]->dwColor;
}

void CGangZonePool::Delete(uint16_t wZone)
{
	if(m_pGangZone[wZone])
	{
		delete m_pGangZone[wZone];
		m_pGangZone[wZone] = 0;
		m_bSlotState[wZone] = false;
	}
}

void CGangZonePool::Draw()
{
	static int iLastTick = 0;
	int iTick = GetTickCount();
	static uint8_t alt = 0;
	if(iTick - iLastTick >= 500)
	{
		alt = ~alt;
		iLastTick = iTick;
	}

	for(uint16_t wZone = 0; wZone < MAX_GANG_ZONES; wZone++)
	{
		if(m_bSlotState[wZone])
			pGame->DrawGangZone(m_pGangZone[wZone]->fPos, alt ? m_pGangZone[wZone]->dwAltColor : m_pGangZone[wZone]->dwColor);
	}
}