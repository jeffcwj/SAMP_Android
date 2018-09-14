#pragma once

#define MAX_PLAYERS			1004
#define MAX_VEHICLES		2000
#define MAX_OBJECTS			1000
#define MAX_GANG_ZONES		1024
#define PLAYER_PED_SLOTS	210

typedef unsigned short VEHICLEID;
typedef unsigned short PLAYERID;

#define PADDING(x,y) uint8_t x[y]

#define IN_VEHICLE(x) ((x->dwStateFlags & 0x100) >> 8)

#pragma pack(1)
typedef struct _RECT
{
	float x; // +0
	float y; // +4
	float x1; // +8
	float y1; // +12
} RECT, *PRECT;

#pragma pack(1)
typedef struct _VECTOR 
{
	float X,Y,Z;
} VECTOR, *PVECTOR;

#pragma pack(1)
typedef struct _MATRIX4X4 
{
	VECTOR right;		// r11 r12 r13
	uint32_t  flags;
	VECTOR up;			// r21 r22 r23
	float  pad_u;
	VECTOR at;			// r31 r32 r33
	float  pad_a;
	VECTOR pos;
	float  pad_p;
} MATRIX4X4, *PMATRIX4X4;

//-----------------------------------------------------------

#pragma pack(1)
typedef struct _QUATERNION
{
	float W;
	float X;
	float Y;
	float Z;
} QUATERNION, *PQUATERNION;

//-----------------------------------------------------------
#pragma pack(1)
typedef struct _ENTITY_TYPE
{
	// ENTITY STUFF
	uint32_t vtable; 		// 0-4		;vtable
	PADDING(_pad91, 16);	// 4-20
	MATRIX4X4 *mat; 		// 20-24	;mat
	PADDING(_pad92, 10);	// 24-34
	uint16_t nModelIndex; 	// 34-36	;ModelIndex
	PADDING(_pad93, 32);	// 36-68
	VECTOR vecMoveSpeed; 	// 68-80	;vecMoveSpeed
	VECTOR vecTurnSpeed; 	// 80-92	;vecTurnSpeed
	PADDING(_pad94, 92);
	//uint32_t dwUnkModelRel; // 180-184
} ENTITY_TYPE;

//-----------------------------------------------------------
#pragma pack(1)
typedef struct _PED_TYPE
{
	ENTITY_TYPE entity; 		// 0000-0184	;entity
	PADDING(_pad106, 174);		// 0184-0358
	uint32_t dwPedType;			// 0358-0362	;dwPedType
	PADDING(_pad101, 734);		// 0362-1096
	uint32_t dwAction;			// 1096-1100	;Action
	PADDING(_pad102, 52);		// 1100-1152
	uint32_t dwStateFlags; 		// 1152-1156	;StateFlags
	PADDING(_pad103, 188);		// 1156-1344
	float fHealth;		 		// 1344-1348	;Health
	float fMaxHealth;			// 1348-1352	;MaxHealth
	float fArmour;				// 1352-1356	;Armour
	PADDING(_pad104, 12);		// 1356-1368
	float fRotation1;			// 1368-1372	;Rotation1
	float fRotation2;			// 1372-1376	;Rotation2
	PADDING(_pad105, 44);		// 1376-1420
	uint32_t pVehicle;			// 1420-1424	;pVehicle
	uint32_t pContactEntity; // 1412 - 1416
} PED_TYPE;

//-----------------------------------------------------------

#pragma pack(1)
typedef struct _VEHICLE_TYPE
{
	ENTITY_TYPE entity; 		// 0000-0184	;entity
	PADDING(_pad201, 936);		// 0184-1120
	PED_TYPE *pDriver;			// 1120-1124	;pDriver
	PED_TYPE *pPassengers[7];	// 1124-1152	;pPassenger
	PADDING(_pad202, 72);
	float fHealth;				// 1224-1228	;fHealth
	uint8_t byteColor1;
	uint8_t byteColor2;
} VEHICLE_TYPE;

//-----------------------------------------------------------

#define ACTION_WASTED					55
#define ACTION_DEATH					54
#define ACTION_INCAR					50
#define ACTION_NORMAL					1
#define ACTION_SCOPE					12
#define ACTION_NONE						0 

//-----------------------------------------------------------

#define TRAIN_PASSENGER_LOCO			538
#define TRAIN_FREIGHT_LOCO				537
#define TRAIN_PASSENGER					570
#define TRAIN_FREIGHT					569
#define TRAIN_TRAM						449
#define HYDRA							520

//-----------------------------------------------------------