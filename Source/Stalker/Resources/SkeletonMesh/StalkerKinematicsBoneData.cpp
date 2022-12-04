#include "StalkerKinematicsBoneData.h"
void FStalkerKinematicsBone::BuildFromLegacy(const CBoneData& Legacy)
{
	Mass  =  Legacy.mass;
	PhysicsMaterialName = Legacy.game_mtl_name.c_str();
	CenterOfMass = StalkerMath::XRayLocationToUnreal(Legacy.center_of_mass);

	Obb.Center = FVector( StalkerMath::XRayLocationToUnreal(Legacy.obb.m_translate));
	Obb.X = Legacy.obb.m_halfsize.x * 200;
	Obb.Y = Legacy.obb.m_halfsize.z * 200;
	Obb.Z = Legacy.obb.m_halfsize.y * 200;
	Fmatrix InRotation;
	Legacy.obb.xform_get(InRotation);
	Obb.Rotation = FRotator(FQuat(StalkerMath::XRayQuatToUnreal(InRotation)));
	
	//shape
	{
		switch (Legacy.shape.type)
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

		if(Legacy.shape.flags .test(int32(SBoneShape::sfNoPickable)))
			Shape.Flags |=int32(EStalkerShapeFlags::ESF_NoPickable);

		if (Legacy.shape.flags.test(int32(SBoneShape::sfNoPhysics)))
			Shape.Flags |= int32(EStalkerShapeFlags::ESF_NoPhysics);

		if (Legacy.shape.flags.test(int32(SBoneShape::sfNoFogCollider)))
			Shape.Flags |= int32(EStalkerShapeFlags::ESF_NoFogCollider);

		if (Legacy.shape.flags.test(int32(SBoneShape::sfRemoveAfterBreak)))
			Shape.Flags |= int32(EStalkerShapeFlags::ESF_RemoveAfterBreak);

		Shape.Obb.Center = FVector(StalkerMath::XRayLocationToUnreal(Legacy.shape.box.m_translate));
		Shape.Obb.X = Legacy.shape.box.m_halfsize.x * 200;
		Shape.Obb.Y = Legacy.shape.box.m_halfsize.z * 200;
		Shape.Obb.Z = Legacy.shape.box.m_halfsize.y * 200;
		Legacy.shape.box.xform_get(InRotation);
		Shape.Obb.Rotation = FRotator(FQuat(StalkerMath::XRayQuatToUnreal(InRotation)));


		Shape.Cylinder.Center = FVector(StalkerMath::XRayLocationToUnreal(Legacy.shape.cylinder.m_center));
		Shape.Cylinder.Radius = Legacy.shape.cylinder.m_radius * 100;
		Shape.Cylinder.Length = Legacy.shape.cylinder.m_height * 100;
		InRotation.identity();
		InRotation.k.set(Legacy.shape.cylinder.m_direction);
		Fvector::generate_orthonormal_basis(InRotation.k, InRotation.j, InRotation.i);
		Fquaternion InQuat; InQuat.set(InRotation);
		FRotator RotationToUnreal(0, 0, -90);
		FRotator OutRotation(FQuat(InQuat.x, -InQuat.z, -InQuat.y, InQuat.w) * FQuat(RotationToUnreal));
		Shape.Cylinder.Rotation = OutRotation;

		Shape.Sphere.Center = FVector(StalkerMath::XRayLocationToUnreal(Legacy.shape.sphere.P));
		Shape.Sphere.Radius = Legacy.shape.sphere.R * 100;
	}
	{
		IKData.BreakForce = Legacy.IK_data.break_force;
		IKData.BreakTorque = Legacy.IK_data.break_torque;
		IKData.DampingFactor = Legacy.IK_data.damping_factor;
		IKData.Friction = Legacy.IK_data.friction;
		IKData.IsBreakable = Legacy.IK_data.ik_flags.test(SJointIKData::flBreakable);
		IKData.LimitsX.Set(Legacy.IK_data.limits[0].limit.x, Legacy.IK_data.limits[0].limit.y, Legacy.IK_data.limits[0].spring_factor, Legacy.IK_data.limits[0].damping_factor);
		IKData.LimitsY.Set(Legacy.IK_data.limits[1].limit.x, Legacy.IK_data.limits[1].limit.y, Legacy.IK_data.limits[1].spring_factor, Legacy.IK_data.limits[1].damping_factor);
		IKData.LimitsZ.Set(Legacy.IK_data.limits[2].limit.x, Legacy.IK_data.limits[2].limit.y, Legacy.IK_data.limits[2].spring_factor, Legacy.IK_data.limits[2].damping_factor);
		IKData.SpringFactor = Legacy.IK_data.spring_factor;
		switch (Legacy.IK_data.type)
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

void FStalkerKinematicsBone::BuildToLegacy(CBoneData& Legacy)
{
	Legacy.mass = Mass;
	Legacy.game_mtl_name = TCHAR_TO_ANSI(*PhysicsMaterialName.ToString());
	Legacy.center_of_mass = StalkerMath::UnrealLocationToXRay(CenterOfMass);
	
	Legacy.obb.m_halfsize.x = Obb.X / 200.f;
	Legacy.obb.m_halfsize.y = Obb.Z / 200.f;
	Legacy.obb.m_halfsize.z = Obb.Y / 200.f;

	Fmatrix InRotation;
	InRotation.rotation(StalkerMath::UnrealQuatToXRay(FQuat(Obb.Rotation)));
	Legacy.obb.xform_set(InRotation);
	Legacy.obb.m_translate = StalkerMath::UnrealLocationToXRay(Obb.Center);
	
	//shape
	{
		switch (Shape.Type)
		{
		case EStalkerShapeType::EST_None:
			Legacy.shape.type = SBoneShape::stNone;
			break;
		case EStalkerShapeType::EST_Box:
			Legacy.shape.type = SBoneShape::stBox;
			break;
		case EStalkerShapeType::EST_Sphere:
			Legacy.shape.type = SBoneShape::stSphere;
			break;
		case EStalkerShapeType::EST_Cylinder:
			Legacy.shape.type = SBoneShape::stCylinder;
			break;
		default:
			Legacy.shape.type = SBoneShape::stNone;
			break;
		}
		Legacy.shape.flags.zero();

		Legacy.shape.flags.set(int32(SBoneShape::sfNoPickable), Shape.Flags & int32(EStalkerShapeFlags::ESF_NoPickable));
		Legacy.shape.flags.set(int32(SBoneShape::sfNoPhysics), Shape.Flags & int32(EStalkerShapeFlags::ESF_NoPhysics));
		Legacy.shape.flags.set(int32(SBoneShape::sfNoFogCollider), Shape.Flags & int32(EStalkerShapeFlags::ESF_NoFogCollider));
		Legacy.shape.flags.set(int32(SBoneShape::sfRemoveAfterBreak), Shape.Flags & int32(EStalkerShapeFlags::ESF_RemoveAfterBreak));

		Legacy.shape.box.m_halfsize.x = Shape.Obb.X / 200.f;
		Legacy.shape.box.m_halfsize.y = Shape.Obb.Z / 200.f;
		Legacy.shape.box.m_halfsize.z = Shape.Obb.Y / 200.f;

		InRotation.rotation(StalkerMath::UnrealQuatToXRay(FQuat(Shape.Obb.Rotation)));
		Legacy.shape.box.xform_set(InRotation);
		Legacy.shape.box.m_translate = StalkerMath::UnrealLocationToXRay(Shape.Obb.Center);
		Legacy.shape.cylinder.m_center = StalkerMath::UnrealLocationToXRay(Shape.Cylinder.Center);
		Legacy.shape.cylinder.m_radius = Shape.Cylinder.Radius / 100.f;
		Legacy.shape.cylinder.m_height = Shape.Cylinder.Length / 100.f;

		FRotator RotationToXRay(0, 0, 90);
		Fquaternion CylinderQuat = StalkerMath::UnrealQuatToXRay(FQuat(Shape.Cylinder.Rotation) * FQuat(RotationToXRay));
		InRotation.rotation(CylinderQuat);
		Legacy.shape.cylinder.m_direction = InRotation.k;

		Legacy.shape.sphere.P = StalkerMath::UnrealLocationToXRay(Shape.Sphere.Center);
		Legacy.shape.sphere.R  = Shape.Sphere.Radius / 100.f;
	}
	{
		Legacy.IK_data.break_force = IKData.BreakForce;
		Legacy.IK_data.break_torque = IKData.BreakTorque;
		Legacy.IK_data.damping_factor = IKData.DampingFactor;
		Legacy.IK_data.friction = IKData.Friction;
		Legacy.IK_data.ik_flags.zero();
		Legacy.IK_data.ik_flags.set(SJointIKData::flBreakable, IKData.IsBreakable);

		Legacy.IK_data.limits[0].limit.x = IKData.LimitsX.X;
		Legacy.IK_data.limits[0].limit.y = IKData.LimitsX.Y;
		Legacy.IK_data.limits[0].spring_factor = IKData.LimitsX.Z;
		Legacy.IK_data.limits[0].damping_factor = IKData.LimitsX.W;

		Legacy.IK_data.limits[1].limit.x = IKData.LimitsY.X;
		Legacy.IK_data.limits[1].limit.y = IKData.LimitsY.Y;
		Legacy.IK_data.limits[1].spring_factor = IKData.LimitsY.Z;
		Legacy.IK_data.limits[1].damping_factor = IKData.LimitsY.W;

		Legacy.IK_data.limits[2].limit.x = IKData.LimitsZ.X;
		Legacy.IK_data.limits[2].limit.y = IKData.LimitsZ.Y;
		Legacy.IK_data.limits[2].spring_factor = IKData.LimitsZ.Z;
		Legacy.IK_data.limits[2].damping_factor = IKData.LimitsZ.W;

		Legacy.IK_data.spring_factor = IKData.SpringFactor;
		switch (IKData.Type)
		{
		default:
			Legacy.IK_data.type = jtNone;
			break;
		case EStalkerJointType::EJT_Rigid:
			Legacy.IK_data.type = jtRigid;
			break;
		case EStalkerJointType::EJT_Cloth:
			Legacy.IK_data.type = jtCloth;
			break;
		case EStalkerJointType::EJT_Joint:
			Legacy.IK_data.type = jtJoint;
			break;
		case EStalkerJointType::EJT_Wheel:
			Legacy.IK_data.type = jtWheel;
			break;
		case EStalkerJointType::EJT_Slider:
			Legacy.IK_data.type = jtSlider;
			break;
		}
	}
}

