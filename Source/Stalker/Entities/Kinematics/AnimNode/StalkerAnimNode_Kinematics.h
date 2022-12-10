#pragma once
#include "StalkerAnimNode_Kinematics.generated.h"

USTRUCT(BlueprintInternalUseOnly)
struct FStalkerAnimNode_Kinematics : public FAnimNode_Base
{
	GENERATED_USTRUCT_BODY()
public:
	FStalkerAnimNode_Kinematics();

	// FStalkerAnimNode_Kinematics interface
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override;
	virtual void Update_AnyThread(const FAnimationUpdateContext& Context) override;
	virtual void Evaluate_AnyThread(FPoseContext& Output) override;
	virtual void GatherDebugData(FNodeDebugData& DebugData) override;


private:
	UPROPERTY()
	class AStalkerKinematics*	Owner;
	TArray<FBoneReference>		Bones;
	TArray<TArray<FBoneReference>>	BonesParts;

	TArray<TSharedPtr<CBlend>>	BlendOfChannal[4];

	void Evaluate_PartID(FPoseContext& Output, int32 PartID);
	void Evaluate_Channal(FPoseContext& Output,  int32 Channal);
	void Evaluate_Blend(FPoseContext& Output, CBlend&AnimBlend);
};