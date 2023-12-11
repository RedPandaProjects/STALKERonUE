#include "StalkerKinematicsAssetUserData.h"
#include "StalkerKinematicsBone.h"

void UStalkerKinematicsAssetUserData::BuildFromLegacy(const TArray<TSharedPtr<CBoneData>>& LegacyBones)
{
	for (const TSharedPtr < CBoneData>& Bone : LegacyBones) 
	{
		FString Name =  Bone->name.c_str();
		Bones.Add(FName( Name)).BuildFromLegacy(*Bone);
	}
}

void UStalkerKinematicsAssetUserData::BuildBones(TArray<StalkerKinematicsBone>& InBones)
{
	USkeletalMesh*Mesh = CastChecked<USkeletalMesh>(GetOuter());
	InBones.Empty(Mesh->RefSkeleton.GetNum());
	InBones.AddDefaulted(Mesh->RefSkeleton.GetNum());
	if (IsValid(Mesh))
	{
		for (int32 i = 0; i < Mesh->RefSkeleton.GetNum(); i++)
		{
			FName Name = Mesh->RefSkeleton.GetBoneName(i);
			InBones[i].Name = TCHAR_TO_ANSI(*Name.ToString().ToLower());
			if (Bones.Contains(Name))
			{
				Bones[Name].Build(InBones[i]);
			}
			InBones[i].SelfID = i;
			InBones[i].ParentID = BI_NONE;
			int32 ParentID = Mesh->RefSkeleton.GetParentIndex(i);
			if (ParentID != INDEX_NONE)
			{
				InBones[ParentID].Childs.Add(&InBones[i]);
				InBones[i].ParentID = static_cast<u16>(ParentID);
			}

			FMatrix BindTransformation =  Mesh->GetRefPoseMatrix(i);
			FQuat BoneRotation =  BindTransformation.ToQuat();
			FVector BonePosition = BindTransformation.GetOrigin();
			InBones[i].BindTransformation.rotation(StalkerMath::UnrealQuaternionToRBMK(BoneRotation));
			InBones[i].BindTransformation.translate_over(StalkerMath::UnrealLocationToRBMK(BonePosition));
		}
	}
}

