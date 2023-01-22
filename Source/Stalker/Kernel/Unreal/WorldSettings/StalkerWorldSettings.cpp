// Fill out your copyright notice in the Description page of Project Settings.


#include "StalkerWorldSettings.h"
#include "Resources/CFrom/StalkerCForm.h"
#include "Resources/AIMap/StalkerAIMap.h"

UStalkerCForm* AStalkerWorldSettings::GetOrCreateCForm()
{
	auto CreatePackageCForm = [this]()
	{
		FString PackageName = GetOutermost()->GetName() + TEXT("_CForm");
		UPackage* BuiltDataPackage = CreatePackage(*PackageName);
		BuiltDataPackage->SetPackageFlags(PKG_ContainsMapData);
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
		MarkPackageDirty();
	}
	return CForm;

}

UStalkerCForm* AStalkerWorldSettings::GetCForm()
{
	return CForm;
}

UStalkerAIMap* AStalkerWorldSettings::GetOrCreateAIMap()
{
	auto CreatePackageAIMap = [this]()
	{
		FString PackageName = GetOutermost()->GetName() + TEXT("_AIMap");
		UPackage* BuiltDataPackage = CreatePackage(*PackageName);
		BuiltDataPackage->SetPackageFlags(PKG_ContainsMapData);
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
		MarkPackageDirty();
	}
	return AIMap;
}

UStalkerAIMap* AStalkerWorldSettings::GetAIMap()
{
	return AIMap;
}

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
