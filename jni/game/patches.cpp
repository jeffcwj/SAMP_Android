#include "main.h"

char *PLAYERS_REALLOC = 0;

void ApplyPreGamePatches()
{
	PLAYERS_REALLOC = new char[404*MAX_PLAYERS];
	UnFuck(g_libGTASA+0x5D021C);
	*(char**)(g_libGTASA+0x5D021C) = PLAYERS_REALLOC;
	LOGI("CWorld::Players new address 0x%X", PLAYERS_REALLOC);
}

void ApplyInGamePatches()
{
	// CPlayerPed__CPlayerPed task fix
	WriteMemory(g_libGTASA+0x458ED1, "\xE0", 1);

	// CPhysical destructor crash fix (CRealTimeShadowManager::ReturnRealTimeShadow)
	UnFuck(g_libGTASA+0x3A0196);
	NOP(g_libGTASA+0x3A0196, 5);

	// Remove the FindPlayerInfoForThisPlayerPed
	UnFuck(g_libGTASA+0x434D94);
	NOP(g_libGTASA+0x434D94, 6);
	
//飞行破限
/*		UnFuck(g_libGTASA+0x510D60);
	*(float*)(g_libGTASA+0x510D60) = 12345678.0;
	*/

	// no  vehicle name rendering
	//不显示车辆名
/*	UnFuck(g_libGTASA+0x3D6FDC);
	NOP(g_libGTASA+0x3D6FDC, 2);
	*/

	// CTaskSimplePlayerOnFoot::PlayIdleAnimations 
	WriteMemory(g_libGTASA+0x4BDB18, "\x70\x47", 2);

	// DoFade()
	WriteMemory(g_libGTASA+0x39A4B4, "\x70\x47", 2);

	// interior peds
	UnFuck(g_libGTASA+0x2C2C22);
	NOP(g_libGTASA+0x2C2C22, 4);

	// CarCtl::GenerateRandomCars nulled from CGame::Process
	UnFuck(g_libGTASA+0x398A3A);
	NOP(g_libGTASA+0x398A3A, 2);

	// CTheCarGenerators::Process nulled from CGame::Process
	UnFuck(g_libGTASA+0x398A34);
	NOP(g_libGTASA+0x398A34, 2);

	// ToggleDebugFPS
	//显示FPS
//	*(uint8_t*)(g_libGTASA+0x8ED875) = 1;

    // VehicleStruct increase (0x32C*0x50 = 0xFDC0)
    WriteMemory(g_libGTASA+0x405338, "\x4F\xF6\xC0\x50", 4);	// MOV  R0, #0xFDC0
    WriteMemory(g_libGTASA+0x405342, "\x50\x20", 2);			// MOVS R0, #0x50
    WriteMemory(g_libGTASA+0x405348, "\x50\x22", 2);			// MOVS R2, #0x50
    WriteMemory(g_libGTASA+0x405374, "\x50\x2B", 2);			// CMP  R3, #0x50

    // vehicle radio
    //CAEVehicleAudioEntity::PlayerAboutToExitVehicleAsDriver
	WriteMemory(g_libGTASA+0x35BDE4, "\x70\x47", 2);
    // CAEVehicleAudioEntity::JustWreckedVehicle
	WriteMemory(g_libGTASA+0x35D2F0, "\x70\x47", 2);
    // CAEVehicleAudioEntity::TurnOnRadioForVehicle
	WriteMemory(g_libGTASA+0x35BD4C, "\x70\x47", 2);

    // radar draw blips
    UnFuck(g_libGTASA+0x3DCA90);
    NOP(g_libGTASA+0x3DCA90, 2);
    UnFuck(g_libGTASA+0x3DD4A4);
    NOP(g_libGTASA+0x3DD4A4, 2);

    // Fix crash in real time shadow code (at CPed::GetBonePosition)
 	UnFuck(g_libGTASA+0x39B2C0);
 	NOP(g_libGTASA+0x39B2C0, 4);

 	// ------ CPools::Initialise -------
 
 	// CPed pool
 	// MOVW R0, #0x5EC8
 	// MOVT R0, #6
 	WriteMemory(g_libGTASA+0x3AF2D0, "\x45\xF6\xC8\x60\xC0\xF2\x06\x00", 8); // MOV  R0, #0x65EC8 | size=0x7C4 (0x43F30)
 	WriteMemory(g_libGTASA+0x3AF2DE, "\xD2\x20", 2); // MOVS R0, #0xD2
 	WriteMemory(g_libGTASA+0x3AF2E4, "\xD2\x22", 2); // MOVS R2, #0xD2
 	WriteMemory(g_libGTASA+0x3AF310, "\xD2\x2B", 2); // CMP  R3, #0xD2
 
	// CPedIntelligence pool
	// movw r0, #0x20B0
 	// movt r0, #2
 	// nop
 	WriteMemory(g_libGTASA+0x3AF7E6, "\x42\xF2\xB0\x00\xC0\xF2\x02\x00\x00\x46", 10); // MOVS R0, #0x220B0 | size=0x298 (0x16B20)
 	WriteMemory(g_libGTASA+0x3AF7F6, "\xD2\x20", 2); // MOVS R0, #0xD2
 	WriteMemory(g_libGTASA+0x3AF7FC, "\xD2\x22", 2); // MOVS R2, #0xD2
 	WriteMemory(g_libGTASA+0x3AF824, "\xD2\x2B", 2); // CMP  R3, 0xD2
 
 	// Task pool
 	/*WriteMemory(g_libGTASA+0x3AF4F4, "\x4F\xF4\xC0\x60", 4); // MOV.W R0, #0x600
 	WriteMemory(g_libGTASA+0x3AF4FC, "\x4F\xF4\xC0\x62", 4); // MOV.W R2, #0x600
 	WriteMemory(g_libGTASA+0x3AF824, "\xB3\xF5\xC0\x6F", 4); // CMP.W R3, #0x600*/
 
 	// Event pool, probably needs more size 
 	/*WriteMemory(g_libGTASA+0x3AF548, "\xFF", 1); // 255
 	WriteMemory(g_libGTASA+0x3AF7FE, "\xFF", 1);
 	WriteMemory(g_libGTASA+0x3AF57A, "\xFF", 1);*/
 
 	// Increase matrix count in CPlaceable::InitMatrixArray 
 	WriteMemory(g_libGTASA+0x3ABB0A, "\x4F\xF4\x7A\x61", 4); // MOV.W R1, #4000
}
