#include "RBMKObjectImporterFactory.h"
#include "../FRBMKEngineFactory.h"
#include "RBMKObjectImportOptions.h"
#include "UI/StalkerImporterOptionsWindow.h"

URBMKObjectImporterFactory::URBMKObjectImporterFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = false;
	bEditAfterNew = false;
	bEditorImport = true;   // binary / general file source
	bText = false;  // text source

	SupportedClass = UStaticMesh::StaticClass();

	Formats.Add(TEXT("object;XRayObject"));
}
  

UObject* URBMKObjectImporterFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{

	auto ShowOptionsWindow = [](const FString& FileName, const FString& PackagePath, URBMKObjectImportOptions& ImporterOptions)->bool
	{
		TSharedPtr<SWindow> ParentWindow;
		if (FModuleManager::Get().IsModuleLoaded("MainFrame"))
		{
			IMainFrameModule& MainFrame = FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
			ParentWindow = MainFrame.GetParentWindow();
		}

		TSharedRef<SWindow> Window = SNew(SWindow).Title(FText::FromString( TEXT("XRay Object Import Options"))).SizingRule(ESizingRule::Autosized);

		TSharedPtr<SStalkerImporterOptionsWindow> OptionsWindow;
		Window->SetContent(
			SAssignNew(OptionsWindow, SStalkerImporterOptionsWindow)
			.ImportOptions(&ImporterOptions)
			.WidgetWindow(Window)
			.FileNameText(FText::FromString(FString::Printf(TEXT(	"Import File  :    %s"),*FPaths::GetCleanFilename(FileName))))
			.FilePathText(FText::FromString(FileName))
			.PackagePathText(FText::FromString(FString::Printf(TEXT("Import To    :    %s"), *PackagePath))));

		FSlateApplication::Get().AddModalWindow(Window, ParentWindow, false);
		return OptionsWindow->ShouldImport();
	};


	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPreImport(this, InClass, InParent, InName, Parms);
	AdditionalImportedObjects.Empty();

	Warn->Log(Filename);
	UObject* Object = nullptr;
	// show import options window

	const FString NewPackageName = UPackageTools::SanitizePackageName(*(FPaths::GetPath(InParent->GetName())));
	UObject* ParentPackage = NewPackageName == InParent->GetName() ? InParent : CreatePackage(*NewPackageName);


	TStrongObjectPtr<URBMKObjectImportOptions> ImporterOptions(NewObject<URBMKObjectImportOptions>(GetTransientPackage(),TEXT("XRay Object Importer Options")));
	if(IsAutomatedImport()||ShowOptionsWindow(Filename, NewPackageName,*ImporterOptions))
	{
		GXRayObjectLibrary->AngleSmooth = ImporterOptions->AngleNormalSmoth;
		switch (ImporterOptions->ObjectImportGameFormat)
		{
		default:
		GXRayObjectLibrary->LoadAsGame = EGame::NONE;
			break;
		case EXRayObjectImportGameFormat::SOC:
		GXRayObjectLibrary->LoadAsGame = EGame::SHOC;
			break;
		case EXRayObjectImportGameFormat::CS_COP:
		GXRayObjectLibrary->LoadAsGame = EGame::COP;
			break;
		}
		GXRayObjectLibrary->ReloadObjects();
		FRBMKEngineFactory Factory(ParentPackage, Flags);
		Object = Factory.ImportObject(Filename,ImporterOptions->DivideSubObject);
	}
	else
	{
		bOutOperationCanceled = true;
		return nullptr;
	}
	if (!IsValid(Object))
	{
		return nullptr;
	}
	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, Object);
	return Object;
}

void URBMKObjectImporterFactory::CleanUp()
{

}

bool URBMKObjectImporterFactory::FactoryCanImport(const FString& Filename)
{
	const FString Extension = FPaths::GetExtension(Filename);
	if (Extension == TEXT("object"))
	{
		return true;
	}
	return false;
}
