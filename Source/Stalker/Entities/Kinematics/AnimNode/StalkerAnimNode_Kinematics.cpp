#include "StalkerAnimNode_Kinematics.h"
#include "Entities/Kinematics/StalkerKinematicsComponent.h"
#include "Resources/SkeletonMesh/StalkerKinematicsAssetUserData.h"
#include "Resources/SkeletonMesh/StalkerKinematicsAnimAssetUserData.h"

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
	Owner = Cast<UStalkerKinematicsComponent>(CastChecked<UAnimInstance>(Context.GetAnimInstanceObject())->GetOwningComponent());	
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
	if (Bones.Num() == 0)
	{
		Output.ResetToRefPose();
		return;
	}	
	if (!IsValid(Owner))
	{
		Output.ResetToRefPose();
		return;
	}
	TempBoneTransforms.Empty(Bones.Num());
	//check(IsInGameThread());

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
		for (int32 BoneID = 0; BoneID < Bones.Num(); BoneID++)
		{
			const FCompactPoseBoneIndex BoneIndex = Bones[BoneID].GetCompactPoseIndex(BoneContainer);
			int32 PartID = BonesParts[Bones[BoneID].BoneName];
			Output.Pose[BoneIndex] = PosesOfPart[PartID].Pose[BoneIndex];
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
			BoneTM = Output.Pose[CompactPoseBoneToModify];
			if (!Owner->LL_GetBoneVisible(u16(BoneID)))
			{
				BoneTM.SetScale3D(FVector(0,0,0));
				Output.Pose[CompactPoseBoneToModify] = BoneTM;
				TempBoneTransforms.Add(BoneTM);
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
					TempBoneTransforms.Add(BoneTM);
					if (ParentId != INDEX_NONE)
					{ 
						TempBoneTransforms.Last() *= TempBoneTransforms[ParentId];
					}
					/*if (!TempBoneTransforms.Last().IsRotationNormalized())
					{
						TempBoneTransforms.Last().NormalizeRotation();
					}
					BoneInstance.Transform = StalkerMath::UnrealMatrixToRBMK(TempBoneTransforms.Last().ToMatrixWithScale());*/

					BoneInstance.Transform.rotation(StalkerMath::UnrealQuaternionToRBMK(TempBoneTransforms.Last().GetRotation()));
					BoneInstance.Transform.translate_over(StalkerMath::UnrealLocationToRBMK(TempBoneTransforms.Last().GetLocation()));

					if (BoneInstance.callback() && (AnimMode != EStalkerKinematicsAnimMode::GetAnimPose || !GetAnimPosIgnoreCallbacks))
					{
						BoneInstance.callback()(&BoneInstance);
					}
				}
				
				if(BoneInstance.bUpdated|| BoneInstance.callback_overwrite())
				{
					BoneInstance.bUpdated = false;
					BoneTM.SetFromMatrix(StalkerMath::RBMKMatrixToUnreal(BoneInstance.Transform));
					if (BoneInstance.callback_overwrite())
					{
						TempBoneTransforms.Add(BoneTM);
					}
					if (ParentId != INDEX_NONE)
					{
						BoneTM.SetToRelativeTransform(TempBoneTransforms[ParentId]);
					}
					Output.Pose[CompactPoseBoneToModify] = BoneTM;
				}


			}

		}
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
	
	int32 MaxBlendChannel = 0;
	int32 MaxAddChannel = 0;
	for (int32 BlendID = 0; BlendID < Owner->BlendsCycles[PartID].Num(); BlendID++)
	{
		CBlend& Blend = *Owner->BlendsCycles[PartID][BlendID];
		checkSlow(Blend.channel < 4);

		if (bInGameThread && AnimMode == EStalkerKinematicsAnimMode::GetAnimPose)
		{
			if (!(GetAnimPosChannelMask & 1 << Blend.channel))
			{
				continue;
			}
		}

		if (BlendOfChannal[Blend.channel].Num() == 16)
		{
			if (Blend.fall_at_end)
				continue;;
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
		if (Blend.channel > 1)
			MaxAddChannel = FMath::Max(MaxAddChannel, int32(Blend.channel-1));
		else
			MaxBlendChannel = FMath::Max(MaxBlendChannel,int32(Blend.channel+1));

	
		BlendOfChannal[Blend.channel].Add(Owner->BlendsCycles[PartID][BlendID]);
	}

	FAnimationPoseData OutAnimationPoseData(Output);
	float   TotalWeights = 0;
	if (MaxBlendChannel)
	{
		FilteredPoses.SetNum(MaxBlendChannel, false);
		FilteredCurve.SetNum(MaxBlendChannel, false);
		FilteredAttributes.SetNum(MaxBlendChannel, false);
		BlendWeights.Add(Owner->ChannelsFactor[0]);
		for (int32 i = 0; i < MaxBlendChannel; ++i)
		{
			PosesToEvaluate.Add(i);
			FPoseContext EvaluateContext(Output);
			float   TotalWeightsForFX = Evaluate_Channal(EvaluateContext, i);

			if (i == 0)
			{
				Evaluate_FX(EvaluateContext, PartID, TotalWeightsForFX);
			}

			FilteredPoses[i].MoveBonesFrom(EvaluateContext.Pose);
			FilteredCurve[i].MoveFrom(EvaluateContext.Curve);
			FilteredAttributes[i].MoveFrom(EvaluateContext.CustomAttributes);
		}
		float SumWeight = Owner->ChannelsFactor[0];
		TotalWeights = Owner->ChannelsFactor[0];
		for (int32 i = 1; i < MaxBlendChannel; ++i)
		{
			TotalWeights += Owner->ChannelsFactor[i];
			if (FMath::IsNearlyZero(TotalWeights))
				BlendWeights.Add(0);
			else
				BlendWeights.Add(Owner->ChannelsFactor[i] / TotalWeights);
			BlendWeights.Last() = FMath::Clamp(BlendWeights.Last(), 0.f, 1.f);
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
		FAnimationRuntime::BlendPosesTogether(FilteredPoses, FilteredCurve, FilteredAttributes, BlendWeights, PosesToEvaluate, OutAnimationPoseData);
	}

	if (MaxAddChannel)
	{
		FilteredPoses.SetNum(MaxAddChannel, false);
		FilteredCurve.SetNum(MaxAddChannel, false);
		FilteredAttributes.SetNum(MaxAddChannel, false);
		for (int32 i = 0; i < MaxAddChannel; ++i)
		{
			FPoseContext EvaluateContext(Output,true);
			Evaluate_Channal(EvaluateContext, i + 2);
			FilteredPoses[i].MoveBonesFrom(EvaluateContext.Pose);
			FilteredCurve[i].MoveFrom(EvaluateContext.Curve);
			FilteredAttributes[i].MoveFrom(EvaluateContext.CustomAttributes);
		}
		for (int32 i = 0; i < MaxAddChannel; i++)
		{
			const FAnimationPoseData AdditiveAnimationPoseData(FilteredPoses[i], FilteredCurve[i], FilteredAttributes[i]);
			FAnimationRuntime::AccumulateAdditivePose(OutAnimationPoseData, AdditiveAnimationPoseData, Owner->ChannelsFactor[i + 2], AAT_LocalSpaceBase);
			Output.Pose.NormalizeRotations();
		}
	}

}

float FStalkerAnimNode_Kinematics::Evaluate_Channal(FPoseContext& Output, int32 Channal)
{	
	if (BlendOfChannal[Channal].Num() == 0)
	{
		Output.ResetToRefPose();
		BlendOfChannal[Channal].Empty(BlendOfChannal[Channal].Num());
		return 0;
	}
	TArray<int32, TInlineAllocator<16>> PosesToEvaluate;
	TArray<FCompactPose, TInlineAllocator<16>> FilteredPoses;
	TArray<FBlendedCurve, TInlineAllocator<16>> FilteredCurve;
	TArray<UE::Anim::FStackAttributeContainer, TInlineAllocator<16>> FilteredAttributes;
	TArray<float, TInlineAllocator<16>> BlendWeights;

	FilteredPoses.SetNum(BlendOfChannal[Channal].Num(), false);
	FilteredCurve.SetNum(BlendOfChannal[Channal].Num(), false);
	FilteredAttributes.SetNum(BlendOfChannal[Channal].Num(), false);
	BlendWeights.Add( BlendOfChannal[Channal][0]->blendAmount);

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
	float SumWeight = TotalWeights;
	for (int32 i = 1; i < BlendOfChannal[Channal].Num(); ++i)
	{
		TotalWeights += BlendOfChannal[Channal][i]->blendAmount;
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
	return TotalWeights;
}

void FStalkerAnimNode_Kinematics::Evaluate_FX(FPoseContext& Output, int32 PartID, float TotalWeights)
{
	TArray<int32, TInlineAllocator<16>> PosesToEvaluate;
	TArray<FCompactPose, TInlineAllocator<16>> FilteredPoses;
	TArray<FBlendedCurve, TInlineAllocator<16>> FilteredCurve;
	TArray<UE::Anim::FStackAttributeContainer, TInlineAllocator<16>> FilteredAttributes;

	const FBoneContainer& BoneContainer = Output.AnimInstanceProxy->GetRequiredBones();

	FilteredPoses.SetNum(Owner->BlendsFX[PartID].Num(), false);
	FilteredCurve.SetNum(Owner->BlendsFX[PartID].Num(), false);
	FilteredAttributes.SetNum(Owner->BlendsFX[PartID].Num(), false);

	for (int32 i = 0; i < Owner->BlendsFX[PartID].Num(); ++i)
	{
		PosesToEvaluate.Add(i);

		FPoseContext EvaluateContext(Output);
		Evaluate_Blend(EvaluateContext, *Owner->BlendsFX[PartID][i]);

		FilteredPoses[i].MoveBonesFrom(EvaluateContext.Pose);
		FilteredCurve[i].MoveFrom(EvaluateContext.Curve);
		FilteredAttributes[i].MoveFrom(EvaluateContext.CustomAttributes);
	}

	TMap<u16,float> BoneTotalWeights;
	for (int32 i = 0; i < Owner->BlendsFX[PartID].Num(); ++i)
	{
		float*Weights = BoneTotalWeights.Find(Owner->BlendsFX[PartID][i]->bone_or_part);
		if (Weights == nullptr)
		{
			Weights = &BoneTotalWeights.Add(Owner->BlendsFX[PartID][i]->bone_or_part,TotalWeights);
		}
		*Weights += Owner->BlendsFX[PartID][i]->blendAmount;

		FCompactPoseBoneIndex CompactPoseBoneToModify = Bones[Owner->BlendsFX[PartID][i]->bone_or_part].GetCompactPoseIndex(BoneContainer);
		float BlendWeights=0;
		if (!FMath::IsNearlyZero(TotalWeights))
			BlendWeights =FMath::Clamp(Owner->BlendsFX[PartID][i]->blendAmount / *Weights,0,1.f);

		FTransform CurrentTransform =  Output.Pose[CompactPoseBoneToModify];
		Output.Pose[CompactPoseBoneToModify].Blend(CurrentTransform, FilteredPoses[i][CompactPoseBoneToModify], BlendWeights);
	}

}

void FStalkerAnimNode_Kinematics::Evaluate_Blend(FPoseContext& Output, CBlend& AnimBlend)
{
	checkSlow(AnimBlend.motionID.valid());
	FAnimationPoseData AnimationPoseData(Output);
	FDeltaTimeRecord DeltaTimeRecord;
	bool NeedAdditive = AnimBlend.channel>1;
	UAnimSequence* Anim = Owner->Anims[AnimBlend.motionID.val];
	Anim->GetAnimationPose(AnimationPoseData, FAnimExtractContext(AnimBlend.timeCurrent, Output.AnimInstanceProxy->ShouldExtractRootMotion(), DeltaTimeRecord, false));
	if (Anim->IsValidAdditive() != NeedAdditive)
	{
		FString Name;
		Anim->GetName(Name);
		UE_LOG(LogStalker,Error,TEXT("AnimSequence %s need %s be Additive"),*Name, NeedAdditive?TEXT(""): TEXT("not"));
	}
}
