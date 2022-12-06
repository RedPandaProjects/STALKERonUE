#include "StalkerKinematicsBone.h"

StalkerKinematicsBone::StalkerKinematicsBone()
{
	BindTransformation.identity();

}

StalkerKinematicsBone::~StalkerKinematicsBone()
{

}

IBoneData&  StalkerKinematicsBone::GetChild(u16 id)
{
	return *Childs[id];
}

const IBoneData&  StalkerKinematicsBone::GetChild(u16 id) const
{
	return *Childs[id];
}

u16  StalkerKinematicsBone::GetSelfID() const
{
	return SelfID;
}

u16  StalkerKinematicsBone::GetNumChildren() const
{
	return Childs.Num();
}

const SJointIKData&  StalkerKinematicsBone::get_IK_data() const
{
	return IKData;
}

const Fmatrix&  StalkerKinematicsBone::get_bind_transform() const
{
	return BindTransformation;
}

const SBoneShape&  StalkerKinematicsBone::get_shape() const
{
	return Shape;
}

const Fobb&  StalkerKinematicsBone::get_obb() const
{
	return Obb;
}

const Fvector&  StalkerKinematicsBone::get_center_of_mass() const
{
	return CenterOfMass;
}

float  StalkerKinematicsBone::get_mass() const
{
	return Mass ;
}

u16  StalkerKinematicsBone::get_game_mtl_idx() const
{
	return GameMaterialID;
}

u16  StalkerKinematicsBone::GetParentID() const
{
	return ParentID;
}

float  StalkerKinematicsBone::lo_limit(u8 k) const
{
	return IKData.limits[k].limit.x;
}

float  StalkerKinematicsBone::hi_limit(u8 k) const
{
	return IKData.limits[k].limit.y;
}

shared_str  StalkerKinematicsBone::GetName() const
{
	return Name;
}
