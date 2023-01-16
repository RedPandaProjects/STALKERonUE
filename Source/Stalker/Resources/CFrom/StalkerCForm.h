#pragma once
#include "StalkerCForm.generated.h"

struct FStalkerCFormTriangle
{
	uint32 VertexIndex0;
	uint32 VertexIndex1;
	uint32 VertexIndex2;
	uint32 MaterialIndex;
};

UCLASS()
class STALKER_API UStalkerCForm : public UObject
{
	GENERATED_BODY()
public:
	

	TArray<FStalkerCFormTriangle>	Triangles;
	TArray<FVector3f>				Vertices;
	
	UPROPERTY()
	TArray<FString>					Name2ID;

	UPROPERTY()
	FBox3f							AABB;

	void Serialize(FArchive& Ar) override;
#if WITH_EDITORONLY_DATA
	void InvalidCForm();
#endif
private:
	const int32 Version = 0;
};
