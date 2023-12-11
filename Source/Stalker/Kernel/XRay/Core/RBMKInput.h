#pragma once
THIRD_PARTY_INCLUDES_START
#include "XrEngine/xr_input.h"
THIRD_PARTY_INCLUDES_END
class FRBMKInput:public XRayInputInterface
{

public:
	FRBMKInput();

	~FRBMKInput() override;

	bool GetDikName(int dik, LPSTR dest, int dest_sz) override;


	void SetAllAcquire(BOOL bAcquire = TRUE) override;


	void iCapture(IInputReceiver* pc) override;


	void iRelease(IInputReceiver* pc) override;


	BOOL iGetAsyncKeyState(int dik) override;


	BOOL iGetAsyncBtnState(int btn) override;


	void iGetLastMouseDelta(Ivector2& p) override;


	void OnFrame(void) override;


	void OnAppActivate(void) override;


	void OnAppDeactivate(void) override;


	IInputReceiver* CurrentIR() override;
	void KeyEvent(const FInputKeyEventArgs& InEvent);
	void MouseEvent(float x,float y);
	void MouseWheelEvent(float x);
	void ClearStates();
private:
	
	void	InitKeyMap();
	enum EKeyState:uint8
	{
		KS_None,
		KS_Down,
		KS_Hold,
		KS_Up
	};
	enum EMouseState :uint8
	{
		MS_None,
		MS_Move,
	};
	uint8					KeysState[COUNT_KB_BUTTONS];
	uint8					MouseState[COUNT_MOUSE_BUTTONS];
	TArray<IInputReceiver*>	Stack;
	TMap<FName, uint32>		FKeyToVK;
	TMap<uint32, FName>		VKToFKey;
	FVector2f				MouseDelta;
	float					MouseWheelDelta;
};