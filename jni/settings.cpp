#include "main.h"
#include "settings.h"
#include "vendor/inih/cpp/INIReader.h"
//ini配置文件
CSettings::CSettings()
{
	Log("Loading settings..");	

	char buff[0x7F];
	sprintf(buff, "/sdcard/SAMP/settings.ini", g_pszStorage);

	INIReader reader(buff);

	if(reader.ParseError() < 0)
	{
		Log("Error: can't load %s", buff);
		std::terminate();
		return;
	}

	// Client
	size_t length = 0;
	sprintf(buff, "__android_%d%d", rand() % 1000, rand() % 1000);
	//游戏名
	length = reader.Get("client", "name", buff).copy(m_Settings.szNickName, MAX_PLAYER_NAME);
	m_Settings.szNickName[length] = '\0';
	//ip
	length = reader.Get("client", "host", "127.0.0.1").copy(m_Settings.szHost, MAX_SETTINGS_STRING);
	m_Settings.szHost[length] = '\0';
	//密码
	length = reader.Get("client", "password", "").copy(m_Settings.szPassword, MAX_SETTINGS_STRING);
	m_Settings.szPassword[length] = '\0';
	//端口
	m_Settings.iPort = reader.GetInteger("client", "port", 7777);

	// Debug
	m_Settings.bDebug = reader.GetBoolean("debug", "debug", true);
	m_Settings.bOnline = reader.GetBoolean("debug", "online", false);
}