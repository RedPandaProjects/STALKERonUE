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
	XRayEngineFactory EngineFactory;
	UObject* ParentPackage;
	EObjectFlags ObjectFlags;
};