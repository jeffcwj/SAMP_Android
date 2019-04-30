/**

https://github.com/4x11/build69
版权归属:4x11

**/

#include <jni.h>
#include <android/log.h>
#include <ucontext.h>
#include <pthread.h>

#include "main.h"
#include "game/game.h"
#include "game/RW/RenderWare.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "chatwindow.h"
#include "spawnscreen.h"
#include "playertags.h"
#include "dialog.h"
#include "keyboard.h"
#include "settings.h"
#include "debug.h"

#include "util/armhook.h"
#include "checkfilehash.h"

uintptr_t g_libGTASA = 0;
const char* g_pszStorage = nullptr;

CGame *pGame = nullptr;
CNetGame *pNetGame = nullptr;
CChatWindow *pChatWindow = nullptr;
CSpawnScreen *pSpawnScreen = nullptr;
CPlayerTags *pPlayerTags = nullptr;
CDialogWindow *pDialogWindow = nullptr;

CGUI *pGUI = nullptr;
CKeyBoard *pKeyBoard = nullptr;
CDebug *pDebug = nullptr;
CSettings *pSettings = nullptr;

void InitHookStuff();
void InstallSpecialHooks();
void InitRenderWareFunctions();
void ApplyInGamePatches();
void ApplyPatches_level0();
void MainLoop();

void InitSAMP()
{
	Log("Initializing SAMP..");
	//数据包位置
	g_pszStorage = (const char*)(g_libGTASA+0x63C4B8);

	if(!g_pszStorage)
	{
		Log("Error: storage path not found!");
		std::terminate();
		return;
	}

	Log("Storage: %s", g_pszStorage);
	
	pSettings = new CSettings();
	Log("Checking samp files..");
	if(!FileCheckSum())
	{
		Log("SOME FILES HAVE BEEN MODIFIED. YOU NEED REINSTALL SAMP!");
	std::terminate();
		return;
	}
}
//初始化菜单
void InitInMenu()
{
	pGame = new CGame();
	pGame->InitInMenu();
	
		if(pSettings->Get().bDebug)
	//初始化调试
		pDebug = new CDebug();
//初始化GUI
	pGUI = new CGUI();
	//初始化键盘
	pKeyBoard = new CKeyBoard();
	//初始化聊天窗口
	pChatWindow = new CChatWindow();
	//初始化屏幕
	pSpawnScreen = new CSpawnScreen();
	//初始化玩家标签
	pPlayerTags = new CPlayerTags();
	//初始化对话框窗口
	pDialogWindow = new CDialogWindow();
}
//初始化游戏
void InitInGame()
{
	static bool bGameInited = false;
	static bool bNetworkInited = false;

	if(!bGameInited)
	{
	//初始化
		pGame->InitInGame();
		pGame->SetMaxStats();

		if(pDebug && !pSettings->Get().bOnline)
		{
			pDebug->SpawnLocalPlayer();
		}

		bGameInited = true;
		return;
	}

	if(!bNetworkInited && pSettings->Get().bOnline)
	{
	//游戏相关参数
	char * name=utf8_to_gbk(pSettings->Get().szNickName);
		pNetGame = new CNetGame(pSettings->Get().szHost,
			pSettings->Get().iPort, 
			name,
			pSettings->Get().szPassword);
		bNetworkInited = true;
		return;
	}
}

void MainLoop()
{
//初始化游戏
	InitInGame();
	if(pDebug) pDebug->Process();
	if(pNetGame) pNetGame->Process();
}

void handler(int signum, siginfo_t *info, void* contextPtr)
{
	ucontext* context = (ucontext_t*)contextPtr;

	if(info->si_signo == SIGSEGV)
	{
		Log("SIGSEGV | Fault address: 0x%X", info->si_addr);
		Log("libGTASA base address: 0x%X", g_libGTASA);
		Log("register states:");

		Log("r0: 0x%X, r1: 0x%X, r2: 0x%X, r3: 0x%X", 
			context->uc_mcontext.arm_r0, 
			context->uc_mcontext.arm_r1, 
			context->uc_mcontext.arm_r2,
			context->uc_mcontext.arm_r3);
		Log("r4: 0x%x, r5: 0x%x, r6: 0x%x, r7: 0x%x",
			context->uc_mcontext.arm_r4,
			context->uc_mcontext.arm_r5,
			context->uc_mcontext.arm_r6,
			context->uc_mcontext.arm_r7);
		Log("r8: 0x%x, r9: 0x%x, sl: 0x%x, fp: 0x%x",
			context->uc_mcontext.arm_r8,
			context->uc_mcontext.arm_r9,
			context->uc_mcontext.arm_r10,
			context->uc_mcontext.arm_fp);
		Log("ip: 0x%x, sp: 0x%x, lr: 0x%x, pc: 0x%x",
			context->uc_mcontext.arm_ip,
			context->uc_mcontext.arm_sp,
			context->uc_mcontext.arm_lr,
			context->uc_mcontext.arm_pc);

		Log("backtrace:");
		Log("1: libGTASA.so + 0x%X", context->uc_mcontext.arm_pc - g_libGTASA);
		Log("2: libGTASA.so + 0x%X", context->uc_mcontext.arm_lr - g_libGTASA);

		exit(0);
	}

	return;
}

void *Init(void *p)
{
	ApplyPatches_level0();

	pthread_exit(0);
}

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
	Log("SAMP library loaded! Build time: " __DATE__ " " __TIME__);
//拿到so地址
	g_libGTASA = FindLibrary("libGTASA.so");
	if(g_libGTASA == 0)
	{
		Log("ERROR: libGTASA.so address not found!");
		return 0;
	}

	Log("libGTASA.so image base address: 0x%X", g_libGTASA);
//写入版本号
		FILE*f;
	f=fopen("/sdcard/SAMP/samp_version.txt","w");
	fputs("1",f);
	fclose(f);
	srand(time(0));
//初始化Hook
	InitHookStuff();
	//初始化RenderWare引擎函数
	InitRenderWareFunctions();
	InstallSpecialHooks();

	pthread_t thread;
	//创建线程
	pthread_create(&thread, 0, Init, 0);

	struct sigaction act;
	act.sa_sigaction = handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGSEGV, &act, 0);

	return JNI_VERSION_1_4;
}

void Log(const char *fmt, ...)
{	
	char buffer[0xFF];
	static FILE* flLog = nullptr;

	if(flLog == nullptr && g_pszStorage != nullptr)
	{
		sprintf(buffer, "/sdcard/SAMP/samp_log.txt", g_pszStorage);
		flLog = fopen(buffer, "a");
	}

	memset(buffer, 0, sizeof(buffer));

	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, arg);
	va_end(arg);

	__android_log_write(ANDROID_LOG_INFO, "SAMP_Debug", buffer);

	if(pDebug) pDebug->AddMessage(buffer);

	if(flLog == nullptr) return;
	fprintf(flLog, "%s\n", buffer);
	fflush(flLog);

	return;
}

uint32_t GetTickCount()
{
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	return (tv.tv_sec*1000+tv.tv_usec/1000);
}
