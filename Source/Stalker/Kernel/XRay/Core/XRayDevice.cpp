#include "XRayDevice.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/IGame_Persistent.h"
THIRD_PARTY_INCLUDES_END

XRayDevice::XRayDevice()
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

XRayDevice::~XRayDevice()
{

}

bool XRayDevice::IsEditorMode()
{
	return GIsEditor;
}

bool XRayDevice::Paused() const
{
	return IsTimerPaused;
}

void XRayDevice::Reset(bool precache /*= false*/)
{
}

void XRayDevice::Pause(BOOL bOn, BOOL bTimer, BOOL bSound, LPCSTR reason)
{	
	if(bOn)
	{
		if( bTimer && (!g_pGamePersistent || g_pGamePersistent->CanBePaused()) )
		{
			IsTimerPaused = true;
		}
	
		if (bSound && ::Sound)
		{
			SndEmitters =					::Sound->pause_emitters(true);
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
			if(SndEmitters>0) 
			{
				SndEmitters =			::Sound->pause_emitters(false);
			}
		}
	}
}

void XRayDevice::PreCache(u32 amount, bool b_draw_loadscreen, bool b_wait_user_input)
{
}

void XRayDevice::Clear()
{
}

bool XRayDevice::Begin()
{	
	return true;
}

void XRayDevice::End()
{
}

CStatsPhysics* _BCL XRayDevice::StatPhysics()
{
	return Statistic;
}

void _BCL XRayDevice::AddSeqFrame(pureFrame* f, bool mt)
{
	seqFrameMT.Add(f, REG_PRIORITY_LOW);
}

void _BCL XRayDevice::RemoveSeqFrame(pureFrame* f)
{
	seqFrameMT.Remove(f);
}

