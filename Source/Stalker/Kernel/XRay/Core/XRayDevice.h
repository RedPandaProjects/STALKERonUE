#pragma once
#include "XrEngine/XrDeviceInterface.h"
class XRayDevice :public XrDeviceInterface
{
public:
	XRayDevice();



	bool IsEditorMode() override;


	bool Paused() const override;


	void Reset(bool precache = false) override;


	void Pause(BOOL bOn, BOOL bTimer, BOOL bSound, LPCSTR reason) override;


	void PreCache(u32 amount, bool b_draw_loadscreen, bool b_wait_user_input) override;


	void Clear() override;


	bool Begin() override;


	void End() override;


	CStatsPhysics* _BCL StatPhysics() override;


	void _BCL AddSeqFrame(pureFrame* f, bool mt) override;


	void _BCL RemoveSeqFrame(pureFrame* f) override;

};