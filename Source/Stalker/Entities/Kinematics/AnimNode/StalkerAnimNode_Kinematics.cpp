#include "StalkerAnimNode_Kinematics.h"
#include "Entities/Kinematics/StalkerKinematics.h"
#include "Animation/AnimInstanceProxy.h"
#include "Animation/AnimSequence.h"
#include "Resources/SkeletonMesh/StalkerKinematicsData.h"
#include "Resources/SkeletonMesh/StalkerKinematicsAnimData.h"

FStalkerAnimNode_Kinematics::FStalkerAnimNode_Kinematics()
{
	AnimMode = EStalkerKinematicsAnimMode::Base;
}

void FStalkerAnimNode_Kinematics::SetAnimMode(EStalkerKinematicsAnimMode NewMode)
{
	AnimMode = NewMode;
}

void FStalkerAnimNode_Kinematics::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	Owner = Cast<AStalkerKinematics>(CastChecked<UAnimInstance>(Context.GetAnimInstanceObject())->GetOwningActor());	
}

void FStalkerAnimNode_Kinematics::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{
	if (IsValid(Owner) && Owner->KinematicsData)
	{
		const FBoneContainer& BoneContainer = Context.AnimInstanceProxy->GetRequiredBones();
		Bones.Empty(Owner->Bones.Num());
		Bones.AddDefaulted(Owner->Bones.Num());
		for (int32 i = 0; i < Owner->Bones.Num(); i++)
		{
			Bones[i] = FBoneReference(FName(Owner->BonesID2Name[i].c_str()));
			Bones[i].Initialize(BoneContainer);
		}
		if(Owner->KinematicsData->Anims.Num())
		{
			for (int32 PartID = 0; PartID < Owner->BonesParts.Num(); PartID++)
			{
				for (FName BoneName: Owner->BonesParts[PartID].Bones)
				{
					BonesParts.Add(BoneName, PartID);
				}
			}
		}
	}

}

void FStalkerAnimNode_Kinematics::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	
}

void FStalkerAnimNode_Kinematics::Evaluate_AnyThread(FPoseContext& Output)
{
	//check(IsInGameThread());
	if (!IsValid(Owner))
	{
		Output.ResetToRefPose();
		return;
	}
	if (AnimMode == EStalkerKinematicsAnimMode::GetBoneInMotionBlend)
	{
		Evaluate_Blend(Output, *GetBoneInMotionBlend);
		return;
	}

	const bool bInGameThread = IsInGameThread();
	const FBoneContainer& BoneContainer = Output.AnimInstanceProxy->GetRequiredBones();
	TArray<FPoseContext, TInlineAllocator<4>> PosesOfPart;
	if(Owner->KinematicsData->Anims.Num())
	{
		check(BonesParts.Num());

		Output.ResetToRefPose();
		for (int32 PartID = 0; PartID < Owner->BonesParts.Num(); PartID++)
		{
			PosesOfPart.Add(Output);
			Evaluate_PartID(PosesOfPart.Last(), PartID);
		}
	}
	else
	{
		Output.ResetToRefPose();
	}
	for (int32 i = 0; i < Bones.Num() && i < Owner->RootBone; i++)
	{
		const FCompactPoseBoneIndex BoneIndex = Bones[i].GetCompactPoseIndex(BoneContainer);
		Output.Pose[BoneIndex].SetFromMatrix(FMatrix::Identity);
		
	}


	{

		const  bool UseAnims = Owner->KinematicsData->Anims.Num()>0;
		TArray<FComponentSpacePoseContext, TInlineAllocator<4>> CSPosesOfPart;

		for (int32 PartID = 0; PartID < PosesOfPart.Num(); PartID++)
		{
			CSPosesOfPart.Add(PosesOfPart[PartID].AnimInstanceProxy);
			FComponentSpacePoseContext&InputCSPose = CSPosesOfPart.Last();
			FTransform ComponentTransform = InputCSPose.AnimInstanceProxy->GetComponentTransform();
			InputCSPose.SetNodeIds(PosesOfPart[PartID]);
			InputCSPose.Pose.InitPose(PosesOfPart[PartID].Pose);
			InputCSPose.Curve = MoveTemp(PosesOfPart[PartID].Curve);
			InputCSPose.CustomAttributes = MoveTemp(PosesOfPart[PartID].CustomAttributes);
		}
		
		FComponentSpacePoseContext InputCSPose(Output.AnimInstanceProxy);
		FTransform ComponentTransform = InputCSPose.AnimInstanceProxy->GetComponentTransform();

		// We need to preserve the node ID chain as we use the proxy-based constructor above
		InputCSPose.SetNodeIds(Output);

		InputCSPose.Pose.InitPose(Output.Pose);
		InputCSPose.Curve = MoveTemp(Output.Curve);
		InputCSPose.CustomAttributes = MoveTemp(Output.CustomAttributes);

		TArray<FBoneTransform> BoneTransforms;
		for (int32 BoneID = 0; BoneID < Bones.Num(); BoneID++)
		{
			FCompactPoseBoneIndex CompactPoseBoneToModify = Bones[BoneID].GetCompactPoseIndex(BoneContainer);
			int32 ParentId = INDEX_NONE;
			
			if (Owner->Bones[BoneID].ParentID != BI_NONE)
			{
				ParentId = Owner->Bones[BoneID].ParentID;
			}
			check(ParentId < BoneID);

			FTransform BoneTM;
			if (!Owner->LL_GetBoneVisible(u16(BoneID)))
			{
				BoneTM = InputCSPose.Pose.GetComponentSpaceTransform(CompactPoseBoneToModify);
				FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTransform, InputCSPose.Pose, BoneTM, CompactPoseBoneToModify, EBoneControlSpace::BCS_ComponentSpace);
				BoneTM.SetScale3D(FVector(0,0,0));
				BoneTransforms.Add(FBoneTransform(CompactPoseBoneToModify, BoneTM));
			}
			else
			{

				StalkerKinematicsBoneInstance& BoneInstance = Owner->BonesInstance[BoneID];

				if (BoneInstance.callback_overwrite())
				{
					if (BoneInstance.callback() && (AnimMode != EStalkerKinematicsAnimMode::GetAnimPose || !GetAnimPosIgnoreCallbacks))
						BoneInstance.callback()(&BoneInstance);
				}
				else
				{
					if (UseAnims)
					{
						int32 PartID = BonesParts[Bones[BoneID].BoneName];
						BoneTM = CSPosesOfPart[PartID].Pose.GetComponentSpaceTransform(CompactPoseBoneToModify);
						FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTransform, CSPosesOfPart[PartID].Pose, BoneTM, CompactPoseBoneToModify, EBoneControlSpace::BCS_ParentBoneSpace);
						//FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTransform, InputCSPose.Pose, BoneTM, CompactPoseBoneToModify, EBoneControlSpace::BCS_ParentBoneSpace);
						if (ParentId != INDEX_NONE)
						{
							BoneTM *= BoneTransforms[ParentId].Transform;
						}
					}
					else
					{
						BoneTM = InputCSPose.Pose.GetComponentSpaceTransform(CompactPoseBoneToModify);
						FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTransform, InputCSPose.Pose, BoneTM, CompactPoseBoneToModify, EBoneControlSpace::BCS_ComponentSpace);
					}

				

					BoneInstance.Transform.rotation(StalkerMath::UnrealQuatToXRay(BoneTM.GetRotation()));
					BoneInstance.Transform.translate_over(StalkerMath::UnrealLocationToXRay(BoneTM.GetLocation()));

					if (BoneInstance.callback() && (AnimMode != EStalkerKinematicsAnimMode::GetAnimPose || !GetAnimPosIgnoreCallbacks))
						BoneInstance.callback()(&BoneInstance);
				}

				BoneTM.SetRotation(FQuat(StalkerMath::XRayQuatToUnreal(BoneInstance.Transform)));
				BoneTM.SetLocation(FVector(StalkerMath::XRayLocationToUnreal(BoneInstance.Transform.c)));
				BoneTM.SetScale3D(FVector(1,1,1));

			}
			// Convert back to Component Space.
			FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTransform, InputCSPose.Pose, BoneTM, CompactPoseBoneToModify, EBoneControlSpace::BCS_ComponentSpace);
			BoneTransforms.Add(FBoneTransform(CompactPoseBoneToModify, BoneTM));
		}
	
		InputCSPose.Pose.LocalBlendCSBoneTransforms(BoneTransforms, 1.f);
		checkSlow(InputCSPose.Pose.GetPose().IsValid());
		FCSPose<FCompactPose>::ConvertComponentPosesToLocalPoses(MoveTemp(InputCSPose.Pose), Output.Pose);
		Output.Curve = MoveTemp(InputCSPose.Curve);
		Output.CustomAttributes = MoveTemp(InputCSPose.CustomAttributes);
	}

		
}

void FStalkerAnimNode_Kinematics::GatherDebugData(FNodeDebugData& DebugData)
{

}


void FStalkerAnimNode_Kinematics::Evaluate_PartID(FPoseContext& Output, int32 PartID)
{
	if (Owner->BlendsCycles[PartID].Num() == 0)
	{
		Output.ResetToRefPose();
		return;
	}
	const bool bInGameThread = IsInGameThread();
	TArray<int32, TInlineAllocator<4>> PosesToEvaluate;
	TArray<FCompactPose, TInlineAllocator<4>> FilteredPoses;
	TArray<FBlendedCurve, TInlineAllocator<4>> FilteredCurve;
	TArray<UE::Anim::FStackAttributeContainer, TInlineAllocator<4>> FilteredAttributes;
	TArray<float, TInlineAllocator<4>> BlendWeights;
	
	int32 MaxChannel = 0;
	for (int32 BlendID = 0; BlendID < Owner->BlendsCycles[PartID].Num(); BlendID++)
	{
		CBlend& Blend = *Owner->BlendsCycles[PartID][BlendID];
		checkSlow(Blend.channel < 4);
		if(Blend.channel>1)continue;

		if (Blend.fall_at_end)
			continue;;

		if (bInGameThread && AnimMode == EStalkerKinematicsAnimMode::GetAnimPose)
		{
			if (!(GetAnimPosChannelMask & 1 << Blend.channel))
			{
				continue;
			}
		}

		if (BlendOfChannal[Blend.channel].Num() == 16)
		{
			float Min = BlendOfChannal[Blend.channel][0]->blendAmount;
			int32 IDToRemove = 0;
			for (int32 i =1;i< BlendOfChannal[Blend.channel].Num(); i++) 
			{
				if (Min > BlendOfChannal[Blend.channel][i]->blendAmount)
				{
					IDToRemove = i;
					Min = BlendOfChannal[Blend.channel][i]->blendAmount;
				}
			}
			BlendOfChannal[Blend.channel].RemoveAt(IDToRemove);
		}
		MaxChannel = FMath::Max(MaxChannel,int32(Blend.channel));

	
		BlendOfChannal[Blend.channel].Add(Owner->BlendsCycles[PartID][BlendID]);
	}
	FilteredPoses.SetNum(MaxChannel+1, false);
	FilteredCurve.SetNum(MaxChannel + 1, false);
	FilteredAttributes.SetNum(MaxChannel + 1, false);
	BlendWeights.Add(1);
	for (int32 i = 0; i < MaxChannel+1; ++i)
	{
		PosesToEvaluate.Add(i);
		FPoseContext EvaluateContext(Output);
		Evaluate_Channal(EvaluateContext, i);

		FilteredPoses[i].MoveBonesFrom(EvaluateContext.Pose);
		FilteredCurve[i].MoveFrom(EvaluateContext.Curve);
		FilteredAttributes[i].MoveFrom(EvaluateContext.CustomAttributes);
	}
	float SumWeight = 1;
	float   TotalWeights = Owner->ChannelsFactor[0];
	for (int32 i = 1; i < MaxChannel + 1; ++i)
	{
		TotalWeights += Owner->ChannelsFactor[i];
		if (FMath::IsNearlyZero(TotalWeights))
			BlendWeights.Add(0);
		else
			BlendWeights.Add(Owner->ChannelsFactor[i] / TotalWeights);
		BlendWeights.Last() = FMath::Clamp(BlendWeights.Last(),0.f,1.f);
		SumWeight += BlendWeights.Last();
	}
	if ((SumWeight > ZERO_ANIMWEIGHT_THRESH) && (FMath::Abs<float>(SumWeight - 1.0f) > ZERO_ANIMWEIGHT_THRESH))
	{
		float ReciprocalSum = 1.0f / SumWeight;
		for (int32 i = 0; i < BlendWeights.Num(); ++i)
		{
			BlendWeights[i] *= ReciprocalSum;
		}
	}
	FAnimationPoseData OutAnimationPoseData(Output);
	FAnimationRuntime::BlendPosesTogether(FilteredPoses, FilteredCurve, FilteredAttributes, BlendWeights, PosesToEvaluate, OutAnimationPoseData);
}

void FStalkerAnimNode_Kinematics::Evaluate_Channal(FPoseContext& Output, int32 Channal)
{
	if (Channal > 1)
	{
		Output.ResetToRefPose();
		BlendOfChannal[Channal].Empty(BlendOfChannal[Channal].Num());
		return;
	}
	checkSlow(BlendOfChannal[Channal].Num());
	TArray<int32, TInlineAllocator<16>> PosesToEvaluate;
	TArray<FCompactPose, TInlineAllocator<16>> FilteredPoses;
	TArray<FBlendedCurve, TInlineAllocator<16>> FilteredCurve;
	TArray<UE::Anim::FStackAttributeContainer, TInlineAllocator<16>> FilteredAttributes;
	TArray<float, TInlineAllocator<16>> BlendWeights;

	FilteredPoses.SetNum(BlendOfChannal[Channal].Num(), false);
	FilteredCurve.SetNum(BlendOfChannal[Channal].Num(), false);
	FilteredAttributes.SetNum(BlendOfChannal[Channal].Num(), false);
	BlendWeights.Add(1);

	for (int32 i = 0; i < BlendOfChannal[Channal].Num(); ++i)
	{
		PosesToEvaluate.Add(i);

		FPoseContext EvaluateContext(Output);
		Evaluate_Blend(EvaluateContext, *BlendOfChannal[Channal][i]);

		FilteredPoses[i].MoveBonesFrom(EvaluateContext.Pose);
		FilteredCurve[i].MoveFrom(EvaluateContext.Curve);
		FilteredAttributes[i].MoveFrom(EvaluateContext.CustomAttributes);
	}

	float   TotalWeights = BlendOfChannal[Channal][0]->blendAmount;
	float SumWeight = 1;
	for (int32 i = 1; i < BlendOfChannal[Channal].Num(); ++i)
	{
		TotalWeights += Owner->ChannelsFactor[i];
		if (FMath::IsNearlyZero(TotalWeights))	
			BlendWeights.Add(0);
		else	
			BlendWeights.Add(BlendOfChannal[Channal][i]->blendAmount / TotalWeights);
		BlendWeights.Last() = FMath::Clamp(BlendWeights.Last(), 0.f, 1.f);
		SumWeight+= BlendWeights.Last();

	}
	// Renormalize the weights
	if ((SumWeight > ZERO_ANIMWEIGHT_THRESH) && (FMath::Abs<float>(SumWeight - 1.0f) > ZERO_ANIMWEIGHT_THRESH))
	{
		float ReciprocalSum = 1.0f / SumWeight;
		for (int32 i = 0; i < BlendWeights.Num(); ++i)
		{
			BlendWeights[i] *= ReciprocalSum;
		}
	}

	BlendOfChannal[Channal].Empty(BlendOfChannal[Channal].Num());
	FAnimationPoseData OutAnimationPoseData(Output);

	
	FAnimationRuntime::BlendPosesTogether(FilteredPoses, FilteredCurve, FilteredAttributes, BlendWeights, PosesToEvaluate, OutAnimationPoseData);
}

void FStalkerAnimNode_Kinematics::Evaluate_Blend(FPoseContext& Output, CBlend& AnimBlend)
{
	checkSlow(AnimBlend.motionID.valid());
	FAnimationPoseData AnimationPoseData(Output);
	FDeltaTimeRecord DeltaTimeRecord;
	Owner->Anims[AnimBlend.motionID.val].Amim->GetAnimationPose(AnimationPoseData, FAnimExtractContext(AnimBlend.timeCurrent, Output.AnimInstanceProxy->ShouldExtractRootMotion(), DeltaTimeRecord, false));
}
