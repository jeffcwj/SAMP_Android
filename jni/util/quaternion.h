#pragma once

class CQuaternion
{
public:
	static void ConvertMatrixToQuaternion(PQUATERNION q, PMATRIX4X4 m);
	static void ConvertQuaternionToMatrix(PMATRIX4X4 m, PQUATERNION q);
	static void Normalize(PQUATERNION q);
	static void Slerp(PQUATERNION out, QUATERNION q1, QUATERNION q2, float t);
};