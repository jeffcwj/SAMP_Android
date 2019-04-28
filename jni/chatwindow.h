#pragma once

#define MAX_MESSAGE_LENGTH 		128
#define MAX_LINE_LENGTH			MAX_MESSAGE_LENGTH/2
#define MAX_MESSAGES			100

enum eChatMessageType
{
	CHAT_TYPE_NONE = 0,
	CHAT_TYPE_CHAT,
	CHAT_TYPE_INFO,
	CHAT_TYPE_DEBUG
};

typedef struct _CHAT_WINDOW_ENTRY
{
	eChatMessageType eType;
	char 		szMessageUtf8[MAX_MESSAGE_LENGTH*4];
	char		szNick[MAX_PLAYER_NAME+1];
	uint32_t	dwTextColor;
	uint32_t	dwNickColor;
} CHAT_WINDOW_ENTRY;

class CChatWindow
{
public:
	CChatWindow();
	~CChatWindow();

	void FilterInvalidChars(char* szString);
	void AddToChatWindowBuffer(eChatMessageType eType, char* szString, char* szNick, uint32_t uTextColor, uint32_t uNickColor);
//这个就是不需要GBK的缓存区
	void AddToChatWindowBufferNoGBK(eChatMessageType eType, char* szString, char* szNick, uint32_t uTextColor, uint32_t uNickColor);
//不需要GBK的调试输出
	void AddDebugMessageNoGBK(char *szFormat, ...);

	void Draw();

	void AddChatMessage(char *szNick, uint32_t dwNickColor, char *szMessage);
	void AddInfoMessage(char *szFormat, ...);
	void AddDebugMessage(char *szFormat, ...);
	void AddClientMessage(uint32_t dwColor, char* szStr);

private:
	std::list<CHAT_WINDOW_ENTRY> m_ChatWindowEntriesList;

	uint32_t m_dwChatTextColor;
	uint32_t m_dwChatInfoColor;
	uint32_t m_dwChatDebugColor;
};
