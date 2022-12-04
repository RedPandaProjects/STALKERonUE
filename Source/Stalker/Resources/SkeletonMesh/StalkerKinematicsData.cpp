#include "StalkerKinematicsData.h"

void UStalkerKinematicsData::BuildFromLegacy(const TArray<TSharedPtr<CBoneData>>& LegacyBones)
{
	for (const TSharedPtr < CBoneData>& Bone : LegacyBones) 
	{
		FString Name =  Bone->name.c_str();
		Bones.Add(FName( Name)).BuildFromLegacy(*Bone);
	}
}

void UStalkerKinematicsData::BuildToLegacy(TArray<TSharedPtr<CBoneData>>& LegacyBones)
{

}
