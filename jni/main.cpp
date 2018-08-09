#include "main.h"
#include "RenderWare/RenderWare.h"
#include "jni.h"
#include <pthread.h>
#include <dlfcn.h>
CGame		*pGame = 0;
CNetGame	*pNetGame = 0;
CChatWindow *pChatWindow = 0;
static std::string pName="";
bool bGameInited = false;
bool bNetworkInited = false;

uintptr_t g_libGTASA = 0;
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
/* ========= */
/*void gen_random(char *s, const int len)
{
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < len; ++i)
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];

	s[len] = 0;
}*/
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
		   FILE *fp;
     char c;
     //读取服务器端口信息
    fp=fopen("/sdcard/SAMP/server.txt","r");
    std::string buf;
    while (fscanf(fp, "%c", &c) != EOF){
      buf += c;
    }
    //剪切取下IP和端口
    std::string ip = buf.substr(0,buf.find_last_of(":"));
    std::string port = buf.substr(buf.find_last_of(":") + 1);
    //读取玩家名
    fp = fopen("/sdcard/SAMP/name.txt", "r");
    buf = "";
    while (fscanf(fp, "%c", &c) != EOF) {
    //直接拿到名字
      buf += c;
    }
    fclose(fp);
    fp=NULL;
    //赋值
	 pName="{66ccff}游戏名:"+buf;
		pNetGame = new CNetGame((char*)ip.data(),atoi(port.c_str()),(char*)buf.data(), "",(char*)pName.data());
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
