#pragma once
#include "XrEngine/Render/WallMarkArray.h"
class XRayWallMarkArray:public IWallMarkArray 
{
public:
				XRayWallMarkArray	();
	void		Copy				(IWallMarkArray &_in) ;
	void		AppendMark			(const char* Texture);
	void		Clear				();
	bool		IsEmpty				() ;
	shared_str	GenerateWallmark	();
private:
	TArray<shared_str>	Textures;
};
