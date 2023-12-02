#pragma once
#include "FoliageType_InstancedStaticMesh.h"

struct FXRayDetailVertex
{
	FVector3f Position;
	FVector2f UV;

	inline bool	operator == (const FXRayDetailVertex& V) const
    {
        if (!UV.Equals(V.UV))	return false;
        if (!Position.Equals(V.Position))	return false;
        return true;
    }
};

class  FXRayDetail
{
public:
													FXRayDetail				();
													~FXRayDetail			();

	bool											Load					(IReader&Reader);
    FString											GetName					() const;
	UStaticMesh*									GetOrCreateStaticMesh	();
	UFoliageType_InstancedStaticMesh*				GetOrCreateFoliageType	();

	FBoxSphereBounds3f								Bound;
	TArray<FXRayDetailVertex>						Vertices;
	TArray<int32>									Indices;
	Flags32											Flags;
	float											MinScale;
	float											MaxScale;
	float 											DensityFactor;
    FString											ReferenceName;
	CEditableObject*								ReferencePtr;

	FString											TextureName;
	FString											ShaderName;
private:
	bool											Update					(const FString&Name);
						
};

