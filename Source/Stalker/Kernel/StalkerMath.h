#pragma once
namespace StalkerMath
{
	inline FMatrix	RBMKMatrixToUnreal(const Fmatrix& InMatrix)
	{
		FPlane InTargetX = FPlane(InMatrix.i.x, -InMatrix.i.z,-InMatrix.i.y,-InMatrix._14);
		FPlane InTargetZ = FPlane( -InMatrix.j.x,InMatrix.j.z,InMatrix.j.y,InMatrix._24);
		FPlane InTargetY = FPlane( -InMatrix.k.x,InMatrix.k.z,InMatrix.k.y,InMatrix._34);
		FPlane InTargetW = FPlane( -InMatrix.c.x*100,InMatrix.c.z*100,InMatrix.c.y*100,InMatrix._44);
		return {InTargetX, InTargetY, InTargetZ, InTargetW};
	}
	inline Fmatrix	UnrealMatrixToRBMK(const FMatrix& InMatrix)
	{
		Fmatrix Result;
		Result.i.set(InMatrix.M[0][0],-InMatrix.M[0][2],-InMatrix.M[0][1]); Result._14 = -InMatrix.M[0][3];
		Result.j.set(-InMatrix.M[2][0],InMatrix.M[2][2],InMatrix.M[2][1]); Result._34 = InMatrix.M[2][3];
		Result.k.set(-InMatrix.M[1][0],InMatrix.M[1][2],InMatrix.M[1][1]); Result._24 = InMatrix.M[1][3];
		Result.c.set(-InMatrix.M[3][0]/100.f,InMatrix.M[3][2]/100.f,InMatrix.M[3][1]/100.f); Result._44 = InMatrix.M[3][3];
		return Result;
	}

	inline FVector3f RBMKLocationToUnreal(const Fvector&Location)
	{
		return FVector3f(-Location.x * 100, Location.z * 100, Location.y * 100);
	}

	inline FVector3f RBMKNormalToUnreal(const Fvector&Location)
	{
		return FVector3f(-Location.x , Location.z , Location.y );
	}

	inline FQuat4f	RBMKQuaternionToUnreal(const Fquaternion&Rotation)
	{
		return FQuat4f(Rotation.x, -Rotation.z, -Rotation.y, Rotation.w);
	}

	inline FQuat4f	RBMKQuaternionToUnreal(const Fmatrix&Matrix)
	{
		Fquaternion Quaternion;
		Quaternion.set(Matrix);
		return RBMKQuaternionToUnreal(Quaternion);
	}

	inline Fvector UnrealLocationToRBMK(const FVector3f Location)
	{
		return Fvector().set(-Location.X / 100.f, Location.Z / 100.f, Location.Y / 100.f);
	}

	inline Fvector UnrealNormalToRBMK(const FVector3f Normal)
	{
		return Fvector().set(-Normal.X, Normal.Z , Normal.Y);
	}

	inline Fvector UnrealLocationToRBMK(const FVector& Location)
	{
		return Fvector().set(-Location.X / 100.f, Location.Z / 100.f, Location.Y / 100.f);
	}

	inline Fvector UnrealNormalToRBMK(const FVector& Normal)
	{
		return Fvector().set(-Normal.X, Normal.Z, Normal.Y);
	}

	inline Fquaternion	UnrealQuaternionToRBMK(const FQuat& Quaternion)
	{
		return Fquaternion().set(Quaternion.W, Quaternion.X, -Quaternion.Z, -Quaternion.Y);
	}
}

FORCEINLINE uint32 GetTypeHash(const shared_str& Name)
{
	// This must match the GetTypeHash behavior of FString
	return Name._get()->dwCRC;
}
