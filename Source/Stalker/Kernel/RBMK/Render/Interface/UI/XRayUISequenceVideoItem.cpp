#include "XRayUISequenceVideoItem.h"

XRayUISequenceVideoItem::XRayUISequenceVideoItem()
{
}

void XRayUISequenceVideoItem::Copy(IUISequenceVideoItem & _in)
{
}

bool XRayUISequenceVideoItem::HasTexture()
{
	return true;
}
void XRayUISequenceVideoItem::CaptureTexture()
{
}

void XRayUISequenceVideoItem::ResetTexture()
{
}

BOOL XRayUISequenceVideoItem::video_IsPlaying()
{
	return FALSE;
}

void XRayUISequenceVideoItem::video_Sync(u32 _time)
{
}

void XRayUISequenceVideoItem::video_Play(BOOL looped, u32 _time)
{
}

void XRayUISequenceVideoItem::video_Stop()
{
}
