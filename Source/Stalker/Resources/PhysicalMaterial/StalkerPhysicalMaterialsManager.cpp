#include "StalkerPhysicalMaterialsManager.h"
#include "Kernel/StalkerEngineManager.h"
#include "../StalkerResourcesManager.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "StalkerPhysicalMaterial.h"
#include "StalkerPhysicalMaterialPairsData.h"

void UStalkerPhysicalMaterialsManager::Unload()
{
}

void UStalkerPhysicalMaterialsManager::Build()
{
	check((PhysicalMaterials.Num()||
	LegacyPhysicalMaterials.Num()||
	LegacyPhysicalMaterialPairs.Num()||
	Name2ID.Num())==false);
	const FString PackageName = GStalkerEngineManager->GetResourcesManager()->GetGamePath() / TEXT("PhysicalMaterials") / TEXT("Materials");
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
#if WITH_EDITOR
	if(!GIsEditor)
	{
		AssetRegistryModule.Get().ScanPathsSynchronous({PackageName},true);
	}
#endif

	TArray<FAssetData> AssetData;
	AssetRegistryModule.Get().GetAssetsByPath(FName(*PackageName), AssetData, true);
	for (FAssetData& Data : AssetData)
	{
		UStalkerPhysicalMaterial* Material = Cast<UStalkerPhysicalMaterial>(Data.GetAsset());
		if (Material)
		{
			FString Name = Data.PackageName.ToString().ToLower();
			verifySlow(Name.RemoveFromStart(PackageName/TEXT("")));
			PhysicalMaterials.Add(Material);
			Name.ReplaceCharInline(TEXT('/'), TEXT('\\'));
			Name2ID.Add(shared_str(TCHAR_TO_ANSI(*Name)), PhysicalMaterials.Num()-1);
			TSharedPtr<SGameMtl> LegacyMtl = MakeShared<SGameMtl>();
			LegacyMtl->ID = PhysicalMaterials.Num() - 1;
			LegacyMtl->m_Name = shared_str(TCHAR_TO_ANSI(*Name));
			Material->BuildToLegacy(*LegacyMtl);
			LegacyPhysicalMaterials.Add(LegacyMtl);
			Names.Add(LegacyMtl->m_Name);
			if (DefaultPhysicalMaterial == nullptr)
			{
				DefaultPhysicalMaterial = Material;
			}
			else if (Name == TEXT("default"))
			{
				DefaultPhysicalMaterial = Material;
			}
		}
	}

	const FString MaterialPairsPackageName = GStalkerEngineManager->GetResourcesManager()->GetGamePath() / TEXT("PhysicalMaterials") / TEXT("MaterialPairs");
	const FString MaterialPairsObjectPath = MaterialPairsPackageName + TEXT(".") + FPaths::GetBaseFilename(MaterialPairsPackageName);

	UStalkerPhysicalMaterialPairsData* PhysicalMaterialPairs = LoadObject<UStalkerPhysicalMaterialPairsData>(nullptr, *MaterialPairsObjectPath, nullptr, LOAD_NoWarn);

	if (IsValid(PhysicalMaterialPairs))
	{
		for (auto & [Material0,Conteniar]:PhysicalMaterialPairs->Pairs)
		{
			for (auto& [Material1, Pair] : Conteniar.Materials)
			{
				if (!Material0 || !Material1)
				{
					continue;
				}
				int32 Index0 = PhysicalMaterials.IndexOfByKey(Material0);
				int32 Index1 = PhysicalMaterials.IndexOfByKey(Material1);
				if (Index0 == INDEX_NONE || Index1 == INDEX_NONE)
				{
					if (Index0 == INDEX_NONE)
					{
						FString Name;
						Material0->GetName(Name);
						UE_LOG(LogStalker,Warning,TEXT("This Material[%s] is not registered"),*Name);
					}
					if (Index1 == INDEX_NONE)
					{
						FString Name;
						Material1->GetName(Name);
						UE_LOG(LogStalker, Warning, TEXT("This Material[%s] is not registered"), *Name);
					}
					continue;
				}
				TSharedPtr<SGameMtlPair> LegacyPair = MakeShared<SGameMtlPair>(this);
				LegacyPair->SetPair(Index0,Index1);
				Pair.BuildToLegacy(*LegacyPair);
				LegacyPhysicalMaterialPairs.Add(LegacyPair);

			}
		}
		
	}
}

void UStalkerPhysicalMaterialsManager::Clear()
{
	Names.Empty();
	PhysicalMaterials.Empty();
	LegacyPhysicalMaterials.Empty();
	LegacyPhysicalMaterialPairs.Empty();
	Name2ID.Empty();
	DefaultPhysicalMaterial = nullptr;
}

u32 UStalkerPhysicalMaterialsManager::GetMaterialID(LPCSTR name)
{
	int32*ID =   Name2ID.Find(name);
	if (!ID)
	{
		return GAMEMTL_NONE_ID;
	}
	return static_cast<u32>(*ID);
}

SGameMtl* UStalkerPhysicalMaterialsManager::GetMaterialByID(s32 id)
{
	if (LegacyPhysicalMaterials.Num() > id&& id >= 0)
	{
		return LegacyPhysicalMaterials[id].Get();
	}
	return nullptr;
}

u16 UStalkerPhysicalMaterialsManager::GetMaterialIdx(int id)
{
	if (LegacyPhysicalMaterials.Num() > id && id >= 0)
	{
		return static_cast<u16>(id);
	}
	return GAMEMTL_NONE_IDX;
}

u16 UStalkerPhysicalMaterialsManager::GetMaterialIdx(LPCSTR name)
{
	int32* ID = Name2ID.Find(name);
	if (!ID)
	{
		return GAMEMTL_NONE_IDX;
	}
	return static_cast<u16>(*ID);
}

SGameMtl* UStalkerPhysicalMaterialsManager::GetMaterialByIdx(u16 id)
{
	if (LegacyPhysicalMaterials.Num() > id && id >= 0)
	{
		return LegacyPhysicalMaterials[id].Get();
	}
	return nullptr;
}
u32 UStalkerPhysicalMaterialsManager::CountMaterial()
{
	return LegacyPhysicalMaterials.Num();
}

SGameMtlPair* UStalkerPhysicalMaterialsManager::GetMaterialPair(u16 idx0, u16 idx1)
{
	TSharedPtr< SGameMtlPair>*Result = LegacyPhysicalMaterialPairs.FindByPredicate([idx0,idx1](TSharedPtr< SGameMtlPair>&Item){return (Item->GetMtl0()==idx0&& Item->GetMtl1() == idx1)|| (Item->GetMtl0() == idx1 && Item->GetMtl1() == idx0);});
	return Result? Result->Get():nullptr;
}

void UStalkerPhysicalMaterialsManager::Load()
{
}

bool UStalkerPhysicalMaterialsManager::Save()
{
	throw std::logic_error("The method or operation is not implemented.");
}
