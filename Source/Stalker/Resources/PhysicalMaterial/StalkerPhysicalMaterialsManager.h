#pragma once
THIRD_PARTY_INCLUDES_START
#include "XrEngine/XrGameMaterialLibraryInterface.h"
THIRD_PARTY_INCLUDES_END
#include "StalkerPhysicalMaterialsManager.generated.h"

UCLASS()
class STALKER_API UStalkerPhysicalMaterialsManager : public UObject,public XrGameMaterialLibraryInterface
{
	GENERATED_BODY()
public:
	u32										GetMaterialID		(LPCSTR name) override;
	SGameMtl*								GetMaterialByID		(s32 id) override;
	u16										GetMaterialIdx		(int ID) override;
	u16										GetMaterialIdx		(LPCSTR name) override;
	SGameMtl*								GetMaterialByIdx	(u16 idx) override;
	u32										CountMaterial		() override;
	SGameMtlPair*							GetMaterialPair		(u16 idx0, u16 idx1) override;
	void									Load				() override;
	bool									Save				() override;
	void									Unload				() override;
						
	void									Build				();
	void									Clear				();
	UPROPERTY(Transient)
	class UStalkerPhysicalMaterial*			DefaultPhysicalMaterial;
	UPROPERTY(Transient)
	TArray<class UStalkerPhysicalMaterial*> PhysicalMaterials;
	TArray<shared_str>						Names;
private:
	TArray<TSharedPtr<SGameMtl>>			LegacyPhysicalMaterials;
	TArray<TSharedPtr<SGameMtlPair>>		LegacyPhysicalMaterialPairs;
	TMap<shared_str,int32>					Name2ID;

};
