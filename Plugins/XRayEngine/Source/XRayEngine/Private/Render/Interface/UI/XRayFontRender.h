#pragma once
#include "XrRender/Public/FontRender.h"
class XRayFontRender:public IFontRender
{
public:
	XRayFontRender();
	~XRayFontRender();
	virtual void Initialize(LPCSTR cShader, LPCSTR cTexture) ;
	virtual void OnRender(CGameFont &owner) ;
	void Flush();
private:
};
