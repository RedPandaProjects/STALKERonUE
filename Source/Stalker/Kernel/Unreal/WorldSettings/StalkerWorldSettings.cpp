// Fill out your copyright notice in the Description page of Project Settings.


#include "StalkerWorldSettings.h"
#include "Resources/CFrom/StalkerCForm.h"

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

class UStalkerCForm* AStalkerWorldSettings::GetCForm()
{
	return CForm;
}
