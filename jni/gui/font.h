#pragma once

#define RGBA_ABGR(color) color = ((color << 24) | ((color << 8) & 0x00FF0000) | ((color >> 8) & 0x0000FF00) | (color >> 24))
#define ARGB_RGBA(color) color = (color << 8 | color >> 24)

enum eFontStyle : uint8_t
{
	FONT_GOTHIC,
	FONT_SUBTITLES,
	FONT_MENU,
	FONT_PRICEDOWN
};

class CFont
{
public:
	
	static void PrintString(float x, float y, uint16_t *text);
	static void RenderFontBuffer();
	static void SetScale(float scale);
	static void SetSlant(float slant);
	static void SetColor(uint8_t *color);
	static void SetFontStyle(eFontStyle style);
	static void SetWrapx(float value);
	static void SetCentreSize(float value);
	static void SetJustify(int value);
	static void SetEdge(int value);
	static void SetRightJustify(int value);
	static void SetAlphaFade(float alpha);
	static void SetDropColor(uint32_t color);
	static void SetDropShadowPosition(int value);
	static void SetProportional(bool on);
	static void SetOrientation(uint32_t  value);
	static void SetBackground(bool enable, bool includeWrap);
	static void SetBackgroundColor(uint32_t color);
	static float GetStringWidth(uint16_t *string, bool unk1, bool unk2);
	static int GetNumberLines(bool print, float x, float y, uint16_t *text);
	static void GetTextRect(void *rect, float x, float y, uint16_t *text);
	static void AsciiToGxtChar(char* ascii, uint16_t *gxt);
	static unsigned int GxtCharStrlen(uint16_t const* gxt);
	static uint16_t * GxtCharStrcat(uint16_t * gxt1, uint16_t * gxt2);
};

