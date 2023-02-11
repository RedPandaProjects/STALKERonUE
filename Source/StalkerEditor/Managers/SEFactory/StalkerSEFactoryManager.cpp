#include "StalkerSEFactoryManager.h"
#include "../../Entities/Scene/SpawnObject/StalkerSpawnObject.h"
#include "Kernel/StalkerEngineManager.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/LightAnimLibrary.h"
THIRD_PARTY_INCLUDES_END
#define LOCTEXT_NAMESPACE "StalkerEditor"

static FName NAME_StalkerSpawn = "StalkerSpawn";

void UStalkerSEFactoryManager::Initialized()
{
	CreatePlaceableSpawn();

	GXRayEngineManager->PostReInitializedMulticastDelegate.AddUObject(this, &UStalkerSEFactoryManager::OnPostReInitialized);
	GXRayEngineManager->PreReInitializedMulticastDelegate.AddUObject(this, &UStalkerSEFactoryManager::OnPreReInitialized);

	OnPostReInitialized();
}

void UStalkerSEFactoryManager::Destroy()
{
	GXRayEngineManager->PostReInitializedMulticastDelegate.RemoveAll(this);
	GXRayEngineManager->PreReInitializedMulticastDelegate.RemoveAll(this);

	OnPreReInitialized();

	if (IPlacementModeModule::IsAvailable())
	{
		IPlacementModeModule::Get().UnregisterPlacementCategory(NAME_StalkerSpawn);
		IPlacementModeModule::Get().OnPlacementModeCategoryRefreshed().RemoveAll(this);
	}
}

void UStalkerSEFactoryManager::Load()
{
	if (!SEFactoryHandle)
	{
		return;
	}
	if (SEFactoryInterface)
	{
		return;
	}
	XRaySEFactoryInterface* (__cdecl * FGetSEFactoryInterface)(void);
	FGetSEFactoryInterface = reinterpret_cast<XRaySEFactoryInterface * (__cdecl * )(void)>(FPlatformProcess::GetDllExport(SEFactoryHandle, TEXT("GetSEFactoryInterface")));
	if (!FGetSEFactoryInterface)
	{
		return;
	}
	SEFactoryInterface = FGetSEFactoryInterface();
	if (!SEFactoryInterface)
	{
		return;
	}
	SEFactoryInterface->Create();
	for (const shared_str& Profile : SEFactoryInterface->GetSmartCovers())
	{
		SmartCoverDescriptions_ForComboBox.Add(TSharedPtr<FString>(new FString(Profile.c_str())));
	}
	
	for (const shared_str& Profile : SEFactoryInterface->GetCharacterProfiles())
	{
		CharacterProfiles_ForComboBox.Add(TSharedPtr<FString>(new FString(Profile.c_str())));
	}

	for (const xr_rtoken& token : SEFactoryInterface->GetSpawnStoryNames())
	{
		SpawnStorys.FindOrAdd(token.name.c_str(), token.id);
		SpawnStorys_ForComboBox.Add(TSharedPtr<FString>(new FString(token.name.c_str())));
	}
	for (const xr_rtoken& token : SEFactoryInterface->GetStoryNames())
	{
		Storys.FindOrAdd(token.name.c_str(), token.id);
		Storys_ForComboBox.Add(TSharedPtr<FString>(new FString(token.name.c_str())));
	}

	for (const xr_rtoken& token : SEFactoryInterface->GetLocations(0))
	{
		Location1.FindOrAdd(token.name.c_str(), token.id);
		Location1_ForComboBox.Add(TSharedPtr<FString>(new FString(token.name.c_str())));
	}

	for (const xr_rtoken& token : SEFactoryInterface->GetLocations(1))
	{
		Location2.FindOrAdd(token.name.c_str(), token.id);
		Location2_ForComboBox.Add(TSharedPtr<FString>(new FString(token.name.c_str())));
	}

	for (const xr_rtoken& token : SEFactoryInterface->GetLocations(2))
	{
		Location3.FindOrAdd(token.name.c_str(), token.id);
		Location3_ForComboBox.Add(TSharedPtr<FString>(new FString(token.name.c_str())));
	}

	for (const xr_rtoken& token : SEFactoryInterface->GetLocations(3))
	{
		Location4.FindOrAdd(token.name.c_str(), token.id);
		Location4_ForComboBox.Add(TSharedPtr<FString>(new FString(token.name.c_str())));
	}
	LightAnims_ForComboBox.Add(TSharedPtr<FString>(new FString(TEXT("none"))));
	for (CLAItem*Item : LALib->Items)
	{
		LightAnims_ForComboBox.Add(TSharedPtr<FString>(new FString(Item->cName.c_str())));
	}
	
	FWorldContext* WorldContext = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
	if (WorldContext)
	{
		UWorld* World = WorldContext->World();
		if (IsValid(World))
		{
			for (TActorIterator<AStalkerSpawnObject> AactorItr(World); AactorItr; ++AactorItr)
			{
				AactorItr->SpawnRead();
			}
		}
	}
}

void UStalkerSEFactoryManager::UnLoad()
{	
	if (!SEFactoryInterface)
	{
		return;
	}

	FWorldContext* WorldContext = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
	if (WorldContext)
	{
		UWorld* World = WorldContext->World();
		if (IsValid(World))
		{
			for (TActorIterator<AStalkerSpawnObject> AactorItr(World); AactorItr; ++AactorItr)
			{
				AactorItr->SpawnWrite();
				AactorItr->DestroyEntity();
			}
		}
	}

	SEFactoryInterface->Destroy();
	SEFactoryInterface = nullptr;


	SpawnStorys.Reset();
	SpawnStorys_ForComboBox.Reset();
	Storys.Reset();
	Storys_ForComboBox.Reset();
	Location1.Reset();
	Location1_ForComboBox.Reset();
	Location2.Reset();
	Location2_ForComboBox.Reset();
	Location3.Reset();
	Location3_ForComboBox.Reset();
	Location4.Reset();
	Location4_ForComboBox.Reset();
	LightAnims_ForComboBox.Reset();
	CharacterProfiles_ForComboBox.Reset();
	SmartCoverDescriptions_ForComboBox.Reset();
}

ISE_Abstract* UStalkerSEFactoryManager::CreateEntity(const char* SectionName)
{
	check(SEFactoryInterface);
	return SEFactoryInterface->CreateEntity(SectionName);
}

void UStalkerSEFactoryManager::DestroyEntity(ISE_Abstract* Entity)
{
	check(SEFactoryInterface);
	SEFactoryInterface->DestroyEntity(Entity);
}

void UStalkerSEFactoryManager::RefreshPlaceableActors()
{
	SpawnObjectTemplates.Empty();
	IPlacementModeModule::Get().RegenerateItemsForCategory(NAME_StalkerSpawn);
	IPlacementModeModule::Get().RegenerateItemsForCategory(FBuiltInPlacementCategories::AllClasses());
}

bool UStalkerSEFactoryManager::IsVoid()
{
	return SEFactoryInterface == nullptr;
}

void UStalkerSEFactoryManager::OnPreReInitialized()
{
	if (!IsVoid())
	{
		UnLoad();
	}
	FPlatformProcess::FreeDllHandle(SEFactoryHandle);
	SEFactoryHandle = nullptr;
}

void UStalkerSEFactoryManager::OnPostReInitialized()
{
	FString Name = TEXT("XrSE_Factory");

	if (FApp::GetBuildConfiguration() == EBuildConfiguration::Debug || FApp::GetBuildConfiguration() == EBuildConfiguration::DebugGame)
	{
		Name += "_Debug";
	}
	else
	{
		Name += "_Release";
	}

	Name += ".dll";

	SEFactoryHandle = FPlatformProcess::GetDllHandle(*Name);
	if (SEFactoryHandle == nullptr)
	{
		UE_LOG(LogStalkerEditor, Log, TEXT("Failed to load %s"), *Name);
	}
	Load();
	RefreshPlaceableActors();
}

void UStalkerSEFactoryManager::OnPlacementModeCategoryRefreshed(FName InName)
{
	if (InName != NAME_StalkerSpawn && InName != FBuiltInPlacementCategories::AllClasses())
	{
		return;
	}
	if (!SpawnObjectTemplates.Num())
	{
		CInifile::Root& Data = ((CInifile*)pSettings)->sections();
		for (CInifile::Sect* Section : Data)
		{
			LPCSTR val;
			if (Section->line_exist("$spawn", &val))
			{
				FString Caption = FString(pSettings->r_string_wb(Section->Name, "$spawn").c_str());
				FString SectionName = FString(Section->Name.c_str());
				if (Caption.Len())
				{
					AStalkerSpawnObject* StalkerSpawn = NewObject< AStalkerSpawnObject>(this);
					StalkerSpawn->SectionName = SectionName;
					StalkerSpawn->DisplayName = Caption;
					SpawnObjectTemplates.Add(StalkerSpawn);
				}
			}
		}
	}
	if (InName == NAME_StalkerSpawn)
	{
		for (FPlacementModeID& Item : SpawnObjectPlacementItems)
		{
			IPlacementModeModule::Get().UnregisterPlaceableItem(Item);
		}
		SpawnObjectPlacementItems.Reset(0);
	}
	int32 SortOrder = 0;
	for (AStalkerSpawnObject* Spawn : SpawnObjectTemplates)
	{
		TSharedRef< FPlaceableItem> PlaceableItem = MakeShareable(new FPlaceableItem(*AStalkerSpawnObject::StaticClass(), FAssetData(Spawn), NAME_None, NAME_None, TOptional<FLinearColor>(), SortOrder++, FText::FromString(Spawn->DisplayName)));
		TOptional<FPlacementModeID> ModeID = IPlacementModeModule::Get().RegisterPlaceableItem(InName, PlaceableItem);
		if (InName == NAME_StalkerSpawn && ModeID.IsSet())
		{
			SpawnObjectPlacementItems.Add(ModeID.GetValue());
		}
	};
}

void UStalkerSEFactoryManager::CreatePlaceableSpawn()
{
	FPlacementCategoryInfo Info(LOCTEXT("StalkerSpawnCategoryName", "Stalker Spawn"), FSlateIcon(), NAME_StalkerSpawn, TEXT("PMStalkerSpawn"), 25);
	IPlacementModeModule::Get().RegisterPlacementCategory(Info);
	IPlacementModeModule::Get().OnPlacementModeCategoryRefreshed().AddUObject(this, &UStalkerSEFactoryManager::OnPlacementModeCategoryRefreshed);
}

#undef LOCTEXT_NAMESPACE