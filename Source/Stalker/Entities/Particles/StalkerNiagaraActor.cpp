#include "StalkerNiagaraActor.h"
#include "NiagaraComponent.h"
#include "Resources/Particle/StalkerNiagaraSystem.h"

AStalkerNiagaraActor::AStalkerNiagaraActor(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	GetNiagaraComponent()->bAutoActivate = false;
	StalkerNiagaraSystem = nullptr;
}

void AStalkerNiagaraActor::AttachTo(XRayUnrealAttachableInterface* AttachableInterface, const char* BoneName)
{
	USceneComponent* SceneComponent = reinterpret_cast<USceneComponent*>(AttachableInterface->CastUnrealObject(EXRayUnrealObjectType::SceneComponent));
	check(SceneComponent);
	check(GetRootComponent());
	FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
	check(AttachToComponent(SceneComponent,AttachmentTransformRules,BoneName));
	if (UPrimitiveComponent* PrimitiveComponen = Cast<UPrimitiveComponent>(SceneComponent))
	{
		SetOwnerNoSee(PrimitiveComponen->bOwnerNoSee);
		SetOnlyOwnerSee(PrimitiveComponen->bOnlyOwnerSee);
	}
}
void AStalkerNiagaraActor::Detach()
{
	FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, false);
	DetachFromActor(DetachmentTransformRules);
}
void AStalkerNiagaraActor::Lock(CObject* Object)
{
	Lock((void*)Object);
}

void AStalkerNiagaraActor::Lock(void* Pointer)
{
	check(Locked == nullptr);
	Locked = Pointer;
}

void AStalkerNiagaraActor::Unlock(void* Pointer)
{
	check(Locked == Pointer);
	Locked = nullptr;
}

void* AStalkerNiagaraActor::CastUnrealObject(EXRayUnrealObjectType ObjectType)
{
	switch (ObjectType)
	{
	case EXRayUnrealObjectType::Object:
		return static_cast<UObject*>(this);
	case EXRayUnrealObjectType::Actor:
		return static_cast<AActor*>(this);
	case EXRayUnrealObjectType::StalkerNiagaraActor:
		return this;
	case EXRayUnrealObjectType::SceneComponent:
		return static_cast<USceneComponent*>(GetNiagaraComponent());
	default:
		return nullptr;
	}
}

void* AStalkerNiagaraActor::QueryInterface(EXRayUnrealInterfaceType AttachableType)
{
	switch (AttachableType)
	{
	case EXRayUnrealInterfaceType::ParticleCustom:
		return static_cast<IParticleCustom*>(this);
	}
	return IRenderVisual::QueryInterface(AttachableType);
}

void AStalkerNiagaraActor::BeginDestroy()
{
	check(Locked == nullptr);
	Super::BeginDestroy();
}

void AStalkerNiagaraActor::Initialize(shared_str InName)
{
	ParticlesName = InName;
	FString Name =  ParticlesName.c_str();
	Name.ReplaceCharInline(TEXT('\\'), TEXT('/'));
	UNiagaraSystem*System = nullptr;
	switch (xrGameManager::GetGame())
	{
	default:
	{
		const FString ParentPackageName = TEXT("/Game/COP/Particles") / Name;
		const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
		System = LoadObject<UNiagaraSystem>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
	}
	break;
	case EGame::SHOC:
	{
		const FString ParentPackageName = TEXT("/Game/SHOC/Particles") / Name;
		const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
		System = LoadObject<UNiagaraSystem>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
	}
	break;
	case EGame::CS:
	{
		const FString ParentPackageName = TEXT("/Game/CS/Particles") / Name;
		const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
		System = LoadObject<UNiagaraSystem>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
	}
	break;
	}
	if (!ensure(IsValid(System)))
	{
#ifdef WITH_EDITOR
		FMessageLog StalkerEditorFMessageLog("StalkerEditor");
		if(FApp::IsGame())
		{
			StalkerEditorFMessageLog.Error(FText::FromString(FString::Printf(TEXT("Can't found particles %S"),InName.c_str())));
			StalkerEditorFMessageLog.Open(EMessageSeverity::Error);
		}
#endif
		UE_LOG(LogStalker, Error, TEXT("Can't found particles %S"),InName.c_str());

		const FString ParentPackageName = TEXT("/Game/Base/Particles/Error") ;
		const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
		System = LoadObject<UNiagaraSystem>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
	}
	if (ensure(System))
	{
		StalkerNiagaraSystem = Cast<UStalkerNiagaraSystem>(System);
		GetNiagaraComponent()->SetAsset(System);
	}
	VisData.hom_frame = -1;
	GetNiagaraComponent()->OnSystemFinished.AddDynamic(this,&AStalkerNiagaraActor::OnSystemFinished);
}

vis_data& AStalkerNiagaraActor::getVisData()
{
	if (VisData.hom_frame == Device->dwFrame)
	{
		return VisData;
	}
	{
		FVector Origin,BoxExtent;
		GetActorBounds(false,Origin,BoxExtent);
		FBox Box = FBox::BuildAABB(Origin,BoxExtent);
		Fvector Min = StalkerMath::UnrealLocationToRBMK(Box.Min);
		Fvector Max = StalkerMath::UnrealLocationToRBMK(Box.Max);
		if (Min.x > Max.x)
		{
			Swap(Min.x,Max.x);
		}
		VisData.box.set(Min,Max);
		if (!IsPlaying())
		{
			VisData.box.grow		(EPS_L);
		}
		Fvector Center;
		VisData.box.getcenter(Center);
		VisData.sphere.set(Center, VisData.box.getradius());
	}
	VisData.hom_frame = Device->dwFrame;
	return VisData;
}

shared_str AStalkerNiagaraActor::getDebugName()
{
	return ParticlesName;
}

void AStalkerNiagaraActor::UpdateParent(const Fmatrix& m, const Fvector& velocity, BOOL bXFORM)
{
	SetActorTransform(FTransform(StalkerMath::RBMKMatrixToUnreal(m)));
	if (!bXFORM)
	{
		static FName NAME_UserVelocity = "User.Velocity";
		FVector3f VelocityUnrealSpace = StalkerMath::RBMKLocationToUnreal(velocity);
		GetNiagaraComponent()->SetVariableVec3(NAME_UserVelocity,FVector(VelocityUnrealSpace));
	}
}

void AStalkerNiagaraActor::Play()
{
	IsLive = true;
	GetNiagaraComponent()->ResetSystem();
}

void AStalkerNiagaraActor::Stop(BOOL bDefferedStop /*= TRUE*/)
{
	if (bDefferedStop)
	{
		GetNiagaraComponent()->DeactivateImmediate();
	}
	else
	{
		GetNiagaraComponent()->Deactivate();
	}
}

BOOL AStalkerNiagaraActor::IsPlaying()
{
	return !GetNiagaraComponent()->IsComplete();
}

BOOL AStalkerNiagaraActor::IsLooping()
{
	if (StalkerNiagaraSystem)
	{
		return StalkerNiagaraSystem->IsLooping;
	}
	return false;
}

const shared_str AStalkerNiagaraActor::Name()
{
	return ParticlesName;
}

void AStalkerNiagaraActor::SetHudMode(BOOL b)
{
	HudMode = b;
	if (StalkerNiagaraSystem)
	{
		for (auto &[Key,Material] : StalkerNiagaraSystem->Materials)
		{
			GetNiagaraComponent()->SetVariableMaterial(Key,b?Material.HudMaterial:nullptr);
		}
	}
}

BOOL AStalkerNiagaraActor::GetHudMode()
{
	return HudMode;
}

bool AStalkerNiagaraActor::Alive()
{
	return IsLive;
}

void AStalkerNiagaraActor::SetOffset(const Fmatrix& offset)
{
	GetNiagaraComponent()->SetRelativeTransform(FTransform(StalkerMath::RBMKMatrixToUnreal(offset)));
}

void AStalkerNiagaraActor::SetEnableVelocity(bool EnableVelocity)
{
	SetActorTickEnabled(EnableVelocity);
	LastPosition = GetActorLocation();
}

void AStalkerNiagaraActor::BeginPlay()
{
	Super::BeginPlay();
	LastPosition = GetActorLocation();
}

void AStalkerNiagaraActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	static FName NAME_UserVelocity = "User.Velocity";
	GetNiagaraComponent()->SetVariableVec3(NAME_UserVelocity,	(GetActorLocation()-LastPosition));
	LastPosition = GetActorLocation();
}

void AStalkerNiagaraActor::SetOwnerNoSee(bool Enable)
{
	if (UPrimitiveComponent* PrimitiveComponen = Cast<UPrimitiveComponent>(GetRootComponent()))
	{
		PrimitiveComponen->SetOwnerNoSee(Enable);
	}
}

void AStalkerNiagaraActor::SetOnlyOwnerSee(bool Enable)
{
	if (UPrimitiveComponent* PrimitiveComponen = Cast<UPrimitiveComponent>(GetRootComponent()))
	{
		PrimitiveComponen->SetOnlyOwnerSee(Enable);
	}
}

void AStalkerNiagaraActor::GetWorldTransform(Fmatrix& OutXForm)
{
	OutXForm = StalkerMath::UnrealMatrixToRBMK(GetRootComponent()->GetComponentToWorld().ToMatrixWithScale());
}

bool AStalkerNiagaraActor::IsAttached(XRayUnrealAttachableInterface* Attach)
{
	if(USceneComponent*RootComp = reinterpret_cast<USceneComponent*>(Attach->CastUnrealObject(EXRayUnrealObjectType::SceneComponent)))
	{
		return RootComponent->IsAttachedTo(RootComp);
	}
	return false;
}

void AStalkerNiagaraActor::SetVisibility(bool NewVisibility)
{
	GetNiagaraComponent()->SetVisibility(NewVisibility);
}

void AStalkerNiagaraActor::OnSystemFinished(class UNiagaraComponent* PSystem)
{
	IsLive = false;
}	
