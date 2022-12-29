#include "XRayObjectImporterFactory.h"
#include "../XRayEngineFactory.h"
UXRayObjectImporterFactory::UXRayObjectImporterFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = false;
	bEditAfterNew = false;
	bEditorImport = true;   // binary / general file source
	bText = false;  // text source

	SupportedClass = UStaticMesh::StaticClass();

	Formats.Add(TEXT("object;XRayObject"));
}
  

UObject* UXRayObjectImporterFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPreImport(this, InClass, InParent, InName, Parms);
	AdditionalImportedObjects.Empty();

	Warn->Log(Filename);
	UObject* Object = nullptr;
	// show import options window

	const FString NewPackageName = UPackageTools::SanitizePackageName(*(FPaths::GetPath(InParent->GetName())));
	UObject* ParentPackage = NewPackageName == InParent->GetName() ? InParent : CreatePackage(*NewPackageName);

	XRayEngineFactory Factory(ParentPackage, Flags);
	Object = Factory.ImportObject(Filename);
	if (!IsValid(Object))
	{
		bOutOperationCanceled = true;
		return nullptr;
	}
	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, Object);
	return Object;
}

void UXRayObjectImporterFactory::CleanUp()
{

}

bool UXRayObjectImporterFactory::FactoryCanImport(const FString& Filename)
{
	const FString Extension = FPaths::GetExtension(Filename);
	if (Extension == TEXT("object"))
	{
		return true;
	}
	return false;
}
