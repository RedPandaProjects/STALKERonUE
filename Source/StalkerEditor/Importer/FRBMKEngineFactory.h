#pragma once
#include "Resources/Sound/StalkerSoundWaveAssetUserData.h"
#include "Shaders/RBMKShadersManager.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/Shader_xrLC.h"
THIRD_PARTY_INCLUDES_END
struct FRbmkOGGComment
{
	float	BaseVolume		= 1.f;
	float	MinDistance		= 1.f;
	float	MaxDistance		= 300.f;
	float	MaxAIDistance	= 300.f;
	int32	Flags = 0;
	bool operator==(const FRbmkOGGComment&Right) const
	{
		return Right.BaseVolume == BaseVolume&&Right.MinDistance == MinDistance &&Right.MaxDistance == MaxDistance && Right.MaxAIDistance == MaxAIDistance&&
			EnumHasAllFlags(static_cast<EStalkerSoundTypes>(Right.Flags), EStalkerSoundTypes::Ambient|EStalkerSoundTypes::World) ==	EnumHasAllFlags(static_cast<EStalkerSoundTypes>(Flags), EStalkerSoundTypes::Ambient|EStalkerSoundTypes::World);
	}
};

FORCEINLINE uint32 GetTypeHash(const FRbmkOGGComment& OGGComment)
{
	uint32 Hash1 = HashCombine(GetTypeHash(OGGComment.BaseVolume), GetTypeHash(OGGComment.MinDistance));
	uint32 Hash2 = HashCombine(GetTypeHash(OGGComment.MaxDistance), GetTypeHash(OGGComment.MaxAIDistance));
	uint32 Hash3 = HashCombine(Hash1, Hash2);
	return HashCombine(Hash3,GetTypeHash (EnumHasAllFlags(static_cast<EStalkerSoundTypes>(OGGComment.Flags), EStalkerSoundTypes::Ambient|EStalkerSoundTypes::World) ));
}

class FRBMKEngineFactory
{
public:
									FRBMKEngineFactory			(UObject* InParentPackage, EObjectFlags InFlags);
									~FRBMKEngineFactory			();								
		UObject*					GetCreatedObject			() { if (ObjectCreated.Num())return ObjectCreated[0]; return nullptr; }

	class USkeletalMesh*			ImportOGF					(const FString& FileName);
	UObject*						ImportObject				(const FString& FileName, bool DivideSubObject = false);
	UStaticMesh*					ImportObjectAsStaticMesh	(CEditableObject* Object, bool UseOnlyFullPath = false);
	TArray<UStaticMesh*>			ImportObjectAsStaticMeshWithDivideSubObject	(CEditableObject* Object);
	UStaticMesh*					ImportObjectAsStaticMesh	(CEditableObject* Object, const FString&Path, int32 MeshID = -1);
	USkeletalMesh*					ImportObjectAsDynamicMesh	(CEditableObject* Object, bool UseOnlyFullPath = false);
	UMaterialInterface*				ImportSurface				(const FString& Path, CSurface* Surface);
	UMaterialInterface*				ImportSurface				(const FString& Path, shared_str ShaderName, shared_str TextureName, shared_str GameMaterial,bool HudMode = false);
	UTexture2D*						ImportTextureTHM			(const FString& FileName);
	UObject*						ImportPhysicsMaterials		(const FString& FileName);
	class UStalkerPhysicalMaterial* ImportPhysicsMaterial		(class SGameMtl*Materials);
	UTexture2D*						ImportTextureDDS			(const FString& FileName);
	UTexture2D*						ImportTexture				(const FString& FileName, bool IsUI = false);
	UTexture2D*						ImportTexture				(const FString& FileName,const FString& NewPath);
	void							ImportTextures				(const TArray<FString>&Prefixes,const TArray<FString>& IgnorePrefixes);
	void							ImportBump2D				(const FString& FileName, TObjectPtr<UTexture2D>& NormalMap, TObjectPtr<UTexture2D>& HeightGloss);

	USoundWave*						ImportSound					(const FString& FileName);
	USoundWave*						ImportSoundWithCombineLR	(const FString& FileName);

	static bool						LoadOrCreateOrOverwriteAsset(UClass* InClass, const FString&Path, EObjectFlags InFlags, UObject*&OutNewObject);
	template<class T>
	inline static bool				LoadOrCreateOrOverwriteAsset( const FString&Path, EObjectFlags InFlags, T*&OutNewObject)
	{
		UObject*InObject = nullptr;
		const bool Result = LoadOrCreateOrOverwriteAsset(T::StaticClass(),Path,InFlags,InObject);
		OutNewObject = CastChecked<T>(InObject,ECastCheckedType::NullAllowed);
		return Result;
	}

	EObjectFlags					GetObjectFlags				() const {return ObjectFlags;}
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
	void							ReadOGGSound				(FArchive&Ar,TArray<uint16>&OutData,int32&OutSampleRate,int32&OutNumChannels,FRbmkOGGComment&OutOGGComment);
	USoundCue*						CreateSoundLoopCue			(USoundWave* InSound,	const FString& Path);
	UObject*						ParentPackage;
	EObjectFlags					ObjectFlags;
	TArray< CEditableObject*>		Objects;
	TArray< UObject*>				ObjectCreated;
	Shader_xrLC_LIB					ShaderXRLC;
	TUniquePtr<FRBMKShadersManager> ShadersManager;
	TMap<FRbmkOGGComment,USoundAttenuation*> SoundAttenuationsCached;
};
