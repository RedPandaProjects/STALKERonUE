#pragma once
class USlateBrushAsset;
class STALKER_API FStalkerResourcesManager	: public FGCObject
{
public:
	class ShaderMaterialInfo {
		friend class FStalkerResourcesManager;

		UTexture *Texture;
		UMaterialInstanceDynamic* Material;
		int32 RefCount;
		bool NeedsReloading;
		FName TextureName;
		FName MaterialName;

	public:
		ShaderMaterialInfo() :
			RefCount(1),
			NeedsReloading(false),
			Texture(nullptr),
			Material(nullptr) {}

		auto *GetMaterial() const { return Material; }
		auto GetTextureName() const { return TextureName; }
	};

														FStalkerResourcesManager	();
														~FStalkerResourcesManager	();
	void												AddReferencedObjects		(FReferenceCollector& Collector) override;
	FString												GetReferencerName			() const override;
	UFont*												GetFont						(FName Name);
	ShaderMaterialInfo*									GetShaderMaterial			(FName NameMaterial, FName NameTexture);
	void												Free						(ShaderMaterialInfo* Material);
	ShaderMaterialInfo*									Copy						(ShaderMaterialInfo* Material);
	USlateBrushAsset*									GetBrush					(ShaderMaterialInfo* Material);
	UTexture*											FindBrushTexture			(USlateBrushAsset* Brush);
	void												Free						(USlateBrushAsset* Brush);
	USlateBrushAsset*									Copy						(USlateBrushAsset* Brush);
	void												CheckLeak					();
	void												Reload						();
	class AStalkerLight*								CreateLight					();
	class AStalkerDecal*								CreateDecal					();
	void												Desotry						(class IRender_Light*Light);
	class UStalkerKinematicsData*						GetKinematics				(const char* Name);
	class UStalkerKinematicsComponent*					CreateKinematics			(const char*Name, bool NeedRefence = false);
	class UStalkerKinematicsComponent*					CreateKinematics			(class UStalkerKinematicsData* KinematicsData);
	void												Destroy						(class UStalkerKinematicsComponent* Mesh);
	void												RegisterKinematics			(class UStalkerKinematicsComponent* Mesh);
	void												UnregisterKinematics		(class UStalkerKinematicsComponent* Mesh);
	void												Refresh						();
	FString												GetGamePath					();
	class UStalkerGameSpawn*							GetGameSpawn				();
#if WITH_EDITORONLY_DATA
	class UStalkerGameSpawn*							GetOrCreateGameSpawn		();
#endif


private:

	TMap<FName, UFont*>									Fonts;

	TMap<FName, TMap<FName, ShaderMaterialInfo*>>		ShaderMaterials;

	struct BrushInfo {
		ShaderMaterialInfo *Material;
		USlateBrushAsset *Brush;
		int32 RefCount;
		bool NeedsReloading;

		BrushInfo() :
			RefCount(1),
			NeedsReloading(false),
			Material(nullptr),
			Brush(nullptr) {}
	};
	TMap<ShaderMaterialInfo *, BrushInfo*>				Brushes;
	TMap<USlateBrushAsset *, BrushInfo *>				BrushInfoRefs;
	
	TSet<UStalkerKinematicsComponent*>					Meshes;

	class UStalkerGameSpawn*							GameSpawn = nullptr;
};