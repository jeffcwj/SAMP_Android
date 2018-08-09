#include "main.h"

void CFont::PrintString(float x, float y, uint16_t *text)
{
	void (*_PrintString)(float x, float y, uint16_t *text);
    *(void **) (&_PrintString) = (void*)(g_libGTASA+0x5353B4+1);
    (*_PrintString)(x, y, text);
}

void CFont::RenderFontBuffer()
{
    void (*_RenderFontBuffer)();
    *(void **) (&_RenderFontBuffer) = (void*)(g_libGTASA+0x53411C+1);
    (*_RenderFontBuffer)();
}

void CFont::SetScale(float scale)
{
	void (*_SetScale)(float scale);
    *(void **) (&_SetScale) = (void*)(g_libGTASA+0x533694+1);
    (*_SetScale)(scale);
}

void CFont::SetSlant(float slant)
{
	void (*_SetSlant)(float slant);
    *(void **) (&_SetSlant) = (void*)(g_libGTASA+0x5336DC+1);
    (*_SetSlant)(slant);
}

void CFont::SetColor(uint8_t* color)
{
	void (*_SetColor)(uint8_t* color);
    *(void **) (&_SetColor) = (void*)(g_libGTASA+0x5336F4+1);
    (*_SetColor)(color);
}

void CFont::SetFontStyle(eFontStyle style)
{
	void (*_SetFontStyle)(eFontStyle style);
    *(void **) (&_SetFontStyle) = (void*)(g_libGTASA+0x533748+1);
    (*_SetFontStyle)(style);
}

void CFont::SetWrapx(float value)
{
	void (*_SetWrapx)(float value);
    *(void **) (&_SetWrapx) = (void*)(g_libGTASA+0x53381C+1);
    (*_SetWrapx)(value);
}

void CFont::SetCentreSize(float value)
{
	void (*_SetCentreSize)(float value);
    *(void **) (&_SetCentreSize) = (void*)(g_libGTASA+0x533834+1);
    (*_SetCentreSize)(value);
}

void CFont::SetEdge(int value)
{
    void (*_SetEdge)(int value);
    *(void **) (&_SetEdge) = (void*)(g_libGTASA+0x53394C+1);
    (*_SetEdge)(value);
}

void CFont::SetJustify(int value)
{
    void (*_SetJustify)(int value);
    *(void **) (&_SetJustify) = (void*)(g_libGTASA+0x5339D0+1);
    (*_SetJustify)(value);
}

void CFont::SetRightJustify(int value)
{
	void (*_SetRightJustify)(int value);
    *(void **) (&_SetRightJustify) = (void*)(g_libGTASA+0x53384C+1);
    (*_SetRightJustify)(value);
}

void CFont::SetAlphaFade(float alpha)
{
	void (*_SetAlphaFade)(float alpha);
    *(void **) (&_SetAlphaFade) = (void*)(g_libGTASA+0x533864+1);
    (*_SetAlphaFade)(alpha);
}

void CFont::SetDropColor(uint32_t color)
{
	void (*_SetDropColor)(uint32_t color);
    *(void **) (&_SetDropColor) = (void*)(g_libGTASA+0x53387C+1);
    (*_SetDropColor)(color);
}

void CFont::SetDropShadowPosition(int value)
{
	void (*_SetDropShadowPosition)(int value);
    *(void **) (&_SetDropShadowPosition) = (void*)(g_libGTASA+0x5338DC+1);
    (*_SetDropShadowPosition)(value);
}

void CFont::SetProportional(bool on)
{
	void (*_SetProportional)(bool on);
    *(void **) (&_SetProportional) = (void*)(g_libGTASA+0x533970+1);
    (*_SetProportional)(on);
}

void CFont::SetOrientation(uint32_t  value)
{
    void (*_SetOrientation)(uint32_t value);
    *(void **) (&_SetOrientation) = (void*)(g_libGTASA+0x5339E8+1);
    (*_SetOrientation)(value);
}

void CFont::SetBackground(bool enable, bool includeWrap)
{
	void (*_SetBackground)(bool enable, bool includeWrap);
    *(void **) (&_SetBackground) = (void*)(g_libGTASA+0x533988+1);
    (*_SetBackground)(enable, includeWrap);
}

void CFont::SetBackgroundColor(uint32_t color)
{
	void (*_SetBackgroundColor)(uint32_t color);
    *(void **) (&_SetBackgroundColor) = (void*)(g_libGTASA+0x5339A4+1);
    (*_SetBackgroundColor)(color);
}

float CFont::GetStringWidth(uint16_t *string, bool unk1, bool unk2)
{
	float (*_GetStringWidth)(uint16_t *string, bool unk1, bool unk2);
    *(void **) (&_GetStringWidth) = (void*)(g_libGTASA+0x534BAC+1);
    return (*_GetStringWidth)(string, unk1, unk2);
}

int CFont::GetNumberLines(bool print, float x, float y, uint16_t *text)
{
	int (*_GetNumberLines)(bool print, float x, float y, uint16_t *text);
    *(void **) (&_GetNumberLines) = (void*)(g_libGTASA+0x5352C4+1);
    return (*_GetNumberLines)(print, x, y, text);
}

void CFont::GetTextRect(void *rect, float x, float y, uint16_t *text)
{
	void (*_GetTextRect)(void *rect, float x, float y, uint16_t *text);
    *(void **) (&_GetTextRect) = (void*)(g_libGTASA+0x5352C4+1);
    (*_GetTextRect)(rect, x, y, text);
}

void CFont::AsciiToGxtChar(char* ascii, uint16_t *gxt)
{
    void (*_AsciiToGxtChar)(char* ascii, uint16_t* gxt);
    *(void **) (&_AsciiToGxtChar) = (void*)(g_libGTASA+0x532D00+1);
    (*_AsciiToGxtChar)(ascii, gxt);
}

unsigned int CFont::GxtCharStrlen(uint16_t const* gxt)
{
    unsigned int (*_GxtCharStrlen)(uint16_t const*);
    *(void **) (&_GxtCharStrlen) = (void*)(g_libGTASA+0x532D7C+1);
    return (*_GxtCharStrlen)(gxt);
}

uint16_t * CFont::GxtCharStrcat(uint16_t * gxt1, uint16_t * gxt2)
{
    uint16_t * (*_GxtCharStrcat)(uint16_t * gxt1, uint16_t * gxt2);
    *(void **) (&_GxtCharStrcat) = (void*)(g_libGTASA+0x532D7C+1);
    return (*_GxtCharStrcat)(gxt1, gxt2);
}
