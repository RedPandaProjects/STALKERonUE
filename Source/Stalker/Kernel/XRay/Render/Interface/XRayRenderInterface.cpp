#include "XRayRenderInterface.h"

#include "IDetailTreeNode.h"
#include "XRayRenderTarget.h"
#include "../../../StalkerEngineManager.h"
#include "Resources/StalkerResourcesManager.h"
#include "Entities/Kinematics/StalkerKinematicsComponent.h"
#include "Entities/Levels/Light/StalkerLight.h"
#include "Entities/Particles/StalkerNiagaraActor.h"
#include "Kernel/Unreal/GameSettings/StalkerGameSettings.h"
#include "Kismet/KismetMaterialLibrary.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/Render/RenderVisual.h"
THIRD_PARTY_INCLUDES_END

XRayRenderInterface GRenderInterface;

XRayRenderInterface::XRayRenderInterface()
{
}

IRender_Target* XRayRenderInterface::getTarget()
{
	return &GRenderTarget;
}

class XRayRenderObjectSpecific :public IRender_ObjectSpecific
{
public:
	XRayRenderObjectSpecific(){}
	~XRayRenderObjectSpecific() override {}
	void force_mode(u32 mode) override
	{
	}


	float get_luminocity() override
	{
		return 1;
	}


	float get_luminocity_hemi() override
	{
		return 1;
	}


	float* get_luminocity_hemi_cube() override
	{
		static float Result[6]={};
		return Result;
	}

};
IRender_ObjectSpecific* XRayRenderInterface::ros_create(IRenderable* parent)
{
	return new XRayRenderObjectSpecific;
}

void XRayRenderInterface::ros_destroy(IRender_ObjectSpecific*& V)
{
	if(V) 
	delete V;
	V= nullptr;
}
IRender_Light* XRayRenderInterface::light_create()
{
	return GStalkerEngineManager->GetResourcesManager()->CreateLight();
}
class XRayRenderGlow :public IRender_Glow
{

public:
	bool IsActive = true;
	void set_active(bool InIsActive) override
	{
		IsActive = InIsActive;
	}


	bool get_active() override
	{
		return IsActive;
	}


	void set_position(const Fvector& P) override
	{
	}


	void set_direction(const Fvector& P) override
	{
	}


	void set_radius(float R) override
	{
	}


	void set_texture(LPCSTR name) override
	{
	}


	void set_color(const Fcolor& C) override
	{
	}


	void set_color(float r, float g, float b) override
	{
	}

};
IRender_Glow* XRayRenderInterface::glow_create()
{
	return new XRayRenderGlow;
}

IRenderVisual* XRayRenderInterface::model_CreateParticles(LPCSTR name)
{
	return GStalkerEngineManager->GetResourcesManager()->CreateParticles(name);
}

IRenderVisual* XRayRenderInterface::model_Create(LPCSTR name, IReader* data)
{
	UStalkerKinematicsComponent* Kinematics =  GStalkerEngineManager->GetResourcesManager()->CreateKinematics(name,true);
	check(Kinematics);
	return Kinematics;
}

void XRayRenderInterface::model_Delete(IRenderVisual*& V, BOOL bDiscard)
{
	if (V)
	{	
		if (V->CastUnrealObject(EXRayUnrealObjectType::StalkerKinematicsComponent))
		{
			GStalkerEngineManager->GetResourcesManager()->Destroy(reinterpret_cast<UStalkerKinematicsComponent*>(V->CastUnrealObject(EXRayUnrealObjectType::StalkerKinematicsComponent)));
		}
		else if (V->dcast_ParticleCustom())
		{
			GStalkerEngineManager->GetResourcesManager()->Desotry(V->dcast_ParticleCustom());
		}
		else
		{
			check(false);
		}
		V = nullptr;
	}
}

void XRayRenderInterface::light_destroy(IRender_Light* p_)
{
	if (p_)
	{
		GStalkerEngineManager->GetResourcesManager()->Desotry(p_);
	}
	p_ = nullptr;
}

void XRayRenderInterface::glow_destroy(IRender_Glow* p_)
{
	if (p_)
	{
		delete p_;
	}
	p_ = nullptr;
}

void XRayRenderInterface::SpawnStaticDecal(const shared_str& InTextureName, const Fvector& Point, const Fvector& Normal, float Size)
{
	FString TextureName = ANSI_TO_TCHAR(*InTextureName);TextureName.ReplaceCharInline(TEXT('\\'),TEXT('/'));
	const FString ParentPackageName =GStalkerEngineManager->GetResourcesManager()->GetGamePath() /TEXT("Textures") / TextureName;
	const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
	if(UTexture2D * Texture2D = LoadObject<UTexture2D>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn))
	{
		FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
		SpawnParameters.ObjectFlags = EObjectFlags::RF_Transient;
		
		FQuat Rotation = FQuat(StalkerMath::RBMKNormalToUnreal(Normal).ToOrientationQuat())*FRotator(-90,0,0).Quaternion();
		Rotation = Rotation*FRotator(0,FMath::FRandRange(0.f,360.f),0).Quaternion();
		ADecalActor* DecalActor = GWorld->SpawnActor<ADecalActor>(FVector(StalkerMath::RBMKLocationToUnreal(Point)), Rotation.Rotator(),SpawnParameters);
		DecalActor->SetFolderPath(TEXT("Decals"));
		DecalActor->GetDecal()->DecalSize = FVector(Size*12.5f,Size*75,Size*75);
		{
			UMaterialInstanceDynamic*Material = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GWorld,GetDefault<UStalkerGameSettings>()->DefaultStaticDecalMaterial.LoadSynchronous(),NAME_None,EMIDCreationFlags::Transient);
			static FName NAME_Diffuse = "Diffuse";
			Material->SetTextureParameterValue(NAME_Diffuse,Texture2D);
			DecalActor->SetDecalMaterial(Material);
		}
		DecalActor->GetDecal()->SetFadeOut(20.f,2.f);
	}
}

void XRayRenderInterface::SpawnSkeletalDecal(const IRenderVisual* InObject, const shared_str& InTextureName,	const Fvector& StartPoint, const Fvector& EndPoint, float Size)
{
	FString TextureName = ANSI_TO_TCHAR(*InTextureName);TextureName.ReplaceCharInline(TEXT('\\'),TEXT('/'));
	const FString ParentPackageName =GStalkerEngineManager->GetResourcesManager()->GetGamePath() /TEXT("Textures") / TextureName;
	const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
	if(UTexture2D * Texture2D = LoadObject<UTexture2D>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn))
	{
		if(UStalkerKinematicsComponent *SceneComponent = reinterpret_cast<UStalkerKinematicsComponent*>(const_cast< IRenderVisual*>(InObject)->CastUnrealObject(EXRayUnrealObjectType::StalkerKinematicsComponent)))
		{
			
			FCollisionQueryParams CollisionQueryParams;
			CollisionQueryParams.bTraceComplex = false;
			CollisionQueryParams.MobilityType = EQueryMobilityType::Dynamic;
			FHitResult HitResult;
			//if(GWorld->LineTraceSingleByChannel(HitResult,FVector(StalkerMath::RBMKLocationToUnreal(StartPoint)),FVector(StalkerMath::RBMKLocationToUnreal(EndPoint)),ECC_Kinematics,CollisionQueryParams))
			if(SceneComponent->LineTraceComponent(HitResult,FVector(StalkerMath::RBMKLocationToUnreal(StartPoint)),FVector(StalkerMath::RBMKLocationToUnreal(EndPoint)),CollisionQueryParams))
			{
				
				FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
				SpawnParameters.ObjectFlags = EObjectFlags::RF_Transient;
				
				FQuat Rotation = FQuat(HitResult.Normal.ToOrientationQuat())*FRotator(-90,0,0).Quaternion();
				Rotation = Rotation*FRotator(0,FMath::FRandRange(0.f,360.f),0).Quaternion();
				ADecalActor* DecalActor = GWorld->SpawnActor<ADecalActor>(HitResult.Location, Rotation.Rotator(),SpawnParameters);
				DecalActor->SetFolderPath(TEXT("Decals"));
				DecalActor->GetDecal()->DecalSize = FVector(Size*50.f,Size*50,Size*50);
				{
					UMaterialInstanceDynamic*Material = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GWorld,GetDefault<UStalkerGameSettings>()->DefaultStaticDecalMaterial.LoadSynchronous(),NAME_None,EMIDCreationFlags::Transient);
					static FName NAME_Diffuse = "Diffuse";
					static FName NAME_CustomDepth = "CustomDepth";
					Material->SetTextureParameterValue(NAME_Diffuse,Texture2D);
					Material->SetScalarParameterValue(NAME_CustomDepth,1);
					DecalActor->SetDecalMaterial(Material);

				}
				DecalActor->GetDecal()->SetFadeOut(30.f,2.f);
				DecalActor->AttachToComponent(SceneComponent,FAttachmentTransformRules(EAttachmentRule::KeepWorld,false),HitResult.BoneName);

			}
			else
			{
				DrawDebugLine(GWorld,FVector(StalkerMath::RBMKLocationToUnreal(StartPoint)),FVector(StalkerMath::RBMKLocationToUnreal(EndPoint)),FColor::Red,false,10.f);
			}

		}
	
	}
}

