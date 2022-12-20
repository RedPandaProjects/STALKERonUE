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

	class XRayUnrealProxyInterface* CreateUnrealProxy(class CObject*) override;
	void Destroy(class XRayUnrealProxyInterface*) override;

protected:
	void	Level_Scan		() override;
	int		Level_ID		(LPCSTR name, LPCSTR ver, bool bSet) override;
	void	Level_Set		(u32 ID) override;
	void	LoadAllArchives	() override;

};