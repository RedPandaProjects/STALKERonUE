#pragma once
#include "FoliageType_InstancedStaticMesh.h"

struct FRBMKDetailVertex
{
	FVector3f Position;
	FVector2f UV;

	inline bool	operator == (const FRBMKDetailVertex& V) const
    {
        if (!UV.Equals(V.UV))	return false;
        if (!Position.Equals(V.Position))	return false;
        return true;
    }
};

class  FRBMKDetail
{
public:
													FRBMKDetail				();
													~FRBMKDetail			();

	bool											Load					(IReader&Reader);
    FString											GetName					() const;
	UStaticMesh*									GetOrCreateStaticMesh	();
	UFoliageType_InstancedStaticMesh*				GetOrCreateFoliageType	();

	FBoxSphereBounds3f								Bound;
	TArray<FRBMKDetailVertex>						Vertices;
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

