#pragma once
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
	void												Free						(USlateBrushAsset* Brush);
	USlateBrushAsset*									Copy						(USlateBrushAsset* Brush);
	void												CheckLeak					();
	void												Reload						();
	class AStalkerLight*								CreateLight					();
	void												Desotry						(class IRender_Light*Light);
	class UStalkerKinematicsData*						GetKinematics				(const char* Name);
	class UStalkerKinematicsComponent*					CreateKinematics			(const char*Name, bool NeedRefence = false);
	class UStalkerKinematicsComponent*					CreateKinematics			(class UStalkerKinematicsData* KinematicsData);
	void												Destroy						(class UStalkerKinematicsComponent* Mesh);
	void												RegisterKinematics			(class UStalkerKinematicsComponent* Mesh);
	void												UnregisterKinematics		(class UStalkerKinematicsComponent* Mesh);
	class AStalkerNiagaraActor*							CreateParticles				(const char* Name);
	void												Desotry						(class IParticleCustom*Particles);
	void												Refresh						();
	FString												GetGamePath					();
	class UStalkerGameSpawn*							GetGameSpawn				();
#if WITH_EDITORONLY_DATA
	class UStalkerGameSpawn*							GetOrCreateGameSpawn		();
#endif
	TMap<USlateBrushAsset*, UTexture*>					BrushesTextures;


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
};