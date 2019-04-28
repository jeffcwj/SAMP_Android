#include "main.h"
#include "jni.h"
#include <pthread.h>
#include <dlfcn.h>
#include "game/util.h"
#include "game/keystuff.h"
#include "imgui.h"
#include "RenderWare/RenderWare.h"
#include "gui/renderware_imgui.h"
CGame		*pGame = 0;
CNetGame	*pNetGame = 0;
CChatWindow *pChatWindow = 0;
static std::string pName="";
bool bGameInited = false;
bool bNetworkInited = false;
uintptr_t g_libGTASA = 0;
std::vector<std::string> item;

void *Init(void *p)
{
	pGame = new CGame();
	pGame->InitGame();
	
	while(true)
	{
		if(*(uintptr_t*)(g_libGTASA+ADDR_GAMESTATE) == 7)
		{
			pGame->StartGame();
			break;
		}
		else
			usleep(5000);
	}

	pthread_exit(0);
}

/* ============= */

void DoInitStuff()
{
	if(!bGameInited)
	{
		pChatWindow = new CChatWindow();
		
		bGameInited = true;
		return;
	}

	if(!bNetworkInited)
	{
		//========
		//随机玩家名不需要
/*		char nick[10];
		gen_random(nick, 8);
		*/
		//========
		//打开文件流
 FILE *f;
	char buf[256];
	f=fopen("/sdcard/SAMP/samp.txt","r");
		while(!feof(f))
	{
		memset(buf, 0, sizeof(buf));
		//读行
		fscanf(f,"%s",buf);
			//推入vector
	 	item.push_back(buf);
	}
	fclose(f);
			std::string ip = item[0].substr(0,item[0].find_last_of(":"));
   std::string port = item[0].substr(item[0].find_last_of(":") + 1);
   //赋值
pName="[66ccff]游戏名:"+item[1];
char * name=ImGuiPlus::utf8_to_gbk((char*)item[1].c_str());
	 //IP,端口,真玩家名称,服务器密码,玩家名称
pNetGame = new CNetGame((char*)ip.data(),atoi(port.c_str()),name,(char*)item[2].c_str(),(char*)pName.c_str());
bNetworkInited = true;
	}
}

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
	LOGI("libSAMP.so loaded! (" __DATE__ ")");
	g_libGTASA = findLibrary("libGTASA.so");

	if(!g_libGTASA)
	{
		LOGI("libGTASA.so address not found.");
		return 0;
	}
	LOGI("libGTASA.so imagebase address 0x%X", g_libGTASA);
	
	InitHookStuff();
	InitRenderWareFunctions();

	pthread_t thread;
	pthread_create(&thread, 0, Init, 0);

	LOGI(VERSION " Inited!");
	return JNI_VERSION_1_4;
}

uint32_t GetTickCount()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return (tv.tv_sec*1000+tv.tv_usec/1000);
}
