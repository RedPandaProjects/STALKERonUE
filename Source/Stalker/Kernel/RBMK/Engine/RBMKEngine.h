#pragma once
THIRD_PARTY_INCLUDES_START
#include "XrEngine/Interfaces/Core/RBMKEngine.h"
THIRD_PARTY_INCLUDES_END

class FRBMKEngine :public IRBMKEngine
{
public:
									FRBMKEngine					();
	void							Initialize					() override;
	void							Destroy						() override;
	void							OnFrame						() override;


	class IRBMKUnrealProxy*			CreateUnrealProxy			() override;
	void							Destroy						(class IRBMKUnrealProxy*) override;

	class ILevelGraph*				GetLevelGraph				(const char*Name) override;
	class IGameGraph*				GetGameGraph				() override;
	IReader							GetGameSpawn				() override;
	void							LoadDefaultWorld			() override;
	void							Exit						() override;

	bool							LoadWorld					(const char* Name) override;
	void							LoadCFormFormCurrentWorld	(class CObjectSpace& ObjectSpace, CDB::build_callback build_callback);
	ERBMKWorldStatus				GetWorldStatus				() override;


	class IRBMKUnrealProxy*			GetUnrealPlayerCharacter	() override;


	shared_str						GetUnrealVersion			() override;
	IRBMKSoundManager*				GetSoundManager				() override;
	IRBMKEnvironment*				GetEnvironment				() override;
	virtual void					RunGame						(const char* ServerParams, const char* ClientParams) override;
};