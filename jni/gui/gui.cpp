#include "main.h"
#include "gui.h"
#include "game/game.h"
#include "net/netgame.h"
#include "game/RW/RenderWare.h"
#include "chatwindow.h"
#include "spawnscreen.h"
#include "playertags.h"
#include "dialog.h"
#include "keyboard.h"
#include "debug.h"

extern CChatWindow *pChatWindow;
extern CSpawnScreen *pSpawnScreen;
extern CPlayerTags *pPlayerTags;
extern CDialogWindow *pDialogWindow;
extern CDebug *pDebug;
extern CKeyBoard *pKeyBoard;
extern CNetGame *pNetGame;

/* imgui_impl_renderware.h */
void ImGui_ImplRenderWare_RenderDrawData(ImDrawData* draw_data);
bool ImGui_ImplRenderWare_Init();
void ImGui_ImplRenderWare_NewFrame();
void ImGui_ImplRenderWare_ShutDown();

/*
	Все координаты GUI-элементов задаются
	относительно разрешения 1920x1080
*/
#define MULT_X	0.00052083333f	// 1/1920
#define MULT_Y	0.00092592592f 	// 1/1080

CGUI::CGUI()
{
	Log("Initializing GUI..");

	// setup ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();

	ImGui_ImplRenderWare_Init();

	// scale
	m_vecScale.x = io.DisplaySize.x * MULT_X;
	m_vecScale.y = io.DisplaySize.y * MULT_Y;
	// font Size
	m_fFontSize = ScaleY(27.0f);

	// mouse/touch
	m_bMousePressed = false;
	m_vecMousePos = ImVec2(0, 0);

	Log("GUI | Scale factor: %f, %f Font size: %f", m_vecScale.x, m_vecScale.y, m_fFontSize);

	// setup style
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScrollbarSize = ScaleY(55.0f);
	style.WindowBorderSize = 0.0f;
	ImGui::StyleColorsDark();


	// load fonts
	char path[0xFF];
	sprintf(path, "%sSAMP/fonts/arial.ttf", g_pszStorage);
 static ImVector<ImWchar> ranges;
 ImFontAtlas::GlyphRangesBuilder grb;
  grb.AddRanges(io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
//添加更多不支持的字
  grb.AddText("\u6363\u6635\u8d26\u5353\u892a\u804a\u796f\u8fea\u8036\u5471\u83c5\u5624\u8759\u51f9\u8760\u51f8\u56e7\u4fa0\u5e27\u8650\u777f\u7a3d\u6de6\u51fd\u9730\u66fc\u55e8\u55bd\u7948\u5e42\u5ced\u60c6\u6005\u515c\u504e\u85fb\u6002\u607f\u4f0a\u7490\u8770\u5514\u82df\u8587\u54af\u6e32\u4fa3\u94dd\u94c0\u949a\u9891\u6fb3\u97e9\u4fc4\u7855\u753a\u658c\u5f6c\u7172\u81e5\u3007\u5c4f\u5c41\u827e\u739b\u900d\u5d29\u6e83\u5496\u903b\u82c7\u8c79\u7f09\u98d9\u7c91\u5c3b\u65ee\u65ef\u9f8c\u8e49\u5c4e\u62df\u9e64\u2640\u2642\u5594\u5537\u561b\u676d\u7784\u5362\u54ce\u9a9a\u5443\u4e2b\u9b1f\u6c50\u55b5\u603c\u548b\u55ef\u70bd\u8428\u5e87\u836b\u7325\u4eb5\u54c7\u5565\u4f6c\u5112\u58a9\u8e39\u63e3\u83b1\u8214\u5520\u55d1\u54e6\u7480\u74a8\u9887\u51cc\u5c34\u5c2c\u632b\u6233\u5c14\u7ff0\u900a\u6d1b\u8428\u6f58\u5f17\u5662\u574e\u6c2e\u4ffa\u6893\u7a46");
 grb.BuildRanges(&ranges);
	Log("GUI | Loading font: Arial.ttf");
	m_pFont = io.Fonts->AddFontFromFileTTF(path, m_fFontSize, 0, ranges.Data);
	Log("GUI | ImFont pointer = 0x%X", m_pFont);
}

CGUI::~CGUI()
{
	ImGui_ImplRenderWare_ShutDown();
	ImGui::DestroyContext();
}

void CGUI::Render()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplRenderWare_NewFrame();
	ImGui::NewFrame();

	RenderVersion();
	//RenderRakNetStatistics();
	if(pPlayerTags) pPlayerTags->Render();
	
	if(pNetGame && pNetGame->GetLabelPool())
	{
		pNetGame->GetLabelPool()->Draw();
	}
	
	if(pChatWindow) pChatWindow->Render();

	if(pDialogWindow) pDialogWindow->Render();

	if(pSpawnScreen) pSpawnScreen->Render();
	if(pKeyBoard) pKeyBoard->Render();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplRenderWare_RenderDrawData(ImGui::GetDrawData());

	if(m_bNeedClearMousePos)
	{
		io.MousePos = ImVec2(-1, -1);
		m_bNeedClearMousePos = false;
	}
}

bool CGUI::OnTouchEvent(int type, bool multi, int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();

	if(!pKeyBoard->OnTouchEvent(type, multi, x, y)) return false;
//	if(!pChatWindow->OnTouchEvent(type, multi, x, y)) return false;

	switch(type)
	{
		case TOUCH_PUSH:
		io.MousePos = ImVec2(x, y);
		io.MouseDown[0] = true;
		break;

		case TOUCH_POP:
		io.MouseDown[0] = false;
		m_bNeedClearMousePos = true;
		break;

		case TOUCH_MOVE:
		io.MousePos = ImVec2(x, y);
		break;
	}

	return true;
}

void CGUI::RenderVersion()
{
	return;

	ImGui::GetOverlayDrawList()->AddText(
		ImVec2(ScaleX(10), ScaleY(10)), 
		ImColor(IM_COL32_BLACK), PORT_VERSION);
}

void CGUI::RenderRakNetStatistics()
{
		//StatisticsToString(rss, message, 0);

		/*ImGui::GetOverlayDrawList()->AddText(
			ImVec2(ScaleX(10), ScaleY(400)),
			ImColor(IM_COL32_BLACK), message);*/
}

void CGUI::RenderText(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end)
{
	int iOffset =1;

	if(bOutline)
	{
		posCur.x -= iOffset;
		ImGui::GetOverlayDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.x += iOffset;
		// right 
		posCur.x += iOffset;
		ImGui::GetOverlayDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.x -= iOffset;
		// above
		posCur.y -= iOffset;
		ImGui::GetOverlayDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.y += iOffset;
		// below
		posCur.y += iOffset;
		ImGui::GetOverlayDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.y -= iOffset;
	}

	ImGui::GetOverlayDrawList()->AddText(posCur, col, text_begin, text_end);
}