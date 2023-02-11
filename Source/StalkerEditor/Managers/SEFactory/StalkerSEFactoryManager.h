#pragma once
#include "IPlacementModeModule.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/XRaySEFactoryInterface.h"
THIRD_PARTY_INCLUDES_END
#include "StalkerSEFactoryManager.generated.h"
UCLASS()
class UStalkerSEFactoryManager : public UObject
{
	GENERATED_BODY()
public:
	void											Initialized				();
	void											Destroy					();
	void											Load					();
	void											UnLoad					();
	ISE_Abstract*									CreateEntity			(const char* SectionName);
	void											DestroyEntity			(ISE_Abstract* Entity);
	void											RefreshPlaceableActors	();
	bool											IsVoid					();


	TArray<TSharedPtr<FString>>						LightAnims_ForComboBox;

	UPROPERTY(Transient)
	TMap<FString,int32>								Location1;
	TArray<TSharedPtr<FString>>						Location1_ForComboBox;

	UPROPERTY(Transient)
	TMap<FString,int32>								Location2;
	TArray<TSharedPtr<FString>>						Location2_ForComboBox;
	
	UPROPERTY(Transient)
	TMap<FString,int32>								Location3;
	TArray<TSharedPtr<FString>>						Location3_ForComboBox;

	UPROPERTY(Transient)
	TMap<FString,int32>								Location4;
	TArray<TSharedPtr<FString>>						Location4_ForComboBox;

	UPROPERTY(Transient)
	TMap<FString,int32>								SpawnStorys;
	TArray<TSharedPtr<FString>>						SpawnStorys_ForComboBox;

	UPROPERTY(Transient)
	TMap<FString,int32>								Storys;
	TArray<TSharedPtr<FString>>						Storys_ForComboBox;

	TArray<TSharedPtr<FString>>						CharacterProfiles_ForComboBox;
	TArray<TSharedPtr<FString>>						SmartCoverDescriptions_ForComboBox;
private:
	void											OnPreReInitialized		();
	void											OnPostReInitialized		(); 

	void											OnPlacementModeCategoryRefreshed(FName InName);
	void											CreatePlaceableSpawn();
	UPROPERTY(Transient)
	TArray<class AStalkerSpawnObject*>				SpawnObjectTemplates;
	TArray<FPlacementModeID>						SpawnObjectPlacementItems;
	XRaySEFactoryInterface*							SEFactoryInterface = nullptr;
	void*											SEFactoryHandle = nullptr;


};
