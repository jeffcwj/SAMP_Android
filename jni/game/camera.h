#pragma once

class CCamera
{
public:
	// допилить
	CCamera() { /*m_matPos = (MATRIX4X4 *)ADDR_CAMERA;*/ };
	~CCamera() {};

	void SetBehindPlayer();
	void SetPosition(float fX, float fY, float fZ, float fRotatinX, float fRotatinY, float fRotatinZ);
	void LookAtPoint(float fX, float fY, float fZ, int iType);
	void Restore();
	void Fade(int iInOut);
	void GetMatrix(PMATRIX4X4 Matrix);

	MATRIX4X4 *m_matPos;
};