#pragma once
namespace StalkerMath
{
	inline FMatrix	XRayMatrixToUnreal(const Fmatrix& InMatrix)
	{
		FPlane InTargetX = FPlane(InMatrix.i.x, -InMatrix.i.z,-InMatrix.i.y,-InMatrix._14);
		FPlane InTargetZ = FPlane( -InMatrix.j.x,InMatrix.j.z,InMatrix.j.y,InMatrix._24);
		FPlane InTargetY = FPlane( -InMatrix.k.x,InMatrix.k.z,InMatrix.k.y,InMatrix._34);
		FPlane InTargetW = FPlane( -InMatrix.c.x*100,InMatrix.c.z*100,InMatrix.c.y*100,InMatrix._44);
		return {InTargetX, InTargetY, InTargetZ, InTargetW};
	}
	/*inline Fmatrix	UnrealMatrixToXRay(const FMatrix& InMatrix)
	{
		Fvector4 InTargetX;InTargetX.set(-InMatrix.M[0], InMatrix.M[2],InMatrix.M[1],InMatrix.M[3]);
		
		return {InTargetX, InTargetY, InTargetZ, InTargetW};
	}*/
	inline FVector3f XRayLocationToUnreal(Fvector Location)
	{
		return FVector3f(-Location.x * 100, Location.z * 100, Location.y * 100);
	}
	inline FVector3f XRayNormalToUnreal(Fvector Location)
	{
		return FVector3f(-Location.x , Location.z , Location.y );
	}
	inline FQuat4f	XRayQuatToUnreal(const Fquaternion&Rotation)
	{
		return FQuat4f(Rotation.x, -Rotation.z, -Rotation.y, Rotation.w);
	}
	inline FQuat4f	XRayQuatToUnreal(const Fmatrix&Matrix)
	{
		Fquaternion Quat;
		Quat.set(Matrix);
		return XRayQuatToUnreal(Quat);
	}
	inline FQuat4f	XRayHPBToUnreal(const Fvector& Rotation)
	{
		Fmatrix Matrix;
		Matrix.setHPB(Rotation.x, Rotation.y, Rotation.z);
		return XRayQuatToUnreal(Matrix);
	}
	inline FQuat4f	XRayXYZToUnreal(const Fvector& Rotation)
	{
		Fmatrix Matrix;
		Matrix.setXYZ(Rotation.x, Rotation.y, Rotation.z);
		return XRayQuatToUnreal(Matrix);
	}
	inline FQuat4f	XRayXYZiToUnreal(const Fvector& Rotation)
	{
		Fmatrix Matrix;
		Matrix.setXYZi(Rotation.x, Rotation.y, Rotation.z);
		return XRayQuatToUnreal(Matrix);
	}



	inline Fvector UnrealLocationToXRay(FVector3f Location)
	{
		return Fvector().set(-Location.X / 100.f, Location.Z / 100.f, Location.Y / 100.f);
	}
	inline Fvector UnrealNormalToXRay(FVector3f Normal)
	{
		return Fvector().set(-Normal.X, Normal.Z , Normal.Y);
	}
	inline Fvector UnrealLocationToXRay(FVector Location)
	{
		return Fvector().set(-Location.X / 100.f, Location.Z / 100.f, Location.Y / 100.f);
	}
	inline Fvector UnrealNormalToXRay(FVector Normal)
	{
		return Fvector().set(-Normal.X, Normal.Z, Normal.Y);
	}

	inline Fquaternion	UnrealQuatToXRay(const FQuat4f& Quat)
	{
		return Fquaternion().set(Quat.W, Quat.X, -Quat.Z, -Quat.Y);
	}

	inline Fquaternion	UnrealQuatToXRay(const FQuat& Quat)
	{
		return Fquaternion().set(Quat.W, Quat.X, -Quat.Z, -Quat.Y);
	}
}

FORCEINLINE uint32 GetTypeHash(shared_str Name)
{
	// This must match the GetTypeHash behavior of FString
	return Name._get()->dwCRC;
}
