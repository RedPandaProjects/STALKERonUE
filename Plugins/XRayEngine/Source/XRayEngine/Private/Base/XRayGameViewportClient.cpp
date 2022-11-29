// Fill out your copyright notice in the Description page of Project Settings.


#include "Base/XRayGameViewportClient.h"
#include "Base/XRayEngineManager.h"
#include "Core/XRayInput.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/XrDeviceInterface.h"
#include "XrEngine/XRayEngineInterface.h"
THIRD_PARTY_INCLUDES_END
#include "Render/Resources/SkeletonMesh/XRaySkeletonMeshManager.h"

void UXRayGameViewportClient::Activated(FViewport* InViewport, const FWindowActivateEvent& InActivateEvent)
{
	Super::Activated(InViewport, InActivateEvent);
	if (!IsActive)
	{
		if (GXRayEngineManager->GetGameViewportClient() == this)
		{
			Device->seqAppActivate.Process(rp_AppActivate);
		}
		
		IsActive = true;
	}
}

void UXRayGameViewportClient::Deactivated(FViewport* InViewport, const FWindowActivateEvent& InActivateEvent)
{
	Super::Deactivated(InViewport, InActivateEvent);
	if (IsActive)
	{
		Device->seqAppDeactivate.Process(rp_AppDeactivate);
		IsActive = false;
	}
}

bool UXRayGameViewportClient::InputKey(const FInputKeyEventArgs& InKeyEvent)
{
	bool Result  = Super::InputKey(InKeyEvent);
	if (IgnoreInput())
	{
		return false;
	}
	if (GXRayEngineManager->GetGameViewportClient()==this)
	{
		GXRayEngineManager->GetInput()->KeyEvent(InKeyEvent);
		return true;
	}
	return Result;
}


bool UXRayGameViewportClient::InputAxis(FViewport* InViewport, FInputDeviceId InputDevice, FKey Key, float Delta, float DeltaTime, int32 NumSamples /*= 1*/, bool bGamepad /*= false*/)
{
	if (!Super::InputAxis(InViewport, InputDevice, Key, Delta, DeltaTime, NumSamples, bGamepad) || (InViewport->IsSlateViewport() && InViewport->IsPlayInEditorViewport()))
	{
		if (IgnoreInput())
		{
			return false;
		}
		if (GXRayEngineManager->GetGameViewportClient() == this)
		{
			if (Key.GetFName() == EKeys::MouseX)
			{
				GXRayEngineManager->GetInput()->MouseEvent(Delta, 0);
			}
			if (Key.GetFName() == EKeys::MouseY)
			{
				GXRayEngineManager->GetInput()->MouseEvent(0, Delta);
			}
			return true;
		}
		return false;
	}
	return true;
}
void UXRayGameViewportClient::LostFocus(FViewport* InViewport)
{
	Super::LostFocus(InViewport);
	if (GIsEditor && GXRayEngineManager->GetGameViewportClient() == this)
	{
		if (!IsActive)
		{
			Device->seqAppDeactivate.Process(rp_AppDeactivate);
			IsActive = true;
		}
	}
}

void UXRayGameViewportClient::ReceivedFocus(FViewport* InViewport)
{
	Super::ReceivedFocus(InViewport);
	if (GIsEditor)
	{
		if (IsActive)
		{
			Device->seqAppDeactivate.Process(rp_AppDeactivate);
			IsActive = false;
		}
	}
}

void UXRayGameViewportClient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (g_loading_events->size())
	{
		if (g_loading_events->front()())
			g_loading_events->pop_front();
		return;
	}
	else
	{
		Device->fTimeDelta = DeltaTime;
		Device->fTimeGlobal = GXRayEngineManager->GetGameWorld()->TimeSeconds;
		Device->dwTimeDelta = static_cast<u32>(DeltaTime * 1000);
		Device->dwTimeGlobal = static_cast<u32>(GXRayEngineManager->GetGameWorld()->TimeSeconds * 1000);
		Device->dwTimeContinual = static_cast<u32>(GXRayEngineManager->GetGameWorld()->UnpausedTimeSeconds * 1000);
		g_Engine->OnFrame();
		Device->dwFrame++;
		GXRaySkeletonMeshManager->Flush();
		{
			for (u32 pit = 0; pit < Device->seqParallel.size(); pit++)
				Device->seqParallel[pit]();
			Device->seqParallel.clear_not_free();
			Device->seqFrameMT.Process(rp_Frame);
		}
	}
}
