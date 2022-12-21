#pragma once
#include "StalkerAnimNode_Kinematics.generated.h"
UENUM()
enum class EStalkerKinematicsAnimMode
{
	Base,
	GetAnimPose,
	GetBoneInMotionBlend,
};
USTRUCT(BlueprintInternalUseOnly)
struct STALKER_API FStalkerAnimNode_Kinematics : public FAnimNode_Base
{
	GENERATED_USTRUCT_BODY()
public:
												FStalkerAnimNode_Kinematics	();
	void										SetAnimMode					(EStalkerKinematicsAnimMode NewMode);
	// FStalkerAnimNode_Kinematics interface
	void										Initialize_AnyThread		(const FAnimationInitializeContext& Context) override;
	void										CacheBones_AnyThread		(const FAnimationCacheBonesContext& Context) override;
	void										Update_AnyThread			(const FAnimationUpdateContext& Context) override;
	void										Evaluate_AnyThread			(FPoseContext& Output) override;
	void										GatherDebugData				(FNodeDebugData& DebugData) override;
	bool										CanUpdateInWorkerThread		() const override { return false; }

	bool										GetAnimPosIgnoreCallbacks;
	u8											GetAnimPosChannelMask;

	CBlend*										GetBoneInMotionBlend;
private:
	UPROPERTY()
	class UStalkerKinematicsComponent*					Owner;

	EStalkerKinematicsAnimMode					AnimMode;

	TArray<FBoneReference>						Bones;
	TMap<FName,int32>							BonesParts;

	TArray<TSharedPtr<CBlend>>					BlendOfChannal[4];

	void Evaluate_PartID						(FPoseContext& Output, int32 PartID);
	float Evaluate_Channal						(FPoseContext& Output,  int32 Channal);
	void Evaluate_FX							(FPoseContext& Output, int32 PartID,float TotalWeights);
	void Evaluate_Blend							(FPoseContext& Output, CBlend&AnimBlend);
	TArray<FTransform>							TempBoneTransforms;
};