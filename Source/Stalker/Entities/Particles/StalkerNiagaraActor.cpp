#include "StalkerNiagaraActor.h"
#include "NiagaraComponent.h"
#include "Resources/Particle/StalkerNiagaraSystem.h"

AStalkerNiagaraActor::AStalkerNiagaraActor(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	GetNiagaraComponent()->bAutoActivate = false;
	StalkerNiagaraSystem = nullptr;
}

void AStalkerNiagaraActor::Lock()
{
	check(IsLocked == false);
	IsLocked = true;
}

void AStalkerNiagaraActor::Unlock()
{
	check(IsLocked == true);
	IsLocked = false;
}

void AStalkerNiagaraActor::BeginDestroy()
{
	check(IsLocked == false);
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
		Fvector Min = StalkerMath::UnrealLocationToXRay(Box.Min);
		Fvector Max = StalkerMath::UnrealLocationToXRay(Box.Max);
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

u32 AStalkerNiagaraActor::getType()
{
	throw MT_PARTICLE_GROUP;
}

shared_str AStalkerNiagaraActor::getDebugName()
{
	return ParticlesName;
}

IParticleCustom* AStalkerNiagaraActor::dcast_ParticleCustom()
{
	return this;
}

void AStalkerNiagaraActor::Detach()
{
	 ensureMsgf(false && "NotImplemented", TEXT("NotImplemented"));
}

void AStalkerNiagaraActor::UpdateParent(const Fmatrix& m, const Fvector& velocity, BOOL bXFORM)
{
	SetActorTransform(FTransform(StalkerMath::XRayMatrixToUnreal(m)));
	if (!bXFORM)
	{
		static FName NAME_UserVelocity = "User.Velocity";
		FVector3f VelocityUnrealSpace = StalkerMath::XRayLocationToUnreal(velocity);
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

void AStalkerNiagaraActor::OnSystemFinished(class UNiagaraComponent* PSystem)
{
	IsLive = false;
}	
