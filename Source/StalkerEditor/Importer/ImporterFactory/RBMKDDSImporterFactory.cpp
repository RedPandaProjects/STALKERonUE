#include "RBMKDDSImporterFactory.h"
#include "../FRBMKEngineFactory.h"
URBMKDDSImporterFactory::URBMKDDSImporterFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = false;
	bEditAfterNew = false;
	bEditorImport = true;   // binary / general file source
	bText = false;  // text source
	SupportedClass = UTexture2D::StaticClass();
	ImportPriority = ImportPriority + 1;
	Formats.Add(TEXT("dds;Red Image DDS"));
}
  

UObject* URBMKDDSImporterFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPreImport(this, InClass, InParent, InName, Parms);
	AdditionalImportedObjects.Empty();

	Warn->Log(Filename);
	UObject* Object = nullptr;
	// show import options window

	const FString NewPackageName = UPackageTools::SanitizePackageName(*(FPaths::GetPath(InParent->GetName())));
	UObject* ParentPackage = NewPackageName == InParent->GetName() ? InParent : CreatePackage(*NewPackageName);

	FRBMKEngineFactory Factory(ParentPackage, Flags);
	Object = Factory.ImportTextureDDS(Filename);
	if (!IsValid(Object))
	{
		return nullptr;
	}
	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, Object);
	return Object;
}

void URBMKDDSImporterFactory::CleanUp()
{

}

bool URBMKDDSImporterFactory::FactoryCanImport(const FString& Filename)
{
	const FString Extension = FPaths::GetExtension(Filename);
	if (Extension == TEXT("dds"))
	{
		return true;
	}
	return false;
}

TArray<FString> URBMKDDSImporterFactory::GetFormats() const
{
	return Formats;
}
