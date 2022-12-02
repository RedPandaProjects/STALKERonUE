#pragma once
namespace StalkerMath
{
	inline FVector3f XRayLocationToUnreal(Fvector Location)
	{
		return FVector3f(-Location.x * 100, Location.z * 100, Location.y * 100);
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
}

FORCEINLINE uint32 GetTypeHash(shared_str Name)
{
	// This must match the GetTypeHash behavior of FString
	return Name._get()->dwCRC;
}
