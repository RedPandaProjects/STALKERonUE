#include "StalkerKinematics.h"
#include "Resources/SkeletonMesh/StalkerKinematicsData.h"
THIRD_PARTY_INCLUDES_START
#include "XrRender/Public/RenderVisual.h"
#include "XrEngine/IRenderable.h"
THIRD_PARTY_INCLUDES_END
AStalkerKinematics::AStalkerKinematics()
{
	PrimaryActorTick.bCanEverTick = true;
	KinematicsData = nullptr;
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(GetRootComponent());
}

void AStalkerKinematics::Initilize(class UStalkerKinematicsData* InKinematicsData)
{
	check(KinematicsData == nullptr);
	check(InKinematicsData);

	KinematicsData = InKinematicsData;
	KinematicsData->BuildBones(Bones);
	BonesInstance.AddDefaulted(Bones.Num());
	if (KinematicsData->UserData.Len())
	{
		TArray<char> UserDataMemory;
		UserDataMemory.Append(TCHAR_TO_ANSI(*KinematicsData->UserData), KinematicsData->UserData.Len());
		IReader Reader(UserDataMemory.GetData(), UserDataMemory.Num());
		UserData = MakeShared<CInifile>(&Reader, FS.get_path("$game_config$")->m_Path);
	}

	for(StalkerKinematicsBone&Bone: Bones)
	{
		BonesName2ID.Add(Bone.GetName(), Bone.SelfID);
		BonesID2Name.Add(Bone.SelfID,Bone.GetName());
	}
	MeshComponent->SetSkeletalMesh(KinematicsData->Mesh);
	for(u16 i =0;i<LL_BoneCount();i++)
		SelfBonesVisible.set(i,true);
}

void AStalkerKinematics::BeginPlay()
{
	Super::BeginPlay();
	FVector Location,BoxExtent;
	FBox Box = GetComponentsBoundingBox();
	
	VisData.box.invalidate();
	VisData.box.modify(StalkerMath::UnrealLocationToXRay(Box.Min));
	VisData.box.modify(StalkerMath::UnrealLocationToXRay(Box.Max));
	Fvector Center;
	VisData.box.getcenter(Center);
	VisData.sphere.set(Center, VisData.box.getradius());
}

class AStalkerKinematics* AStalkerKinematics::CastToAStalkerKinematics()
{
	return this;
}

vis_data& _BCL AStalkerKinematics::getVisData()
{
	return VisData;
}

u32 AStalkerKinematics::getType()
{
	return MT_SKELETON_RIGID;
}

IKinematics* _BCL AStalkerKinematics::dcast_PKinematics()
{
	return this;
}

void AStalkerKinematics::Bone_Calculate(const IBoneData* bd, const Fmatrix* parent)
{
	
}

void AStalkerKinematics::Bone_GetAnimPos(Fmatrix& pos, u16 id, u8 channel_mask, bool ignore_callbacks)
{
	
}

bool AStalkerKinematics::PickBone(const Fmatrix& parent_xform, pick_result& r, float dist, const Fvector& start, const Fvector& dir, u16 bone_id)
{
	return false;
}

void AStalkerKinematics::EnumBoneVertices(SEnumVerticesCallback& C, u16 bone_id)
{

}

u16 AStalkerKinematics::LL_BoneID(LPCSTR B)
{
	u16 *ID = BonesName2ID.Find(B);
	return ID?*ID:BI_NONE;
}

u16 AStalkerKinematics::LL_BoneID(const shared_str& B)
{
	u16* ID = BonesName2ID.Find(B);
	return ID ? *ID : BI_NONE;
}

LPCSTR AStalkerKinematics::LL_BoneName_dbg(u16 ID)
{
	shared_str* Name = BonesID2Name.Find(ID);
	return Name ? Name->c_str() : "";
}

CInifile* AStalkerKinematics::LL_UserData()
{
	return UserData.Get();
}

IBoneInstance& AStalkerKinematics::LL_GetBoneInstance(u16 bone_id)
{
	return BonesInstance[bone_id];
}

const IBoneData& AStalkerKinematics::GetBoneData(u16 bone_id) const
{
	return Bones[bone_id];
}

u16 AStalkerKinematics::LL_BoneCount() const
{
	return Bones.Num();
}

u16 AStalkerKinematics::LL_VisibleBoneCount()
{
	u32 Count = (LL_BoneCount() / 64) + 1;
	u64 CountBone = 0;
	for (u32 i = 0; i < Count - 1; i++)
	{
		CountBone += btwCount1(SelfBonesVisible.visimask[i].flags);
	}
	{
		u64 flags = SelfBonesVisible.visimask[Count - 1].flags;
		flags &= (u64(1) << (LL_BoneCount() % 64)) - 1;
		CountBone += btwCount1(flags);
	}
	return (u16)CountBone;
}

const Fmatrix& AStalkerKinematics::LL_GetTransform(u16 bone_id) const
{
	return Fidentity;
}

const Fmatrix& AStalkerKinematics::LL_GetTransform_R(u16 bone_id)
{
	return Fidentity;
}

void AStalkerKinematics::LL_SetTransform(u16 bone_id, const Fmatrix& Matrix)
{
	
}

Fobb& AStalkerKinematics::LL_GetBox(u16 bone_id)
{
	return Bones[bone_id].Obb;
}

const Fbox& AStalkerKinematics::GetBox() const
{
	return VisData.box;
}

void AStalkerKinematics::LL_GetBindTransform(xr_vector<Fmatrix>& matrices)
{
	
}

int AStalkerKinematics::LL_GetBoneGroups(xr_vector<xr_vector<u16>>& groups)
{
	return 0;
}

u16 AStalkerKinematics::LL_GetBoneRoot()
{
	return 0;
}

void AStalkerKinematics::LL_SetBoneRoot(u16 bone_id)
{
	checkSlow(false);
}

BOOL AStalkerKinematics::LL_GetBoneVisible(u16 bone_id)
{
	return SelfBonesVisible.is(bone_id);
}

void AStalkerKinematics::LL_SetBoneVisible(u16 bone_id, BOOL val, BOOL bRecursive)
{
	VERIFY(bone_id < LL_BoneCount());
	SelfBonesVisible.set(bone_id, val);
}

BonesVisible AStalkerKinematics::LL_GetBonesVisible()
{
	return SelfBonesVisible;
}

void AStalkerKinematics::LL_SetBonesVisible(BonesVisible mask)
{
	SelfBonesVisible = mask;
}

void AStalkerKinematics::CalculateBones(BOOL bForceExact /*= FALSE*/)
{
	for (StalkerKinematicsBoneInstance& Bone : BonesInstance)
	{
		if (Bone.callback())
		{
			Bone.callback()(&Bone);
		}
	}
}

void AStalkerKinematics::CalculateBones_Invalidate()
{

}

void AStalkerKinematics::Callback(UpdateCallback C, void* Param)
{
}

void AStalkerKinematics::SetUpdateCallback(UpdateCallback pCallback)
{
}

void AStalkerKinematics::SetUpdateCallbackParam(void* pCallbackParam)
{
}

UpdateCallback AStalkerKinematics::GetUpdateCallback()
{
	return nullptr;
}

void* AStalkerKinematics::GetUpdateCallbackParam()
{
	return nullptr;
}

IRenderVisual* AStalkerKinematics::dcast_RenderVisual()
{
	return this;
}

IKinematicsAnimated* AStalkerKinematics::dcast_PKinematicsAnimated()
{
	return nullptr;
}

void AStalkerKinematics::DebugRender(Fmatrix& XFORM)
{
	
}

shared_str AStalkerKinematics::getDebugName()
{
	FString Name;
	GetName(Name);
	return shared_str(TCHAR_TO_ANSI(*Name));
}

// Called every frame
void AStalkerKinematics::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Renderable)
	{
		//Kinematics->CalculateBones(true);
		Fquaternion XRayQuat;
		XRayQuat.set(Renderable->renderable.xform);
		FQuat Quat(XRayQuat.x, -XRayQuat.z, -XRayQuat.y, XRayQuat.w);
		Fvector InLocation = Renderable->renderable.xform.c;
		FVector Location(-InLocation.x * 100, InLocation.z * 100, InLocation.y * 100);
		SetActorLocationAndRotation(Location, Quat);
	}
}

