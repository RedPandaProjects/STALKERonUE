#pragma once
#include "LoadingScreen/StalkerLoadingScreenManager.h"
class USlateBrushAsset;
class STALKER_API FStalkerResourcesManager	: public FGCObject
{
public:
														FStalkerResourcesManager	();
														~FStalkerResourcesManager	();
	void												AddReferencedObjects		(FReferenceCollector& Collector) override;
	FString												GetReferencerName			() const override;
	USlateBrushAsset*									GetBrush					(FName NameMaterial, FName NameTexture);
	UFont*												GetFont						(FName Name);
	void												Free						(const USlateBrushAsset* Brush);
	USlateBrushAsset*									Copy						(USlateBrushAsset* Brush);
	void												CheckLeak					() const;
	void												Reload						();
	class AStalkerLight*								CreateLight					();
														static void												Destroy						(class IRender_Light*Light);
	class USkeletalMesh*								GetKinematics				(const char* Name);
	class UStalkerKinematicsComponent*					CreateKinematics			(const char*Name, bool NeedReference = false);
	class UStalkerKinematicsComponent*					CreateKinematics			(class USkeletalMesh* KinematicsData);
	void												Destroy						(class UStalkerKinematicsComponent* Mesh);
	void												RegisterKinematics			(class UStalkerKinematicsComponent* Mesh);
	void												UnregisterKinematics		(const class UStalkerKinematicsComponent* Mesh);
	class AStalkerNiagaraActor*							CreateParticles				(const char* Name);
														static void												Destroy						(class IParticleCustom*Particles);
	void												Refresh						();
	FString												GetGamePath					();
	class UStalkerGameSpawn*							GetGameSpawn				();
	class UStalkerSoundManager*							GetSoundManager				() {return SoundManager;}
#if WITH_EDITORONLY_DATA
	class UStalkerGameSpawn*							GetOrCreateGameSpawn		();
#endif
	TMap<USlateBrushAsset*, UTexture*>					BrushesTextures;

	FStalkerLoadingScreenManager						LoadingScreenManager;
private:

	TMap<FName, UFont*>									Fonts;

	TMap<USlateBrushAsset*,int32>						BrushesCounter;
	TMap<USlateBrushAsset*, UMaterialInstanceDynamic*>	BrushesMaterials;
#if WITH_EDITORONLY_DATA
	TSet< USlateBrushAsset*>							BrushesNeedReloading;
#endif
	TMap<FName, TMap<FName, USlateBrushAsset*>>			Brushes;
	struct  BrushInfo
	{
		FName Matrrial;
		FName Texture;
	};
	TMap<USlateBrushAsset*, BrushInfo>					BrushesInfo;
	
	TSet<UStalkerKinematicsComponent*>					Meshes;

	class UStalkerGameSpawn*							GameSpawn = nullptr;
	class UStalkerSoundManager*							SoundManager = nullptr;
};