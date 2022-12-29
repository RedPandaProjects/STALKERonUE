#include "XRayWallMarkArray.h"

FACTORY_PTR_INSTANCIATE(UIShader)
XRayWallMarkArray::XRayWallMarkArray()
{
}

void XRayWallMarkArray::Copy(IWallMarkArray & _in)
{
}

void XRayWallMarkArray::AppendMark(LPCSTR s_textures)
{
}

void XRayWallMarkArray::clear()
{
}

bool XRayWallMarkArray::empty()
{
	return false;
}

wm_shader XRayWallMarkArray::GenerateWallmark()
{
	return wm_shader();
}
