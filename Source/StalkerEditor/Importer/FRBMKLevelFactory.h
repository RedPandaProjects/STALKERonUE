#pragma once
#include "FRBMKEngineFactory.h"
class FRBMKLevelFactory
{
public:
	FRBMKLevelFactory(UObject* InParentPackage, EObjectFlags InFlags);
	~FRBMKLevelFactory();
	bool ImportLevel(const FString& FileName,class URBMKLevelImportOptions&LevelImportOptions);
	inline UObject* GetCreatedObject() { return EngineFactory.GetCreatedObject(); }
private:
	FRBMKEngineFactory EngineFactory;
	UObject* ParentPackage;
	EObjectFlags ObjectFlags;
};