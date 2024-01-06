#pragma once
THIRD_PARTY_INCLUDES_START
#include "XrEngine/Render/FontRender.h"
THIRD_PARTY_INCLUDES_END
#include "XRayUIRender.h"
class XRayFontRender:public IFontRender
{
public:
	XRayFontRender();
	~XRayFontRender();
	void OnRender(CGameFont &owner) override;
	void Initialize(LPCSTR font, float size) override;
	float GetTextSize(LPCSTR s) override;
	float GetTextSize(const wide_char* wsStr) override;
	float GetTextSize(const char cChar) override;
	float GetTextSize(const TCHAR* Text) ;

private:
	UFont* Font;
	float  FontSize;
	TArray<WCHAR>  TextChace;
};
