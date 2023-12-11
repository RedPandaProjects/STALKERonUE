#include "StalkerKinematicsBoneData.h"
#include "StalkerKinematicsBone.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/XrGameMaterialLibraryInterface.h"
THIRD_PARTY_INCLUDES_END

void FStalkerKinematicsBone::BuildFromLegacy(const CBoneData& BoneData)
{
	Mass  =  BoneData.mass;
	PhysicsMaterialName = BoneData.game_mtl_name.c_str();
	CenterOfMass = StalkerMath::RBMKLocationToUnreal(BoneData.center_of_mass);

	Obb.Center = FVector( StalkerMath::RBMKLocationToUnreal(BoneData.obb.m_translate));
	Obb.X = BoneData.obb.m_halfsize.x * 200;
	Obb.Y = BoneData.obb.m_halfsize.z * 200;
	Obb.Z = BoneData.obb.m_halfsize.y * 200;
	Fmatrix InRotation;
	BoneData.obb.xform_get(InRotation);
	Obb.Rotation = FRotator(FQuat(StalkerMath::RBMKQuaternionToUnreal(InRotation)));
	
	//shape
	{
		switch (BoneData.shape.type)
		{
		case SBoneShape::stBox:
			Shape.Type = EStalkerShapeType::EST_Box;
			break;
		case SBoneShape::stSphere:
			Shape.Type = EStalkerShapeType::EST_Sphere;
			break;
		case SBoneShape::stCylinder:
			Shape.Type = EStalkerShapeType::EST_Cylinder;
			break;
		default:
			Shape.Type = EStalkerShapeType::EST_None;
			break;
		}
		Shape.Flags = 0 ;

		if(BoneData.shape.flags .test(int32(SBoneShape::sfNoPickable)))
			Shape.Flags |=int32(EStalkerShapeFlags::ESF_NoPickable);

		if (BoneData.shape.flags.test(int32(SBoneShape::sfNoPhysics)))
			Shape.Flags |= int32(EStalkerShapeFlags::ESF_NoPhysics);

		if (BoneData.shape.flags.test(int32(SBoneShape::sfNoFogCollider)))
			Shape.Flags |= int32(EStalkerShapeFlags::ESF_NoFogCollider);

		if (BoneData.shape.flags.test(int32(SBoneShape::sfRemoveAfterBreak)))
			Shape.Flags |= int32(EStalkerShapeFlags::ESF_RemoveAfterBreak);

		Shape.Obb.Center = FVector(StalkerMath::RBMKLocationToUnreal(BoneData.shape.box.m_translate));
		Shape.Obb.X = BoneData.shape.box.m_halfsize.x * 200;
		Shape.Obb.Y = BoneData.shape.box.m_halfsize.z * 200;
		Shape.Obb.Z = BoneData.shape.box.m_halfsize.y * 200;
		BoneData.shape.box.xform_get(InRotation);
		Shape.Obb.Rotation = FRotator(FQuat(StalkerMath::RBMKQuaternionToUnreal(InRotation)));


		Shape.Cylinder.Center = FVector(StalkerMath::RBMKLocationToUnreal(BoneData.shape.cylinder.m_center));
		Shape.Cylinder.Radius = BoneData.shape.cylinder.m_radius * 100;
		Shape.Cylinder.Length = BoneData.shape.cylinder.m_height * 100;
		InRotation.identity();
		InRotation.k.set(BoneData.shape.cylinder.m_direction);
		Fvector::generate_orthonormal_basis(InRotation.k, InRotation.j, InRotation.i);
		Fquaternion InQuat; InQuat.set(InRotation);
		FRotator RotationToUnreal(0, 0, -90);
		FRotator OutRotation(FQuat(InQuat.x, -InQuat.z, -InQuat.y, InQuat.w) * FQuat(RotationToUnreal));
		Shape.Cylinder.Rotation = OutRotation;

		Shape.Sphere.Center = FVector(StalkerMath::RBMKLocationToUnreal(BoneData.shape.sphere.P));
		Shape.Sphere.Radius = BoneData.shape.sphere.R * 100;
	}
	{
		IKData.BreakForce = BoneData.IK_data.break_force;
		IKData.BreakTorque = BoneData.IK_data.break_torque;
		IKData.DampingFactor = BoneData.IK_data.damping_factor;
		IKData.Friction = BoneData.IK_data.friction;
		IKData.IsBreakable = BoneData.IK_data.ik_flags.test(SJointIKData::flBreakable);
		IKData.LimitsX.Set(BoneData.IK_data.limits[0].limit.x, BoneData.IK_data.limits[0].limit.y, BoneData.IK_data.limits[0].spring_factor, BoneData.IK_data.limits[0].damping_factor);
		IKData.LimitsY.Set(BoneData.IK_data.limits[1].limit.x, BoneData.IK_data.limits[1].limit.y, BoneData.IK_data.limits[1].spring_factor, BoneData.IK_data.limits[1].damping_factor);
		IKData.LimitsZ.Set(BoneData.IK_data.limits[2].limit.x, BoneData.IK_data.limits[2].limit.y, BoneData.IK_data.limits[2].spring_factor, BoneData.IK_data.limits[2].damping_factor);
		IKData.SpringFactor = BoneData.IK_data.spring_factor;
		switch (BoneData.IK_data.type)
		{
		case jtRigid:
			IKData.Type = EStalkerJointType::EJT_Rigid;
			break;
		case jtCloth:
			IKData.Type = EStalkerJointType::EJT_Cloth;
			break;
		case jtJoint:
			IKData.Type = EStalkerJointType::EJT_Joint;
			break;
		case jtWheel:
			IKData.Type = EStalkerJointType::EJT_Wheel;
			break;
		case jtSlider:
			IKData.Type = EStalkerJointType::EJT_Slider;
			break;
		default:
			IKData.Type = EStalkerJointType::EJT_None;
			break;
		}
	}
}

void FStalkerKinematicsBone::Build(StalkerKinematicsBone& BoneData)
{
	BoneData.Mass = Mass;
	BoneData.GameMaterialID = GameMaterialLibrary->GetMaterialIdx(TCHAR_TO_ANSI(*PhysicsMaterialName.ToString()));
	BoneData.CenterOfMass = StalkerMath::UnrealLocationToRBMK(CenterOfMass);
	
	BoneData.Obb.m_halfsize.x = Obb.X / 200.f;
	BoneData.Obb.m_halfsize.y = Obb.Z / 200.f;
	BoneData.Obb.m_halfsize.z = Obb.Y / 200.f;

	Fmatrix InRotation;
	InRotation.rotation(StalkerMath::UnrealQuaternionToRBMK(FQuat(Obb.Rotation)));
	BoneData.Obb.xform_set(InRotation);
	BoneData.Obb.m_translate = StalkerMath::UnrealLocationToRBMK(Obb.Center);
	
	//shape
	{
		switch (Shape.Type)
		{
		case EStalkerShapeType::EST_None:
			BoneData.Shape.type = SBoneShape::stNone;
			break;
		case EStalkerShapeType::EST_Box:
			BoneData.Shape.type = SBoneShape::stBox;
			break;
		case EStalkerShapeType::EST_Sphere:
			BoneData.Shape.type = SBoneShape::stSphere;
			break;
		case EStalkerShapeType::EST_Cylinder:
			BoneData.Shape.type = SBoneShape::stCylinder;
			break;
		default:
			BoneData.Shape.type = SBoneShape::stNone;
			break;
		}
		BoneData.Shape.flags.zero();

		BoneData.Shape.flags.set(int32(SBoneShape::sfNoPickable), Shape.Flags & int32(EStalkerShapeFlags::ESF_NoPickable));
		BoneData.Shape.flags.set(int32(SBoneShape::sfNoPhysics), Shape.Flags & int32(EStalkerShapeFlags::ESF_NoPhysics));
		BoneData.Shape.flags.set(int32(SBoneShape::sfNoFogCollider), Shape.Flags & int32(EStalkerShapeFlags::ESF_NoFogCollider));
		BoneData.Shape.flags.set(int32(SBoneShape::sfRemoveAfterBreak), Shape.Flags & int32(EStalkerShapeFlags::ESF_RemoveAfterBreak));

		BoneData.Shape.box.m_halfsize.x = Shape.Obb.X / 200.f;
		BoneData.Shape.box.m_halfsize.y = Shape.Obb.Z / 200.f;
		BoneData.Shape.box.m_halfsize.z = Shape.Obb.Y / 200.f;

		InRotation.rotation(StalkerMath::UnrealQuaternionToRBMK(FQuat(Shape.Obb.Rotation)));
		BoneData.Shape.box.xform_set(InRotation);
		BoneData.Shape.box.m_translate = StalkerMath::UnrealLocationToRBMK(Shape.Obb.Center);
		BoneData.Shape.cylinder.m_center = StalkerMath::UnrealLocationToRBMK(Shape.Cylinder.Center);
		BoneData.Shape.cylinder.m_radius = Shape.Cylinder.Radius / 100.f;
		BoneData.Shape.cylinder.m_height = Shape.Cylinder.Length / 100.f;

		FRotator RotationToXRay(0, 0, 90);
		Fquaternion CylinderQuat = StalkerMath::UnrealQuaternionToRBMK(FQuat(Shape.Cylinder.Rotation) * FQuat(RotationToXRay));
		InRotation.rotation(CylinderQuat);
		BoneData.Shape.cylinder.m_direction = InRotation.k;

		BoneData.Shape.sphere.P = StalkerMath::UnrealLocationToRBMK(Shape.Sphere.Center);
		BoneData.Shape.sphere.R  = Shape.Sphere.Radius / 100.f;
	}
	{
		BoneData.IKData.break_force = IKData.BreakForce;
		BoneData.IKData.break_torque = IKData.BreakTorque;
		BoneData.IKData.damping_factor = IKData.DampingFactor;
		BoneData.IKData.friction = IKData.Friction;
		BoneData.IKData.ik_flags.zero();
		BoneData.IKData.ik_flags.set(SJointIKData::flBreakable, IKData.IsBreakable);

		BoneData.IKData.limits[0].limit.x = IKData.LimitsX.X;
		BoneData.IKData.limits[0].limit.y = IKData.LimitsX.Y;
		BoneData.IKData.limits[0].spring_factor = IKData.LimitsX.Z;
		BoneData.IKData.limits[0].damping_factor = IKData.LimitsX.W;

		BoneData.IKData.limits[1].limit.x = IKData.LimitsY.X;
		BoneData.IKData.limits[1].limit.y = IKData.LimitsY.Y;
		BoneData.IKData.limits[1].spring_factor = IKData.LimitsY.Z;
		BoneData.IKData.limits[1].damping_factor = IKData.LimitsY.W;

		BoneData.IKData.limits[2].limit.x = IKData.LimitsZ.X;
		BoneData.IKData.limits[2].limit.y = IKData.LimitsZ.Y;
		BoneData.IKData.limits[2].spring_factor = IKData.LimitsZ.Z;
		BoneData.IKData.limits[2].damping_factor = IKData.LimitsZ.W;

		BoneData.IKData.spring_factor = IKData.SpringFactor;
		switch (IKData.Type)
		{
		default:
			BoneData.IKData.type = jtNone;
			break;
		case EStalkerJointType::EJT_Rigid:
			BoneData.IKData.type = jtRigid;
			break;
		case EStalkerJointType::EJT_Cloth:
			BoneData.IKData.type = jtCloth;
			break;
		case EStalkerJointType::EJT_Joint:
			BoneData.IKData.type = jtJoint;
			break;
		case EStalkerJointType::EJT_Wheel:
			BoneData.IKData.type = jtWheel;
			break;
		case EStalkerJointType::EJT_Slider:
			BoneData.IKData.type = jtSlider;
			break;
		}
	}
}

