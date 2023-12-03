#pragma once
THIRD_PARTY_INCLUDES_START
#include "XrEngine/Shader_xrLC.h"
THIRD_PARTY_INCLUDES_END
class RBMKEngineFactory
{
public:
									RBMKEngineFactory			(UObject* InParentPackage, EObjectFlags InFlags);
									~RBMKEngineFactory			();								
	inline UObject*					GetCreatedObject			() { if (ObjectCreated.Num())return ObjectCreated[0]; return nullptr; }

	class USkeletalMesh*			ImportOGF					(const FString& FileName);
	UObject*						ImportObject				(const FString& FileName, bool DivideSubObject = false);
	UStaticMesh*					ImportObjectAsStaticMesh	(CEditableObject* Object, bool UseOnlyFullPath = false);
	UStaticMesh*					ImportObjectAsStaticMesh	(CEditableObject* Object, const FString&Name, int32 MeshID = -1);
	USkeletalMesh*					ImportObjectAsDynamicMesh	(CEditableObject* Object, bool UseOnlyFullPath = false);
	UMaterialInterface*				ImportSurface				(const FString& Path, CSurface* Surface);
	UMaterialInterface*				ImportSurface				(const FString& Path, shared_str ShaderName, shared_str TextureName, shared_str GameMaterial,bool HudMode = false);
	UMaterialInterface*				ImportSurfaceSOC			(const FString& Path, shared_str ShaderName, shared_str TextureName,  shared_str GameMaterial,bool HudMode = false);
	UTexture2D*						ImportTextureTHM			(const FString& FileName);
	UObject*						ImportPhysicsMaterials		(const FString& FileName);
	class UStalkerPhysicalMaterial* ImportPhysicsMaterial		(class SGameMtl*Materials);
	UTexture2D*						ImportTextureDDS			(const FString& FileName);
	UTexture2D*						ImportTexture				(const FString& FileName, bool IsUI = false);
	UTexture2D*						ImportTexture				(const FString& FileName,const FString& PacketName);
	void							ImportBump2D				(const FString& FileName, TObjectPtr<UTexture2D>& NormalMap, TObjectPtr<UTexture2D>& HeightGloss);
	

private:
	int32							FindBoneIDOrAdd				(TArray<CBone*>& InBones, CBone* InBone);
	int32							FindBoneIDOrAdd				(TArray<uint32>& OutBones, TArray<TSharedPtr<CBoneData>>& InBones, CBoneData* InBone);
	bool							CreateSkeletalMesh			(USkeletalMesh* SkeletalMesh, TArray<FSkeletalMeshImportData>& LODIndexToSkeletalMeshImportData, const TArray<SkeletalMeshImportData::FBone>& InSkeletonBones);
	USkeleton*						FindOrCreateSkeleton		(const FString&Name , USkeletalMesh* InMesh);
	void							CreateAnims					(const FString& Name, USkeleton* InMesh, CEditableObject* Object);
	void							CreateAnims					(const FString& Name, USkeleton* InMesh, CSMotion* InMotion);
	void							CreateAnims					(const FString& Name, USkeletalMesh*InMesh, TArray<TSharedPtr<CBoneData>>&BoneData, IReader* InMotion);
	UAnimSequence*					CreateAnim					(const FString& Name, USkeleton* InMesh,TArray<TSharedPtr<CBoneData>>&BoneData, shared_motions&InMotion,u16 ID);
	void							CreatePhysicsAsset			(const FString& Name, USkeletalMesh* InMesh, const TArray<IBoneData*>& Bones);

	UObject*						ParentPackage;
	EObjectFlags					ObjectFlags;
	TArray< CEditableObject*>		Objects;
	TArray< UObject*>				ObjectCreated;
	Shader_xrLC_LIB					ShaderXRLC;
};
