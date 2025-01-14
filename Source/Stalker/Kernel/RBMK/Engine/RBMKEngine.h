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


	class IRBMKUnrealProxy*			CreateUnrealProxy			(const char*UnrealProxyClass) override;
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
	void							OnRunGame					(const char* ServerParams, const char* ClientParams) override;
	IRBMKLoadingScreenManager*		GetLoadingScreen			() override;

	void							ExecUeCmd					(const char* cmd) override;
	void							ChangeUeSettingsInt			(const std::pair<int, int>& settinglist) override;
	int								GetSettingInt				(int setting, int& min, int& max) override;
	void							ChangeUeSettingsFloat(const std::pair<int, float>& settinglist) override;
	float							GetSettingFloat(int setting, float& min, float& max) override;

	void				GetResolutions(std::vector<xr_string>& ResVec) override;
	void				GetCurrentResolution(u32& w, u32& h) override;
	void				SetResolution(u32 w, u32 h) override;
private:
	enum ESettingsListInt
	{
		ShadowQuality = 1,
		ViewDistanceQuality,
		GlobalIlluminationQuality,
		AntiAliasingQuality,
		ReflectionQuality,
		PostProcessingQuality,
		TextureQuality,
		VisualEffectsQuality,
		FoliageQuality,
		ShadingQuality,
		FrameLimit
	};
	enum ESettingsListFloat
	{
		EffectsVolume = 1,
		MusicVolume,
		MasterVolume
	};
};