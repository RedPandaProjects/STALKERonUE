#pragma once
THIRD_PARTY_INCLUDES_START
#include "XrEngine/XRayEngineInterface.h"
THIRD_PARTY_INCLUDES_END

class XRayEngine :public XRayEngineInterface
{
public:
									XRayEngine					();
	void							Initialize					() override;
	void							Destroy						() override;


	class XRayUnrealProxyInterface* CreateUnrealProxy			() override;
	void							Destroy						(class XRayUnrealProxyInterface*) override;

	class ILevelGraph*				GetLevelGraph				(const char*Name) override;
	class IGameGraph*				GetGameGraph				() override;
	IReader							GetGameSpawn				() override;


	bool							LoadWorld					(const char* Name) override;
	void							LoadCFormFormCurrentWorld	(class CObjectSpace& ObjectSpace, CDB::build_callback build_callback);
	EXRayWorldStatus				GetWorldStatus				() override;


	class XRayUnrealProxyInterface* GetUnrealPlayerCharacter	() override;

};