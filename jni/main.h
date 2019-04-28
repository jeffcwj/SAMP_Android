#pragma once

#include <cstdlib>
#include <string>
#include <vector>
#include <list>
#include <unistd.h>

#define SAMP_VERSION	"0.3.7"
#define PORT_VERSION	"0.69"
//最大玩家
#define MAX_PLAYERS		1004
//最大车辆
#define MAX_VEHICLES	2000
//最长玩家名
#define MAX_PLAYER_NAME	24

#define RAKSAMP_CLIENT
#define NETCODE_CONNCOOKIELULZ 0x6969
#include "vendor/RakNet/SAMP/samp_netencr.h"
#include "vendor/RakNet/SAMP/SAMPRPC.h"

#include "util/util.h"

extern uintptr_t g_libGTASA;
extern const char* g_pszStorage;

void Log(const char *fmt, ...);
uint32_t GetTickCount();