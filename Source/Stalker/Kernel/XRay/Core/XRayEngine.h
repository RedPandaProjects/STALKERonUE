#pragma once
THIRD_PARTY_INCLUDES_START
#include "XrEngine/XRayEngineInterface.h"
THIRD_PARTY_INCLUDES_END
class XRayEngine :public XRayEngineInterface
{
public:
			XRayEngine			();
	void	Initialize			() override;
	void	Destroy				() override;

	FName	CurrentLevelName;
protected:
	void	Level_Scan		() override;
	int		Level_ID		(LPCSTR name, LPCSTR ver, bool bSet) override;
	void	Level_Set		(u32 ID) override;
	void	LoadAllArchives	() override;

};