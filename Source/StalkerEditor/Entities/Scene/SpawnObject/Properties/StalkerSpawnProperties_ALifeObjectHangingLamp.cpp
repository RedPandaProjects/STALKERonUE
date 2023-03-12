#include "StalkerSpawnProperties_ALifeObjectHangingLamp.h"

void UStalkerSpawnProperties_ALifeObjectHangingLamp::SetEntity(ISE_Abstract* InEntity)
{
	ALifeObjectHangingLamp = nullptr;

	if (InEntity)
	{
		ALifeObjectHangingLamp = reinterpret_cast<ISE_ALifeObjectHangingLamp*>(InEntity->QueryPropertiesInterface(EXRaySpawnPropertiesType::CSE_ALifeObjectHangingLamp));
		check(ALifeObjectHangingLamp);
	}
	Super::SetEntity(InEntity);
}

void UStalkerSpawnProperties_ALifeObjectHangingLamp::FillProperties()
{
	Super::FillProperties();
	Color = FColor(ALifeObjectHangingLamp->color);
	Intensity = ALifeObjectHangingLamp->brightness;
	Range = ALifeObjectHangingLamp->range;
	LightFunctionMaterial = nullptr;
	if (ALifeObjectHangingLamp->light_material.size())
	{
		LightFunctionMaterial = LoadObject<UMaterialInterface>(this,ANSI_TO_TCHAR(ALifeObjectHangingLamp->light_material.c_str()), nullptr, LOAD_NoWarn);
	}
	AttachBone = ALifeObjectHangingLamp->light_main_bone.c_str();
	ColorAnimator.Value =TEXT("none");
	if (ALifeObjectHangingLamp->color_animator.size())
	{
		ColorAnimator.Value = ALifeObjectHangingLamp->color_animator.c_str();
	}

	SpotConeAngle = FMath::RadiansToDegrees(ALifeObjectHangingLamp->spot_cone_angle);
	FString FixedBonesSTR = ALifeObjectHangingLamp->fixed_bones.c_str();
	FixedBonesSTR.ParseIntoArray(FixedBones,TEXT(","));
	Physic = ALifeObjectHangingLamp->light_flags.is(ISE_ALifeObjectHangingLamp::flPhysic);
	CastShadow = ALifeObjectHangingLamp->light_flags.is(ISE_ALifeObjectHangingLamp::flCastShadow);
	IsSpot = ALifeObjectHangingLamp->light_flags.is(ISE_ALifeObjectHangingLamp::flTypeSpot);
	Health = ALifeObjectHangingLamp->m_health;

}

void UStalkerSpawnProperties_ALifeObjectHangingLamp::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property||!ALifeObjectHangingLamp)
	{
		return;
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeObjectHangingLamp, Color))
	{
		ALifeObjectHangingLamp->color = Color.ToPackedARGB();
		Entity->set_editor_flag(ISE_Abstract::flLightChange);
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeObjectHangingLamp, Intensity))
	{
		ALifeObjectHangingLamp->brightness = Intensity;
		Entity->set_editor_flag(ISE_Abstract::flLightChange);
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeObjectHangingLamp, Range))
	{
		ALifeObjectHangingLamp->range = Range;
		Entity->set_editor_flag(ISE_Abstract::flLightChange);
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeObjectHangingLamp, LightFunctionMaterial))
	{
		ALifeObjectHangingLamp->light_material = "";
		if (LightFunctionMaterial)
		{
			ALifeObjectHangingLamp->light_material = TCHAR_TO_ANSI(*LightFunctionMaterial->GetPathName());
		}
		Entity->set_editor_flag(ISE_Abstract::flLightChange);
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeObjectHangingLamp, AttachBone))
	{
		ALifeObjectHangingLamp->light_main_bone = TCHAR_TO_ANSI(*AttachBone);
		Entity->set_editor_flag(ISE_Abstract::flLightChange);
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeObjectHangingLamp, SpotConeAngle))
	{
		ALifeObjectHangingLamp->spot_cone_angle = FMath::DegreesToRadians(SpotConeAngle);
		Entity->set_editor_flag(ISE_Abstract::flLightChange);
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeObjectHangingLamp, FixedBones))
	{
		FString FixedBonesSTR;
		for (FString& Bone : FixedBones)
		{
			if (FixedBonesSTR.Len())
			{
				FixedBonesSTR.AppendChar(TEXT(','));
			}
			FixedBonesSTR.Append(Bone);
		}
		ALifeObjectHangingLamp->fixed_bones = TCHAR_TO_ANSI(*FixedBonesSTR);
		Entity->set_editor_flag(ISE_Abstract::flLightChange);
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeObjectHangingLamp, Physic))
	{
		ALifeObjectHangingLamp->light_flags.set(ISE_ALifeObjectHangingLamp::flPhysic, Physic);
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeObjectHangingLamp, CastShadow))
	{
		ALifeObjectHangingLamp->light_flags.set(ISE_ALifeObjectHangingLamp::flCastShadow, CastShadow);
		Entity->set_editor_flag(ISE_Abstract::flLightChange);
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeObjectHangingLamp, IsSpot))
	{
		ALifeObjectHangingLamp->light_flags.set(ISE_ALifeObjectHangingLamp::flTypeSpot, IsSpot);
		Entity->set_editor_flag(ISE_Abstract::flLightChange);
	}
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeObjectHangingLamp, Health))
	{
		ALifeObjectHangingLamp->m_health = Health;
	}
	if (!PropertyChangedEvent.MemberProperty)
	{
		return;
	}
	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerSpawnProperties_ALifeObjectHangingLamp, ColorAnimator))
	{
		ALifeObjectHangingLamp->color_animator = "";
		if (ColorAnimator.Value != TEXT("none"))
		{
			ALifeObjectHangingLamp->color_animator = TCHAR_TO_ANSI(*ColorAnimator.Value);
		}
		Entity->set_editor_flag(ISE_Abstract::flLightChange);
	}

}
