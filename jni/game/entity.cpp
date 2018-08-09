#include "main.h"

extern CGame *pGame;
extern CNetGame *pNetGame;

void CEntity::GetMatrix(PMATRIX4X4 Matrix)
{
	if (!m_pEntity || !m_pEntity->mat) return;

	Matrix->right.X = m_pEntity->mat->right.X;
	Matrix->right.Y = m_pEntity->mat->right.Y;
	Matrix->right.Z = m_pEntity->mat->right.Z;

	Matrix->up.X = m_pEntity->mat->up.X;
	Matrix->up.Y = m_pEntity->mat->up.Y;
	Matrix->up.Z = m_pEntity->mat->up.Z;

	Matrix->at.X = m_pEntity->mat->at.X;
	Matrix->at.Y = m_pEntity->mat->at.Y;
	Matrix->at.Z = m_pEntity->mat->at.Z;

	Matrix->pos.X = m_pEntity->mat->pos.X;
	Matrix->pos.Y = m_pEntity->mat->pos.Y;
	Matrix->pos.Z = m_pEntity->mat->pos.Z;
}

void CEntity::SetMatrix(MATRIX4X4 Matrix)
{
	if (!m_pEntity || !m_pEntity->mat) return;

	m_pEntity->mat->right.X = Matrix.right.X;
	m_pEntity->mat->right.Y = Matrix.right.Y;
	m_pEntity->mat->right.Z = Matrix.right.Z;

	m_pEntity->mat->up.X = Matrix.up.X;
	m_pEntity->mat->up.Y = Matrix.up.Y;
	m_pEntity->mat->up.Z = Matrix.up.Z;

	m_pEntity->mat->at.X = Matrix.at.X;
	m_pEntity->mat->at.Y = Matrix.at.Y;
	m_pEntity->mat->at.Z = Matrix.at.Z;

	m_pEntity->mat->pos.X = Matrix.pos.X;
	m_pEntity->mat->pos.Y = Matrix.pos.Y;
	m_pEntity->mat->pos.Z = Matrix.pos.Z;
}

void CEntity::GetMoveSpeedVector(PVECTOR Vector)
{
	Vector->X = m_pEntity->vecMoveSpeed.X;
	Vector->Y = m_pEntity->vecMoveSpeed.Y;
	Vector->Z = m_pEntity->vecMoveSpeed.Z;
}

void CEntity::SetMoveSpeedVector(VECTOR Vector)
{
	m_pEntity->vecMoveSpeed.X = Vector.X;
	m_pEntity->vecMoveSpeed.Y = Vector.Y;
	m_pEntity->vecMoveSpeed.Z = Vector.Z;
}

void CEntity::GetTurnSpeedVector(PVECTOR Vector)
{
	Vector->X = m_pEntity->vecTurnSpeed.X;
	Vector->Y = m_pEntity->vecTurnSpeed.Y;
	Vector->Z = m_pEntity->vecTurnSpeed.Z;
}

void CEntity::SetTurnSpeedVector(VECTOR Vector)
{
	m_pEntity->vecTurnSpeed.X = Vector.X;
	m_pEntity->vecTurnSpeed.Y = Vector.Y;
	m_pEntity->vecTurnSpeed.Z = Vector.Z;
}

void CEntity::SetModelIndex(uint16_t uiModel)
{
	LOGI("CEntity::SetModelIndex (%d)", uiModel);

	if(!m_pEntity) return;

	if(!pGame->IsModelLoaded(uiModel))
	{
		pGame->RequestModel(uiModel);
		pGame->LoadRequestedModels();
	}

	// CEntity__DeleteRWObject (0x3915D8)
	void (*DeleteRWObject)(ENTITY_TYPE*);
	*(void **) (&DeleteRWObject) = (void*)(g_libGTASA+0x3915D8+1);
	(*DeleteRWObject)(m_pEntity);

	// CEntity__SetModelIndex call (0x4336F4)
	void (*SetModelIndex)(ENTITY_TYPE*, unsigned int uiModel);
	*(void **) (&SetModelIndex) = (void*)(g_libGTASA+0x4336F4+1);
	(*SetModelIndex)(m_pEntity, uiModel);
}

uint16_t CEntity::GetModelIndex()
{
		return m_pEntity->nModelIndex;
}

void CEntity::TeleportTo(float x, float y, float z)
{
	if(m_pEntity)
	{
		if(	GetModelIndex() != TRAIN_PASSENGER_LOCO &&
			GetModelIndex() != TRAIN_FREIGHT_LOCO &&
			GetModelIndex() != TRAIN_TRAM)
		{
			    m_pEntity->mat->pos.X = x;
                m_pEntity->mat->pos.Y = y;
                m_pEntity->mat->pos.Z = z;
		}
		else
			ScriptCommand(&put_train_at, m_dwGTAId, x, y, z);
	}
}

float CEntity::GetDistanceFromLocalPlayerPed()
{
	MATRIX4X4 matFromPlayer;
	MATRIX4X4 matThis;
	float fSX, fSY, fSZ;

	CPlayerPed *pLocalPlayerPed = pGame->FindPlayerPed();
	CLocalPlayer *pLocalPlayer = 0;

	if(!pLocalPlayerPed) return 10000.0f;

	GetMatrix(&matThis);

	if(pNetGame)
	{	
		// допилить
		pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
		//if(pLocalPlayer && (PlocalPlayer->IsSpectating() || pLocalPlayer->IsInRCMode()))
		//	pGame->GetCamera()->GetMatrix(&matFromPlayer);
		//else
		pLocalPlayerPed->GetMatrix(&matFromPlayer);
	}
	else
		pLocalPlayerPed->GetMatrix(&matFromPlayer);

	fSX = (matThis.pos.X - matFromPlayer.pos.X) * (matThis.pos.X - matFromPlayer.pos.X);
	fSY = (matThis.pos.Y - matFromPlayer.pos.Y) * (matThis.pos.Y - matFromPlayer.pos.Y);
	fSZ = (matThis.pos.Z - matFromPlayer.pos.Z) * (matThis.pos.Z - matFromPlayer.pos.Z);

	return (float)sqrt(fSX + fSY + fSZ);
}