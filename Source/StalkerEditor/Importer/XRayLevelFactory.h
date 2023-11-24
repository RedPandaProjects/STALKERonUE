#pragma once
#include "XRayEngineFactory.h"
class XRayLevelFactory
{
public:
	XRayLevelFactory(UObject* InParentPackage, EObjectFlags InFlags);
	~XRayLevelFactory();
	bool ImportLevel(const FString& FileName,class UXRayLevelImportOptions&LevelImportOptions);
	inline UObject* GetCreatedObject() { return EngineFactory.GetCreatedObject(); }
private:
	UMaterialInterface*	ImportSurfaceForDecal(shared_str ShaderName, shared_str TextureName);
	XRayEngineFactory EngineFactory;
	UObject* ParentPackage;
	EObjectFlags ObjectFlags;
};