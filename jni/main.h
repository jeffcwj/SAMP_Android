#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <algorithm>
#include <list>
#include <sys/mman.h>
#include <android/log.h>
//iconv
#include <iconv.h>
#define MAX_PLAYER_NAME			24
#define CSCANNER_DISTANCE		200.0f

#include "RakNet/PacketEnumerations.h"
#include "RakNet/RakNetworkFactory.h"
#include "RakNet/RakClientInterface.h"
#include "RakNet/NetworkTypes.h"
#include "RakNet/BitStream.h"
#include "RakNet/StringCompressor.h"

#include "RakNet/SAMP/samp_netencr.h"
#include "RakNet/SAMP/samp_auth.h"
#include "RakNet/SAMP/SAMPRPC.h"

#include "game/common.h"
#include "game/address.h"
#include "game/entity.h"
#include "game/vehicle.h"
#include "game/playerped.h"
#include "game/object.h"
#include "game/camera.h"
#include "game/game.h"
#include "game/scripting.h"

#include "net/localplayer.h"
#include "net/remoteplayer.h"
#include "net/playerpool.h"
#include "net/vehiclepool.h"
#include "net/objectpool.h"
#include "net/pickuppool.h"
#include "net/gangzonepool.h"
#include "net/netrpc.h"
#include "net/netgame.h"
#include "net/scriptrpc.h"

#include "gui/font.h"
#include "gui/gui.h"

#include "chatwindow.h"

#include "util/util.h"
#include "util/quaternion.h"
#include "util/armhook.h"

#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO, "qq1198", __VA_ARGS__)
#define VERSION "SA:MP 0.3.7-5b"

#ifndef PAGESIZE
#define PAGESIZE PAGE_SIZE
#endif

extern uintptr_t g_libGTASA;

uint32_t GetTickCount();
