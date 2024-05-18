#pragma once
#include "XrEngine/XrDeviceInterface.h"
class FRBMKDevice :public XrDeviceInterface
{
public:
						FRBMKDevice		();
						~FRBMKDevice	() override;
#if !(UE_BUILD_SHIPPING||UE_BUILD_TEST)
	bool				IsEditorMode	() override;
#endif
	bool				Paused			() const override;
	void				Reset			(bool precache = false) override;
	void				Pause			(BOOL bOn, BOOL bTimer, BOOL bSound, LPCSTR reason) override;
	void				PreCache		(u32 amount, bool b_draw_loadscreen, bool b_wait_user_input) override;
	void				Clear			() override;
	bool				Begin			() override;
	void				End				() override;
	CStatsPhysics*		StatPhysics		() override;
	void				AddSeqFrame		(pureFrame* f, bool mt) override;
	void				RemoveSeqFrame	(pureFrame* f) override;
private:
	bool				IsTimerPaused;
	int32				SoundEmittersPaused = -1;

};