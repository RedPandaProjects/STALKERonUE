// Fill out your copyright notice in the Description page of Project Settings.


#include "StalkerWorldSettings.h"
#include "Resources/CFrom/StalkerCForm.h"
#include "Resources/AIMap/StalkerAIMap.h"
#include "Resources/Spawn/StalkerLevelSpawn.h"

UStalkerCForm* AStalkerWorldSettings::GetCForm()
{
	return CForm;
}

UStalkerAIMap* AStalkerWorldSettings::GetAIMap()
{
	return AIMap;
}
#if WITH_EDITORONLY_DATA
UStalkerAIMap* AStalkerWorldSettings::GetOrCreateAIMap()
{
	auto CreatePackageAIMap = [this]()
	{
		FString PackageName = GetOutermost()->GetName() + TEXT("_AIMap");
		UPackage* BuiltDataPackage = CreatePackage(*PackageName);
		return BuiltDataPackage;
	};

	if (!AIMap || !AIMap->HasAllFlags(RF_Public | RF_Standalone))
	{
		if (AIMap)
		{
			AIMap->ClearFlags(RF_Standalone);
		}

		UPackage* BuiltDataPackage = CreatePackageAIMap();

		FName ShortPackageName = FPackageName::GetShortFName(BuiltDataPackage->GetFName());
		AIMap = NewObject<UStalkerAIMap>(BuiltDataPackage, ShortPackageName, RF_Standalone | RF_Public | RF_Transactional);
		Modify();
	}
	return AIMap;
}

UStalkerCForm* AStalkerWorldSettings::GetOrCreateCForm()
{
	auto CreatePackageCForm = [this]()
	{
		FString PackageName = GetOutermost()->GetName() + TEXT("_CForm");
		UPackage* BuiltDataPackage = CreatePackage(*PackageName);
		return BuiltDataPackage;
	};

	if (!CForm || !CForm->HasAllFlags(RF_Public | RF_Standalone))
	{
		if (CForm)
		{
			CForm->ClearFlags(RF_Standalone);
		}

		UPackage* BuiltDataPackage = CreatePackageCForm();

		FName ShortPackageName = FPackageName::GetShortFName(BuiltDataPackage->GetFName());
		CForm = NewObject<UStalkerCForm>(BuiltDataPackage, ShortPackageName, RF_Standalone | RF_Public);
		Modify();
	}
	return CForm;

}

UStalkerLevelSpawn* AStalkerWorldSettings::GetOrCreateSpawn()
{
	auto CreatePackageCForm = [this]()
	{
		FString PackageName = GetOutermost()->GetName() + TEXT("_Spawn");
		UPackage* BuiltDataPackage = CreatePackage(*PackageName);
		return BuiltDataPackage;
	};

	if (!Spawn || !Spawn->HasAllFlags(RF_Public | RF_Standalone))
	{
		if (Spawn)
		{
			Spawn->ClearFlags(RF_Standalone);
		}

		UPackage* BuiltDataPackage = CreatePackageCForm();

		FName ShortPackageName = FPackageName::GetShortFName(BuiltDataPackage->GetFName());
		Spawn = NewObject<UStalkerLevelSpawn>(BuiltDataPackage, ShortPackageName, RF_Standalone | RF_Public);
		Modify();
	}
	return Spawn;
}

UStalkerLevelSpawn* AStalkerWorldSettings::GetSpawn()
{
	return Spawn;
}
#endif
void AStalkerWorldSettings::PostInitProperties()
{
	Super::PostInitProperties();
#if WITH_EDITORONLY_DATA
	if (HasAnyFlags(RF_NeedLoad | RF_WasLoaded | RF_ClassDefaultObject) == false)
	{
		EditorMaterialAIMap = LoadObject<UMaterialInterface>(this, TEXT("/Game/Editor/Materials/ai_map.ai_map"));
	}
#endif
}
