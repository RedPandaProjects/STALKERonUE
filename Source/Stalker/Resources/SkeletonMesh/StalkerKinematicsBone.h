#pragma once
class StalkerKinematicsBone:public IBoneData
{

public:
	StalkerKinematicsBone();
	~StalkerKinematicsBone();

	SJointIKData					IKData;
	TArray<StalkerKinematicsBone*>	Childs;
	Fmatrix							BindTransformation;
	SBoneShape						Shape;
	Fobb							Obb;
	shared_str						Name;
	Fvector							CenterOfMass;
	u16								ParentID;
	u16								GameMaterialID;
	u16								SelfID;
	float							Mass;

	IBoneData&						GetChild			(u16 id) override;
	const IBoneData&				GetChild			(u16 id) const override;
	u16								GetSelfID			() const override;
	u16								GetNumChildren		() const override;
	const SJointIKData&				get_IK_data			() const override;
	const Fmatrix&					get_bind_transform	() const override;
	const SBoneShape&				get_shape			() const override;
	const Fobb&						get_obb				() const override;
	const Fvector&					get_center_of_mass	() const override;
	float							get_mass			() const override;
	u16								get_game_mtl_idx	() const override;
	u16								GetParentID			() const override;
	float							lo_limit			(u8 k) const override;
	float							hi_limit			(u8 k) const override;
	shared_str						GetName				() const override;

};