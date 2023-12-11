#include "RBMKLevelImporterFactory.h"
#include "../FRBMKLevelFactory.h"
#include "UI/StalkerImporterOptionsWindow.h"
#include "RBMKLevelImportOptions.h"
URBMKLevelImporterFactory::URBMKLevelImporterFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = false;
	bEditAfterNew = false;
	bEditorImport = true;   // binary / general file source
	bText = false;  // text source

	SupportedClass = ULevel::StaticClass();

	Formats.Add(TEXT("level;XRaySDKLevel"));
}
UObject* URBMKLevelImporterFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{ 

	auto ShowOptionsWindow = [](const FString& FileName, const FString& PackagePath, URBMKLevelImportOptions& ImporterOptions)->bool
	{
		TSharedPtr<SWindow> ParentWindow;
		if (FModuleManager::Get().IsModuleLoaded("MainFrame"))
		{
			IMainFrameModule& MainFrame = FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
			ParentWindow = MainFrame.GetParentWindow();
		}

		TSharedRef<SWindow> Window = SNew(SWindow).Title(FText::FromString( TEXT("XRay Level Import Options"))).SizingRule(ESizingRule::Autosized);

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
	// show import options window
	TStrongObjectPtr<URBMKLevelImportOptions> ImporterOptions(NewObject<URBMKLevelImportOptions>(GetTransientPackage(),TEXT("XRay Level Importer Options")));
	
	const FString NewPackageName = UPackageTools::SanitizePackageName(*(FPaths::GetPath(InParent->GetName())));
	UObject* ParentPackage = NewPackageName == InParent->GetName() ? InParent : CreatePackage(*NewPackageName);
	UObject* Object = nullptr;
	if(ShowOptionsWindow(Filename, NewPackageName,*ImporterOptions))
	{
		FRBMKLevelFactory Factory(ParentPackage, Flags);
		if (!Factory.ImportLevel(Filename,*ImporterOptions))
		{
			bOutOperationCanceled = true;
			return nullptr;
		}
		Object = Factory.GetCreatedObject();
	}
	else
	{
			bOutOperationCanceled = true;
			return nullptr;
	}
	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, Object);
	return Object;
}

void URBMKLevelImporterFactory::CleanUp()
{

}

bool URBMKLevelImporterFactory::FactoryCanImport(const FString& Filename)
{
	const FString Extension = FPaths::GetExtension(Filename);
	if (Extension == TEXT("level"))
	{
		return true;
	}
	return false;
}
