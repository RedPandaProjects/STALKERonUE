// Fill out your copyright notice in the Description page of Project Settings.


#include "StalkerPhysicalMaterial.h"

void UStalkerPhysicalMaterial::BuildFromLegacy(const SGameMtl& Legacy)
{

	EStalkerPhysicalMaterialFlags1 InFlags1 = EStalkerPhysicalMaterialFlags1::NONE;
	EStalkerPhysicalMaterialFlags2 InFlags2 = EStalkerPhysicalMaterialFlags2::NONE;
	EStalkerPhysicalMaterialFlags3 InFlags3 = EStalkerPhysicalMaterialFlags3::NONE;

	if(Legacy.Flags.is(SGameMtl::flBreakable))
		InFlags1 |= EStalkerPhysicalMaterialFlags1::Breakable;
	if (Legacy.Flags.is(SGameMtl::flBounceable))
		InFlags1 |= EStalkerPhysicalMaterialFlags1::Bounceable;
	if (Legacy.Flags.is(SGameMtl::flBloodmark))
		InFlags1 |= EStalkerPhysicalMaterialFlags1::Bloodmark;
	if (Legacy.Flags.is(SGameMtl::flClimable))
		InFlags1 |= EStalkerPhysicalMaterialFlags1::Climable;
	if (Legacy.Flags.is(SGameMtl::flPassable))
		InFlags1 |= EStalkerPhysicalMaterialFlags1::Passable;
	if (Legacy.Flags.is(SGameMtl::flSkidmark))
		InFlags1 |= EStalkerPhysicalMaterialFlags1::Skidmark;

	if (Legacy.Flags.is(SGameMtl::flActorObstacle))
		InFlags2 |= EStalkerPhysicalMaterialFlags2::ActorObstacle;
	if (Legacy.Flags.is(SGameMtl::flDynamic))
		InFlags2 |= EStalkerPhysicalMaterialFlags2::Dynamic;
	if (Legacy.Flags.is(SGameMtl::flInjurious))
		InFlags2 |= EStalkerPhysicalMaterialFlags2::Injurious;
	if (Legacy.Flags.is(SGameMtl::flLiquid))
		InFlags2 |= EStalkerPhysicalMaterialFlags2::Liquid;
	if (Legacy.Flags.is(SGameMtl::flNoRicoshet))
		InFlags2 |= EStalkerPhysicalMaterialFlags2::NoRicoshet;
	if (Legacy.Flags.is(SGameMtl::flShootable))
		InFlags2 |= EStalkerPhysicalMaterialFlags2::Shootable;
	if (Legacy.Flags.is(SGameMtl::flSuppressShadows))
		InFlags2 |= EStalkerPhysicalMaterialFlags2::SuppressShadows;
	if (Legacy.Flags.is(SGameMtl::flSuppressWallmarks))
		InFlags2 |= EStalkerPhysicalMaterialFlags2::SuppressWallmarks;


	if (Legacy.Flags.is(SGameMtl::flSlowDown))
		InFlags3 |= EStalkerPhysicalMaterialFlags3::SlowDown;
	if (Legacy.Flags.is(SGameMtl::flTransparent))
		InFlags3 |= EStalkerPhysicalMaterialFlags3::Transparent;

	Flags1 = static_cast<int32>(InFlags1);
	Flags2 = static_cast<int32>(InFlags2);
	Flags3 = static_cast<int32>(InFlags3);

	BounceDamageFactor = Legacy.fBounceDamageFactor;
	DensityFactor = Legacy.fDensityFactor;
	FlotationFactor = Legacy.fFlotationFactor;
	InjuriousSpeed = Legacy.fInjuriousSpeed;
	PHBounceStartVelocity = Legacy.fPHBounceStartVelocity;
	PHBouncing = Legacy.fPHBouncing;
	PHDamping = Legacy.fPHDamping;
	PHFriction = Legacy.fPHFriction;
	PHSpring = Legacy.fPHSpring;
	ShootFactor = Legacy.fShootFactor;
	ShootFactorMP = Legacy.fShootFactorMP;
	SndOcclusionFactor = Legacy.fSndOcclusionFactor;
	VisTransparencyFactor = Legacy.fVisTransparencyFactor;
}

void UStalkerPhysicalMaterial::BuildToLegacy(SGameMtl& Legacy)
{
	EStalkerPhysicalMaterialFlags1 InFlags1 = static_cast<EStalkerPhysicalMaterialFlags1>(Flags1);
	EStalkerPhysicalMaterialFlags2 InFlags2 = static_cast<EStalkerPhysicalMaterialFlags2>(Flags2);
	EStalkerPhysicalMaterialFlags3 InFlags3 = static_cast<EStalkerPhysicalMaterialFlags3>(Flags3);
	Legacy.Flags.zero();
	if(EnumHasAnyFlags(InFlags1, EStalkerPhysicalMaterialFlags1::Breakable))
		Legacy.Flags.set(SGameMtl::flBreakable,TRUE);
	if (EnumHasAnyFlags(InFlags1, EStalkerPhysicalMaterialFlags1::Bloodmark))
		Legacy.Flags.set(SGameMtl::flBloodmark, TRUE);
	if (EnumHasAnyFlags(InFlags1, EStalkerPhysicalMaterialFlags1::Bounceable))
		Legacy.Flags.set(SGameMtl::flBounceable, TRUE);
	if (EnumHasAnyFlags(InFlags1, EStalkerPhysicalMaterialFlags1::Climable))
		Legacy.Flags.set(SGameMtl::flClimable, TRUE);
	if (EnumHasAnyFlags(InFlags1, EStalkerPhysicalMaterialFlags1::Passable))
		Legacy.Flags.set(SGameMtl::flPassable, TRUE);
	if (EnumHasAnyFlags(InFlags1, EStalkerPhysicalMaterialFlags1::Skidmark))
		Legacy.Flags.set(SGameMtl::flSkidmark, TRUE);


	if (EnumHasAnyFlags(InFlags2, EStalkerPhysicalMaterialFlags2::ActorObstacle))
		Legacy.Flags.set(SGameMtl::flActorObstacle, TRUE);
	if (EnumHasAnyFlags(InFlags2, EStalkerPhysicalMaterialFlags2::Dynamic))
		Legacy.Flags.set(SGameMtl::flDynamic, TRUE);
	if (EnumHasAnyFlags(InFlags2, EStalkerPhysicalMaterialFlags2::Injurious))
		Legacy.Flags.set(SGameMtl::flInjurious, TRUE);
	if (EnumHasAnyFlags(InFlags2, EStalkerPhysicalMaterialFlags2::Liquid))
		Legacy.Flags.set(SGameMtl::flLiquid, TRUE);
	if (EnumHasAnyFlags(InFlags2, EStalkerPhysicalMaterialFlags2::NoRicoshet))
		Legacy.Flags.set(SGameMtl::flNoRicoshet, TRUE);
	if (EnumHasAnyFlags(InFlags2, EStalkerPhysicalMaterialFlags2::Shootable))
		Legacy.Flags.set(SGameMtl::flShootable, TRUE);
	if (EnumHasAnyFlags(InFlags2, EStalkerPhysicalMaterialFlags2::SuppressShadows))
		Legacy.Flags.set(SGameMtl::flSuppressShadows, TRUE);
	if (EnumHasAnyFlags(InFlags2, EStalkerPhysicalMaterialFlags2::SuppressWallmarks))
		Legacy.Flags.set(SGameMtl::flSuppressWallmarks, TRUE);

	if (EnumHasAnyFlags(InFlags3, EStalkerPhysicalMaterialFlags3::SlowDown))
		Legacy.Flags.set(SGameMtl::flSlowDown, TRUE);
	if (EnumHasAnyFlags(InFlags3, EStalkerPhysicalMaterialFlags3::Transparent))
		Legacy.Flags.set(SGameMtl::flTransparent, TRUE);

	Legacy.fBounceDamageFactor = BounceDamageFactor;
	Legacy.fDensityFactor = DensityFactor;
	Legacy.fFlotationFactor = FlotationFactor;
	Legacy.fInjuriousSpeed = InjuriousSpeed ;
	Legacy.fPHBounceStartVelocity = PHBounceStartVelocity;
	Legacy.fPHBouncing = PHBouncing;
	Legacy.fPHDamping = PHDamping;
	Legacy.fPHFriction = PHFriction;
	Legacy.fPHSpring = PHSpring;
	Legacy.fShootFactor = ShootFactor;
	Legacy.fShootFactorMP = ShootFactorMP;
	Legacy.fSndOcclusionFactor = SndOcclusionFactor;
	Legacy.fVisTransparencyFactor = VisTransparencyFactor ;

}
