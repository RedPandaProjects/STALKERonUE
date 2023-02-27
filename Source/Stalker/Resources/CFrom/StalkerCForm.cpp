// Fill out your copyright notice in the Description page of Project Settings.


#include "StalkerCForm.h"
FArchive& operator<<(FArchive&Ar, FStalkerCFormTriangle&TRI)
{
	Ar<<TRI.VertexIndex0 << TRI.VertexIndex1 << TRI.VertexIndex2 << TRI.MaterialIndex;
	return Ar;
}
void UStalkerCForm::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	int32 CurrentVersion = Version;
	Ar << CurrentVersion;
	
	if(CurrentVersion!=0)
		return;

	if (Ar.IsLoading() || Ar.IsSaving())
	{
		Ar << Triangles;
		Ar << Vertices;
	}
}

#if WITH_EDITORONLY_DATA
void UStalkerCForm::InvalidCForm()
{
	Vertices.Empty();
	Triangles.Empty();
	Name2ID.Empty();
	AABB = FBox3f(ForceInit);
	Modify();
}
#endif