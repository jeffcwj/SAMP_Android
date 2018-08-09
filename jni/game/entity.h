#pragma once

class CEntity
{
public:
	CEntity() {};
	~CEntity() {};

	void GetMatrix(PMATRIX4X4 Matrix);
	void SetMatrix(MATRIX4X4 Matrix);
	void GetMoveSpeedVector(PVECTOR Vector);
	void SetMoveSpeedVector(VECTOR Vector);
	void GetTurnSpeedVector(PVECTOR Vector);
	void SetTurnSpeedVector(VECTOR Vector);
	uint16_t GetModelIndex();
	void SetModelIndex(uint16_t uiModel);
	void TeleportTo(float x, float y, float z);
	float GetDistanceFromLocalPlayerPed();

	ENTITY_TYPE *m_pEntity;
	uint32_t m_dwGTAId;
};