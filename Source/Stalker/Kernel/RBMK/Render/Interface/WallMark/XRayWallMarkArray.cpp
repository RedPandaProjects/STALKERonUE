#include "XRayWallMarkArray.h"
#if !(UE_BUILD_SHIPPING||UE_BUILD_TEST)
FACTORY_PTR_INSTANCIATE(UIShader)
#endif
XRayWallMarkArray::XRayWallMarkArray()
{
}

void XRayWallMarkArray::Copy(IWallMarkArray & _in)
{
	Textures = static_cast<XRayWallMarkArray*>(&_in)->Textures;
}

void XRayWallMarkArray::AppendMark(const char* Texture)
{
	Textures.Add(Texture);
}

void XRayWallMarkArray::Clear()
{
	Textures.Empty();
}

bool XRayWallMarkArray::IsEmpty()
{
	return Textures.IsEmpty();
}

shared_str XRayWallMarkArray::GenerateWallmark()
{
	check(!Textures.IsEmpty());
	return Textures[FMath::RandRange(0,Textures.Num()-1)];
}
