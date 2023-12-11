#include "RBMKOGFImporterFactory.h"
#include "../FRBMKEngineFactory.h"
#include "Resources/SkeletonMesh/StalkerKinematicsAssetUserData.h"
URBMKOGFImporterFactory::URBMKOGFImporterFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = false;
	bEditAfterNew = false;
	bEditorImport = true;   // binary / general file source
	bText = false;  // text source
	SupportedClass = USkeletalMesh::StaticClass();

	Formats.Add(TEXT("ogf;XRay Sekeleton Mesh"));
}
  

UObject* URBMKOGFImporterFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPreImport(this, InClass, InParent, InName, Parms);
	AdditionalImportedObjects.Empty();

	Warn->Log(Filename);
	UObject* Object = nullptr;
	// show import options window

	const FString NewPackageName = UPackageTools::SanitizePackageName(*(FPaths::GetPath(InParent->GetName())));
	UObject* ParentPackage = NewPackageName == InParent->GetName() ? InParent : CreatePackage(*NewPackageName);

	FRBMKEngineFactory Factory(ParentPackage, Flags);
	Object = Factory.ImportOGF(Filename);
	if (!ensure(IsValid(Object)))
	{
		return nullptr;
	}
	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, Object);
	return Object;
}

void URBMKOGFImporterFactory::CleanUp()
{

}

bool URBMKOGFImporterFactory::FactoryCanImport(const FString& Filename)
{
	const FString Extension = FPaths::GetExtension(Filename);
	if (Extension == TEXT("ogf"))
	{
		return true;
	}
	return false;
}

TArray<FString> URBMKOGFImporterFactory::GetFormats() const
{
	TArray<FString> FormatArray;
	const bool bUseLegacyOgf = true;

	for (const FString& Format : Formats)
	{
	
		if (Format.StartsWith(TEXT("obj")))
		{
			//Skip if interchange obj is enabled
			if (bUseLegacyOgf)
			{
				FormatArray.Add(Format);
			}
		}
		else
		{
			FormatArray.Add(Format);
		}
	}
	return FormatArray;
}
