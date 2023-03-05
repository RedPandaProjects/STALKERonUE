#pragma once
#include "Resources/SkeletonMesh/StalkerKinematicsBone.h"
#include "Resources/SkeletonMesh/StalkerKinematicsBoneInstance.h"
#include "Resources/SkeletonMesh/StalkerKinematicsAnimData.h"
#include "Resources/SkeletonMesh/StalkerKinematicsAnimsData.h"
THIRD_PARTY_INCLUDES_START
#include "XrRender/Public/animation_blend.h"
#include "XrEngine/vis_common.h"
THIRD_PARTY_INCLUDES_END
#include "StalkerKinematics.generated.h"

UCLASS(BlueprintType)
class STALKER_API UStalkerKinematicsComponent : public USkeletalMeshComponent, public IRenderVisual,public IKinematics,public IKinematicsAnimated
{
	GENERATED_BODY()
	
public:	
													UStalkerKinematicsComponent			();
	UFUNCTION(BlueprintCallable)
	void											Initilize							(class UStalkerKinematicsData* KinematicsData);
	void											InitilizeEditor						();
	void											PostLoad							() override;
	void											BeginDestroy						() override;
	void											Lock								(void* Parent) override;
	void											Unlock								(void* Parent) override;
	void											Detach								() override;
	
	void											SetOwnerNoSee						(bool Enable) override;
	void											SetOnlyOwnerSee						(bool Enable) override;
	void											SetOffset							(const Fmatrix&offset,bool IsWorldLocation = false,bool IsWorldRotation = false) override;


	void											TickComponent						(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

//IKinematics
	void											Bone_Calculate						(const IBoneData* bd, const Fmatrix* parent) override;
	void											Bone_GetAnimPos						(Fmatrix& pos, u16 id, u8 channel_mask, bool ignore_callbacks) override;
	bool											PickBone							(const Fmatrix& parent_xform, pick_result& r, float dist, const Fvector& start, const Fvector& dir, u16 bone_id) override;
	void											EnumBoneVertices					(SEnumVerticesCallback& C, u16 bone_id) override;
	u16												LL_BoneID							(LPCSTR B) override;
	u16												LL_BoneID							(const shared_str& B) override;
	LPCSTR											LL_BoneName_dbg						(u16 ID) override;
	CInifile*										LL_UserData							() override;
	IBoneInstance&									LL_GetBoneInstance					(u16 bone_id) override;
	const IBoneData&								GetBoneData							(u16 bone_id) const override;
	u16												LL_BoneCount						() const override;
	u16												LL_VisibleBoneCount					() override;
	const Fmatrix&									LL_GetTransform						(u16 bone_id) const override;
	const Fmatrix&									LL_GetTransform_R					(u16 bone_id) override;
	void											LL_SetTransform						(u16 bone_id, const Fmatrix& Matrix) override;
	Fobb&											LL_GetBox							(u16 bone_id) override;
	const Fbox&										GetBox								() const override;
	void											LL_GetBindTransform					(xr_vector<Fmatrix>& matrices) override;
	u16												LL_GetBoneRoot						() override;
	void											LL_SetBoneRoot						(u16 bone_id) override;
	BOOL											LL_GetBoneVisible					(u16 bone_id) override;
	void											LL_SetBoneVisible					(u16 bone_id, BOOL val, BOOL bRecursive) override;
	BonesVisible									LL_GetBonesVisible					() override;
	void											LL_SetBonesVisible					(BonesVisible mask) override;
	void											CalculateBones						(BOOL bForceExact = FALSE) override;
	void											CalculateBones_Invalidate			() override;
	void											Callback							(UpdateCallback C, void* Param) override;
	void											SetUpdateCallback					(UpdateCallback pCallback) override;
	void											SetUpdateCallbackParam				(void* pCallbackParam) override;
	UpdateCallback									GetUpdateCallback					() override;
	void*											GetUpdateCallbackParam				() override;
	IRenderVisual*									dcast_RenderVisual					() override;
	IKinematicsAnimated*							dcast_PKinematicsAnimated			() override;
	void											DebugRender							(Fmatrix& XFORM) override;
	shared_str										getDebugName						() override;
	vis_data& _BCL									getVisData							() override;
	u32												getType								() override;
	IKinematics* _BCL								dcast_PKinematics					() override;


//IKinematicsAnimated
	class UStalkerKinematicsComponent*				CastToStalkerKinematicsComponent	() override;
	void											OnCalculateBones					() override;
	std::pair<LPCSTR, LPCSTR>						LL_MotionDefName_dbg				(MotionID ID) override;
	void											LL_DumpBlends_dbg					() override;


	CMotionDef*										LL_GetMotionDef						(MotionID id) override;

	u32												LL_PartBlendsCount					(u32 bone_part_id) override;
	CBlend*											LL_PartBlend						(u32 bone_part_id, u32 n) override;



	void											GetBoneInMotion						(Fmatrix& OutPosition, u16 BoneID, CBlend* InBlend) override;


	void											LL_IterateBlends					(IterateBlendsCallback& callback) override;

	IBlendDestroyCallback*							GetBlendDestroyCallback				() override;
	void											SetBlendDestroyCallback				(IBlendDestroyCallback* cb) override;
	void											SetUpdateTracksCalback				(IUpdateTracksCallback* callback) override;
	IUpdateTracksCallback*							GetUpdateTracksCalback				() override;


	void											LL_CloseCycle						(u16 BonesPartID, u8 mask_channel = (1 << 0)) override;
	void											LL_SetChannelFactor					(u16 channel, float factor) override;

	void											UpdateTracks						() override;
	void											LL_UpdateTracks						(float dt, bool b_force, bool leave_blends) override;

	MotionID										ID_Cycle							(LPCSTR N) override;
	MotionID										ID_Cycle_Safe						(LPCSTR N) override;
	MotionID										ID_Cycle							(shared_str N) override;
	MotionID										ID_Cycle_Safe						(shared_str N) override;

	CBlend*											PlayCycle							(LPCSTR N, BOOL bMixIn = TRUE, PlayCallback Callback = 0, LPVOID CallbackParam = 0, u8 channel = 0) override;
	CBlend*											PlayCycle							(MotionID InMotionID, BOOL bMixIn = TRUE, PlayCallback Callback = 0, LPVOID CallbackParam = 0, u8 channel = 0) override;
	void											EditorPlay							(MotionID InMotionID,bool InLoop=true);
	CBlend*											PlayCycle							(u16 BonesPartID, MotionID InMotionID, BOOL bMixIn = TRUE, PlayCallback Callback = 0, LPVOID CallbackParam = 0, u8 channel = 0) override;
	CBlend*											LL_PlayCycle						(u16 BonesPartID, MotionID InMotionID, BOOL bMixIn, float blendAccrue, float blendFalloff, float Speed, BOOL noloop, PlayCallback Callback, LPVOID CallbackParam, u8 channel = 0) override;
	CBlend*											LL_PlayCycle						(u16 BonesPartID, MotionID InMotionID, BOOL bMixIn, PlayCallback Callback, LPVOID CallbackParam, u8 channel = 0) override;

	MotionID										ID_FX								(LPCSTR N) override;
	MotionID										ID_FX_Safe							(LPCSTR N) override;

	CBlend*											PlayFX								(LPCSTR N, float power_scale) override;
	CBlend*											PlayFX								(MotionID M, float power_scale) override;

	MotionID										LL_MotionID							(LPCSTR B) override;
	float											LL_GetMotionTime					(MotionID id) override;
	float											get_animation_length				(MotionID motion_ID) override;

	u16												LL_PartID							(LPCSTR B) override;
	u32												BonesPartsCount						() const override;

	shared_str										GetNameData							() override;

	UPROPERTY()
	class UStalkerKinematicsData*					KinematicsData;

	TArray<StalkerKinematicsBone>					Bones;
	TMap<u16, shared_str>							BonesID2Name;

	TArray<FStalkerKinematicsAnimData>				Anims;
	TArray <TSharedPtr<CBlend>>						BlendsCycles[4];
	TArray<TSharedPtr<CBlend>>						BlendsFX[4];


	TArray< FStalkerKinematicsAnimsBonesPart>		BonesParts;

	float											ChannelsFactor[4];

	u16												RootBone;
	TArray<StalkerKinematicsBoneInstance>			BonesInstance;

private:	
	void											BlendSetup							(CBlend& Blend, u32 PartID, u8 Channel, MotionID InMotionID, bool  IsMixing, float BlendAccrue,  float Speed, bool NoLoop, PlayCallback Callback, LPVOID CallbackParam);
	void											FXBlendSetup						(CBlend& Blend, MotionID InMotionID, float BlendAccrue, float BlendFalloff, float Power, float Speed, u16 Bone);
	TSharedPtr<CBlend>								CreateBlend							();
	void											DestroyBlend						(TSharedPtr<CBlend>& Blend);

	TArray <TSharedPtr<CBlend>>						BlendsPool;

	vis_data										VisData;
    TSharedPtr<CInifile>							UserData;
	TMap<shared_str, u16>							BonesName2ID;
	BonesVisible									SelfBonesVisible;

	TArray<CMotionDef>								AnimsDef;
	TMap<shared_str, u32>							AnimsName2ID;

	TMap<shared_str, u32>							BonesPartsName2ID;
	TMap<u32, u32>									BonesPartsBoneID2ID;
	shared_str										DataName;


	IBlendDestroyCallback*							BlendDestroyCallback;
	IUpdateTracksCallback*							UpdateTracksCallback;

	UpdateCallback									MyUpdateCallback;
	void*											UpdateCallbackParam;

	UPROPERTY(VisibleAnywhere)
	USceneComponent*								SceneComponent;

	UPROPERTY(Transient)
	float											SkipDeltaTime;

	UPROPERTY(Transient)
	class UStalkerKinematicsAnimInstance_Default*	KinematicsAnimInstanceForCompute;
#if WITH_EDITORONLY_DATA
	UPROPERTY(Transient)
	bool											bIsErrorMesh;
#endif
	void*											XRayParent  = nullptr;		
};
