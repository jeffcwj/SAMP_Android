#include "main.h"
#include "game/util.h"
#include "game/keystuff.h"
#include "imgui.h"
#include "RenderWare/RenderWare.h"
#include "gui/renderware_imgui.h"

extern CNetGame *pNetGame;
extern CGame *pGame;
extern CChatWindow *pChatWindow;

void DrawPlayerTags()
{
	MATRIX4X4 matPlayer, matLocal;
	uint16_t buf[50];

	if(pNetGame)
	{
		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
		pGame->FindPlayerPed()->GetMatrix(&matLocal);

		for (int x=0; x < MAX_PLAYERS; x++)
		{
			if (pPlayerPool->GetSlotState(x) == true) 
			{
				CRemotePlayer* Player = pPlayerPool->GetAt(x);
				if(Player && Player->IsActive())
				{
					CPlayerPed* PlayerPed = Player->GetPlayerPed();
					if(PlayerPed && Player->GetDistanceFromLocalPlayer() <= pNetGame->m_fNameTagDrawDistance)
					{
						PlayerPed->GetMatrix(&matPlayer);
						VECTOR tagPos;
						tagPos.X = matPlayer.pos.X;;
						tagPos.Y = matPlayer.pos.Y;
						tagPos.Z = matPlayer.pos.Z;
						tagPos.Z += 1.0f + (Player->GetDistanceFromLocalPlayer() * 0.05f);

						VECTOR out;
						// CSprite::CalcScreenCoors
						bool (*CalcScreenCoors)(PVECTOR RwV3d_posn, PVECTOR RwV3d_out, float *w, float *h, bool checkMaxVisible, bool checkMinVisible);
						*(void**) (&CalcScreenCoors) = (void*)(g_libGTASA+0x54EEC0+1);
						(*CalcScreenCoors)(&tagPos, &out, 0, 0, 0, 0);

						if(out.Z < 1.0f)
							continue;

						CFont::SetOrientation(1);
						CFont::SetProportional(1);
						CFont::SetFontStyle(FONT_SUBTITLES);
						uint32_t uColor = Player->GetPlayerColorAsRGBA();
						RGBA_ABGR(uColor);
						CFont::SetColor((uint8_t*)&uColor);
						CFont::SetScale(1.0f);
						char * str=(char*)malloc(1024);
						//转码
		 		ImGuiPlus::mo_ren(str,(char*)pPlayerPool->GetPlayerName(x));
						CFont::AsciiToGxtChar(str, buf);
						float len = CFont::GetStringWidth(buf, 0, 0);
						CFont::PrintString((out.X - (len/2)) , out.Y, buf);

						// HEALTH BAR
						float fHealth = Player->GetReportedHealth();
						if(fHealth > 100.0f)
							fHealth = 100.0f;
						fHealth /= 2.6f;
						fHealth -= 19.0f;

						RECT HealthBarBGRect;
						RECT HealthBarRect;
						HealthBarBGRect.x 	= HealthBarRect.x 	= (float)out.X - 19.0f; // top left
						HealthBarBGRect.y 	= HealthBarRect.y 	= (float)out.Y + 19.0f;
						HealthBarBGRect.x1 	= HealthBarRect.x1 	= (float)out.X + 19.0f; // bottom right
						HealthBarBGRect.y1 	= HealthBarRect.y1	= (float)out.Y + 23.0f;
						uint32_t BGColor = 0xFF140B4B;
						uint32_t color = 0xFF2822B9;

						HealthBarRect.x1 = out.X + fHealth;

						uint32_t (*DrawRect)(PRECT rect, uint32_t *color);
						*(void**) (&DrawRect) = (void*)(g_libGTASA+0x5529AC+1);
						// background
						(*DrawRect)(&HealthBarBGRect, &BGColor);
						// health
						(*DrawRect)(&HealthBarRect, &color);
					}
				}
			}
		}
	}
}