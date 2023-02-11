#include "StalkerSpawnObject.h"
#include "../../../StalkerEditorManager.h"
#include "../../../Managers/SEFactory/StalkerSEFactoryManager.h"
#include "Entities/Kinematics/StalkerKinematics.h"
#include "Kernel/StalkerEngineManager.h"
#include "Resources/StalkerResourcesManager.h"

THIRD_PARTY_INCLUDES_START
#include "XrEngine/LightAnimLibrary.h"
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

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.SetTickFunctionEnable(true);
	PrimaryActorTick.TickInterval = 0.2f;
}

void AStalkerSpawnObject::PostActorCreated()
{
	Super::PostActorCreated();
	CreateEntity();
	CreateSpawnData();
}

void AStalkerSpawnObject::Attach(class USceneComponent* InComponent)
{
	check(InComponent);
	checkSlow(XRayComponents.Find(InComponent) == nullptr);
	XRayComponents.FindOrAdd(InComponent);
	FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::KeepRelative, false);
	InComponent->Rename(nullptr, this);

	InComponent->AttachToComponent(GetRootComponent(), AttachmentTransformRules);
	InComponent->RegisterComponent();
}

void AStalkerSpawnObject::Detach(class USceneComponent* InComponent)
{
	check(InComponent);
	checkSlow(XRayComponents.Find(InComponent));
	XRayComponents.Remove(InComponent);
	FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepRelative, false);
	InComponent->DetachFromComponent(DetachmentTransformRules);
	InComponent->UnregisterComponent();
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
		if (!XRayEntity)
		{
			return;
		}
		bool Selected = false;
		if (XRayEntity->GetPropertiesType() == EXRaySpawnPropertiesType::CSE_SmartCover)
		{
			Selected = IsSelected();
			if (!Selected && Visuals.Num())
			{
				XRayEntity->set_editor_flag(ISE_Abstract::flVisualChange);
			}
		}
		if (XRayEntity->m_editor_flags.is(ISE_Abstract::flVisualChange))
		{
			if (MainVisual)
			{
				Detach(MainVisual->CastToStalkerKinematicsComponent());
				MainVisual->MarkAsGarbage();
			}
			if (Visuals.Num())
			{
				for (UStalkerKinematicsComponent* Visual: Visuals)
				{
					Detach(Visual->CastToStalkerKinematicsComponent());
					Visual->MarkAsGarbage();
				}
				Visuals.Empty();
			}
			if (XRayEntity->visual()&&XRayEntity->visual()->visual_name.size())
			{
				MainVisual = GXRayEngineManager->GetResourcesManager()->CreateKinematics(XRayEntity->visual()->visual_name.c_str());
				if(MainVisual)
				{
					Attach(MainVisual->CastToStalkerKinematicsComponent());
					XRayEntity->set_editor_flag(ISE_Abstract::flVisualAnimationChange);
				
				}
			}
			if (XRayEntity->GetPropertiesType() == EXRaySpawnPropertiesType::CSE_ALifeObjectHangingLamp)
			{
				XRayEntity->set_editor_flag(ISE_Abstract::flLightChange);
			}
		}
		if (XRayEntity->GetPropertiesType() == EXRaySpawnPropertiesType::CSE_SmartCover)
		{
			if (Selected && !Visuals.Num())
			{
				for (u32 i = 0; i < XRayEntity->visual_collection_size(); i++)
				{
					Fmatrix		Matrix =   XRayEntity->visual_collection()[i].matrix;
					ISE_Visual*	IVisual =   XRayEntity->visual_collection()[i].visual;
					if (IVisual)
					{
						UStalkerKinematicsComponent* Visual = GXRayEngineManager->GetResourcesManager()->CreateKinematics(IVisual->visual_name.c_str());
						if (Visual)
						{
							Attach(Visual->CastToStalkerKinematicsComponent());
							Visual->SetRelativeLocation(FVector(StalkerMath::XRayLocationToUnreal(Matrix.c)));
							Visual->SetRelativeRotation(FQuat(StalkerMath::XRayQuatToUnreal(Matrix)));
							MotionID M = Visual->ID_Cycle_Safe(IVisual->startup_animation.c_str());
							if (M.valid())
							{
								Visual->EditorPlay(M,false);
							}
							Visuals.Add(Visual);
						}
					}
				}
			}
		}
		if (XRayEntity->m_editor_flags.is(ISE_Abstract::flVisualAnimationChange) && MainVisual)
		{
			if (XRayEntity->visual()->startup_animation.size() && XRayEntity->visual()->startup_animation != "$editor")
			{
				MotionID M = MainVisual->ID_Cycle_Safe(XRayEntity->visual()->startup_animation.c_str());
				if (M.valid())
				{
					MainVisual->EditorPlay(M);
				}
			}
		}
		if (XRayEntity->m_editor_flags.is(ISE_Abstract::flLightChange) )
		{
			ISE_ALifeObjectHangingLamp* ALifeObjectHangingLamp = reinterpret_cast<ISE_ALifeObjectHangingLamp*>(XRayEntity->QueryPropertiesInterface(EXRaySpawnPropertiesType::CSE_ALifeObjectHangingLamp));

			LightAnim = LALib->FindItem(*ALifeObjectHangingLamp->color_animator);;
			if (ALifeObjectHangingLamp->light_flags.is(ISE_ALifeObjectHangingLamp::flTypeSpot))
			{
				if (PointLight)
				{
					Detach(PointLight);
					PointLight->MarkAsGarbage();
					PointLight = nullptr;
				}
				if (!SpotLight)
				{
					SpotLight = NewObject<USpotLightComponent>(this);
					Attach(SpotLight);
				}
			}
			else
			{
				if (SpotLight)
				{
					Detach(SpotLight);
					SpotLight->MarkAsGarbage();
					SpotLight = nullptr;
				}
				if (!PointLight)
				{
					PointLight = NewObject<UPointLightComponent>(this);
					Attach(PointLight);
				}
			}
			if (SpotLight)
			{
				if (ALifeObjectHangingLamp->light_main_bone.size()&& MainVisual)
				{
					FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::KeepRelative, false);
					SpotLight->AttachToComponent(MainVisual, AttachmentTransformRules, ALifeObjectHangingLamp->light_main_bone.size() ? FName(ALifeObjectHangingLamp->light_main_bone.c_str()) : NAME_None);
				}
				else
				{
					FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::KeepRelative, false);
					SpotLight->AttachToComponent(GetRootComponent(), AttachmentTransformRules);
				}
				
				SpotLight->SetIntensityUnits(ELightUnits::Lumens);
				SpotLight->SetIntensity(ALifeObjectHangingLamp->brightness);
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
				PointLight->SetIntensityUnits(ELightUnits::Lumens);
				PointLight->SetIntensity(ALifeObjectHangingLamp->brightness);
				PointLight->SetSourceRadius(ALifeObjectHangingLamp->range * 100);
				PointLight->SetLightFColor(FColor(ALifeObjectHangingLamp->color));
				PointLight->SetCastShadows(ALifeObjectHangingLamp->light_flags.is(ISE_ALifeObjectHangingLamp::flCastShadow));
				PointLight->SetActive(true);
				PointLight->SetRelativeRotation(FRotator(0, 90, 0));
			}

		}
		if (XRayEntity->m_editor_flags.is(ISE_Abstract::flUpdateProperties))
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
		XRayEntity->m_editor_flags.zero();
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
	MainVisual = nullptr;
	PointLight = nullptr;
	SpotLight = nullptr;
	if (XRayComponents.Num())
	{
		TSet<USceneComponent*>	TempXRayComponents = XRayComponents;
		for (USceneComponent* Component : TempXRayComponents)
		{
			if (Component)
			{
				Detach(Component);
			}
		}
		XRayComponents.Empty();
	}
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
enum
{
	M_SPAWN_OBJECT_ASPLAYER = (1 << 3),	// after spawn it must become viewable
};
void AStalkerSpawnObject::CreateEntity()
{
	if (XRayEntity)
	{
		DestroyEntity();
	}
	if (SectionName.Len() == 0)
	{
		return;
	}
	if (GStalkerEditorManager->SEFactoryManager->IsVoid())
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
			Attach(NewObject<UStalkerSpawnObjectGraphPointComponent>(this));
		}
		if (XRayEntity->QueryPropertiesInterface(EXRaySpawnPropertiesType::CSE_ALifeObjectHangingLamp))
		{
			XRayEntity->set_editor_flag(ISE_Abstract::flLightChange);
		}
		if (pSettings->line_exist(XRayEntity->name(), "$player"))
		{
			if (pSettings->r_bool(XRayEntity->name(), "$player"))
			{
				XRayEntity->flags().set(M_SPAWN_OBJECT_ASPLAYER, TRUE);
			}
		}
		if (pSettings->line_exist(XRayEntity->name(), "$def_sphere"))
		{
			UStalkerSpawnObjectSphereShapeComponent* NewShapeComponent = NewObject< UStalkerSpawnObjectSphereShapeComponent>(this, NAME_None, RF_Transactional);
			AddInstanceComponent(NewShapeComponent);
			NewShapeComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			NewShapeComponent->OnComponentCreated();
			NewShapeComponent->RegisterComponent();
			NewShapeComponent->SphereRadius = pSettings->r_float(XRayEntity->name(), "$def_sphere") * 100;
			NewShapeComponent->UpdateBounds();
			NewShapeComponent->MarkRenderStateDirty();
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
	NET_Packet 			Packet;
	Packet.B.count = EntityData.Num();
	for (u32 i = 0; i < Packet.B.count; i++)
	{
		Packet.B.data[i] = EntityData[i];
	}
	CreateEntity();
	if (XRayEntity)
	{
		if (EntityData.Num() && !XRayEntity->Spawn_Read(Packet))
		{
			DestroyEntity();
			EntityData.Reset();
		}
	}
	CreateSpawnData();
}

void AStalkerSpawnObject::PostInitProperties()
{
	Super::PostInitProperties();
	if (HasAnyFlags(RF_NeedLoad | RF_WasLoaded | RF_ClassDefaultObject) == false)
	{
		SpawnBillboard->Sprite = LoadObject<UTexture2D>(this, TEXT("/Game/Editor/Textures/ed_actor.ed_actor"));
	}
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
		Ar<<EntityData;
		if (Ar.IsLoading())
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
		SpawnData = NewObject<UStalkerSpawnProperties_ALifeObject>(this,NAME_None, RF_Transactional);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeSpaceRestrictor:
		SpawnData = NewObject<UStalkerSpawnProperties_ALifeSpaceRestrictor>(this, NAME_None, RF_Transactional);
		break;
	case EXRaySpawnPropertiesType::ALifeGraphPoint:
		SpawnData = NewObject<UStalkerSpawnProperties_ALifeGraphPoint>(this, NAME_None, RF_Transactional);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeObjectHangingLamp:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeObjectHangingLamp>(this, NAME_None, RF_Transactional);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeItemWeapon:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeItemWeapon>(this, NAME_None, RF_Transactional);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeItemArtefact:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeItemArtefact>(this, NAME_None, RF_Transactional);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeItemDocument:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeItemDocument>(this, NAME_None, RF_Transactional);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeItemAmmo:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeItemAmmo>(this, NAME_None, RF_Transactional);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeObjectBreakable:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeObjectBreakable>(this, NAME_None, RF_Transactional);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeCar:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeCar>(this, NAME_None, RF_Transactional);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeHelicopter:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeHelicopter>(this, NAME_None, RF_Transactional);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeObjectPhysic:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeObjectPhysic>(this, NAME_None, RF_Transactional);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeZoneVisual:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeZoneVisual>(this, NAME_None, RF_Transactional);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeAnomalousZone:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeAnomalousZone>(this, NAME_None, RF_Transactional);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeCustomZone:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeCustomZone>(this, NAME_None, RF_Transactional);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeLevelChanger:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeLevelChanger>(this, NAME_None, RF_Transactional);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeMonsterZombie:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeMonsterZombie>(this, NAME_None, RF_Transactional);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeHumanAbstract:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeHumanAbstract>(this, NAME_None, RF_Transactional);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeMonsterRat:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeMonsterRat>(this, NAME_None, RF_Transactional);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeMonsterAbstract:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeMonsterAbstract>(this, NAME_None, RF_Transactional);
		break;
	case EXRaySpawnPropertiesType::CSE_ALifeCreatureAbstract:
		SpawnData = NewObject <UStalkerSpawnProperties_ALifeCreatureAbstract>(this, NAME_None, RF_Transactional);
		break;
	case EXRaySpawnPropertiesType::CSE_SmartCover:
		SpawnData = NewObject <UStalkerSpawnProperties_SmartCover>(this, NAME_None, RF_Transactional);
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

	VisualData = NewObject<UStalkerSpawnProperties_Visual>(this, NAME_None, RF_Transactional);
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

	InventoryItemData = NewObject<UStalkerSpawnProperties_ALifeInventoryItem>(this, NAME_None, RF_Transactional);
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

	NCPData = NewObject<UStalkerSpawnProperties_ALifeTraderAbstract>(this, NAME_None, RF_Transactional);
	NCPData->SetEntity(XRayEntity);
}
