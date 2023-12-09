#include "XRayGameMtlImporterFactory.h"
#include "../FRBMKEngineFactory.h"
#include "Resources/PhysicalMaterial/StalkerPhysicalMaterial.h"
UXRayGameMtlImporterFactory::UXRayGameMtlImporterFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = false;
	bEditAfterNew = false;
	bEditorImport = true;   // binary / general file source
	bText = false;  // text source
	SupportedClass = UStalkerPhysicalMaterial::StaticClass();
	Formats.Add(TEXT("xr;XRay Resource Package"));
}
  

UObject* UXRayGameMtlImporterFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPreImport(this, InClass, InParent, InName, Parms);
	AdditionalImportedObjects.Empty();
	Warn->Log(Filename);
	UObject* Object = nullptr;
	// show import options window

	const FString NewPackageName = UPackageTools::SanitizePackageName(*(FPaths::GetPath(InParent->GetName())));
	UObject* ParentPackage = NewPackageName == InParent->GetName() ? InParent : CreatePackage(*NewPackageName);

	FRBMKEngineFactory Factory(ParentPackage, Flags);
	Object = Factory.ImportPhysicsMaterials(Filename);
	if (!IsValid(Object))
	{
		bOutOperationCanceled = true;
		return nullptr;
	}
	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, Object);
	return Object;
}

void UXRayGameMtlImporterFactory::CleanUp()
{

}

bool UXRayGameMtlImporterFactory::FactoryCanImport(const FString& Filename)
{
	const FString FileName = FPaths::GetCleanFilename(Filename).ToLower();
	if (FileName == TEXT("gamemtl.xr"))
	{
		return true;
	}
	return false;
}

TArray<FString> UXRayGameMtlImporterFactory::GetFormats() const
{
	return Formats;
}
