#include "RBMKInput.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/IInputReceiver.h"
THIRD_PARTY_INCLUDES_END

FRBMKInput::FRBMKInput()
{
	MouseWheelDelta = 0;
	MouseDelta.Set(0, 0);
	for (int32 i = 0; i < COUNT_KB_BUTTONS; i++)
	{
		KeysState[i] = KS_None;
	}
	for (int32 i = 0; i < COUNT_MOUSE_BUTTONS; i++)
	{
		MouseState[i] = KS_None;
	}
	InitKeyMap();
}

FRBMKInput::~FRBMKInput()
{

}

bool FRBMKInput::GetDikName(int dik, LPSTR dest, int dest_sz)
{
	auto Iterator = VKToFKey.Find(dik);
	if (Iterator)
	{
		FString Name = Iterator->ToString();
		if (Name.Len() > dest_sz)
		{
			return false;
		}
		FCStringAnsi::Strcpy(dest, dest_sz,TCHAR_TO_ANSI(*Name));
		return true;
	}
	return false;
}

void FRBMKInput::SetAllAcquire(BOOL bAcquire /*= TRUE*/)
{
	
}

void FRBMKInput::iCapture(IInputReceiver* pc)
{
	if (Stack.Num())
	{
		Stack.Last()->IR_OnDeactivate();
	}
	Stack.Add(pc);
	Stack.Last()->IR_OnActivate();
}

void FRBMKInput::iRelease(IInputReceiver* pc)
{
	if (pc == Stack.Last())
	{
		pc->IR_OnDeactivate();
		Stack.Remove(pc);
		if (Stack.Num())
		{
			Stack.Last()->IR_OnActivate();
		}
	}
	else
	{
		Stack.Remove(pc);
	}
}

BOOL FRBMKInput::iGetAsyncKeyState(int dik)
{
	return KeysState[dik]==KS_Down || KeysState[dik] == KS_Hold;
}

BOOL FRBMKInput::iGetAsyncBtnState(int dik)
{
	return MouseState[dik] == KS_Down || MouseState[dik] == KS_Hold;
}

void FRBMKInput::iGetLastMouseDelta(Ivector2& p)
{
	 p.set(MouseDelta.X, MouseDelta.Y);
}

void FRBMKInput::OnFrame(void)
{
	if (CurrentIR()&&MouseDelta.X+MouseDelta.Y!=0.f)
	{
		CurrentIR()->IR_OnMouseMove(MouseDelta.X,MouseDelta.Y);
	}
	if (CurrentIR()&&MouseWheelDelta!=0.f)
	{
		CurrentIR()->IR_OnMouseWheel(static_cast<int>(FMath::Sign( MouseWheelDelta)));
	}
	for (int32 i = 0; i < COUNT_MOUSE_BUTTONS; i++)
	{
		if (CurrentIR())
		{
			if (MouseState[i] == KS_Down)
			{
				CurrentIR()->IR_OnMousePress(i);
			}
			if (MouseState[i] == KS_Hold)
			{
				CurrentIR()->IR_OnMouseHold(i);
			}

			if (MouseState[i] == KS_Up)
			{
				CurrentIR()->IR_OnMouseRelease(i);
			}
		}
		if (MouseState[i] == KS_Down)
		{
			MouseState[i] = KS_Hold;
		}
		if (MouseState[i] == KS_Up)
		{
			MouseState[i] = KS_None;
		}
	}

	for (int32 i = 0; i < COUNT_KB_BUTTONS; i++)
	{
		if (CurrentIR())
		{
			if (KeysState[i] == KS_Down)
			{
				CurrentIR()->IR_OnKeyboardPress(i);
			}
			if (KeysState[i] == KS_Hold)
			{
				CurrentIR()->IR_OnKeyboardHold(i);
			}

			if (KeysState[i] == KS_Up)
			{
				CurrentIR()->IR_OnKeyboardRelease(i);
			}
		}
		if (KeysState[i] == KS_Down)
		{
			KeysState[i] = KS_Hold;
		}
		if (KeysState[i] == KS_Up)
		{
			KeysState[i] = KS_None;
		}
	}
	MouseWheelDelta = 0;
	MouseDelta.Set(0, 0);
}

void FRBMKInput::OnAppActivate(void)
{
	ClearStates();
}

void FRBMKInput::OnAppDeactivate(void)
{
	ClearStates();
}

IInputReceiver* FRBMKInput::CurrentIR()
{
	if(Stack.Num())
	return Stack.Last();
	return nullptr;
}

void FRBMKInput::KeyEvent(const FInputKeyEventArgs& InEvent)
{
	if (InEvent.Key == EKeys::LeftMouseButton && (InEvent.Event == IE_Pressed || InEvent.Event == IE_Released))
	{
		MouseState[0] = InEvent.Event == IE_Pressed ? KS_Down : KS_Up;
	}
	else if (InEvent.Key == EKeys::MiddleMouseButton && (InEvent.Event == IE_Pressed || InEvent.Event == IE_Released))
	{
		MouseState[2] = InEvent.Event == IE_Pressed ? KS_Down : KS_Up;
	}
	else if (InEvent.Key == EKeys::RightMouseButton && (InEvent.Event == IE_Pressed || InEvent.Event == IE_Released))
	{
		MouseState[1] = InEvent.Event == IE_Pressed ? KS_Down : KS_Up;
	}
	else if (InEvent.Key == EKeys::ThumbMouseButton && (InEvent.Event == IE_Pressed || InEvent.Event == IE_Released))
	{
		MouseState[3] = InEvent.Event == IE_Pressed ? KS_Down : KS_Up;
	}
	else if (InEvent.Key == EKeys::ThumbMouseButton2 && (InEvent.Event == IE_Pressed || InEvent.Event == IE_Released))
	{
		MouseState[4] = InEvent.Event == IE_Pressed ? KS_Down : KS_Up;
	}
	else if (FKeyToVK.Contains(InEvent.Key.GetFName())&&( InEvent.Event == IE_Pressed || InEvent.Event == IE_Released))
	{
		uint32 ID = FKeyToVK[InEvent.Key.GetFName()];
		KeysState[ID] = InEvent.Event == IE_Pressed? KS_Down:KS_Up;
	}
	
}

void FRBMKInput::MouseEvent(float x, float y)
{
	MouseDelta+=FVector2f(x,-y);
}

void FRBMKInput::MouseWheelEvent(float x)
{
	MouseWheelDelta+=x;
}

void FRBMKInput::ClearStates()
{
	for (int32 i = 0; i < COUNT_KB_BUTTONS; i++)
	{
		KeysState[i] = KS_None;
	}
	for (int32 i = 0; i < COUNT_MOUSE_BUTTONS; i++)
	{
		MouseState[i] = KS_None;
	}
	MouseDelta.Set(0, 0);
	MouseWheelDelta = 0;
}

void FRBMKInput::InitKeyMap()
{
	uint32 KeyCodes[COUNT_KB_BUTTONS];
	FString KeyNames[COUNT_KB_BUTTONS];
	int32 NumMappings = 0;
#define ADDKEYMAP(KeyCode, KeyName)		if (NumMappings<COUNT_KB_BUTTONS) { KeyCodes[NumMappings]=KeyCode; KeyNames[NumMappings]=KeyName; ++NumMappings; };

	ADDKEYMAP('0', TEXT("Zero"));
	ADDKEYMAP('1', TEXT("One"));
	ADDKEYMAP('2', TEXT("Two"));
	ADDKEYMAP('3', TEXT("Three"));
	ADDKEYMAP('4', TEXT("Four"));
	ADDKEYMAP('5', TEXT("Five"));
	ADDKEYMAP('6', TEXT("Six"));
	ADDKEYMAP('7', TEXT("Seven"));
	ADDKEYMAP('8', TEXT("Eight"));
	ADDKEYMAP('9', TEXT("Nine"));

	// we map both upper and lower
	if (true)
	{
		ADDKEYMAP('A', TEXT("A"));
		ADDKEYMAP('B', TEXT("B"));
		ADDKEYMAP('C', TEXT("C"));
		ADDKEYMAP('D', TEXT("D"));
		ADDKEYMAP('E', TEXT("E"));
		ADDKEYMAP('F', TEXT("F"));
		ADDKEYMAP('G', TEXT("G"));
		ADDKEYMAP('H', TEXT("H"));
		ADDKEYMAP('I', TEXT("I"));
		ADDKEYMAP('J', TEXT("J"));
		ADDKEYMAP('K', TEXT("K"));
		ADDKEYMAP('L', TEXT("L"));
		ADDKEYMAP('M', TEXT("M"));
		ADDKEYMAP('N', TEXT("N"));
		ADDKEYMAP('O', TEXT("O"));
		ADDKEYMAP('P', TEXT("P"));
		ADDKEYMAP('Q', TEXT("Q"));
		ADDKEYMAP('R', TEXT("R"));
		ADDKEYMAP('S', TEXT("S"));
		ADDKEYMAP('T', TEXT("T"));
		ADDKEYMAP('U', TEXT("U"));
		ADDKEYMAP('V', TEXT("V"));
		ADDKEYMAP('W', TEXT("W"));
		ADDKEYMAP('X', TEXT("X"));
		ADDKEYMAP('Y', TEXT("Y"));
		ADDKEYMAP('Z', TEXT("Z"));
	}

	if (false)
	{
		ADDKEYMAP('a', TEXT("A"));
		ADDKEYMAP('b', TEXT("B"));
		ADDKEYMAP('c', TEXT("C"));
		ADDKEYMAP('d', TEXT("D"));
		ADDKEYMAP('e', TEXT("E"));
		ADDKEYMAP('f', TEXT("F"));
		ADDKEYMAP('g', TEXT("G"));
		ADDKEYMAP('h', TEXT("H"));
		ADDKEYMAP('i', TEXT("I"));
		ADDKEYMAP('j', TEXT("J"));
		ADDKEYMAP('k', TEXT("K"));
		ADDKEYMAP('l', TEXT("L"));
		ADDKEYMAP('m', TEXT("M"));
		ADDKEYMAP('n', TEXT("N"));
		ADDKEYMAP('o', TEXT("O"));
		ADDKEYMAP('p', TEXT("P"));
		ADDKEYMAP('q', TEXT("Q"));
		ADDKEYMAP('r', TEXT("R"));
		ADDKEYMAP('s', TEXT("S"));
		ADDKEYMAP('t', TEXT("T"));
		ADDKEYMAP('u', TEXT("U"));
		ADDKEYMAP('v', TEXT("V"));
		ADDKEYMAP('w', TEXT("W"));
		ADDKEYMAP('x', TEXT("X"));
		ADDKEYMAP('y', TEXT("Y"));
		ADDKEYMAP('z', TEXT("Z"));
	}

	ADDKEYMAP(';', TEXT("Semicolon"));
	ADDKEYMAP('=', TEXT("Equals"));
	ADDKEYMAP(',', TEXT("Comma"));
	ADDKEYMAP('-', TEXT("Hyphen"));
	ADDKEYMAP('.', TEXT("Period"));
	ADDKEYMAP('/', TEXT("Slash"));
	ADDKEYMAP('`', TEXT("Tilde"));
	ADDKEYMAP('[', TEXT("LeftBracket"));
	ADDKEYMAP('\\', TEXT("Backslash"));
	ADDKEYMAP(']', TEXT("RightBracket"));
	ADDKEYMAP('\'', TEXT("Apostrophe"));
	ADDKEYMAP(' ', TEXT("SpaceBar"));

	// AZERTY Keys
	ADDKEYMAP('&', TEXT("Ampersand"));
	ADDKEYMAP('*', TEXT("Asterix"));
	ADDKEYMAP('^', TEXT("Caret"));
	ADDKEYMAP(':', TEXT("Colon"));
	ADDKEYMAP('$', TEXT("Dollar"));
	ADDKEYMAP('!', TEXT("Exclamation"));
	ADDKEYMAP('(', TEXT("LeftParantheses"));
	ADDKEYMAP(')', TEXT("RightParantheses"));
	ADDKEYMAP('"', TEXT("Quote"));
	ADDKEYMAP('_', TEXT("Underscore"));
	ADDKEYMAP(224, TEXT("A_AccentGrave"));
	ADDKEYMAP(231, TEXT("C_Cedille"));
	ADDKEYMAP(233, TEXT("E_AccentAigu"));
	ADDKEYMAP(232, TEXT("E_AccentGrave"));
	ADDKEYMAP(167, TEXT("Section"));

	ADDKEYMAP(VK_LBUTTON, TEXT("LeftMouseButton"));
	ADDKEYMAP(VK_RBUTTON, TEXT("RightMouseButton"));
	ADDKEYMAP(VK_MBUTTON, TEXT("MiddleMouseButton"));

	ADDKEYMAP(VK_XBUTTON1, TEXT("ThumbMouseButton"));
	ADDKEYMAP(VK_XBUTTON2, TEXT("ThumbMouseButton2"));

	ADDKEYMAP(VK_BACK, TEXT("BackSpace"));
	ADDKEYMAP(VK_TAB, TEXT("Tab"));
	ADDKEYMAP(VK_RETURN, TEXT("Enter"));
	ADDKEYMAP(VK_PAUSE, TEXT("Pause"));

	ADDKEYMAP(VK_CAPITAL, TEXT("CapsLock"));
	ADDKEYMAP(VK_ESCAPE, TEXT("Escape"));
	ADDKEYMAP(VK_SPACE, TEXT("SpaceBar"));
	ADDKEYMAP(VK_PRIOR, TEXT("PageUp"));
	ADDKEYMAP(VK_NEXT, TEXT("PageDown"));
	ADDKEYMAP(VK_END, TEXT("End"));
	ADDKEYMAP(VK_HOME, TEXT("Home"));

	ADDKEYMAP(VK_LEFT, TEXT("Left"));
	ADDKEYMAP(VK_UP, TEXT("Up"));
	ADDKEYMAP(VK_RIGHT, TEXT("Right"));
	ADDKEYMAP(VK_DOWN, TEXT("Down"));

	ADDKEYMAP(VK_INSERT, TEXT("Insert"));
	ADDKEYMAP(VK_DELETE, TEXT("Delete"));

	ADDKEYMAP(VK_NUMPAD0, TEXT("NumPadZero"));
	ADDKEYMAP(VK_NUMPAD1, TEXT("NumPadOne"));
	ADDKEYMAP(VK_NUMPAD2, TEXT("NumPadTwo"));
	ADDKEYMAP(VK_NUMPAD3, TEXT("NumPadThree"));
	ADDKEYMAP(VK_NUMPAD4, TEXT("NumPadFour"));
	ADDKEYMAP(VK_NUMPAD5, TEXT("NumPadFive"));
	ADDKEYMAP(VK_NUMPAD6, TEXT("NumPadSix"));
	ADDKEYMAP(VK_NUMPAD7, TEXT("NumPadSeven"));
	ADDKEYMAP(VK_NUMPAD8, TEXT("NumPadEight"));
	ADDKEYMAP(VK_NUMPAD9, TEXT("NumPadNine"));

	ADDKEYMAP(VK_MULTIPLY, TEXT("Multiply"));
	ADDKEYMAP(VK_ADD, TEXT("Add"));
	ADDKEYMAP(VK_SUBTRACT, TEXT("Subtract"));
	ADDKEYMAP(VK_DECIMAL, TEXT("Decimal"));
	ADDKEYMAP(VK_DIVIDE, TEXT("Divide"));

	ADDKEYMAP(VK_F1, TEXT("F1"));
	ADDKEYMAP(VK_F2, TEXT("F2"));
	ADDKEYMAP(VK_F3, TEXT("F3"));
	ADDKEYMAP(VK_F4, TEXT("F4"));
	ADDKEYMAP(VK_F5, TEXT("F5"));
	ADDKEYMAP(VK_F6, TEXT("F6"));
	ADDKEYMAP(VK_F7, TEXT("F7"));
	ADDKEYMAP(VK_F8, TEXT("F8"));
	ADDKEYMAP(VK_F9, TEXT("F9"));
	ADDKEYMAP(VK_F10, TEXT("F10"));
	ADDKEYMAP(VK_F11, TEXT("F11"));
	ADDKEYMAP(VK_F12, TEXT("F12"));

	ADDKEYMAP(VK_NUMLOCK, TEXT("NumLock"));

	ADDKEYMAP(VK_SCROLL, TEXT("ScrollLock"));

	ADDKEYMAP(VK_LSHIFT, TEXT("LeftShift"));
	ADDKEYMAP(VK_RSHIFT, TEXT("RightShift"));
	ADDKEYMAP(VK_LCONTROL, TEXT("LeftControl"));
	ADDKEYMAP(VK_RCONTROL, TEXT("RightControl"));
	ADDKEYMAP(VK_LMENU, TEXT("LeftAlt"));
	ADDKEYMAP(VK_RMENU, TEXT("RightAlt"));
	ADDKEYMAP(VK_LWIN, TEXT("LeftCommand"));
	ADDKEYMAP(VK_RWIN, TEXT("RightCommand"));
	const int32 CharMappings = NumMappings;
	TMap<uint32, uint32> ScanToVKMap;
#define MAP_OEM_VK_TO_SCAN(KeyCode) { const uint32 CharCode = MapVirtualKey(KeyCode,2); if (CharCode != 0) { ScanToVKMap.Add(CharCode,KeyCode); } }
	MAP_OEM_VK_TO_SCAN(VK_OEM_1);
	MAP_OEM_VK_TO_SCAN(VK_OEM_2);
	MAP_OEM_VK_TO_SCAN(VK_OEM_3);
	MAP_OEM_VK_TO_SCAN(VK_OEM_4);
	MAP_OEM_VK_TO_SCAN(VK_OEM_5);
	MAP_OEM_VK_TO_SCAN(VK_OEM_6);
	MAP_OEM_VK_TO_SCAN(VK_OEM_7);
	MAP_OEM_VK_TO_SCAN(VK_OEM_8);
	MAP_OEM_VK_TO_SCAN(VK_OEM_PLUS);
	MAP_OEM_VK_TO_SCAN(VK_OEM_COMMA);
	MAP_OEM_VK_TO_SCAN(VK_OEM_MINUS);
	MAP_OEM_VK_TO_SCAN(VK_OEM_PERIOD);
	MAP_OEM_VK_TO_SCAN(VK_OEM_102);
#undef  MAP_OEM_VK_TO_SCAN

	for (int32 MappingIndex = 0; MappingIndex < CharMappings; ++MappingIndex)
	{
		ScanToVKMap.Remove(KeyCodes[MappingIndex]);
	}

	for (auto It(ScanToVKMap.CreateConstIterator()); It; ++It)
	{
		ADDKEYMAP(It.Value(), FString::Chr(It.Key()));
	}


#undef  ADDKEYMAP

	for (int32 MappingIndex = 0; MappingIndex < NumMappings; ++MappingIndex)
	{
		VKToFKey.Add(MapVirtualKey(KeyCodes[MappingIndex], MAPVK_VK_TO_VSC), FName(KeyNames[MappingIndex]));
		FKeyToVK.Add(FName(KeyNames[MappingIndex]), MapVirtualKey(KeyCodes[MappingIndex], MAPVK_VK_TO_VSC));
	}
}
