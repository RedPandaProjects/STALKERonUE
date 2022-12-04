#pragma once
class XRayEngineFactory
{
public:
								XRayEngineFactory			(UObject* InParentPackage, EObjectFlags InFlags);
								~XRayEngineFactory			();
	class UStalkerKinematicsData*ImportOGF					(const FString& FileName);
	UObject*					ImportObject				(const FString& FileName, bool UseOnlyFullPath = false);
	UStaticMesh*				ImportObjectAsStaticMesh	(CEditableObject* Object, bool UseOnlyFullPath = false);
	USkeletalMesh*				ImportObjectAsDynamicMesh	(CEditableObject* Object, bool UseOnlyFullPath = false);
	UMaterialInterface*			ImportSurface				(const FString& Path, CSurface* Surface);
	UMaterialInterface*			ImportSurface				(const FString& Path, shared_str ShaderName, shared_str TextureName);
	UTexture2D*					ImportTextureTHM			(const FString& FileName);
	UTexture2D*					ImportTexture				(const FString& FileName);
	void						ImportBump2D				(const FString& FileName, TObjectPtr<UTexture2D>& NormalMap, TObjectPtr<UTexture2D>& Specular, TObjectPtr<UTexture2D>& Height);
	inline UObject*				GetCreatedObject			() { if (ObjectCreated.Num())return ObjectCreated[0]; return nullptr; }
private:
	int32						FindBoneIDOrAdd				( TArray<CBone*>& InBones, CBone* InBone);
	int32						FindBoneIDOrAdd				(TArray<uint32>& OutBones, TArray<TSharedPtr<CBoneData>>& InBones, CBoneData* InBone);
	bool						CreateSkeletalMesh			(USkeletalMesh* SkeletalMesh, TArray<FSkeletalMeshImportData>& LODIndexToSkeletalMeshImportData, const TArray<SkeletalMeshImportData::FBone>& InSkeletonBones);
	USkeleton*					FindOrCreateSkeleton				(const FString&Name , USkeletalMesh* InMesh);
	void						CreateAnims					(const FString& Name, USkeleton* InMesh, CEditableObject* Object);
	void						CreateAnims					(const FString& Name, USkeleton* InMesh, CSMotion* InMotion);
	void						CreateAnims					(const FString& Name, USkeleton* InMesh, TArray<TSharedPtr<CBoneData>>&BoneData, IReader* InMotion);
	void						CreateAnims					(const FString& Name, USkeleton* InMesh,TArray<TSharedPtr<CBoneData>>&BoneData, shared_motions&InMotion,u16 ID);
	void						CreatePhysicsAsset			(const FString& Name, USkeletalMesh* InMesh, CEditableObject* Object);
	FString						GetGamePath					();
	UObject*					ParentPackage;
	EObjectFlags				ObjectFlags;
	TArray< CEditableObject*>	Objects;
	TArray< UObject*>			ObjectCreated;
	TArray< USkeleton*>			Skeletons;
};