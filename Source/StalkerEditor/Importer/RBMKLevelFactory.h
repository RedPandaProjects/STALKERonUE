#pragma once
#include "RBMKEngineFactory.h"
class RBMKLevelFactory
{
public:
	RBMKLevelFactory(UObject* InParentPackage, EObjectFlags InFlags);
	~RBMKLevelFactory();
	bool ImportLevel(const FString& FileName,class UXRayLevelImportOptions&LevelImportOptions);
	inline UObject* GetCreatedObject() { return EngineFactory.GetCreatedObject(); }
private:
	UMaterialInterface*	ImportSurfaceForDecal(shared_str ShaderName, shared_str TextureName);
	RBMKEngineFactory EngineFactory;
	UObject* ParentPackage;
	EObjectFlags ObjectFlags;
};