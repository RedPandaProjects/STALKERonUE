#include "RBMKTextureTHMImporterFactory.h"
#include "../FRBMKEngineFactory.h"
URBMKTextureTHMImporterFactory::URBMKTextureTHMImporterFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = false;
	bEditAfterNew = false;
	bEditorImport = true;   // binary / general file source
	bText = false;  // text source

	SupportedClass = ULevel::StaticClass();

	Formats.Add(TEXT("thm;XRay Texture THM"));
}


UObject* URBMKTextureTHMImporterFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPreImport(this, InClass, InParent, InName, Parms);
	AdditionalImportedObjects.Empty();

	Warn->Log(Filename);
	UObject* Object = nullptr;
	// show import options window

	const FString NewPackageName = UPackageTools::SanitizePackageName(*(FPaths::GetPath(InParent->GetName())));
	UObject* ParentPackage = NewPackageName == InParent->GetName() ? InParent : CreatePackage(*NewPackageName);

	FRBMKEngineFactory Factory(ParentPackage, Flags);
	Object = Factory.ImportTextureTHM(Filename);
	if (!IsValid(Object))
	{
		return nullptr;
	}
	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, Object);
	return Object;
}

void URBMKTextureTHMImporterFactory::CleanUp()
{

}

bool URBMKTextureTHMImporterFactory::FactoryCanImport(const FString& Filename)
{
	const FString Extension = FPaths::GetExtension(Filename);
	if (Extension == TEXT("thm"))
	{
		return true;
	}
	return false;
}
