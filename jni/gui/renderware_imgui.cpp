#include "main.h"
#include "RenderWare/RenderWare.h"
#include "imgui.h"
#include "renderware_imgui.h"
#include "EncodingUtils.hpp"

#define FONT_SCALE 0.015625f // 33px

EncodingUtils *eu=new EncodingUtils();

RwRaster* 	g_FontTexture = 0;
uint32_t	g_Time = 0;

bool ImGuiPlus::ProcessInlineHexColor(const char* start, const char* end, ImVec4& color)
{
  const int hexCount = (int)(end - start);
  if (hexCount == 6 || hexCount == 8)
  {
    char hex[9];
    strncpy(hex, start, hexCount);
    hex[hexCount] = 0;

    unsigned int hexColor = 0;
    if (sscanf(hex, "%x", &hexColor) > 0)
    {
      color.x = static_cast< float >((hexColor & 0x00FF0000) >> 16) / 255.0f;
      color.y = static_cast< float >((hexColor & 0x0000FF00) >> 8) / 255.0f;
      color.z = static_cast< float >((hexColor & 0x000000FF)) / 255.0f;
      color.w = 1.0f;

      if (hexCount == 8)
      {
        color.w = static_cast< float >((hexColor & 0xFF000000) >> 24) / 255.0f;
      }

      return true;
    }
  }

  return false;
}
void ImGuiPlus::TextWithColors(const char* fmt, ...)
{
  char tempStr[4096];

  va_list argPtr;
  va_start(argPtr, fmt);
  vsnprintf(tempStr, sizeof(tempStr), fmt, argPtr);
  va_end(argPtr);
  tempStr[sizeof(tempStr) - 1] = '\0';

  bool pushedColorStyle = false;
  const char* textStart = tempStr;
  const char* textCur = tempStr;
  while (textCur < (tempStr + sizeof(tempStr)) && *textCur != '\0')
  {
    if (*textCur == '[')
    {
      // Print accumulated text
      if (textCur != textStart)
      {
        ImGui::TextUnformatted(textStart, textCur);
        ImGui::SameLine(0.0f, 0.0f);
      }

      // Process color code
      const char* colorStart = textCur + 1;
      do
      {
        ++textCur;
      } while (*textCur != '\0' && *textCur != ']');

      // Change color
      if (pushedColorStyle)
      {
        ImGui::PopStyleColor();
        pushedColorStyle = false;
      }

      ImVec4 textColor;
      if (ProcessInlineHexColor(colorStart, textCur, textColor))
      {
        ImGui::PushStyleColor(ImGuiCol_Text, textColor);
        pushedColorStyle = true;
      }

      textStart = textCur + 1;
    }
    else if (*textCur == '\n')
    {
      // Print accumulated text an go to next line
      ImGui::TextUnformatted(textStart, textCur);
      textStart = textCur + 1;
    }

    ++textCur;
  }

  if (textCur != textStart)
  {
    ImGui::TextUnformatted(textStart, textCur);
  }
  else
  {
    ImGui::NewLine();
  }

  if (pushedColorStyle)
  {
    ImGui::PopStyleColor();
  }
}
//简化转码
int ImGuiPlus::code_to(char *f_charset,char* to_charset,char *inbuf,int inlen,char *outbuf,int outlen){
 
iconv_t cd;
int r;
//输入字符串缓存区
char **in=&inbuf;
//输出字符串缓存区
char **out=&outbuf;
//打开编码模式
cd=iconv_open(to_charset,f_charset);
//如果打开失败就结束
if(cd==0) return -1;
//内存设置
memset(outbuf,0,outlen);
//开始转码
if(iconv(cd,in,(size_t*)&inlen,out,(size_t*)&outlen)==-1) return -1;
//转码后关闭iconv
iconv_close(cd);
//结束运行
return 0;
 
}

//gbk to utf8
int ImGuiPlus::g_to_u(char *inbuf,int inlen,char *outbuf,int outlen)  
{
return code_to("GBK","UTF-8",inbuf,inlen,outbuf,outlen);
}
 
//utf8转gbk
char* ImGuiPlus::utf8_to_gbk(char *in){
return eu->convertUTF8toGBK(in);
}
//gbk转utf8
void ImGuiPlus::gbk_to_utf8(char *out,char *in){
g_to_u(in,strlen(in),out,255);
}
//默认的中文支持
void ImGuiPlus::def_lang(char *out, const char *in) 
{
   static const int table[] = 
    { 
        0x82D0,0x83D0,0x9A80E2,0x93D1,0x9E80E2,0xA680E2,0xA080E2,0xA180E2,
        0xAC82E2,0xB080E2,0x89D0,0xB980E2,0x8AD0,0x8CD0,0x8BD0,0x8FD0,    
        0x92D1,0x9880E2,0x9980E2,0x9C80E2,0x9D80E2,0xA280E2,0x9380E2,0x9480E2,
        0,0xA284E2,0x99D1,0xBA80E2,0x9AD1,0x9CD1,0x9BD1,0x9FD1,               
        0xA0C2,0x8ED0,0x9ED1,0x88D0,0xA4C2,0x90D2,0xA6C2,0xA7C2,              
        0x81D0,0xA9C2,0x84D0,0xABC2,0xACC2,0xADC2,0xAEC2,0x87D0,              
        0xB0C2,0xB1C2,0x86D0,0x96D1,0x91D2,0xB5C2,0xB6C2,0xB7C2,              
        0x91D1,0x9684E2,0x94D1,0xBBC2,0x98D1,0x85D0,0x95D1,0x97D1,            
        0x90D0,0x91D0,0x92D0,0x93D0,0x94D0,0x95D0,0x96D0,0x97D0,
        0x98D0,0x99D0,0x9AD0,0x9BD0,0x9CD0,0x9DD0,0x9ED0,0x9FD0,
        0xA0D0,0xA1D0,0xA2D0,0xA3D0,0xA4D0,0xA5D0,0xA6D0,0xA7D0,
        0xA8D0,0xA9D0,0xAAD0,0xABD0,0xACD0,0xADD0,0xAED0,0xAFD0,
        0xB0D0,0xB1D0,0xB2D0,0xB3D0,0xB4D0,0xB5D0,0xB6D0,0xB7D0,
        0xB8D0,0xB9D0,0xBAD0,0xBBD0,0xBCD0,0xBDD0,0xBED0,0xBFD0,
        0x80D1,0x81D1,0x82D1,0x83D1,0x84D1,0x85D1,0x86D1,0x87D1,
        0x88D1,0x89D1,0x8AD1,0x8BD1,0x8CD1,0x8DD1,0x8ED1,0x8FD1
		};
		//判断输入是否为空
  while (*in)
    {
    //不是空就进入循环
    
     if (*in & 0x00) 
        {
            int v = table[(int)(0x7f & *in++)];
            if (!v)
                continue;
            *out++ = (char)v;
            *out++ = (char)(v >> 8);
            if (v >>= 16)
                *out++ = (char)v;
        }
        else
            *out++ = *in++;
    }
//是空就填充为0
    *out = 0;
    }
void ImGui_RenderWare_Init()
{
	ImGuiIO &io = ImGui::GetIO();
	io.RenderDrawListsFn = ImGui_RenderWare_DrawLists;

	LOGI("DisplaySize = %dx%d", RsGlobal->maximumWidth, RsGlobal->maximumHeight);
}

void ImGui_RenderWare_Shutdown()
{
	ImGui_RenderWare_InvalidateDeviceObjects();
	ImGui::Shutdown();
}

void ImGui_RenderWare_NewFrame()
{
	if(!g_FontTexture)
		ImGui_RenderWare_CreateDeviceObjects();

	ImGuiIO &io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float)RsGlobal->maximumWidth, (float)RsGlobal->maximumHeight);

	uint32_t time = GetTickCount();
	float current_time = (float)(time/1000);
	//io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f/60.0f);
	g_Time = current_time;

	ImGui::NewFrame();
}
//绘制列表
void ImGui_RenderWare_DrawLists(ImDrawData *draw_data)
{
	ImGuiIO &io = ImGui::GetIO();
	if(io.DisplaySize.x <= 0.0f || io.DisplaySize.y <= 0.0f) return;

	static RwReal nearScreenZ = 	*(RwReal*)(g_libGTASA+0x9DAA60);	// CSprite2d::NearScreenZ 009DAA60
	static RwReal recipNearClip = 	*(RwReal*)(g_libGTASA+0x9DAA64);	// CSprite2d::RecipNearClip 009DAA64
	static RwIm2DVertex* vert = 0;

	for(int n=0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;
		const int nVert = cmd_list->VtxBuffer.Size;
		vert = new RwIm2DVertex[nVert];

		for(int i=0; i<nVert; i++)
		{
			RwIm2DVertexSetScreenX(&vert[i], cmd_list->VtxBuffer[i].pos.x);
			RwIm2DVertexSetScreenY(&vert[i], cmd_list->VtxBuffer[i].pos.y);
			RwIm2DVertexSetScreenZ(&vert[i], nearScreenZ);
			RwIm2DVertexSetRecipCameraZ(&vert[i], recipNearClip);
			//vert[i].emissiveColor = (
			//	cmd_list->VtxBuffer[i].col & 0xFF00FF00) 
			//| ((cmd_list->VtxBuffer[i].col & 0xFF0000) >> 16) 
			//| ((cmd_list->VtxBuffer[i].col & 0xFF) << 16);
			vert[i].emissiveColor = cmd_list->VtxBuffer[i].col;
			RwIm2DVertexSetU(&vert[i], cmd_list->VtxBuffer[i].uv.x, recipCameraZ);
			RwIm2DVertexSetV(&vert[i], cmd_list->VtxBuffer[i].uv.y, recipCameraZ);
		}

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
//用户回调
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE);
				RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)1);
				RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
				RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)pcmd->TextureId);
				RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, vert, nVert, (RwImVertexIndex*)idx_buffer, pcmd->ElemCount);
			}

			idx_buffer += pcmd->ElemCount;
		}

		if(vert)
		{
			delete vert;
			vert = 0;
		}
	}
}
//创建
void ImGui_RenderWare_CreateDeviceObjects()
{
//得到IO
	ImGuiIO &io = ImGui::GetIO();
	unsigned char* pxs;
	//长宽像素
	int width, height, bytes_per_pixel;
	char path[0xFF];
	//得到游戏数据目录
	char* storage = (char*)(g_libGTASA+0x63C4B8);
	sprintf(path, "%sSAMP/fonts/arial.ttf", storage);
	ImVector<ImWchar> ranges;
 ImFontAtlas::GlyphRangesBuilder grb;
 grb.AddText("\u8fea\u8036\u5471\u83c5\u5624\u8759\u51f9\u8760\u51f8\u56e7\u4fa0\u5e27\u8650\u777f\u7a3d\u6de6\u51fd\u9730\u66fc\u55e8\u55bd\u7948\u5e42\u5ced\u60c6\u6005\u515c\u504e\u85fb\u6002\u607f\u4f0a\u7490\u8770\u5514\u82df\u8587\u54af\u6e32\u4fa3\u94dd\u94c0\u949a\u9891\u6fb3\u97e9\u4fc4\u7855\u753a\u658c\u5f6c\u7172\u81e5\u3007\u5c4f\u5c41\u827e\u739b\u900d\u5d29\u6e83\u5496\u903b\u82c7\u8c79\u7f09\u98d9\u7c91\u5c3b\u65ee\u65ef\u9f8c\u8e49\u5c4e\u62df\u9e64\u2640\u2642\u5594\u5537\u561b\u676d\u7784\u5362\u54ce\u9a9a\u5443\u4e2b\u9b1f\u6c50\u55b5\u603c\u548b\u55ef\u70bd\u8428\u5e87\u836b\u7325\u4eb5\u54c7\u5565\u4f6c\u5112\u58a9\u8e39\u63e3\u83b1\u8214\u5520\u55d1\u54e6\u7480\u74a8\u9887\u51cc\u5c34\u5c2c\u632b\u6233\u5c14\u7ff0\u900a\u6d1b\u8428\u6f58\u5f17\u5662\u574e\u6c2e\u4ffa\u6893\u7a46");
  //聊
 grb.AddChar(0x804a);
// 卓
 grb.AddChar(0x5353);
 //褪
 grb.AddChar(0x892a);
 grb.AddRanges(io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
 grb.BuildRanges(&ranges);
//添加字体文件
	io.Fonts->AddFontFromFileTTF(path,27/*字体大小*/,0,ranges.Data/*编码模式*/);
	io.Fonts->GetTexDataAsRGBA32(&pxs, &width, &height, &bytes_per_pixel);
	LOGI("bytes_per_pixel: %d, %d, %d", width, height, bytes_per_pixel);

	RwImage *font_img = RwImageCreate(width, height, bytes_per_pixel*8);
	RwImageAllocatePixels(font_img);

	RwUInt8 *pixels = font_img->cpPixels;
	for(int y = 0; y < font_img->height; y++)
	{
		memcpy((unsigned char*)pixels, pxs + font_img->stride * y, font_img->stride);
		pixels += font_img->stride;
	}

	RwInt32 w, h, d, flags;
	RwImageFindRasterFormat(font_img, rwRASTERTYPETEXTURE, &w, &h, &d, &flags);
	g_FontTexture = RwRasterCreate(w, h, d, flags);
	g_FontTexture = RwRasterSetFromImage(g_FontTexture, font_img);
	RwImageDestroy(font_img);

	io.Fonts->SetTexID((ImTextureID)g_FontTexture);
}

//销毁
void ImGui_RenderWare_InvalidateDeviceObjects()
{
	if(g_FontTexture)
	{
		RwRasterDestroy(g_FontTexture);
		ImGui::GetIO().Fonts->SetTexID(0);
		g_FontTexture = 0;
	}
}
