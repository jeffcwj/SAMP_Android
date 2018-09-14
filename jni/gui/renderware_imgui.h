#pragma once

namespace ImGuiPlus
{
	bool          	ProcessInlineHexColor(const char* start, const char* end, ImVec4& color);
	void          	TextWithColors(const char* fmt, ...);
//	void			cp1251_to_utf8(char *out, const char *in);
int code_to(char *f_charset,char* to_charset,char *inbuf,int inlen,char *outbuf,int outlen);
	//gbk转utf8
void gbk_to_utf8(char *out,char *in);
int g_to_u(char *inbuf,int inlen,char *outbuf,int outlen);
//utf8转gbk
char* utf8_to_gbk(char *in);
//默认
void mo_ren(char *out, const char *in);
}

void ImGui_RenderWare_Init();
void ImGui_RenderWare_Shutdown();
void ImGui_RenderWare_NewFrame();
void ImGui_RenderWare_DrawLists(ImDrawData *draw_data);
void ImGui_RenderWare_CreateDeviceObjects();
void ImGui_RenderWare_InvalidateDeviceObjects();