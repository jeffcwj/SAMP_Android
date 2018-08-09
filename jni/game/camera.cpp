#include "main.h"

void CCamera::SetBehindPlayer()
{
	ScriptCommand(&set_camera_behind_player);
	ScriptCommand(&restore_camera_jumpcut);
}

void CCamera::LookAtPoint(float fX, float fY, float fZ, int iType)
{
	ScriptCommand(&point_camera, fX, fY, fZ, iType);
}

void CCamera::SetPosition(float fX, float fY, float fZ, float fRotationX, float fRotationY, float fRotationZ)
{
	ScriptCommand(&set_camera_position, fX, fY, fZ, fRotationX, fRotationY, fRotationZ);
}

void CCamera::Restore()
{
	ScriptCommand(&restore_camera_jumpcut);
}