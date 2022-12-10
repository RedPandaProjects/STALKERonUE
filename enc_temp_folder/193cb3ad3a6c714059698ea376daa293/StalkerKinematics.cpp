#include "StalkerKinematics.h"
#include "Resources/SkeletonMesh/StalkerKinematicsData.h"
#include "Resources/SkeletonMesh/StalkerKinematicsAnimsData.h"
#include "Animation/AnimSequence.h"
#include "AnimInstance/StalkerKinematicsAnimInstance_Default.h"
THIRD_PARTY_INCLUDES_START
#include "XrRender/Public/RenderVisual.h"
#include "XrEngine/IRenderable.h"
#include "XrCDB/ISpatial.h"
THIRD_PARTY_INCLUDES_END
AStalkerKinematics::AStalkerKinematics()
{
	PrimaryActorTick.bCanEverTick = true;
	KinematicsData = nullptr;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(SceneComponent);
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(GetRootComponent());
	MeshComponent->SetAnimationMode(EAnimationMode::Type::AnimationBlueprint);
	MeshComponent->AnimClass = UStalkerKinematicsAnimInstance_Default::StaticClass();
	//SetHidden(true);
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
	for(u16 i =0;i<LL_BoneCount();i++)
		SelfBonesVisible.set(i,true);

	u32 BonesPartID = 0;
	if (KinematicsData->Anims.Num())
	{
		for (auto& [Name, BonesPart] : KinematicsData->Anims[0]->BonesParts)
		{
			if(BonesPart.Bones.Num() == 0)continue;
			BonesPartsName2ID.Add(TCHAR_TO_ANSI(*Name.ToString()), BonesPartID);
			BonesParts.Add(BonesPart);
			BonesPartID++;
		}
	}

	u32 AnimID = 0;
	for (int32 i = 0; i < KinematicsData->Anims.Num(); i++)
	{
		for (auto& [Name,AnimData ]: KinematicsData->Anims[i]->Anims)
		{
			if (!IsValid(AnimData.Amim))
			{
				checkSlow(false);
				continue;
			}
			if (!AnimData.Amim->GetSkeleton()->IsCompatible(KinematicsData->Mesh->GetSkeleton()))
			{
				checkSlow(false);
				continue;
			}
			AnimsName2ID.Add(TCHAR_TO_ANSI(*Name.ToString()),AnimID);
			Anims.Add(AnimData);
			AnimID++;
			AnimsDef.AddDefaulted();
			CMotionDef& MotionDef =  AnimsDef.Last();
			AnimData.BuildToLegacy(MotionDef,BonesName2ID,BonesPartsName2ID);
			MotionDef.speed = MotionDef.Quantize(AnimData.Amim->RateScale);
		}
	
	}
	LL_SetChannelFactor(0, 1);
	LL_SetChannelFactor(1, 1);
	LL_SetChannelFactor(2, 1);
	LL_SetChannelFactor(3, 1);
	RootBone = 0;
	MeshComponent->SetSkeletalMesh(KinematicsData->Mesh);
	{
		FBox Box = KinematicsData->Mesh->GetBounds().GetBox();

		VisData.box.invalidate();
		VisData.box.modify(StalkerMath::UnrealLocationToXRay(Box.Min));
		VisData.box.modify(StalkerMath::UnrealLocationToXRay(Box.Max));
		Fvector Center;
		VisData.box.getcenter(Center);
		VisData.sphere.set(Center, VisData.box.getradius());
	}
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
	SkipDeltaTime = 0;

}

void AStalkerKinematics::BlendSetup(CBlend& Blend, u32 PartID, u8 Channel, MotionID InMotionID, bool IsMixing, float BlendAccrue, float Speed, bool NoLoop, PlayCallback Callback, LPVOID CallbackParam)
{
	VERIFY(Blend.channel < MAX_CHANNELS);
	if (IsMixing)
	{
		Blend.set_accrue_state();
		Blend.blendAmount = EPS_S;
	}
	else 
	{
		Blend.set_accrue_state();
		Blend.blendAmount = 1;
	}
	Blend.blendAccrue = BlendAccrue;
	Blend.blendFalloff = 0; // 
	Blend.blendPower = 1;
	Blend.speed = Speed;
	Blend.motionID = InMotionID;
	Blend.timeCurrent = 0;
	Blend.timeTotal = Anims[InMotionID.val].Amim->GetPlayLength();
	Blend.bone_or_part = PartID;
	Blend.stop_at_end = NoLoop;
	Blend.playing = TRUE;
	Blend.stop_at_end_callback = TRUE;
	Blend.Callback = Callback;
	Blend.CallbackParam = CallbackParam;

	Blend.channel = Channel;
	Blend.fall_at_end = Blend.stop_at_end && (Channel > 1);
}

void AStalkerKinematics::FXBlendSetup(CBlend& Blend, MotionID InMotionID, float BlendAccrue, float BlendFalloff, float Power, float Speed, u16 Bone)
{
	Blend.set_accrue_state();
	Blend.blendAmount = EPS_S;
	Blend.blendAccrue = BlendAccrue;
	Blend.blendFalloff = BlendFalloff;
	Blend.blendPower = Power;
	Blend.speed = Speed;
	Blend.motionID = InMotionID;
	Blend.timeCurrent = 0;
	Blend.timeTotal = Anims[InMotionID.val].Amim->GetPlayLength();
	Blend.bone_or_part = Bone;

	Blend.playing = TRUE;
	Blend.stop_at_end_callback = TRUE;
	Blend.stop_at_end = FALSE;
	//
	Blend.Callback = 0;
	Blend.CallbackParam = 0;

	Blend.channel = 0;
	Blend.fall_at_end = FALSE;
}

bool AStalkerKinematics::AnimsEqual(IKinematicsAnimated* Animated)
{
	AStalkerKinematics *Left = Animated->dcast_RenderVisual()->CastToAStalkerKinematics();
	if (KinematicsData->Anims.Num() != Left->KinematicsData->Anims.Num())
	{
		return false;
	}
	for (int32 i = 0; i < KinematicsData->Anims.Num(); i++)
	{
		if (KinematicsData->Anims[i] != Left->KinematicsData->Anims[i])
		{
			return false;
		}
	}
	return true;
}

float AStalkerKinematics::LL_GetMotionTime(MotionID id)
{
	if (id.valid())
	{
		return Anims[id.val].Amim->GetPlayLength() / Anims[id.val].Amim->RateScale;
	}
	return 0;
}

class AStalkerKinematics* AStalkerKinematics::CastToAStalkerKinematics()
{
	return this;
}

void AStalkerKinematics::OnCalculateBones()
{
	//throw std::logic_error("The method or operation is not implemented.");
	//MeshComponent->TickAnimation(0,false);
}

std::pair<LPCSTR, LPCSTR> AStalkerKinematics::LL_MotionDefName_dbg(MotionID ID)
{
	return std::pair<LPCSTR, LPCSTR>("","");
}

void AStalkerKinematics::LL_DumpBlends_dbg()
{
	
}

u32 AStalkerKinematics::LL_PartBlendsCount(u32 bone_part_id)
{
	checkSlow(bone_part_id<4);
	return BlendsCycles[bone_part_id].Num();
}

CBlend* AStalkerKinematics::LL_PartBlend(u32 bone_part_id, u32 ID)
{
	checkSlow(bone_part_id < 4);
	return BlendsCycles[bone_part_id][ID].Get();
}

void AStalkerKinematics::LL_IterateBlends(IterateBlendsCallback& callback)
{
	for (int32 PartID = 0; PartID < 4; PartID++)
	{
		for (int32 i = 0; i < BlendsCycles[PartID].Num();i++)
		{
			callback(*BlendsCycles[PartID][i]);
		}
	}
	for (int32 i = 0; i < BlendsFX.Num(); i++)
	{
		callback(*BlendsFX[i]);
	}
}



CMotionDef* AStalkerKinematics::LL_GetMotionDef(MotionID id)
{
	if (id.valid())
	{
		return &AnimsDef[id.val];
	}
	return nullptr;
}


void AStalkerKinematics::LL_BuldBoneMatrixDequatize(const IBoneData* bd, u8 channel_mask, SKeyTable& keys)
{

}

void AStalkerKinematics::LL_BoneMatrixBuild(IBoneInstance& bi, const Fmatrix* parent, const SKeyTable& keys)
{
	
}

IBlendDestroyCallback* AStalkerKinematics::GetBlendDestroyCallback()
{
	return BlendDestroyCallback;
}

void AStalkerKinematics::SetBlendDestroyCallback(IBlendDestroyCallback* cb)
{
	BlendDestroyCallback = cb;
}

void AStalkerKinematics::SetUpdateTracksCalback(IUpdateTracksCallback* callback)
{
	UpdateTracksCallback = callback;
}

IUpdateTracksCallback* AStalkerKinematics::GetUpdateTracksCalback()
{
	return UpdateTracksCallback;
}

MotionID AStalkerKinematics::LL_MotionID(LPCSTR B)
{
	MotionID ResultMotionID;
	u32*ID =  AnimsName2ID.Find(B);
	if (ID)
	{
		ResultMotionID.val = *ID;
	}
	return ResultMotionID;
}

u16 AStalkerKinematics::LL_PartID(LPCSTR B)
{
	u32* ID = BonesPartsName2ID.Find(B);
	if (ID)
	{
		return static_cast<u16>( *ID);
	}
	return BI_NONE;
}

u32 AStalkerKinematics::BonesPartsCount() const
{
	return BonesParts.Num();
}

CBlend* AStalkerKinematics::LL_PlayCycle(u16 BonesPartID, MotionID InMotionID, BOOL bMixing, float blendAccrue, float blendFalloff, float Speed, BOOL noloop, PlayCallback Callback, LPVOID CallbackParam, u8 channel /*= 0*/)
{
	if (!InMotionID.valid())
	{
		return 0;
	}
	if (BI_NONE == BonesPartID) 
	{
		for (u16 i = 0; i < BonesPartsCount(); i++)
		{
			LL_PlayCycle(i, InMotionID, bMixing, blendAccrue, blendFalloff, Speed, noloop, Callback, CallbackParam, channel);
		}
		return 0;
	}
	if (BonesPartID >= BonesPartsCount())	return 0;

	if (channel == 0)
	{
		if (bMixing)	
		{
			for (TSharedPtr<CBlend>&Blend: BlendsCycles[BonesPartID])
			{
				if (Blend->channel != channel)
				{
					continue;
				}
				Blend->set_falloff_state();
				Blend->blendFalloff = blendFalloff;
				if (Blend->stop_at_end) Blend->stop_at_end_callback = FALSE;		// callback не должен приходить!
			}
		}
		else			
		{
			LL_CloseCycle(BonesPartID, 1 << channel);
		}
	}
	BlendsCycles[BonesPartID].Add(MakeShared<CBlend>());
	BlendSetup(*BlendsCycles[BonesPartID].Last(), BonesPartID, channel, InMotionID, bMixing, blendAccrue, Speed, noloop, Callback, CallbackParam);
	return		BlendsCycles[BonesPartID].Last().Get();
}

CBlend* AStalkerKinematics::LL_PlayCycle(u16 BonesPartID, MotionID InMotionID, BOOL bMixIn, PlayCallback Callback, LPVOID CallbackParam, u8 channel /*= 0*/)
{
	checkSlow(InMotionID.valid());
	CMotionDef&MotionDef = AnimsDef[InMotionID.val];
	return LL_PlayCycle(BonesPartID, InMotionID, bMixIn, MotionDef.Accrue(), MotionDef.Falloff(), MotionDef.Speed(), MotionDef.StopAtEnd(),Callback, CallbackParam, channel);
}

void AStalkerKinematics::LL_CloseCycle(u16 BonePartID, u8 mask_channel /*= (1 << 0)*/)
{
	for (int32 i = 0; i < BlendsCycles[BonePartID].Num();)
	{
		if (mask_channel & (1 << BlendsCycles[BonePartID][i]->channel))
		{
			if (GetBlendDestroyCallback())
				GetBlendDestroyCallback()->BlendDestroy(*BlendsCycles[BonePartID][i].Get());
			BlendsCycles[BonePartID][i]->set_free_state();
			BlendsCycles[BonePartID].RemoveAt(i);
		}
		else
		{
			i++;
		}
	}
}

void AStalkerKinematics::LL_SetChannelFactor(u16 channel, float factor)
{
	check(channel<MAX_CHANNELS);
	ChannelsFactor[channel] = factor;
}

void AStalkerKinematics::UpdateTracks()
{

}

void AStalkerKinematics::LL_UpdateTracks(float Delta, bool b_force, bool leave_blends)
{
	for (int32 PartID = 0; PartID < 4; PartID++)
	{
		for (int32 i = 0; i < BlendsCycles[PartID].Num();)
		{
			if (!b_force && BlendsCycles[PartID][i]->dwFrame == Device->dwFrame)
			{
				i++;
				continue;
			}
			BlendsCycles[PartID][i]->dwFrame = Device->dwFrame;
			if (BlendsCycles[PartID][i]->update(Delta, BlendsCycles[PartID][i]->Callback) && !leave_blends)
			{
				if (GetBlendDestroyCallback())
					GetBlendDestroyCallback()->BlendDestroy(*BlendsCycles[PartID][i].Get());
				BlendsCycles[PartID][i]->set_free_state();
				BlendsCycles[PartID].RemoveAt(i);
			}
			else
			{
				i++;
			}
		}
	}

	for (int32 i = 0; i < BlendsFX.Num();)
	{
		CBlend& B = *BlendsFX[i];
		if (!B.stop_at_end_callback)
		{
			B.playing = FALSE;
			i++;
			continue;
		}
		B.update_time(Delta);
		switch (B.blend_state())
		{
		case CBlend::eFREE_SLOT:
			checkSlow(false);
			i++;
			break;
		case CBlend::eAccrue:
			B.blendAmount += Delta * B.blendAccrue * B.blendPower * B.speed;
			if (B.blendAmount >= B.blendPower) 
			{
				B.blendAmount = B.blendPower;
				B.set_falloff_state();
			}
			i++;
			break;
		case CBlend::eFalloff:
			B.blendAmount -= Delta * B.blendFalloff * B.blendPower * B.speed;
			if (B.blendAmount <= 0) 
			{
				B.set_free_state();
				BlendsFX.RemoveAt(i);
			}
			else
			{
				i++;
			}
			break;
		default: 	
			checkSlow(false);
			i++;
			break;
		}
	}
		
}

MotionID AStalkerKinematics::ID_Cycle(LPCSTR Name)
{
	return ID_Cycle(shared_str(Name));
}

MotionID AStalkerKinematics::ID_Cycle(shared_str Name)
{
	MotionID Result = ID_Cycle_Safe(Name);
	check(Result.valid());
	return Result;
}

MotionID AStalkerKinematics::ID_Cycle_Safe(LPCSTR Name)
{
	return ID_Cycle_Safe(shared_str(Name));
}

MotionID AStalkerKinematics::ID_Cycle_Safe(shared_str Name)
{
	MotionID ResultMotionID;
	u32* ID = AnimsName2ID.Find(Name);
	if (ID)
	{
		if (Anims[*ID].Flags & int32(EStalkerKinematicsAnimFlags::FX))
		{
			return ResultMotionID;
		}
		ResultMotionID.val = *ID;
	}
	return ResultMotionID;
}

CBlend* AStalkerKinematics::PlayCycle(LPCSTR Name, BOOL bMixIn /*= TRUE*/, PlayCallback Callback /*= 0*/, LPVOID CallbackParam /*= 0*/, u8 channel /*= 0*/)
{
	return PlayCycle(ID_Cycle(Name),bMixIn, Callback, CallbackParam, channel);
}

CBlend* AStalkerKinematics::PlayCycle(MotionID InMotionID, BOOL bMixIn /*= TRUE*/, PlayCallback Callback /*= 0*/, LPVOID CallbackParam /*= 0*/, u8 channel /*= 0*/)
{
	checkSlow(InMotionID.valid());
	CMotionDef& MotionDef = AnimsDef[InMotionID.val];
	return LL_PlayCycle(MotionDef.bone_or_part, InMotionID, bMixIn, MotionDef.Accrue(), MotionDef.Falloff(), MotionDef.Speed(), MotionDef.StopAtEnd(), Callback, CallbackParam, channel);
}

CBlend* AStalkerKinematics::PlayCycle(u16 BonesPartID, MotionID InMotionID, BOOL bMixIn /*= TRUE*/, PlayCallback Callback /*= 0*/, LPVOID CallbackParam /*= 0*/, u8 channel /*= 0*/)
{
	return LL_PlayCycle(BonesPartID, InMotionID, bMixIn, Callback, CallbackParam, channel);
}

MotionID AStalkerKinematics::ID_FX(LPCSTR Name)
{
	MotionID Result = ID_FX_Safe(Name);
	check(Result.valid());
	return Result;
}

MotionID AStalkerKinematics::ID_FX_Safe(LPCSTR Name)
{
	MotionID ResultMotionID;
	u32* ID = AnimsName2ID.Find(Name);
	if (ID)
	{
		if (!(Anims[*ID].Flags & int32(EStalkerKinematicsAnimFlags::FX)))
		{
			return ResultMotionID;
		}
		ResultMotionID.val = *ID;
	}
	return ResultMotionID;
}

CBlend* AStalkerKinematics::PlayFX(LPCSTR Name, float PowerScale)
{
	MotionID InMotionID = ID_FX(Name);
	return PlayFX(InMotionID,PowerScale);
}

CBlend* AStalkerKinematics::PlayFX(MotionID InMotionID, float PowerScale)
{
	if (!InMotionID.valid())	return 0;
	if (BlendsFX.Num() >= MAX_BLENDED) return 0;
	CMotionDef& MotionDef = AnimsDef[InMotionID.val];

	u16 BoneID  = MotionDef.bone_or_part;
	if (BI_NONE == BoneID)		BoneID = 0;

	BlendsFX.Add(MakeShared<CBlend>());
	FXBlendSetup(*BlendsFX.Last(), InMotionID, MotionDef.Accrue(), MotionDef.Falloff(), MotionDef.Power(), MotionDef.Speed(), BoneID);
	return BlendsFX.Last().Get();
}

float AStalkerKinematics::get_animation_length(MotionID InMotionID)
{
	return LL_GetMotionTime(InMotionID);
	//throw std::logic_error("The method or operation is not implemented.");
}

vis_data& _BCL AStalkerKinematics::getVisData()
{
	return VisData;
}

u32 AStalkerKinematics::getType()
{
	return Anims.Num() ? MT_SKELETON_ANIM : MT_SKELETON_RIGID;
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
	return Bones[bone_id].BindTransformation;
}

const Fmatrix& AStalkerKinematics::LL_GetTransform_R(u16 bone_id)
{
	return Fidentity;
}

void AStalkerKinematics::LL_SetTransform(u16 bone_id, const Fmatrix& Matrix)
{
	UE_DEBUG_BREAK();
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
	matrices.clear();
	for (IBoneData &BoneData : Bones)
	{
		matrices.push_back(BoneData.get_bind_transform());
	}
}


u16 AStalkerKinematics::LL_GetBoneRoot()
{
	return RootBone;
}

void AStalkerKinematics::LL_SetBoneRoot(u16 bone_id)
{
	RootBone = bone_id;
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
	return  Anims.Num() ? this : nullptr;
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
	if(Renderable&& Renderable->MySpatial)
		SetActorHiddenInGame(!(Renderable->MySpatial->spatial.type & STYPE_RENDERABLE)|| (Renderable->MySpatial->spatial.type & STYPE_LIGHTSOURCE));
	else
		SetActorHiddenInGame(true);
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

	{

		if (GetUpdateTracksCalback())
		{
			SkipDeltaTime += DeltaTime;
			if ((*GetUpdateTracksCalback())(DeltaTime, *this))
				SkipDeltaTime = 0;
			return;
		}
		LL_UpdateTracks(DeltaTime+ SkipDeltaTime, false, false);
		SkipDeltaTime = 0;
	}
}

