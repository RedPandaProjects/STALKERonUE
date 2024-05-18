#include "RBMKDevice.h"

#include "Kernel/StalkerEngineManager.h"
#include "Resources/StalkerResourcesManager.h"
#include "Resources/Sound/StalkerSoundManager.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/IGame_Persistent.h"
THIRD_PARTY_INCLUDES_END

FRBMKDevice::FRBMKDevice()
{
	psDeviceFlags.assign(0);
	// default initialization
	dwMaximized = 0;
	dwWidth = dwHeight = 256;
	fWidth_2 = fHeight_2 = 128;
	mProject.identity();
	mFullTransform.identity();
	mView.identity();

	vCameraPosition.set(0,0,0);
	vCameraTop.set(1, 0, 0);
	vCameraDirection.set(0, 1, 0);
	vCameraRight.set(0, 0, 1);
	b_is_Ready = FALSE;
	b_is_Active = FALSE;

	// Engine flow-control
	fTimeDelta = 0;
	fTimeGlobal = 0;
	dwTimeDelta = 0;
	dwTimeGlobal = 0;
	fTimeContinual = 0;
	fASPECT = 1.f;
	fFOV = 60.f;
	dwPrecacheFrame = 0;
	IsTimerPaused = false;
}

FRBMKDevice::~FRBMKDevice()
{

}
#if !(UE_BUILD_SHIPPING||UE_BUILD_TEST)
bool FRBMKDevice::IsEditorMode()
{
	return GIsEditor;
}
#endif

bool FRBMKDevice::Paused() const
{
	return IsTimerPaused;
}

void FRBMKDevice::Reset(bool precache /*= false*/)
{
}

void FRBMKDevice::Pause(BOOL bOn, BOOL bTimer, BOOL bSound, LPCSTR reason)
{	
	if(bOn)
	{
		if( bTimer && (!g_pGamePersistent || g_pGamePersistent->CanBePaused()) )
		{
			IsTimerPaused = true;
		}
		if(bSound)
		{
			if(UStalkerSoundManager* StalkerSoundManager = GStalkerEngineManager->GetResourcesManager()->GetSoundManager())
			{
				SoundEmittersPaused = StalkerSoundManager->PauseEmitters(true);
			}
		}
	}
	else
	{
		if( bTimer && IsTimerPaused )
		{
			fTimeDelta						= EPS_S + EPS_S;
			IsTimerPaused = false;
		}
		if(bSound)
		{
			if(UStalkerSoundManager* StalkerSoundManager = GStalkerEngineManager->GetResourcesManager()->GetSoundManager())
			{
				if(SoundEmittersPaused > 0)
				{
					SoundEmittersPaused = StalkerSoundManager->PauseEmitters(false);
				}
			}
		}
	
	}
}

void FRBMKDevice::PreCache(u32 amount, bool b_draw_loadscreen, bool b_wait_user_input)
{
}

void FRBMKDevice::Clear()
{
}

bool FRBMKDevice::Begin()
{	
	return true;
}

void FRBMKDevice::End()
{
}

CStatsPhysics* _BCL FRBMKDevice::StatPhysics()
{
	return Statistic;
}

void _BCL FRBMKDevice::AddSeqFrame(pureFrame* f, bool mt)
{
	seqFrameMT.Add(f, REG_PRIORITY_LOW);
}

void _BCL FRBMKDevice::RemoveSeqFrame(pureFrame* f)
{
	seqFrameMT.Remove(f);
}

