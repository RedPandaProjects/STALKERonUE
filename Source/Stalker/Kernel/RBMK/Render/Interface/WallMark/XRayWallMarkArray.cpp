#include "XRayWallMarkArray.h"

FACTORY_PTR_INSTANCIATE(UIShader)
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
