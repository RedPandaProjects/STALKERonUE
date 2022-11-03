#pragma once
class XRayEngineFactory
{
public:
	XRayEngineFactory(UObject* InParentPackage, EObjectFlags InFlags);
	~XRayEngineFactory();
	UStaticMesh* ImportObjectAsStaticMesh(const FString&FileName);
private:
	UObject* ParentPackage;
	EObjectFlags ObjectFlags;
};