#include "StalkerHUD.h"

#include "Kernel/RBMK/Render/Interface/UI/XRayUIRender.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/IGame_Persistent.h"
#include "XrEngine/CustomHUD.h"
#include "XrEngine/IGame_Level.h"
THIRD_PARTY_INCLUDES_END
extern ENGINE_API  BOOL g_bRendering;

AStalkerHUD::AStalkerHUD()
{
}

void AStalkerHUD::DrawHUD()
{
	Super::DrawHUD();
	if(Device->dwFrame>LastFrame)
	{
		LastFrame = Device->dwFrame;
		g_bRendering = true;
		if(g_hud && g_pGameLevel)
		{
			g_hud->RenderUI();
		}
		if (g_pGamePersistent)	
		{
			g_pGamePersistent->OnRenderPPUI_main();
			if (psDeviceFlags.is_any(rsCameraPos) || psDeviceFlags.is_any(rsStatistic) || !Device->Statistic->errors.empty())
			{
				Device->Statistic->Show();
			}
			g_pGamePersistent->OnRenderPPUI_PP();
		}
		Device->seqRenderUI.Process(rp_RenderUI);
		g_bRendering = false;
	}
}
