#include "StalkerSoundManager.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Kernel/StalkerEngineManager.h"
#include "Kernel/Unreal/GameSettings/StalkerGameSettings.h"
#include "Resources/StalkerResourcesManager.h"

IRBMKSoundSource* UStalkerSoundManager::CreateSource(const char* InName)
{
	return CreateSource(InName,static_cast<s32>(EStalkerSoundTypes::FromSource));
}

IRBMKSoundSource* UStalkerSoundManager::CreateSource(const char* InName, s32 CustomFlags)
{
	const FString Name = FPaths::ChangeExtension( ANSI_TO_TCHAR(InName),TEXT("")).Replace(TEXT("\\"),TEXT("/"));
	if(!ensure(SoundsList.Contains(Name)))
	{
		return nullptr;
	}
	const FString PackageName = GStalkerEngineManager->GetGamePath() / TEXT("Sounds") / Name;
	const FString ObjectName = PackageName + TEXT(".") + FPaths::GetBaseFilename(PackageName);
	USoundBase*Sound = LoadObject<USoundBase>(nullptr, *ObjectName, nullptr, LOAD_NoWarn);
	if(!Sound)
	{
		return nullptr;
	}

	const FString LoopCuePackageName = GStalkerEngineManager->GetGamePath() / TEXT("Sounds") / Name + TEXT("_LoopCue");
	const FString LoopCueObjectName = LoopCuePackageName + TEXT(".") + FPaths::GetBaseFilename(LoopCuePackageName);
	USoundBase*LoopCueSound = LoadObject<USoundBase>(nullptr, *LoopCueObjectName, nullptr, LOAD_NoWarn);
	
	UStalkerSoundSource*StalkerSoundSource = NewObject<UStalkerSoundSource>(this);
	SoundSources.Add(StalkerSoundSource);
	StalkerSoundSource->RBMKName = InName;
	StalkerSoundSource->Sound = Sound;
	StalkerSoundSource->LoopingSound = LoopCueSound;
	if(const UStalkerSoundWaveAssetUserData*StalkerSoundWaveAssetUserData = Sound->GetAssetUserData<UStalkerSoundWaveAssetUserData>())
	{
		if(static_cast<s32>(EStalkerSoundTypes::FromSource) == CustomFlags)
		{
			StalkerSoundSource->Flags = StalkerSoundWaveAssetUserData->Flags;
		}
		else
		{
			StalkerSoundSource->Flags = CustomFlags;
		}
		StalkerSoundSource->AIRatioToDistance = StalkerSoundWaveAssetUserData->AIRatioToDistance;
	}
	return StalkerSoundSource;
}

void UStalkerSoundManager::Destroy(IRBMKSoundSource* SoundSource)
{
	if(UStalkerSoundSource*StalkerSoundSource = static_cast<UStalkerSoundSource*>(SoundSource))
	{
		if(--StalkerSoundSource->ReferenceCounter == 0)
		{
			SoundSources.Remove(StalkerSoundSource);
			StalkerSoundSource->MarkAsGarbage();
		}
	}

}

IRBMKSoundSource* UStalkerSoundManager::AddReference(IRBMKSoundSource* SoundSource)
{
	if(UStalkerSoundSource*StalkerSoundSource = static_cast<UStalkerSoundSource*>(SoundSource))
	{
		StalkerSoundSource->ReferenceCounter++;
	}
	return SoundSource;
}

IRBMKSoundSource* UStalkerSoundManager::Duplicate(IRBMKSoundSource* SoundSource)
{
	return Duplicate(SoundSource,static_cast<s32>(EStalkerSoundTypes::FromSource));
}

IRBMKSoundSource* UStalkerSoundManager::Duplicate(IRBMKSoundSource* InSoundSource, s32 CustomFlags)
{
	if(InSoundSource == nullptr)
	{
		return nullptr;
	}
	UStalkerSoundSource*SoundSource = static_cast<UStalkerSoundSource*>(InSoundSource);
	UStalkerSoundSource*NewSoundSource = NewObject<UStalkerSoundSource>(this);
	SoundSources.Add(NewSoundSource);
	NewSoundSource->RBMKName = SoundSource->RBMKName;
	NewSoundSource->Sound = SoundSource->Sound;
	NewSoundSource->LoopingSound = SoundSource->LoopingSound;
	if(static_cast<s32>(EStalkerSoundTypes::FromSource) == CustomFlags)
	{
		NewSoundSource->Flags = SoundSource->Flags;
	}
	else
	{
		NewSoundSource->Flags = CustomFlags;
	}
	NewSoundSource->AIRatioToDistance = SoundSource->AIRatioToDistance;
	return NewSoundSource;
}

void UStalkerSoundManager::Destroyed(IRBMKSoundActorListener* SoundActorListener)
{
	//SoundActorListeners.Remove(SoundActorListener);
}

void UStalkerSoundManager::Destroyed(CObject* Object)
{
	if(UStalkerSoundSource**Value = RBMKObject2SoundSource.Find(Object))
	{
		RBMKObject2SoundSource.Remove(Object);
		(*Value)->RBMKSource = nullptr;
	}
	
}

void UStalkerSoundManager::SetShockEffectorVolume(float NewVolume)
{
	const UWorld* ThisWorld = GWorld;
	if (!ensure(ThisWorld && ThisWorld->IsGameWorld()))
	{
		return;
	}
	const UStalkerGameSettings*StalkerGameSettings = GetDefault<UStalkerGameSettings>();
	USoundMix* ShockEffectorMix = StalkerGameSettings->ShockEffectorMix.LoadSynchronous();
	if(!ensure(ShockEffectorMix))
	{
		return;
	}
	
	USoundClass* MasterSoundClass = StalkerGameSettings->MasterSoundClass.LoadSynchronous();
	if(!ensure(MasterSoundClass))
	{
		return;
	}
	UGameplayStatics::SetSoundMixClassOverride(ThisWorld,ShockEffectorMix,MasterSoundClass,NewVolume);
}

bool UStalkerSoundManager::ExistSoundWave(const char* InName) const
{
	const FString Name = FPaths::ChangeExtension( ANSI_TO_TCHAR(InName),TEXT("")).Replace(TEXT("\\"),TEXT("/"));
	return SoundsList.Contains(Name);
}

void UStalkerSoundManager::Tick(float DeltaTime)
{
	for(auto Iterator = RBMKObject2SoundSource.CreateIterator();Iterator;++Iterator)
	{
		if(!Iterator.Value()->IsPlaying())
		{
			Iterator.RemoveCurrent();
			continue;
		}
		if(Iterator.Value()->TimeToPropagade<=0)
		{
			Iterator.Value()->TimeToPropagade = FMath::RandRange(0.5f-0.3f,0.5f+0.3f);\
			if (Iterator.Key() && Iterator.Key()->getDestroy())
			{
				Iterator.RemoveCurrent(); continue;
			}
			float Range = FMath::Clamp(Iterator.Value()->GetMaxDistance()*Iterator.Value()->AIRatioToDistance,0.1,500.f);
	
			Fvector WorldPosition = Iterator.Value()->GetWorldPosition();
			if(Iterator.Value()->IsRelative())
			{
				WorldPosition = Device->vCameraPosition;
			}
			TempSpatial.clear();
			g_SpatialSpace->q_sphere	(TempSpatial,0,STYPE_REACTTOSOUND,WorldPosition,Range+0.1f);
			for(ISpatial*Spatial:TempSpatial)
			{
				if(IRBMKSoundActorListener*SoundActorListener = Spatial->CastToSoundActorListener())
				{
					CObject* CO = Spatial->dcast_CObject();
					if(!ensure(CO)) continue;
					if (CO->getDestroy()) continue;
					
					const float Distance			= WorldPosition.distance_to(Spatial->spatial.sphere.P);
					if (Distance>Range) continue;
					
					const float Power = (1.f-Distance/Range)*Iterator.Value()->GetVolume();
					if (!FMath::IsNearlyZero(Power))
					{
						SoundActorListener->ListenSound(Iterator.Value()->RBMKSource,Iterator.Value()->Flags,WorldPosition,Power,Iterator.Value()->UserData);
						// FTraceDelegate TraceDelegate;
						// TraceDelegate.BindWeakLambda(Iterator.Value(),[Power,SoundActorListener,Emitter=Iterator.Value(),WorldPosition,CO](const FTraceHandle& TraceHandle, FTraceDatum& TraceResults)
						// {
						// 	if(TraceResults.OutHits.Num() > 0)
						// 	{
						// 		return;
						// 	}
						// 	
						// 	UStalkerSoundManager* StalkerSoundManager = GStalkerEngineManager->GetResourcesManager()->GetSoundManager();
						// 	if(Emitter->RBMKSource&&!Emitter->RBMKSource->getDestroy()&&
						// 		!CO->getDestroy()&&
						// 		StalkerSoundManager->SoundActorListeners.Contains(SoundActorListener))
						// 	{
						// 		SoundActorListener->ListenSound(Emitter->RBMKSource,Emitter->Flags,WorldPosition,Power,Emitter->UserData);
						// 	}
						// });
						//SoundActorListeners.Add(SoundActorListener);
						//GWorld->AsyncLineTraceByChannel(EAsyncTraceType::Test,,,ECC_WorldStatic,TraceDelegate);
					}
				}
			}
			
		}
		Iterator.Value()->TimeToPropagade-=DeltaTime;
		
	}
	for(auto Iterator = SoundSourcesWithFeedback.CreateIterator();Iterator;++Iterator)
	{
		if(!Iterator.ElementIt->Value->IsPlaying())
		{
			Iterator.ElementIt->Value->MarkAsGarbage();
			Iterator.RemoveCurrent();
		}
	}
}

void UStalkerSoundManager::Build()
{
	const FString PackageName = GStalkerEngineManager->GetResourcesManager()->GetGamePath() / TEXT("Sounds");
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AssetData;
	AssetRegistryModule.Get().GetAssetsByPath(FName(*PackageName), AssetData, true);
	for (FAssetData& Data : AssetData)
	{
		if(Data.IsInstanceOf(USoundBase::StaticClass()))
		{
			FString Name = Data.PackageName.ToString().ToLower();
			verifySlow(Name.RemoveFromStart(PackageName/TEXT("")));
			SoundsList.Add(Name);
		}
	}
}
