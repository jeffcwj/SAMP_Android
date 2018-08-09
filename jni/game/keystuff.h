#pragma once

typedef struct _GTA_CONTROLSET
{
	uint16_t 	wWalkLR;
	uint16_t 	wWalkUD;
	uint16_t	wSteeringLR;
	uint16_t	wSteeringUD;
	uint16_t 	wAccelerate;
	uint16_t	wBrake;
	uint16_t 	wHandBrake;
	uint8_t		bDuckJustDown;
	bool		bDuckState;
	uint8_t		bJumpJustDown;
	bool		bJumpState;
	uint8_t		bSprintJustDown;
	uint8_t		bMeleeAttackJustDown;
	bool		bMeleeAttackState;
} GTA_CONTROLSET;

extern GTA_CONTROLSET GcsLocalPlayerKeys;
extern GTA_CONTROLSET GcsRemotePlayerKeys[PLAYER_PED_SLOTS];

void GameKeyStatesInit();