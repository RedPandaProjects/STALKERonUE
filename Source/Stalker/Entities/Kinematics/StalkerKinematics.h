#pragma once
#include "Resources/SkeletonMesh/StalkerKinematicsBone.h"
#include "Resources/SkeletonMesh/StalkerKinematicsBoneInstance.h"
#include "StalkerKinematics.generated.h"

UCLASS()
class STALKER_API AStalkerKinematics : public AActor,public IKinematics,public IRenderVisual
{
	GENERATED_BODY()
	
public:	
	AStalkerKinematics();
	void Initilize(class UStalkerKinematicsData* KinematicsData);


	virtual void Tick(float DeltaTime) override;
	void Bone_Calculate(const IBoneData* bd, const Fmatrix* parent) override;
	void Bone_GetAnimPos(Fmatrix& pos, u16 id, u8 channel_mask, bool ignore_callbacks) override;
	bool PickBone(const Fmatrix& parent_xform, pick_result& r, float dist, const Fvector& start, const Fvector& dir, u16 bone_id) override;
	void EnumBoneVertices(SEnumVerticesCallback& C, u16 bone_id) override;
	u16 LL_BoneID(LPCSTR B) override;
	u16 LL_BoneID(const shared_str& B) override;
	LPCSTR LL_BoneName_dbg(u16 ID) override;
	CInifile* LL_UserData() override;
	IBoneInstance& LL_GetBoneInstance(u16 bone_id) override;
	const IBoneData& GetBoneData(u16 bone_id) const override;
	u16 LL_BoneCount() const override;
	u16 LL_VisibleBoneCount() override;
	const Fmatrix& LL_GetTransform(u16 bone_id) const override;
	const Fmatrix& LL_GetTransform_R(u16 bone_id) override;
	void LL_SetTransform(u16 bone_id, const Fmatrix& Matrix) override;
	Fobb& LL_GetBox(u16 bone_id) override;
	const Fbox& GetBox() const override;
	void LL_GetBindTransform(xr_vector<Fmatrix>& matrices) override;
	int LL_GetBoneGroups(xr_vector<xr_vector<u16>>& groups) override;
	u16 LL_GetBoneRoot() override;
	void LL_SetBoneRoot(u16 bone_id) override;
	BOOL LL_GetBoneVisible(u16 bone_id) override;
	void LL_SetBoneVisible(u16 bone_id, BOOL val, BOOL bRecursive) override;
	BonesVisible LL_GetBonesVisible() override;
	void LL_SetBonesVisible(BonesVisible mask) override;
	void CalculateBones(BOOL bForceExact = FALSE) override;
	void CalculateBones_Invalidate() override;
	void Callback(UpdateCallback C, void* Param) override;
	void SetUpdateCallback(UpdateCallback pCallback) override;
	void SetUpdateCallbackParam(void* pCallbackParam) override;
	UpdateCallback GetUpdateCallback() override;
	void* GetUpdateCallbackParam() override;
	IRenderVisual* dcast_RenderVisual() override;
	IKinematicsAnimated* dcast_PKinematicsAnimated() override;
	void DebugRender(Fmatrix& XFORM) override;
	shared_str getDebugName() override;
	vis_data& _BCL getVisData() override;
	u32 getType() override;
	IKinematics* _BCL dcast_PKinematics() override;

protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY()
	class UStalkerKinematicsData*			KinematicsData;


	TArray<StalkerKinematicsBone>			Bones;
	TArray<StalkerKinematicsBoneInstance>	BonesInstance;
	vis_data								VisData;
    TSharedPtr<CInifile>					UserData;
	TMap<shared_str, u16>					BonesName2ID;
	TMap<u16, shared_str>					BonesID2Name;
	BonesVisible							SelfBonesVisible;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneComponent;

public:
	class AStalkerKinematics* CastToAStalkerKinematics() override;

};
