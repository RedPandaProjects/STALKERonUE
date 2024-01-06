// Fill out your copyright notice in the Description page of Project Settings.


#include "StalkerGameViewportClient.h"

#include "Resources/StalkerResourcesManager.h"
#include "Resources/Sound/StalkerSoundManager.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/XrDeviceInterface.h"
#include "XrEngine/CustomHUD.h"
#include "XrEngine/IGame_Level.h"
THIRD_PARTY_INCLUDES_END
#include "Kernel/StalkerEngineManager.h"
#include "Kernel/RBMK/Engine/RBMKInput.h"
#include "../GameMode/StalkerGameMode.h"
#include "../WorldSettings/StalkerWorldSettings.h"
#include "../LevelScriptActor/StalkerLevelScriptActor.h"
#include "Engine/Console.h"
DECLARE_CYCLE_STAT(TEXT("XRay ~ Frame"), STAT_XRayEngineFrame, STATGROUP_XRayEngine);
DECLARE_CYCLE_STAT(TEXT("XRay ~ MT Frame"), STAT_XRayEngineMTFrame, STATGROUP_XRayEngine);
void UStalkerGameViewportClient::Activated(FViewport* InViewport, const FWindowActivateEvent& InActivateEvent)
{
	Super::Activated(InViewport, InActivateEvent);
	if (!IsActive)
	{
		if (GStalkerEngineManager->GetGameViewportClient() == this)
		{
			Device->seqAppActivate.Process(rp_AppActivate);
			IsActive = true;
		}
	}
}

void UStalkerGameViewportClient::Deactivated(FViewport* InViewport, const FWindowActivateEvent& InActivateEvent)
{
	Super::Deactivated(InViewport, InActivateEvent);
	if (IsActive)
	{
		Device->seqAppDeactivate.Process(rp_AppDeactivate);
		IsActive = false;
	}
}

bool UStalkerGameViewportClient::InputKey(const FInputKeyEventArgs& InKeyEvent)
{
	bool Result  = Super::InputKey(InKeyEvent);
	if (IgnoreInput())
	{
		return false;
	}
	if (ViewportConsole && ViewportConsole->ConsoleState != NAME_None)
	{
		return Result;
	}
	if (GStalkerEngineManager->GetGameViewportClient()==this)
	{
		GStalkerEngineManager->GetInput()->KeyEvent(InKeyEvent);
		return true;
	}
	return Result;
}


bool UStalkerGameViewportClient::InputAxis(FViewport* InViewport, FInputDeviceId InputDevice, FKey Key, float Delta, float DeltaTime, int32 NumSamples /*= 1*/, bool bGamepad /*= false*/)
{
	if (!Super::InputAxis(InViewport, InputDevice, Key, Delta, DeltaTime, NumSamples, bGamepad) || (InViewport->IsSlateViewport() && InViewport->IsPlayInEditorViewport()))
	{
		if (IgnoreInput())
		{
			return false;
		}
		if (GStalkerEngineManager->GetGameViewportClient() == this)
		{
			if (Key.GetFName() == EKeys::MouseX)
			{
				GStalkerEngineManager->GetInput()->MouseEvent(Delta, 0);
			}
			if (Key.GetFName() == EKeys::MouseY)
			{
				GStalkerEngineManager->GetInput()->MouseEvent(0, Delta);
			}
			if (Key.GetFName() == EKeys::MouseWheelAxis)
			{
				GStalkerEngineManager->GetInput()->MouseWheelEvent(Delta);
			}
			return true;
		}
		return false;
	}
	return true;
}
void UStalkerGameViewportClient::LostFocus(FViewport* InViewport)
{
	Super::LostFocus(InViewport);
	if (GIsEditor && GStalkerEngineManager->GetGameViewportClient() == this)
	{
		if (IsActive)
		{
			Device->seqAppDeactivate.Process(rp_AppDeactivate);
			IsActive = false;
		}
	}
}

void UStalkerGameViewportClient::ReceivedFocus(FViewport* InViewport)
{
	Super::ReceivedFocus(InViewport);
	if (GIsEditor)
	{
		if (!IsActive)
		{
			Device->seqAppActivate.Process(rp_AppActivate);
			IsActive = true;
		}
	}
}

void UStalkerGameViewportClient::BeginDestroy()
{
	check(GStalkerEngineManager->GetGameViewportClient()!=this);
	Super::BeginDestroy();
}

void UStalkerGameViewportClient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Device->fTimeContinual += DeltaTime;
	Device->dwTimeContinual = static_cast<u32>(Device->fTimeContinual * 1000); 
	if (!Device->Paused())
	{
		Device->fTimeDelta = DeltaTime;
	}
	else
	{
		Device->fTimeDelta = 0;
	}
	
	Device->dwTimeDelta = static_cast<u32>(Device->fTimeDelta * 1000);

	Device->fTimeGlobal += 	Device->fTimeDelta;
	Device->dwTimeGlobal = static_cast<u32>(Device->fTimeGlobal * 1000);

	if (g_loading_events->size())
	{
		if (g_loading_events->front()())
			g_loading_events->pop_front();
		return;
	}
	else
	{
		SCOPE_CYCLE_COUNTER(STAT_XRayEngineFrame);
		Device->mFullTransform.mul(Device->mProject, Device->mView);
		Device->dwFrame++;
		g_Engine->OnFrame();
		{
			SCOPE_CYCLE_COUNTER(STAT_XRayEngineMTFrame);
			for (u32 pit = 0; pit < Device->seqParallel.size(); pit++)
				Device->seqParallel[pit]();
			Device->seqParallel.clear();
			Device->seqFrameMT.Process(rp_Frame);
		}
		GStalkerEngineManager->GetResourcesManager()->GetSoundManager()->Tick(DeltaTime);
	}
}
