#include "main.h"
#include "imgui.h"
#include "RenderWare/RenderWare.h"
#include "gui/renderware_imgui.h"

extern CGame *pGame;

#define CHAT_POS_X		0.169270f // 325
#define CHAT_POS_Y		0.013148f // 25
#define CHAT_SIZE_X		0.598958f // 1150
#define CHAT_SIZE_Y		0.259703f // 220

CChatWindow::CChatWindow()
{
	m_dwChatTextColor 	= 0xFFFFFFFF;
	m_dwChatInfoColor 	= 0x00C8C8FF;
	m_dwChatDebugColor 	= 0xBEBEBEFF;
}

CChatWindow::~CChatWindow(){}

void CChatWindow::Draw()
{
	// 1920x1080
	ImGui::Begin("Chat Window", (bool*)true,  ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize);
	//位置
	ImGui::SetWindowPos(ImVec2(CHAT_POS_X*RsGlobal->maximumWidth, CHAT_POS_Y*RsGlobal->maximumHeight));
//大小
		ImGui::SetWindowSize(ImVec2(CHAT_SIZE_X*RsGlobal->maximumWidth, CHAT_SIZE_Y*RsGlobal->maximumHeight));

	std::list<CHAT_WINDOW_ENTRY>::iterator it = m_ChatWindowEntriesList.begin();
	while(it != m_ChatWindowEntriesList.end())
	{
		switch(it->eType)
		{
			case CHAT_TYPE_CHAT:
				if(it->szNick[0] != 0)
				{
					ImGui::TextColored(ImColor(it->dwNickColor), "%s", it->szNick);
					ImGui::SameLine();
				}
				ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(it->dwTextColor));
				ImGuiPlus::TextWithColors("%s", it->szMessageUtf8);
				ImGui::PopStyleColor();
			break;

			case CHAT_TYPE_INFO:
			case CHAT_TYPE_DEBUG:
				ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(it->dwTextColor));
				ImGuiPlus::TextWithColors("%s", it->szMessageUtf8);
				ImGui::PopStyleColor();
			break;
		}

		it++;
	}

	ImGui::SetScrollHere();
	ImGui::End();
}

void CChatWindow::AddChatMessage(char *szNick, uint32_t dwNickColor, char *szMessage)
{
	FilterInvalidChars(szMessage);
	AddToChatWindowBuffer(CHAT_TYPE_CHAT, szMessage, szNick, m_dwChatTextColor, dwNickColor);
}

void CChatWindow::AddInfoMessage(char *szFormat, ...)
{
	char tmp_buffer[512];
	memset(tmp_buffer, 0, 512);

	va_list args;
	va_start(args, szFormat);
	vsprintf(tmp_buffer, szFormat, args);
	va_end(args);

	FilterInvalidChars(tmp_buffer);
	AddToChatWindowBuffer(CHAT_TYPE_INFO, tmp_buffer, 0, m_dwChatInfoColor, 0);
}

void CChatWindow::AddDebugMessage(char *szFormat, ...)
{
	char tmp_buf[512];
	memset(tmp_buf,0,512);

	va_list args;
	va_start(args, szFormat);
	vsprintf(tmp_buf, szFormat, args);
	va_end(args);

	FilterInvalidChars(tmp_buf);
	AddToChatWindowBuffer(CHAT_TYPE_DEBUG, tmp_buf, 0, m_dwChatDebugColor, 0);
}

void CChatWindow::AddClientMessage(uint32_t dwColor, char* szStr)
{
	FilterInvalidChars(szStr);
	AddToChatWindowBuffer(CHAT_TYPE_INFO, szStr, 0, dwColor, 0);
}

void CChatWindow::FilterInvalidChars(char* szString)
{
//确保不是空
	while(*szString) 
	{
		if(*szString > 0 && *szString < ' ') 
			*szString = ' ';

		szString++;
	}
}

void CChatWindow::AddToChatWindowBuffer(eChatMessageType eType, char* szString, 
										char* szNick, uint32_t dwTextColor, uint32_t dwNickColor)
{
	RGBA_ABGR(dwTextColor);
	RGBA_ABGR(dwNickColor);

	CHAT_WINDOW_ENTRY ChatWindowEntry;

	ChatWindowEntry.eType = eType;
	ChatWindowEntry.dwTextColor = dwTextColor;
	ChatWindowEntry.dwNickColor = dwNickColor;

	if(szNick)
	{
		strcpy(ChatWindowEntry.szNick, szNick);
		strcat(ChatWindowEntry.szNick, ":");
	}
	else
		ChatWindowEntry.szNick[0] = '\0';
ImGuiPlus::gbk_to_utf8(ChatWindowEntry.szMessageUtf8,szString);

	if(m_ChatWindowEntriesList.size() > MAX_MESSAGES)
		m_ChatWindowEntriesList.pop_front();

	m_ChatWindowEntriesList.push_back(ChatWindowEntry);
}

/**

下面开始扯淡

**/
//-----------------------------------------------------------------


//不需要GBK的调试对话框(这哥们不需要)
void CChatWindow::AddDebugMessageNoGBK(char *szFormat, ...)
{
	char tmp_buf[512];
	memset(tmp_buf,0,512);

	va_list args;
	va_start(args, szFormat);
	vsprintf(tmp_buf, szFormat, args);
	va_end(args);

	FilterInvalidChars(tmp_buf);
	AddToChatWindowBufferNoGBK(CHAT_TYPE_DEBUG, tmp_buf, 0, m_dwChatDebugColor, 0);
}

//不需要GBK的消息)缓存区
void CChatWindow::AddToChatWindowBufferNoGBK(eChatMessageType eType, char* szString, 
										char* szNick, uint32_t dwTextColor, uint32_t dwNickColor)
{
	RGBA_ABGR(dwTextColor);
	RGBA_ABGR(dwNickColor);

	CHAT_WINDOW_ENTRY ChatWindowEntry;

	ChatWindowEntry.eType = eType;
	ChatWindowEntry.dwTextColor = dwTextColor;
	ChatWindowEntry.dwNickColor = dwNickColor;

	if(szNick)
	{
		strcpy(ChatWindowEntry.szNick, szNick);
		strcat(ChatWindowEntry.szNick, ":");
	}
	else
		ChatWindowEntry.szNick[0] = '\0';
ImGuiPlus::mo_ren(ChatWindowEntry.szMessageUtf8, szString);

	if(m_ChatWindowEntriesList.size() > MAX_MESSAGES)
		m_ChatWindowEntriesList.pop_front();

	m_ChatWindowEntriesList.push_back(ChatWindowEntry);
}