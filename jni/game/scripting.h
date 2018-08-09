#pragma once

#define MAX_SCRIPT_VARS 16
#define MAX_SCRIPT_SIZE	255

struct GAME_SCRIPT_THREAD
{
	uint8_t Pad1[20];			// +00
	uintptr_t dwScriptIP;		// +20
	uint8_t Pad2[36];			// +24
	uint32_t dwLocalVar[18];	// +60
	uint8_t Pad3[120];			// +132
	uint8_t bHasLocalCopy;		// +252
};

struct SCRIPT_COMMAND
{
	uint32_t OpCode;
	char Params[MAX_SCRIPT_VARS];
};

int ScriptCommand(const SCRIPT_COMMAND *pScriptCommand, ...);

const SCRIPT_COMMAND remove_actor_from_car_and_put_at	= { 0x0362, "ifff" };
const SCRIPT_COMMAND put_train_at						= { 0x07c7, "ifff" };
const SCRIPT_COMMAND set_camera_behind_player			= { 0x0373, "" };
const SCRIPT_COMMAND restore_camera_jumpcut				= { 0x02EB, "" };
const SCRIPT_COMMAND point_camera						= { 0x0160, "fffi" };
const SCRIPT_COMMAND set_camera_position				= { 0x015F, "ffffff" };
const SCRIPT_COMMAND enable_zone_names					= { 0x09BA, "i" };
const SCRIPT_COMMAND set_actor_z_angle					= { 0x0173, "if" };
const SCRIPT_COMMAND refresh_streaming_at				= { 0x04E4, "ff" };
const SCRIPT_COMMAND restart_if_wasted_at				= { 0x016C, "ffffi" };
const SCRIPT_COMMAND create_player						= { 0x0053, "vfffv" };
const SCRIPT_COMMAND create_actor_from_player			= { 0x01F5, "vv" };
const SCRIPT_COMMAND create_arrow_above_actor			= { 0x0187, "iv" };
const SCRIPT_COMMAND set_marker_color					= { 0x0165, "ii" };
const SCRIPT_COMMAND show_on_radar2						= { 0x018b, "ii" };
const SCRIPT_COMMAND disable_marker						= { 0x0164, "i" };
const SCRIPT_COMMAND create_car							= { 0x00A5, "ifffv" };
const SCRIPT_COMMAND set_car_z_angle					= { 0x0175, "if" };
const SCRIPT_COMMAND put_actor_in_car					= { 0x036A, "ii" };
const SCRIPT_COMMAND put_actor_in_car2					= { 0x0430, "iii" };
const SCRIPT_COMMAND destroy_car						= { 0x00A6, "i" };
const SCRIPT_COMMAND set_car_immunities					= { 0x02ac, "iiiiii" };
//武器
const SCRIPT_COMMAND set_actor_armed_weapon				= { 0x01b9, "ii" };
const SCRIPT_COMMAND get_actor_armed_weapon				= { 0x0470, "iv" };
//-------
const SCRIPT_COMMAND toggle_car_tires_vulnerable		= { 0x053f, "ii" };
const SCRIPT_COMMAND tie_marker_to_car					= { 0x0161, "iiiv" };
const SCRIPT_COMMAND select_interior					= { 0x04BB, "i" };
const SCRIPT_COMMAND link_actor_to_interior				= { 0x0860, "ii" };
const SCRIPT_COMMAND get_ground_z						= { 0x02ce, "fffv" };
const SCRIPT_COMMAND text_clear_all						= { 0x00be, "" };
const SCRIPT_COMMAND add_to_player_money				= { 0x0109, "ii" };
const SCRIPT_COMMAND play_sound							= { 0x018c, "fffi" };
const SCRIPT_COMMAND create_explosion_with_radius		= { 0x0948, "fffii" };
const SCRIPT_COMMAND create_object						= { 0x0107, "ifffv" };
const SCRIPT_COMMAND put_object_at						= { 0x01Bc, "ifff" };
const SCRIPT_COMMAND destroy_object						= { 0x0108, "i" };
const SCRIPT_COMMAND set_object_rotation				= { 0x0453, "ifff" };
const SCRIPT_COMMAND destroy_pickup						= { 0x0215, "i" };
const SCRIPT_COMMAND create_pickup						= { 0x0213, "iifffv" };
const SCRIPT_COMMAND create_radar_marker_without_sphere	= { 0x04CE, "fffiv" };
const SCRIPT_COMMAND show_on_radar						= { 0x0168, "ii" };
const SCRIPT_COMMAND is_actor_near_point_3d				= { 0x00FE, "iffffffi" };
const SCRIPT_COMMAND send_actor_to_car_driverseat		= { 0x05CB, "iii" };
const SCRIPT_COMMAND send_actor_to_car_passenger		= { 0x05CA, "iiii" };
const SCRIPT_COMMAND make_actor_leave_car				= { 0x05CD, "ii" };
const SCRIPT_COMMAND set_actor_immunities				= { 0x02ab, "iiiiii" };
const SCRIPT_COMMAND is_pickup_picked_up				= { 0x0214, "i" };
const SCRIPT_COMMAND toggle_player_controllable 		= { 0x01B4, "ii" };
const SCRIPT_COMMAND lock_actor							= { 0x04d7, "ii" };
const SCRIPT_COMMAND is_model_available					= { 0x0248, "i" };
const SCRIPT_COMMAND request_model						= { 0x0247, "i" };
const SCRIPT_COMMAND load_requested_models				= { 0x038B, "" };
const SCRIPT_COMMAND create_racing_checkpoint			= { 0x06d5,	"ifffffffv"};
const SCRIPT_COMMAND destroy_racing_checkpoint			= { 0x06d6,	"i" };