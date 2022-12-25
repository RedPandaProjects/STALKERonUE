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
	

	void Unload() override;
	GameMtlIt GetMaterialIt(LPCSTR name) override;
	GameMtlIt GetMaterialIt(shared_str& name) override;
	GameMtlIt GetMaterialItByID(int id) override;
	u32 GetMaterialID(LPCSTR name) override;
	SGameMtl* GetMaterialByID(s32 id) override;
	u16 GetMaterialIdx(int ID) override;
	u16 GetMaterialIdx(LPCSTR name) override;
	SGameMtl* GetMaterialByIdx(u16 idx) override;
	GameMtlIt FirstMaterial() override;
	GameMtlIt LastMaterial() override;
	u32 CountMaterial() override;
	SGameMtlPair* GetMaterialPair(u16 idx0, u16 idx1) override;
	GameMtlPairIt FirstMaterialPair() override;
	GameMtlPairIt LastMaterialPair() override;
	void Load() override;
	bool Save() override;

};
