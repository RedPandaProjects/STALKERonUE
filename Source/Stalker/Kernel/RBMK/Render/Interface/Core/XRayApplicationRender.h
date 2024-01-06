#pragma once
#include "XrEngine/Render/ApplicationRender.h"
class XRayApplicationRender :public  IApplicationRender
{
public:
	XRayApplicationRender();
	virtual void Copy(IApplicationRender &_in);
	virtual void LoadTitleInt(LPCSTR str);
	virtual void LoadBegin();
	virtual void destroy_loading_shaders();
	virtual void setLevelLogo(LPCSTR pszLogoName);
	virtual void load_draw_internal(IRBMKEngine &owner);
	//	?????
	virtual void KillHW();
};