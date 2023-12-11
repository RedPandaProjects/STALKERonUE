#include "StalkerSpawnObject.h"
#include "../../../StalkerEditorManager.h"
#include "../../../Managers/SEFactory/StalkerSEFactoryManager.h"
#include "Entities/Kinematics/StalkerKinematicsComponent.h"
#include "Kernel/StalkerEngineManager.h"
#include "Resources/StalkerResourcesManager.h"

THIRD_PARTY_INCLUDES_START
#include "XrEngine/LightAnimLibrary.h"
#include "xrServerEntities/xrMessages.h"
THIRD_PARTY_INCLUDES_END

#include "Components/StalkerSpawnObjectSphereShapeComponent.h"
#include "Components/StalkerSpawnObjectGraphPointComponent.h"

#include "Properties/StalkerSpawnProperties_ALifeObject.h"
#include "Properties/StalkerSpawnProperties_Visual.h"
#include "Properties/StalkerSpawnProperties_ALifeSpaceRestrictor.h"
#include "Properties/StalkerSpawnProperties_ALifeGraphPoint.h"
#include "Properties/StalkerSpawnProperties_ALifeObjectHangingLamp.h"
#include "Properties/StalkerSpawnProperties_ALifeInventoryItem.h"
#include "Properties/StalkerSpawnProperties_ALifeItem.h"
#include "Properties/StalkerSpawnProperties_ALifeDynamicObjectVisual.h"
#include "Properties/StalkerSpawnProperties_ALifeCreatureAbstract.h"
#include "Properties/StalkerSpawnProperties_SmartCover.h"
#include "Components/StalkerSpawnObjectBoxShapeComponent.h"
#include "Kernel/Unreal/WorldSettings/StalkerWorldSettings.h"
#include "Resources/Spawn/StalkerLevelSpawn.h"
#include "UObject/ConstructorHelpers.h"

TCustomShowFlag<> StalkerShowSpawnShape(TEXT("StalkerShowSpawnShape"), true /*DefaultEnabled*/, SFG_Normal, FText::FromString(TEXT("Spawn shape")));

AStalkerSpawnObject::AStalkerSpawnObject(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	LightAnim = nullptr;
	XRayEntity = nullptr;
	bIsEditorOnlyActor = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(SceneComponent);
		
	SpawnBillboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	SpawnBillboard->SetupAttachment(SceneComponent);
	SpawnBillboard->SetWorldScale3D(FVector(2,2,2));		
	SpawnBillboard->Sprite = LoadObject<UTexture2D>(this, TEXT("/Game/Editor/Textures/ed_actor.ed_actor"));

	SpawnBillboardBad = CreateDefaultSubobject<UBillboardComponent>(TEXT("BillboardBad"));
	SpawnBillboardBad->SetupAttachment(SceneComponent);
	SpawnBillboardBad->SetWorldScale3D(FVector(2,2,2));		
	SpawnBillboardBad->Sprite = LoadObject<UTexture2D>(this, TEXT("/Game/Editor/Textures/ed_actor_bad.ed_actor_bad"));
	SpawnBillboardBad->SetVisibility(false);

	checkSlow(SpawnBillboard->Sprite);
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.SetTickFunctionEnable(true);
	PrimaryActorTick.TickInterval = 0.3f;

	ArrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	if (ArrowComponent)
	{
		static FName ID_Navigation = "Navigation";
		ArrowComponent->ArrowColor = FColor(150, 200, 255);
		ArrowComponent->ArrowSize = 1.0f;
		ArrowComponent->bTreatAsASprite = true;
		ArrowComponent->SpriteInfo.Category = ID_Navigation;
		ArrowComponent->SpriteInfo.DisplayName = NSLOCTEXT("SpriteCategory", "Navigation", "Navigation");
		ArrowComponent->SetupAttachment(SceneComponent);
		ArrowComponent->bIsScreenSizeScaled = true;
		ArrowComponent->SetVisibility(false);
	}
	GameType = EStalkerGame::Unkown;
}

void AStalkerSpawnObject::PostActorCreated()
{
	Super::PostActorCreated();
	SpawnRead();
	if(EntityData.Num() == 0&&XRayEntity&& XRayEntity->CastALifeTraderAbstract())
	{
		XRayEntity->CastALifeTraderAbstract()->OnChangeProfile();
	}
}

void AStalkerSpawnObject::Attach(class USceneComponent* InComponent)
{
	check(InComponent);
	FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::KeepRelative, false);
	InComponent->Rename(nullptr, this);
	InComponent->AttachToComponent(GetRootComponent(), AttachmentTransformRules);
	InComponent->RegisterComponent();
}

void AStalkerSpawnObject::Detach(class USceneComponent* InComponent)
{
	check(InComponent);
	FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepRelative, false);
	InComponent->DetachFromComponent(DetachmentTransformRules);
	InComponent->UnregisterComponent();
	InComponent->DestroyComponent();
}

bool AStalkerSpawnObject::ShouldTickIfViewportsOnly() const
{
	return true;
}

void AStalkerSpawnObject::Tick(float DeltaSeconds)
{
#if WITH_EDITOR
	if (GetWorld() != nullptr && GetWorld()->WorldType == EWorldType::Editor)
	{
		bool Selected = IsSelected();
		SpawnBillboardBad->SetVisibility(XRayEntity == nullptr);
		SpawnBillboard->SetVisibility(XRayEntity != nullptr);
		if (XRayEntity&&XRayEntity->visual())
		{
			if (XRayEntity->m_editor_flags.is(ISE_Abstract::flVisualChange))
			{
				if (!MainVisual)
				{
					MainVisual = NewObject< UStalkerKinematicsComponent>(this, NAME_None);
					MainVisual->SetFlags(RF_TextExportTransient);
					Attach(MainVisual);
				}
				if (XRayEntity->visual()->visual_name.size())
				{
					MainVisual->Initilize(GStalkerEngineManager->GetResourcesManager()->GetKinematics(XRayEntity->visual()->visual_name.c_str()));
					XRayEntity->set_editor_flag(ISE_Abstract::flVisualAnimationChange);
				}
				else
				{
					MainVisual->Initilize(nullptr);
				}
				if (XRayEntity->GetPropertiesType() == EXRaySpawnPropertiesType::CSE_ALifeObjectHangingLamp)
				{
					XRayEntity->set_editor_flag(ISE_Abstract::flLightChange);
				}
				ArrowComponent->SetVisibility(true);
				ArrowComponent->SetRelativeLocationAndRotation(MainVisual->GetLocalBounds().GetSphere().Center,FRotator(0,90,0));
			}

		}
		else if (MainVisual)
		{
			if (XRayEntity && XRayEntity->GetPropertiesType() == EXRaySpawnPropertiesType::CSE_ALifeObjectHangingLamp)
			{
				XRayEntity->set_editor_flag(ISE_Abstract::flLightChange);
			}
			Detach(MainVisual);
			MainVisual->MarkAsGarbage();
			MainVisual = nullptr;
			ArrowComponent->SetVisibility(false);
		}
		if (XRayEntity && XRayEntity->GetPropertiesType() == EXRaySpawnPropertiesType::CSE_SmartCover)
		{
			if (XRayEntity->m_editor_flags.is(ISE_Abstract::flVisualChange))
			{
				if (XRayEntity->visual_collection_size() != Visuals.Num())
				{
					for (u32 i = static_cast<u32>(Visuals.Num()); i < XRayEntity->visual_collection_size(); i++)
					{
						UStalkerKinematicsComponent* Visual = NewObject< UStalkerKinematicsComponent>(this, NAME_None);
						Visual->SetFlags(RF_TextExportTransient);
						Attach(Visual);
						Visuals.Add(Visual);
					}
					for (u32 i = XRayEntity->visual_collection_size(); i < static_cast<u32>(Visuals.Num());)
					{
						Detach(Visuals[i]);
						Visuals[i]->MarkAsGarbage();
						Visuals.RemoveAt(i);
					}

					for (u32 i = 0; i < XRayEntity->visual_collection_size(); i++)
					{
						Fmatrix		Matrix = XRayEntity->visual_collection()[i].matrix;
						UStalkerKinematicsComponent* Visual = Visuals[i];
						Visual->InitilizeEditor();
						Visual->SetRelativeLocation(FVector(StalkerMath::RBMKLocationToUnreal(Matrix.c)));
						Visual->SetRelativeRotation(FQuat(StalkerMath::RBMKQuaternionToUnreal(Matrix)));
					}

				}

			}
			for (u32 i = 0; i < XRayEntity->visual_collection_size(); i++)
			{
				UStalkerKinematicsComponent* Visual = Visuals[i];
				ISE_Visual* IVisual = XRayEntity->visual_collection()[i].visual;
				if ((!!Visual->Kinematics) != Selected)
				{
					if (Selected)
					{
						check(IVisual);
						Visual->Initilize(GStalkerEngineManager->GetResourcesManager()->GetKinematics(IVisual->visual_name.c_str()));
						MotionID M = Visual->ID_Cycle_Safe(IVisual->startup_animation.c_str());
						if (M.valid())
						{
							Visual->EditorPlay(M, false);
						}
					}
					else
					{
						Visual->Initilize(nullptr);
					}
				}
			}

		}
		else if (Visuals.Num())
		{
			for (UStalkerKinematicsComponent* Visual : Visuals)
			{
				Detach(Visual);
				Visual->MarkAsGarbage();
			}
			Visuals.Empty();
		}
		if (XRayEntity && XRayEntity->m_editor_flags.is(ISE_Abstract::flVisualAnimationChange) && MainVisual)
		{
			if (XRayEntity->visual()->startup_animation.size() && XRayEntity->visual()->startup_animation != "$editor")
			{
				MotionID M = MainVisual->ID_Cycle_Safe(XRayEntity->visual()->startup_animation.c_str());
				if (M.valid())
				{
					MainVisual->EditorPlay(M);
				}
			}
			else
			{
				for (u32 i = 0; i < 4; i++)
				{
					MainVisual->LL_CloseCycle(i,0xFF);
				}
				
			}
		}
		if (XRayEntity && XRayEntity->GetPropertiesType() == EXRaySpawnPropertiesType::CSE_ALifeObjectHangingLamp)
		{

			if (XRayEntity->m_editor_flags.is(ISE_Abstract::flLightChange))
			{
				ISE_ALifeObjectHangingLamp* ALifeObjectHangingLamp = reinterpret_cast<ISE_ALifeObjectHangingLamp*>(XRayEntity->QueryPropertiesInterface(EXRaySpawnPropertiesType::CSE_ALifeObjectHangingLamp));

				LightAnim = LALib->FindItem(*ALifeObjectHangingLamp->color_animator);;
				if (ALifeObjectHangingLamp->light_flags.is(ISE_ALifeObjectHangingLamp::flTypeSpot))
				{

					if (!SpotLight)
					{
						SpotLight = NewObject<USpotLightComponent>(this, NAME_None, RF_TextExportTransient);
						Attach(SpotLight);
					}
					if (PointLight)
					{
						Detach(PointLight);
						PointLight->MarkAsGarbage();
						PointLight = nullptr;
					}
				}
				else
				{
					if (!PointLight)
					{
						PointLight = NewObject<UPointLightComponent>(this, NAME_None, RF_TextExportTransient);
						Attach(PointLight);
					}
					if (SpotLight)
					{
						Detach(SpotLight);
						SpotLight->MarkAsGarbage();
						SpotLight = nullptr;
					}
				}
				if (SpotLight)
				{
					if (ALifeObjectHangingLamp->light_main_bone.size() && MainVisual)
					{
						FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::KeepRelative, false);
						SpotLight->AttachToComponent(MainVisual, AttachmentTransformRules, ALifeObjectHangingLamp->light_main_bone.size() ? FName(ALifeObjectHangingLamp->light_main_bone.c_str()) : NAME_None);
					}
					else
					{
						FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::KeepRelative, false);
						SpotLight->AttachToComponent(GetRootComponent(), AttachmentTransformRules);
					}

					SpotLight->SetIntensityUnits(ELightUnits::Candelas);
					SpotLight->SetLightBrightness(ALifeObjectHangingLamp->brightness * 100.f * 100.f);
					SpotLight->SetAttenuationRadius(ALifeObjectHangingLamp->range * 100);
					SpotLight->SetInnerConeAngle(rad2deg(ALifeObjectHangingLamp->spot_cone_angle) * 0.125f);
					SpotLight->SetOuterConeAngle(rad2deg(ALifeObjectHangingLamp->spot_cone_angle) * 0.5f);
					SpotLight->SetLightFColor(FColor(ALifeObjectHangingLamp->color));
					SpotLight->SetCastShadows(ALifeObjectHangingLamp->light_flags.is(ISE_ALifeObjectHangingLamp::flCastShadow));
					SpotLight->SetActive(true);
					SpotLight->SetRelativeRotation(FRotator(0, 90, 0));
				}

				if (PointLight)
				{
					Attach(PointLight);
					if (ALifeObjectHangingLamp->light_main_bone.size() && MainVisual)
					{
						FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::KeepRelative, false);
						PointLight->AttachToComponent(MainVisual, AttachmentTransformRules, ALifeObjectHangingLamp->light_main_bone.size() ? FName(ALifeObjectHangingLamp->light_main_bone.c_str()) : NAME_None);
					}
					else
					{
						FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::KeepRelative, false);
						PointLight->AttachToComponent(GetRootComponent(), AttachmentTransformRules);
					}
					PointLight->SetIntensityUnits(ELightUnits::Candelas);
					PointLight->SetAttenuationRadius(ALifeObjectHangingLamp->range * 100);
					PointLight->SetIntensity(ALifeObjectHangingLamp->brightness);
					PointLight->SetLightFColor(FColor(ALifeObjectHangingLamp->color));
					PointLight->SetCastShadows(ALifeObjectHangingLamp->light_flags.is(ISE_ALifeObjectHangingLamp::flCastShadow));
					PointLight->SetActive(true);
				}
			}
		}
		else
		{
			if (PointLight)
			{
				Detach(PointLight);
				PointLight->MarkAsGarbage();
				PointLight = nullptr;
			}
			if (SpotLight)
			{
				Detach(SpotLight);
				SpotLight->MarkAsGarbage();
				SpotLight = nullptr;
			}
		}
		if (XRayEntity && XRayEntity->m_editor_flags.is(ISE_Abstract::flUpdateProperties))
		{
			if (SpawnData)
			{
				SpawnData->FillProperties();
			}
			if (VisualData)
			{
				VisualData->FillProperties();
			}
			if (InventoryItemData)
			{
				InventoryItemData->FillProperties();
			}
			if (NCPData)
			{
				NCPData->FillProperties();
			}

		}
		if (XRayEntity)
		{
			XRayEntity->m_editor_flags.zero();
		}
		if (LightAnim)
		{
			int Frame;
			u32 NewColor = LightAnim->CalculateRGB(GetWorld()->UnpausedTimeSeconds, Frame);
			if (PointLight)PointLight->SetLightFColor(FColor(NewColor));
			if (SpotLight)SpotLight->SetLightFColor(FColor(NewColor));
		}
	}
	else
#endif
	{
		Super::Tick(DeltaSeconds);
	}

}

void AStalkerSpawnObject::DestroyEntity()
{
	LightAnim = nullptr;
	if (XRayEntity)
	{
		GStalkerEditorManager->SEFactoryManager->DestroyEntity(XRayEntity);
		XRayEntity = nullptr;
	}
	if (SpawnData)
	{
		SpawnData->MarkAsGarbage();
		SpawnData = nullptr;
	}
	if (VisualData)
	{
		VisualData->MarkAsGarbage();
		VisualData = nullptr;
	}
	if (InventoryItemData)
	{
		InventoryItemData->MarkAsGarbage();
		InventoryItemData = nullptr;
	}
	if (NCPData)
	{
		NCPData->MarkAsGarbage();
		NCPData = nullptr;
	}
}
void AStalkerSpawnObject::CreateEntity()
{
	DestroyEntity();
	if (!IsValid(GetWorld()))
	{
		return;
	}
	if (SectionName.Len() == 0)
	{
		return;
	}
	if (GetWorld()->IsGameWorld())
	{
		return;
	}
	if (GStalkerEditorManager->SEFactoryManager->IsVoid())
	{
		return;
	}
	if(GameType == EStalkerGame::Unkown)
	{
		GameType = GStalkerEngineManager->GetCurrentGame();
	}
	if(GameType != GStalkerEngineManager->GetCurrentGame())
	{
		return;
	}
	XRayEntity = GStalkerEditorManager->SEFactoryManager->CreateEntity(TCHAR_TO_ANSI(*SectionName));
	if (XRayEntity)
	{
		XRayEntity->set_name(TCHAR_TO_ANSI(*SectionName));
		if (XRayEntity->visual())
		{
			XRayEntity->set_editor_flag(ISE_Abstract::flVisualChange | ISE_Abstract::flVisualAnimationChange);
		}
		if (XRayEntity->CastALifeGraphPoint())
		{
			if (GraphPointComponent == nullptr)
			{
				GraphPointComponent = NewObject<UStalkerSpawnObjectGraphPointComponent>(this, NAME_None, RF_TextExportTransient);
				Attach(GraphPointComponent);
			}
		}
		else if(GraphPointComponent)
		{
			Detach(GraphPointComponent);
			GraphPointComponent = nullptr;
		}
		if (XRayEntity->QueryPropertiesInterface(EXRaySpawnPropertiesType::CSE_ALifeObjectHangingLamp))
		{
			XRayEntity->set_editor_flag(ISE_Abstract::flLightChange);
		}
		if (pSettings->line_exist(XRayEntity->name(), "$player") && EntityData.Num() == 0)
		{
			if (pSettings->r_bool(XRayEntity->name(), "$player"))
			{
				XRayEntity->flags().set(M_SPAWN_OBJECT_ASPLAYER, TRUE);
			}
		}
		if (pSettings->line_exist(XRayEntity->name(), "$def_sphere") && EntityData.Num() == 0)
		{
			if (!IsValid(FindComponentByClass(UStalkerSpawnObjectSphereShapeComponent::StaticClass())) && !IsValid(FindComponentByClass(UStalkerSpawnObjectBoxShapeComponent::StaticClass())))
			{
				UStalkerSpawnObjectSphereShapeComponent* NewShapeComponent = NewObject< UStalkerSpawnObjectSphereShapeComponent>(this, NAME_None, RF_Transactional | RF_TextExportTransient);
				AddInstanceComponent(NewShapeComponent);
				NewShapeComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				NewShapeComponent->OnComponentCreated();
				NewShapeComponent->RegisterComponent();
				NewShapeComponent->SphereRadius = pSettings->r_float(XRayEntity->name(), "$def_sphere") * 100;
				NewShapeComponent->UpdateBounds();
				NewShapeComponent->MarkRenderStateDirty();
			}
		}
	}
	TInlineComponentArray<UStalkerSpawnObjectSphereShapeComponent*> SphereShapeComponents;
	GetComponents(SphereShapeComponents);
	for (UStalkerSpawnObjectSphereShapeComponent* SphereShapeComponent : SphereShapeComponents)
	{
		SphereShapeComponent->UpdateColor();
	}
}

void AStalkerSpawnObject::SpawnWrite()
{
	if (XRayEntity)
	{
		EntityData.Reset();
		NET_Packet 			Packet;
		XRayEntity->Spawn_Write(Packet, TRUE);
		for (u32 i = 0; i < Packet.B.count; i++)
		{
			EntityData.Add(Packet.B.data[i]);
		}
	}
}

void AStalkerSpawnObject::SpawnRead()
{
	if (!IsValid(GetWorld()))
	{
		return;
	}
	CreateEntity();
	if (XRayEntity)
	{
		NET_Packet 			Packet;
		Packet.B.count = EntityData.Num();
		for (u32 i = 0; i < Packet.B.count; i++)
		{
			Packet.B.data[i] = EntityData[i];
		}
		if (EntityData.Num() && !XRayEntity->Spawn_Read(Packet))
		{
			DestroyEntity();
			EntityData.Reset();
		}
		else if (XRayEntity->QueryPropertiesInterface(EXRaySpawnPropertiesType::CSE_SmartCover))
		{
			reinterpret_cast<ISE_SmartCover*>(XRayEntity->QueryPropertiesInterface(EXRaySpawnPropertiesType::CSE_SmartCover))->OnChangeLoopholes();
		}
	}
	CreateSpawnData();
}

void AStalkerSpawnObject::Destroyed()
{
	Super::Destroyed();
	if (!IsValid(GetWorld()) || GetWorld()->IsGameWorld())
	{
		return;
	}
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(GetWorld()->GetWorldSettings());
	if (IsValid(StalkerWorldSettings))
	{
		UStalkerLevelSpawn* Spawn = StalkerWorldSettings->GetSpawn();
		if (IsValid(Spawn))
		{
			StalkerWorldSettings->Modify();
			StalkerWorldSettings->NeedRebuildSpawn = true;
			Spawn->NeedRebuild = true;
			Spawn->Modify();
		}
	}
}

bool AStalkerSpawnObject::Modify(bool bAlwaysMarkDirty /*= true*/)
{
	bool bResult = Super::Modify(bAlwaysMarkDirty);
	if (!IsValid(GetWorld()) || GetWorld()->IsGameWorld()||!bAlwaysMarkDirty)
	{
		return bResult;
	}
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(GetWorld()->GetWorldSettings());
	if (IsValid(StalkerWorldSettings))
	{
		UStalkerLevelSpawn* Spawn = StalkerWorldSettings->GetSpawn();
		if (IsValid(Spawn))
		{
			StalkerWorldSettings->Modify();
			StalkerWorldSettings->NeedRebuildSpawn = true;
			Spawn->NeedRebuild = true;
			Spawn->Modify();
		}
	}
	return bResult;
}

void AStalkerSpawnObject::PostEditUndo()
{
	Super::PostEditUndo();
	if (!IsValid(GetWorld()))
	{
		return;
	}
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(GetWorld()->GetWorldSettings());
	if (IsValid(StalkerWorldSettings))
	{
		UStalkerLevelSpawn* Spawn = StalkerWorldSettings->GetSpawn();
		if (IsValid(Spawn))
		{
			StalkerWorldSettings->Modify();
			StalkerWorldSettings->NeedRebuildSpawn = true;
			Spawn->NeedRebuild = true;
			Spawn->Modify();
		}
	}
}

void AStalkerSpawnObject::PostEditImport()
{
	Super::PostEditImport();
	SpawnRead();
}

FString AStalkerSpawnObject::GetDefaultActorLabel() const
{
	if (SectionName.IsEmpty())
	{
		return Super::GetDefaultActorLabel();
	}
	return SectionName;
}

void AStalkerSpawnObject::BeginDestroy()
{
	
	DestroyEntity();
	Super::BeginDestroy();
}

void AStalkerSpawnObject::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	if (Ar.IsLoading() || Ar.IsSaving())
	{
		if (Ar.IsSaving())
		{
			SpawnWrite();
		}
		Ar<< EntityData;
		if (Ar.IsLoading()&&!Ar.IsCooking())
		{
			SpawnRead();
		}
	}
}


void AStalkerSpawnObject::CreateSpawnData()
{
	CreateVisualData();
	CreateInventoryItemData();
	CreateNCPData();
	if (SpawnData)
	{
		SpawnData->MarkAsGarbage();
		SpawnData = nullptr;
	}

	if (!XRayEntity)
	{
		return;
	}

	EXRaySpawnPropertiesType PropertiesType = XRayEntity->GetPropertiesType();
	switch (PropertiesType)
	{
	case EXRaySpawnPropertiesType::None:
		SpawnData = nullptr;
		break;
	case EXRaySpawnPropertiesType::ALifeObject:
		SpawnData = NewObject<UStalkerSpawnProperties_ALifeObject>(this,NAME_None, RF_Transactional | RF_TextExportTransient);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeSpaceRestrictor:
		SpawnData = NewObject<UStalkerSpawnProperties_ALifeSpaceRestrictor>(this, NAME_None, RF_Transactional | RF_TextExportTransient);
		break;
	case EXRaySpawnPropertiesType::ALifeGraphPoint:
		SpawnData = NewObject<UStalkerSpawnProperties_ALifeGraphPoint>(this, NAME_None, RF_Transactional | RF_TextExportTransient);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeObjectHangingLamp:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeObjectHangingLamp>(this, NAME_None, RF_Transactional | RF_TextExportTransient);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeItemWeapon:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeItemWeapon>(this, NAME_None, RF_Transactional | RF_TextExportTransient);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeItemArtefact:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeItemArtefact>(this, NAME_None, RF_Transactional | RF_TextExportTransient);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeItemDocument:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeItemDocument>(this, NAME_None, RF_Transactional | RF_TextExportTransient);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeItemAmmo:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeItemAmmo>(this, NAME_None, RF_Transactional | RF_TextExportTransient);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeObjectBreakable:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeObjectBreakable>(this, NAME_None, RF_Transactional | RF_TextExportTransient);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeCar:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeCar>(this, NAME_None, RF_Transactional | RF_TextExportTransient);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeHelicopter:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeHelicopter>(this, NAME_None, RF_Transactional | RF_TextExportTransient);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeObjectPhysic:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeObjectPhysic>(this, NAME_None, RF_Transactional | RF_TextExportTransient);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeZoneVisual:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeZoneVisual>(this, NAME_None, RF_Transactional | RF_TextExportTransient);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeAnomalousZone:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeAnomalousZone>(this, NAME_None, RF_Transactional | RF_TextExportTransient);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeCustomZone:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeCustomZone>(this, NAME_None, RF_Transactional | RF_TextExportTransient);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeLevelChanger:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeLevelChanger>(this, NAME_None, RF_Transactional | RF_TextExportTransient);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeMonsterZombie:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeMonsterZombie>(this, NAME_None, RF_Transactional | RF_TextExportTransient);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeHumanAbstract:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeHumanAbstract>(this, NAME_None, RF_Transactional | RF_TextExportTransient);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeMonsterRat:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeMonsterRat>(this, NAME_None, RF_Transactional | RF_TextExportTransient);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeMonsterAbstract:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeMonsterAbstract>(this, NAME_None, RF_Transactional | RF_TextExportTransient);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeCreatureAbstract:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeCreatureAbstract>(this, NAME_None, RF_Transactional | RF_TextExportTransient);
		break;
	case EXRaySpawnPropertiesType::CSE_SmartCover:
		SpawnData = NewObject <UStalkerSpawnProperties_SmartCover>(this, NAME_None, RF_Transactional| RF_TextExportTransient);
		break;
	default:
		break;
	}

	if (SpawnData)
	{
		SpawnData->SetEntity(XRayEntity);
	}

}

void AStalkerSpawnObject::CreateVisualData()
{
	if (VisualData)
	{
		VisualData->MarkAsGarbage();
		VisualData = nullptr;
	}
	if (!XRayEntity)
	{
		return;
	}
	if (!XRayEntity->visual())
	{
		return;
	}

	VisualData = NewObject<UStalkerSpawnProperties_Visual>(this, NAME_None, RF_Transactional | RF_TextExportTransient);
	VisualData->SetEntity(XRayEntity);
}

void AStalkerSpawnObject::CreateInventoryItemData()
{
	if (InventoryItemData)
	{
		InventoryItemData->MarkAsGarbage();
		InventoryItemData = nullptr;
	}
	if (!XRayEntity)
	{
		return;
	}
	if (!XRayEntity->CastALifeInventoryItem())
	{
		return;
	}

	InventoryItemData = NewObject<UStalkerSpawnProperties_ALifeInventoryItem>(this, NAME_None, RF_Transactional | RF_TextExportTransient);
	InventoryItemData->SetEntity(XRayEntity);
}

void AStalkerSpawnObject::CreateNCPData()
{
	if (NCPData)
	{
		NCPData->MarkAsGarbage();
		NCPData = nullptr;
	}
	if (!XRayEntity)
	{
		return;
	}
	if (!XRayEntity->CastALifeTraderAbstract())
	{
		return;
	}

	NCPData = NewObject<UStalkerSpawnProperties_ALifeTraderAbstract>(this, NAME_None, RF_Transactional | RF_TextExportTransient);
	NCPData->SetEntity(XRayEntity);
}

void AStalkerSpawnObject::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(GetWorld())||GetWorld()->IsGameWorld())
	{
		Destroy();
	}
}
