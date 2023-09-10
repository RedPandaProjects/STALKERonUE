#include "StalkerKinematicsComponent.h"
#include "Resources/SkeletonMesh/StalkerKinematicsData.h"
#include "Resources/SkeletonMesh/StalkerKinematicsAnimsData.h"
#include "Animation/AnimSequence.h"
#include "AnimInstance/StalkerKinematicsAnimInstance_Default.h"
#include "AnimInstance/StalkerKinematicsAnimInstanceProxy.h"
#include "Kernel/StalkerEngineManager.h"
#include "Resources/StalkerResourcesManager.h"
THIRD_PARTY_INCLUDES_START
#include "XrRender/Public/RenderVisual.h"
#include "XrEngine/IRenderable.h"
#include "XrCDB/ISpatial.h"
#include "XrEngine/EnnumerateVertices.h"
THIRD_PARTY_INCLUDES_END

DECLARE_CYCLE_STAT(TEXT("XRay ~ Kinematics Frame"), STAT_XRayEngineKinematics, STATGROUP_XRayEngine);
UStalkerKinematicsComponent::UStalkerKinematicsComponent()
{
	BlendDestroyCallback = nullptr;
	UpdateTracksCallback = nullptr;
	MyUpdateCallback = nullptr;
	UpdateCallbackParam = nullptr;
	SetAnimationMode(EAnimationMode::Type::AnimationBlueprint);
	AnimClass = UStalkerKinematicsAnimInstance_Default::StaticClass();
	SkipDeltaTime = 0;
#if WITH_EDITORONLY_DATA
	bIsErrorMesh = false;
#endif
}

void UStalkerKinematicsComponent::Initilize(class UStalkerKinematicsData* InKinematicsData)
{
	if (KinematicsData)
	{
		ClearAnimScriptInstance();
	
		for (TArray <TSharedPtr<CBlend>>& InBlendsCycles : BlendsCycles)
		{
			InBlendsCycles.Empty();
		}
		for (TArray <TSharedPtr<CBlend>>& InBlendsFX : BlendsFX)
		{
			InBlendsFX.Empty();
		}
	
		Anims.Empty();
		SkipDeltaTime = 0;
		BonesInstance.Empty();
		Bones.Empty();
		BonesName2ID.Empty();
		BonesID2Name.Empty();
		VisData.clear();
		BlendsPool.Empty();
		UserData.Reset();
		SelfBonesVisible.zero();
		AnimsDef.Empty();
		AnimsName2ID.Empty();
		BonesPartsName2ID.Empty();
		BonesPartsBoneID2ID.Empty();
		BonesParts.Empty();
		DelayUpdateVisData = -1;
		DataName = "";
#if WITH_EDITORONLY_DATA
		bIsErrorMesh = false;
#endif
		BlendDestroyCallback  = nullptr;
		UpdateTracksCallback = nullptr;
		MyUpdateCallback = nullptr;
		UpdateCallbackParam = nullptr;
		if (KinematicsAnimInstanceForCompute)
		{
			KinematicsAnimInstanceForCompute->MarkAsGarbage();
		}
		KinematicsAnimInstanceForCompute = nullptr;
		SetSkeletalMesh(nullptr);
	}
	KinematicsData = InKinematicsData;
	if (!KinematicsData)
	{
		return;
	}
#if WITH_EDITORONLY_DATA
	bIsErrorMesh =  InKinematicsData->GetPathName() == TEXT("/Game/Base/Meshes/Error_KinematicsData.Error_KinematicsData");
#endif
	for (float& Factor : ChannelsFactor)
	{
		Factor = 0;
	}
	ChannelsFactor[0] = 1;

	KinematicsData->BuildBones(Bones);
	BonesInstance.AddDefaulted(Bones.Num());
	if (KinematicsData->UserData.Len()&&FApp::IsGame())
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
			BonesPartsName2ID.Add(TCHAR_TO_ANSI(*Name.ToString().ToLower()), BonesPartID);
			BonesParts.Add(BonesPart);
			BonesPartID++;
		}
	}
	int32 BonesPartsID = 0;
	for (FStalkerKinematicsAnimsBonesPart& BonesPart : BonesParts)
	{
		for (FName& BoneOfPart : BonesPart.Bones)
		{
			shared_str BoneName = TCHAR_TO_ANSI(*BoneOfPart.ToString().ToLower());
			BonesPartsBoneID2ID.Add(BonesName2ID[BoneName], BonesPartsID);
		}
		BonesPartsID++;
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
			AnimsName2ID.Add(TCHAR_TO_ANSI(*Name.ToString().ToLower()),AnimID);
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
	for (int32 i = 0; i < BonesInstance.Num(); i++)
	{
		Fmatrix Parent; Parent.identity();
		if (Bones[i].GetParentID() != BI_NONE)
		{
			Parent = BonesInstance[Bones[i].GetParentID()].GetTransform();
		}
		BonesInstance[i].Transform.mul_43(Parent,Bones[i].get_bind_transform());
	}
	SetSkeletalMesh(KinematicsData->Mesh);
	{
		FBox Box = KinematicsData->Mesh->GetBounds().GetBox();

		VisData.box.invalidate();
		VisData.box.modify(StalkerMath::UnrealLocationToXRay(Box.Min));
		VisData.box.modify(StalkerMath::UnrealLocationToXRay(Box.Max));
		Fvector Center;
		VisData.box.getcenter(Center);
		VisData.sphere.set(Center, VisData.box.getradius());
	}
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FString InDataName;
	InKinematicsData->GetName(InDataName);
	DataName = TCHAR_TO_ANSI(*InDataName);
	TickAnimation(0,false);
}

#if WITH_EDITOR
void UStalkerKinematicsComponent::InitilizeEditor()
{
	bUpdateAnimationInEditor = true;
	InitAnim(false);
}
#endif

void UStalkerKinematicsComponent::BlendSetup(CBlend& Blend, u32 PartID, u8 Channel, MotionID InMotionID, bool IsMixing, float BlendAccrue, float Speed, bool NoLoop, PlayCallback Callback, LPVOID CallbackParam)
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

void UStalkerKinematicsComponent::FXBlendSetup(CBlend& Blend, MotionID InMotionID, float BlendAccrue, float BlendFalloff, float Power, float Speed, u16 Bone)
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

TSharedPtr<CBlend> UStalkerKinematicsComponent::CreateBlend()
{
	if (BlendsPool.Num())
	{
		return BlendsPool.Pop();
	}
	return MakeShared<CBlend>();
}

void UStalkerKinematicsComponent::DestroyBlend(TSharedPtr<CBlend>& Blend)
{
	BlendsPool.Add(Blend);
}

void* UStalkerKinematicsComponent::CastUnrealObject(EXRayUnrealObjectType ObjectType)
{
	switch (ObjectType)
	{
	case EXRayUnrealObjectType::Object:
		return static_cast<UObject*>(this);
	case EXRayUnrealObjectType::SceneComponent:
		return static_cast<USceneComponent*>(this);
	case EXRayUnrealObjectType::StalkerKinematicsComponent:
		return this;
	default: 
		return nullptr;
	}
}

void* UStalkerKinematicsComponent::QueryInterface(EXRayUnrealInterfaceType AttachableType)
{
	switch (AttachableType)
	{
	case EXRayUnrealInterfaceType::Kinematics:
		return static_cast<IKinematics*>(this);
	case EXRayUnrealInterfaceType::KinematicsAnimated:
		return Anims.Num()>0? static_cast<IKinematicsAnimated*>(this):nullptr;
	default: ;
		return IRenderVisual::QueryInterface(AttachableType);
	}
}

void UStalkerKinematicsComponent::SetOwnerNoSee(bool Enable)
{
	Super::SetOwnerNoSee(Enable);
}

void UStalkerKinematicsComponent::SetOnlyOwnerSee(bool Enable)
{
	Super::SetOnlyOwnerSee(Enable);
}

void UStalkerKinematicsComponent::SetOffset(const Fmatrix& offset)
{
	SetRelativeTransform(FTransform(StalkerMath::XRayMatrixToUnreal( offset)));
}

void UStalkerKinematicsComponent::GetWorldTransform(Fmatrix& OutXForm)
{
	OutXForm = StalkerMath::UnrealMatrixToXRay(GetComponentToWorld().ToMatrixWithScale());
}

void UStalkerKinematicsComponent::Lock(void* InXRayParent)
{
	check(XRayParent == nullptr);
	check(InXRayParent);
	XRayParent = InXRayParent;
}

void UStalkerKinematicsComponent::Unlock(void* InXRayParent)
{
	check(XRayParent == InXRayParent);
	XRayParent = nullptr;
}

void UStalkerKinematicsComponent::Detach()
{
	if (IsRegistered())
	{
		FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, false);
		DetachFromComponent(DetachmentTransformRules);
		UnregisterComponent();
	}

	GStalkerEngineManager->GetResourcesManager()->RegisterKinematics(this);
}

void UStalkerKinematicsComponent::AttachTo(XRayUnrealAttachableInterface* AttachableInterface, const char* BoneName)
{
	USceneComponent* InSceneComponent = reinterpret_cast<USceneComponent*>(AttachableInterface->CastUnrealObject(EXRayUnrealObjectType::SceneComponent));
	check(InSceneComponent);
	AActor* ActorOwner = InSceneComponent->GetOwner();
	check(ActorOwner);
	GStalkerEngineManager->GetResourcesManager()->UnregisterKinematics(this);
	Rename(nullptr,ActorOwner);
	//Set
	FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
	AttachToComponent(InSceneComponent,AttachmentTransformRules,BoneName);
	SetSimulatePhysics(false);
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RegisterComponent();
	
	if (UPrimitiveComponent* PrimitiveComponen = Cast<UPrimitiveComponent>(InSceneComponent))
	{
		SetOwnerNoSee(PrimitiveComponen->bOwnerNoSee);
		SetOnlyOwnerSee(PrimitiveComponen->bOnlyOwnerSee);
	}
	
	ActorOwner->AddInstanceComponent(this);
}

void UStalkerKinematicsComponent::BeginDestroy()
{
	check(XRayParent == nullptr);
	Super::BeginDestroy();
}

void UStalkerKinematicsComponent::Lock(CObject*InObject)
{
	Lock((void*)InObject);
}

void UStalkerKinematicsComponent::PostLoad()
{
	Super::PostLoad();
	UStalkerKinematicsData *InKinematicsData = KinematicsData;
	if (InKinematicsData)
	{
		Initilize(nullptr);
		Initilize(InKinematicsData);
	}
}

shared_str UStalkerKinematicsComponent::GetNameData()
{
	if (KinematicsData)
	{
		return DataName;
	}
	return "";
}


float UStalkerKinematicsComponent::LL_GetMotionTime(MotionID id)
{
	if (id.valid())
	{
		return Anims[id.val].Amim->GetPlayLength() / Anims[id.val].Amim->RateScale;
	}
	return 0;
}

void UStalkerKinematicsComponent::OnCalculateBones()
{
	//throw std::logic_error("The method or operation is not implemented.");
	//MeshComponent->TickAnimation(0,false);
}

std::pair<LPCSTR, LPCSTR> UStalkerKinematicsComponent::LL_MotionDefName_dbg(MotionID ID)
{
	return std::pair<LPCSTR, LPCSTR>("","");
}

void UStalkerKinematicsComponent::LL_DumpBlends_dbg()
{
	
}

u32 UStalkerKinematicsComponent::LL_PartBlendsCount(u32 bone_part_id)
{
	checkSlow(bone_part_id<4);
	return BlendsCycles[bone_part_id].Num();
}

CBlend* UStalkerKinematicsComponent::LL_PartBlend(u32 bone_part_id, u32 ID)
{
	checkSlow(bone_part_id < 4);
	return BlendsCycles[bone_part_id][ID].Get();
}

void UStalkerKinematicsComponent::GetBoneInMotion(Fmatrix& OutPosition, u16 BoneID, CBlend* InBlend)
{
	bool Result = false;
#if WITH_EDITOR
	for (int32 BonesPartID = 0; BonesPartID < 4&&!Result; BonesPartID++)
	{
		for (TSharedPtr<CBlend>& Blend : BlendsCycles[BonesPartID])
		{
			if (Blend.Get() == InBlend)
			{
				Result = true;
				break;
			}
		}
	}

	if (!Result)
	{
		check(false);
		OutPosition.identity();
		return;
	}
#endif
	TArray<FTransform>	BonesTransform;
	TArray<FTransform>	OutBoneSpaceTransforms;
	FBlendedHeapCurve   OutCurve;
	FVector				OutRootBoneTranslation;

	if (!IsValid(KinematicsAnimInstanceForCompute))
	{
		KinematicsAnimInstanceForCompute = NewObject< UStalkerKinematicsAnimInstance_Default>(this, TEXT("KinematicsAnimInstance"), RF_Transient);
		KinematicsAnimInstanceForCompute->InitializeAnimation();
	}
	KinematicsAnimInstanceForCompute->GetProxy().GetKinematicsRootNode().SetAnimMode(EStalkerKinematicsAnimMode::GetBoneInMotionBlend);
	KinematicsAnimInstanceForCompute->GetProxy().GetKinematicsRootNode().GetBoneInMotionBlend = InBlend;

	BonesTransform.AddDefaulted(Bones.Num());

	PerformAnimationProcessing(SkeletalMesh, KinematicsAnimInstanceForCompute,true, BonesTransform, OutBoneSpaceTransforms, OutRootBoneTranslation, OutCurve);

	OutPosition.rotation(StalkerMath::UnrealQuatToXRay(BonesTransform[BoneID].GetRotation()));
	OutPosition.translate_over(StalkerMath::UnrealLocationToXRay(BonesTransform[BoneID].GetTranslation()));
}

void UStalkerKinematicsComponent::LL_IterateBlends(IterateBlendsCallback& callback)
{
	for (int32 PartID = 0; PartID < 4; PartID++)
	{
		for (int32 i = 0; i < BlendsCycles[PartID].Num();i++)
		{
			callback(*BlendsCycles[PartID][i]);
		}
		for (int32 i = 0; i < BlendsFX[PartID].Num(); i++)
		{
			callback(*BlendsFX[PartID][i]);
		}
	}

}



CMotionDef* UStalkerKinematicsComponent::LL_GetMotionDef(MotionID id)
{
	if (id.valid())
	{
		return &AnimsDef[id.val];
	}
	return nullptr;
}


IBlendDestroyCallback* UStalkerKinematicsComponent::GetBlendDestroyCallback()
{
	return BlendDestroyCallback;
}

void UStalkerKinematicsComponent::SetBlendDestroyCallback(IBlendDestroyCallback* cb)
{
	BlendDestroyCallback = cb;
}

void UStalkerKinematicsComponent::SetUpdateTracksCalback(IUpdateTracksCallback* callback)
{
	UpdateTracksCallback = callback;
}

IUpdateTracksCallback* UStalkerKinematicsComponent::GetUpdateTracksCalback()
{
	return UpdateTracksCallback;
}

MotionID UStalkerKinematicsComponent::LL_MotionID(LPCSTR B)
{
	MotionID ResultMotionID;
	u32*ID =  AnimsName2ID.Find(B);
	if (ID)
	{
		ResultMotionID.val = *ID;
	}
	return ResultMotionID;
}

u16 UStalkerKinematicsComponent::LL_PartID(LPCSTR B)
{
	u32* ID = BonesPartsName2ID.Find(B);
	if (ID)
	{
		return static_cast<u16>( *ID);
	}
	return BI_NONE;
}

u32 UStalkerKinematicsComponent::BonesPartsCount() const
{
	return BonesParts.Num();
}

CBlend* UStalkerKinematicsComponent::LL_PlayCycle(u16 BonesPartID, MotionID InMotionID, BOOL bMixing, float blendAccrue, float blendFalloff, float Speed, BOOL noloop, PlayCallback Callback, LPVOID CallbackParam, u8 channel /*= 0*/)
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
				if (Blend->stop_at_end) Blend->stop_at_end_callback = FALSE;		// callback íå äîëæåí ïðèõîäèòü!
			}
		}
		else			
		{
			LL_CloseCycle(BonesPartID, 1 << channel);
		}
	}
	BlendsCycles[BonesPartID].Add(CreateBlend());
	BlendSetup(*BlendsCycles[BonesPartID].Last(), BonesPartID, channel, InMotionID, bMixing, blendAccrue, Speed, noloop, Callback, CallbackParam);
	return		BlendsCycles[BonesPartID].Last().Get();
}

CBlend* UStalkerKinematicsComponent::LL_PlayCycle(u16 BonesPartID, MotionID InMotionID, BOOL bMixIn, PlayCallback Callback, LPVOID CallbackParam, u8 channel /*= 0*/)
{
	checkSlow(InMotionID.valid());
	CMotionDef&MotionDef = AnimsDef[InMotionID.val];
	return LL_PlayCycle(BonesPartID, InMotionID, bMixIn, MotionDef.Accrue(), MotionDef.Falloff(), MotionDef.Speed(), MotionDef.StopAtEnd(),Callback, CallbackParam, channel);
}

void UStalkerKinematicsComponent::LL_CloseCycle(u16 BonePartID, u8 mask_channel /*= (1 << 0)*/)
{
	for (int32 i = 0; i < BlendsCycles[BonePartID].Num();)
	{
		if (mask_channel & (1 << BlendsCycles[BonePartID][i]->channel))
		{
			if (GetBlendDestroyCallback())
				GetBlendDestroyCallback()->BlendDestroy(*BlendsCycles[BonePartID][i].Get());
			BlendsCycles[BonePartID][i]->set_free_state();
			DestroyBlend(BlendsCycles[BonePartID][i]);
			BlendsCycles[BonePartID].RemoveAt(i);
		}
		else
		{
			i++;
		}
	}
}

void UStalkerKinematicsComponent::LL_SetChannelFactor(u16 channel, float factor)
{
	check(channel<MAX_CHANNELS);
	ChannelsFactor[channel] = factor;
}

void UStalkerKinematicsComponent::UpdateTracks()
{

}

void UStalkerKinematicsComponent::LL_UpdateTracks(float Delta, bool b_force, bool leave_blends)
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
				DestroyBlend(BlendsCycles[PartID][i]);
				BlendsCycles[PartID].RemoveAt(i);
			}
			else
			{
				i++;
			}
		}
		for (int32 i = 0; i < BlendsFX[PartID].Num();)
		{
			CBlend& B = *BlendsFX[PartID][i];
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
				if (B.blendAmount == 0)
				{
					B.set_free_state();
					DestroyBlend(BlendsFX[PartID][i]);
					BlendsFX[PartID].RemoveAt(i);
				}
				else
				{
					i++;
				}
				B.blendAmount -= Delta * B.blendFalloff * B.blendPower * B.speed;
				B.blendAmount  = FMath::Max(0,B.blendAmount);
				break;
			default:
				checkSlow(false);
				i++;
				break;
			}
		}
	}

	
		
}

MotionID UStalkerKinematicsComponent::ID_Cycle(LPCSTR Name)
{
	return ID_Cycle(shared_str(Name));
}

MotionID UStalkerKinematicsComponent::ID_Cycle(shared_str Name)
{
	MotionID Result = ID_Cycle_Safe(Name);
	check(Result.valid());
	return Result;
}

MotionID UStalkerKinematicsComponent::ID_Cycle_Safe(LPCSTR Name)
{
	return ID_Cycle_Safe(shared_str(Name));
}

MotionID UStalkerKinematicsComponent::ID_Cycle_Safe(shared_str Name)
{
#if WITH_EDITORONLY_DATA
	if (bIsErrorMesh)
	{
		MotionID ResultMotionID;
		ResultMotionID.val = 0;
		return ResultMotionID;
	}
#endif
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

CBlend* UStalkerKinematicsComponent::PlayCycle(LPCSTR Name, BOOL bMixIn /*= TRUE*/, PlayCallback Callback /*= 0*/, LPVOID CallbackParam /*= 0*/, u8 channel /*= 0*/)
{
	return PlayCycle(ID_Cycle(Name),bMixIn, Callback, CallbackParam, channel);
}

CBlend* UStalkerKinematicsComponent::PlayCycle(MotionID InMotionID, BOOL bMixIn /*= TRUE*/, PlayCallback Callback /*= 0*/, LPVOID CallbackParam /*= 0*/, u8 channel /*= 0*/)
{
	checkSlow(InMotionID.valid());
	CMotionDef& MotionDef = AnimsDef[InMotionID.val];
	return LL_PlayCycle(MotionDef.bone_or_part, InMotionID, bMixIn, MotionDef.Accrue(), MotionDef.Falloff(), MotionDef.Speed(), MotionDef.StopAtEnd(), Callback, CallbackParam, channel);
}

CBlend* UStalkerKinematicsComponent::PlayCycle(u16 BonesPartID, MotionID InMotionID, BOOL bMixIn /*= TRUE*/, PlayCallback Callback /*= 0*/, LPVOID CallbackParam /*= 0*/, u8 channel /*= 0*/)
{
	return LL_PlayCycle(BonesPartID, InMotionID, bMixIn, Callback, CallbackParam, channel);
}
#if WITH_EDITOR
void UStalkerKinematicsComponent::EditorPlay(MotionID InMotionID, bool InLoop)
{
	if(!bUpdateAnimationInEditor)
	{
		InitilizeEditor();
	}
	checkSlow(InMotionID.valid());
	CMotionDef& MotionDef = AnimsDef[InMotionID.val];
	LL_PlayCycle(MotionDef.bone_or_part, InMotionID, FALSE, MotionDef.Accrue(), MotionDef.Falloff(), MotionDef.Speed(), !InLoop, nullptr, nullptr, 0);
}
#endif
MotionID UStalkerKinematicsComponent::ID_FX(LPCSTR Name)
{
	MotionID Result = ID_FX_Safe(Name);
	check(Result.valid());
	return Result;
}

MotionID UStalkerKinematicsComponent::ID_FX_Safe(LPCSTR Name)
{
#if WITH_EDITORONLY_DATA
	if (bIsErrorMesh)
	{
		MotionID ResultMotionID;
		ResultMotionID.val = 0;
		return ResultMotionID;
	}
#endif
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

CBlend* UStalkerKinematicsComponent::PlayFX(LPCSTR Name, float PowerScale)
{
	MotionID InMotionID = ID_FX(Name);
	return PlayFX(InMotionID,PowerScale);
}

CBlend* UStalkerKinematicsComponent::PlayFX(MotionID InMotionID, float PowerScale)
{
	if (!InMotionID.valid())	return 0;
	CMotionDef& MotionDef = AnimsDef[InMotionID.val];
	
	u16 BoneID  = MotionDef.bone_or_part;
	if (BI_NONE == BoneID)		BoneID = 0;
	u32 BonesPartID = BonesPartsBoneID2ID[BoneID];
	if (BlendsFX[BonesPartID].Num() >= MAX_BLENDED) return 0;
	BlendsFX[BonesPartID].Add(CreateBlend());
	FXBlendSetup(*BlendsFX[BonesPartID].Last(), InMotionID, MotionDef.Accrue(), MotionDef.Falloff(), MotionDef.Power(), MotionDef.Speed(), BoneID);
	return BlendsFX[BonesPartID].Last().Get();
}

float UStalkerKinematicsComponent::get_animation_length(MotionID InMotionID)
{
	return LL_GetMotionTime(InMotionID);
	//throw std::logic_error("The method or operation is not implemented.");
}

vis_data& _BCL UStalkerKinematicsComponent::getVisData()
{
	return VisData;
}


IKinematics* _BCL UStalkerKinematicsComponent::dcast_PKinematics()
{
	return this;
}

void UStalkerKinematicsComponent::Bone_Calculate(const IBoneData* bd, const Fmatrix* parent)
{
	
}

void UStalkerKinematicsComponent::Bone_GetAnimPos(Fmatrix& pos, u16 id, u8 channel_mask, bool ignore_callbacks)
{
	pos = LL_GetTransform(id);
	if (!ignore_callbacks)
	{
		for (int32 BoneID = 0; BoneID < Bones.Num(); BoneID++)
		{
			if (LL_GetBoneVisible(u16(BoneID)))
			{
				StalkerKinematicsBoneInstance& BoneInstance = BonesInstance[BoneID];
				if (BoneInstance.callback_overwrite())
				{
					if (BoneInstance.callback())
						BoneInstance.callback()(&BoneInstance);
				}
				else
				{
					if (BoneInstance.callback())
					{
						BoneInstance.callback()(&BoneInstance);
					}
				}
			}
		}
	}
	/*TArray<FTransform>	BonesTransform;
	TArray<FTransform>	OutBoneSpaceTransforms;
	FBlendedHeapCurve   OutCurve;
	FVector				OutRootBoneTranslation;
	KinematicsAnimInstanceForCompute->GetProxy().GetKinematicsRootNode().SetAnimMode(EStalkerKinematicsAnimMode::GetAnimPose);
	KinematicsAnimInstanceForCompute->GetProxy().GetKinematicsRootNode().GetAnimPosIgnoreCallbacks = ignore_callbacks;
	KinematicsAnimInstanceForCompute->GetProxy().GetKinematicsRootNode().GetAnimPosChannelMask = channel_mask;

	BonesTransform.AddDefaulted(Bones.Num());

	MeshComponent->PerformAnimationProcessing(MeshComponent->SkeletalMesh, KinematicsAnimInstanceForCompute,true, BonesTransform, OutBoneSpaceTransforms, OutRootBoneTranslation, OutCurve);

	pos.rotation(StalkerMath::UnrealQuatToXRay(BonesTransform[id].GetRotation()));
	pos.translate_over(StalkerMath::UnrealLocationToXRay(BonesTransform[id].GetTranslation()));*/
}

bool UStalkerKinematicsComponent::PickBone(const Fmatrix& parent_xform, pick_result& r, float dist, const Fvector& start, const Fvector& dir, u16 bone_id)
{
	//ÍÀÄÎ ÍÀÑÒÐÎÈÒÜ !!!!
	return false;
	//FCollisionQueryParams CollisionQueryParams;
	//FCollisionResponseParams CollisionResponseParams = FCollisionResponseParams::DefaultResponseParam;
	//CollisionResponseParams.CollisionResponse.SetAllChannels(ECR_Overlap);

	//Fvector InStartLocation = start;
	//Fvector InStartRotation = dir;

	////Fmatrix P;	P.invert(parent_xform);
	////P.transform_tiny(InStartLocation, start);

	//FName BoneName = Bones[bone_id].GetName().c_str();
	//FHitResult HitResult;
	//if (GetWorld()->LineTraceSingleByChannel(HitResult, FVector(StalkerMath::XRayLocationToUnreal(InStartLocation)), FVector(StalkerMath::XRayLocationToUnreal(InStartLocation)) + FVector(-InStartRotation.x, InStartRotation.z, InStartRotation.y) * (dist * 100.f), ECC_WorldDynamic, CollisionQueryParams, CollisionResponseParams))
	//{
	//	if (HitResult.GetActor() == this&& HitResult.BoneName == BoneName)
	//	{
	//	//	DrawDebugLine(GetWorld(), FVector(StalkerMath::XRayLocationToUnreal(InStartLocation)), FVector(StalkerMath::XRayLocationToUnreal(InStartLocation)) + FVector(-InStartRotation.x, InStartRotation.z, InStartRotation.y) * (dist * 100.f), FColor::Green, false, 10000.f);
	//		//DrawDebugSphere(GetWorld(), FVector(StalkerMath::XRayLocationToUnreal(InStartLocation)), 1000, 0, FColor::Green, false, 1000);

	//		r.dist = HitResult.Distance /100.f;
	//		r.normal = Fvector().set(- HitResult.Normal.X, HitResult.Normal.Z, HitResult.Normal.Y);
	//		return true;
	//	}
	//}
	////DrawDebugLine(GetWorld(), FVector(StalkerMath::XRayLocationToUnreal(InStartLocation)), FVector(StalkerMath::XRayLocationToUnreal(InStartLocation)) + FVector(-InStartRotation.x, InStartRotation.z, InStartRotation.y) * (dist * 100.f), FColor::Red, false, 10000.f);
	////DrawDebugSphere(GetWorld(), FVector(StalkerMath::XRayLocationToUnreal(InStartLocation)), 1000, 0, FColor::Red, false, 1000);

	return false;
}

void UStalkerKinematicsComponent::EnumBoneVertices(SEnumVerticesCallback& C, u16 bone_id)
{
	FSkeletalMeshRenderData* RenderResource = KinematicsData->Mesh->GetResourceForRendering();
	FStaticMeshVertexBuffers* StaticVertexBuffers = &RenderResource->LODRenderData[0].StaticVertexBuffers;
	FPositionVertexBuffer* PositionVertexBuffer = &StaticVertexBuffers->PositionVertexBuffer;
	FSkinWeightVertexBuffer* SkinWeightVertexBuffer = RenderResource->LODRenderData[0].GetSkinWeightVertexBuffer();
	for (uint32 i = 0; SkinWeightVertexBuffer->GetNumVertices() > i; i++)
	{
		FSkinWeightInfo SkinWeightInfo = SkinWeightVertexBuffer->GetVertexSkinWeights(i);
	
		for (int32 a = 0;12 > a; a++)
		{
			if (SkinWeightInfo.InfluenceWeights[a]>30)
			{
				if (SkinWeightInfo.InfluenceBones[a] == bone_id)
				{
					FVector3f InVertex = PositionVertexBuffer->VertexPosition(i);
					C(StalkerMath::UnrealLocationToXRay(InVertex));
				}
			}
		}
	}

}

u16 UStalkerKinematicsComponent::LL_BoneID(LPCSTR B)
{
#if WITH_EDITORONLY_DATA
	if (bIsErrorMesh)
	{
		return 0;
	}
#endif
	u16 *ID = BonesName2ID.Find(B);
	return ID?*ID:BI_NONE;
}

u16 UStalkerKinematicsComponent::LL_BoneID(const shared_str& B)
{
#if WITH_EDITORONLY_DATA
	if (bIsErrorMesh)
	{
		return 0;
	}
#endif
	u16* ID = BonesName2ID.Find(B);
	return ID ? *ID : BI_NONE;
}

LPCSTR UStalkerKinematicsComponent::LL_BoneName_dbg(u16 ID)
{
	shared_str* Name = BonesID2Name.Find(ID);
	return Name ? Name->c_str() : "";
}

CInifile* UStalkerKinematicsComponent::LL_UserData()
{
	return UserData.Get();
}

IBoneInstance& UStalkerKinematicsComponent::LL_GetBoneInstance(u16 bone_id)
{
	return BonesInstance[bone_id];
}

const IBoneData& UStalkerKinematicsComponent::GetBoneData(u16 bone_id) const
{
	return Bones[bone_id];
}

u16 UStalkerKinematicsComponent::LL_BoneCount() const
{
	return Bones.Num();
}

u16 UStalkerKinematicsComponent::LL_VisibleBoneCount()
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

const Fmatrix& UStalkerKinematicsComponent::LL_GetTransform(u16 bone_id) const
{
	return BonesInstance[bone_id].GetTransform();
}

const Fmatrix& UStalkerKinematicsComponent::LL_GetTransform_R(u16 bone_id)
{
	return BonesInstance[bone_id].GetTransform();
}

void UStalkerKinematicsComponent::LL_SetTransform(u16 bone_id, const Fmatrix& Matrix)
{
	if (BonesInstance[bone_id].callback_overwrite())
	{
		BonesInstance[bone_id].SetTransform(Matrix);
	}
}

Fobb& UStalkerKinematicsComponent::LL_GetBox(u16 bone_id)
{
	return Bones[bone_id].Obb;
}

const Fbox& UStalkerKinematicsComponent::GetBox() const
{
	return VisData.box;
}

void UStalkerKinematicsComponent::LL_GetBindTransform(xr_vector<Fmatrix>& matrices)
{
	matrices.clear();
	for (IBoneData &BoneData : Bones)
	{
		matrices.push_back(BoneData.get_bind_transform());
	}
}


u16 UStalkerKinematicsComponent::LL_GetBoneRoot()
{
	return RootBone;
}

void UStalkerKinematicsComponent::LL_SetBoneRoot(u16 bone_id)
{
	RootBone = bone_id;
}

BOOL UStalkerKinematicsComponent::LL_GetBoneVisible(u16 bone_id)
{
	return SelfBonesVisible.is(bone_id);
}

void UStalkerKinematicsComponent::LL_SetBoneVisible(u16 bone_id, BOOL val, BOOL bRecursive)
{
	VERIFY(bone_id < LL_BoneCount());
	SelfBonesVisible.set(bone_id, val);
}

BonesVisible UStalkerKinematicsComponent::LL_GetBonesVisible()
{
	return SelfBonesVisible;
}

void UStalkerKinematicsComponent::LL_SetBonesVisible(BonesVisible mask)
{
	SelfBonesVisible = mask;
}

void UStalkerKinematicsComponent::CalculateBones(BOOL bForceExact /*= FALSE*/)
{
	if (bForceExact)
	{
		for (int32 BoneID = 0; BoneID < Bones.Num(); BoneID++)
		{
			if (LL_GetBoneVisible(u16(BoneID)))
			{
				StalkerKinematicsBoneInstance& BoneInstance = BonesInstance[BoneID];
				if (BoneInstance.callback_overwrite())
				{
					if (BoneInstance.callback())
						BoneInstance.callback()(&BoneInstance);
				}
				else
				{
					if (BoneInstance.callback())
					{
						BoneInstance.callback()(&BoneInstance);
					}
				}
			}
		}
		if (MyUpdateCallback)	MyUpdateCallback(this);
	}
	
}

void UStalkerKinematicsComponent::CalculateBones_Invalidate()
{

}

void UStalkerKinematicsComponent::Callback(UpdateCallback C, void* Param)
{
	MyUpdateCallback = C;
	UpdateCallbackParam = Param;
}

void UStalkerKinematicsComponent::SetUpdateCallback(UpdateCallback pCallback)
{
	MyUpdateCallback = pCallback;
}

void UStalkerKinematicsComponent::SetUpdateCallbackParam(void* pCallbackParam)
{
	UpdateCallbackParam = pCallbackParam;
}

UpdateCallback UStalkerKinematicsComponent::GetUpdateCallback()
{
	return MyUpdateCallback;
}

void* UStalkerKinematicsComponent::GetUpdateCallbackParam()
{
	return UpdateCallbackParam;
}

IRenderVisual* UStalkerKinematicsComponent::dcast_RenderVisual()
{
	return this;
}

IKinematicsAnimated* UStalkerKinematicsComponent::dcast_PKinematicsAnimated()
{
	return  Anims.Num() ? this : nullptr;
}

void UStalkerKinematicsComponent::DebugRender(Fmatrix& XFORM)
{
	
}

shared_str UStalkerKinematicsComponent::getDebugName()
{
	FString Name;
	GetName(Name);
	return shared_str(TCHAR_TO_ANSI(*Name));
}

bool UStalkerKinematicsComponent::IsAttached(XRayUnrealAttachableInterface* Attach)
{
	if(USceneComponent*RootComp = reinterpret_cast<USceneComponent*>(Attach->CastUnrealObject(EXRayUnrealObjectType::SceneComponent)))
	{
		return IsAttachedTo(RootComp);
	}
	return false;
}

void UStalkerKinematicsComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	SCOPE_CYCLE_COUNTER(STAT_XRayEngineKinematics);
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	Fbox Box;Box.invalidate();
	if(DelayUpdateVisData<0)
	{
		if(GetWorld()->IsGameWorld())
		{
			for(const StalkerKinematicsBone&Bone:Bones)
			{
				Fmatrix&	Mbone	= BonesInstance[Bone.SelfID].Transform;
				Fmatrix		Mbox;	Bone.Obb.xform_get(Mbox);
				Fmatrix		X;		X.mul_43(Mbone,Mbox);
				Fvector	S		= Bone.Obb.m_halfsize;
				
				Fvector			P,A;

				A.set( -S.x,	-S.y,	-S.z ); X.transform_tiny(P,A); Box.modify(P);
				A.set( -S.x,	-S.y,	 S.z ); X.transform_tiny(P,A); Box.modify(P);
				A.set(  S.x,	-S.y,	 S.z ); X.transform_tiny(P,A); Box.modify(P);
				A.set(  S.x,	-S.y,	-S.z ); X.transform_tiny(P,A); Box.modify(P);
				A.set( -S.x,	 S.y,	-S.z ); X.transform_tiny(P,A); Box.modify(P);
				A.set( -S.x,	 S.y,	 S.z ); X.transform_tiny(P,A); Box.modify(P);
				A.set(  S.x, 	 S.y,	 S.z ); X.transform_tiny(P,A); Box.modify(P);
				A.set(  S.x, 	 S.y,	-S.z ); X.transform_tiny(P,A); Box.modify(P);
			}
			VisData.box = Box;
			VisData.box.getsphere	(VisData.sphere.P,VisData.sphere.R);
		}
		DelayUpdateVisData = FMath::RandRange(2,32);
	}
	else
	{
		DelayUpdateVisData --;
	}

	if (GetUpdateTracksCalback())
	{
		SkipDeltaTime += DeltaTime;
		if ((*GetUpdateTracksCalback())(DeltaTime, *this))
			SkipDeltaTime = 0;
		return;
	}
	LL_UpdateTracks(DeltaTime + SkipDeltaTime,
#if WITH_EDITORONLY_DATA
	bUpdateAnimationInEditor
#else
false
#endif
	, false);
	SkipDeltaTime = 0;

	if (MyUpdateCallback)	MyUpdateCallback(this);

}
