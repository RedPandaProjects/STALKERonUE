#include "XRayLevelImporterFactory.h"
#include "XRayLevelFactory.h"
UXRayLevelImporterFactory::UXRayLevelImporterFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = false;
	bEditAfterNew = false;
	bEditorImport = true;   // binary / general file source
	bText = false;  // text source

	SupportedClass = ULevel::StaticClass();

	Formats.Add(TEXT("level;XRaySDKLevel"));
}
UObject* UXRayLevelImporterFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{ 

	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPreImport(this, InClass, InParent, InName, Parms);
	AdditionalImportedObjects.Empty();

	Warn->Log(Filename);
	// show import options window

	const FString NewPackageName = UPackageTools::SanitizePackageName(*(FPaths::GetPath(InParent->GetName())));
	UObject* ParentPackage = NewPackageName == InParent->GetName() ? InParent : CreatePackage(*NewPackageName);

	XRayLevelFactory Factory(ParentPackage, Flags);
	if (!Factory.ImportLevel(Filename))
	{
		bOutOperationCanceled = true;
		return false;
	}

	UObject* Object = Factory.GetCreatedObject();
	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, Object);
	return Object;
}

void UXRayLevelImporterFactory::CleanUp()
{

}

bool UXRayLevelImporterFactory::FactoryCanImport(const FString& Filename)
{
	const FString Extension = FPaths::GetExtension(Filename);
	if (Extension == TEXT("level"))
	{
		return true;
	}
	return false;
}
