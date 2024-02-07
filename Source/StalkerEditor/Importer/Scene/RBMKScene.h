#pragma once
#include "RBMKSceneObjectType.h"
#include "Tools/Base/RBMKSceneToolBase.h"

DECLARE_DELEGATE_OneParam(FRMBKSceneAppendObjectDelegate, TSharedPtr<FRBMKSceneObjectBase>);

class FRBMKScene 
{
public:
										FRBMKScene			();
										~FRBMKScene			();

	bool 								Load				(const FString&FileName);
	bool 								LoadLTX				(const FString&FileName);
	bool 								LoadSelection		(const FString&FileName);

	FRBMKSceneToolBase*					GetTool				(ERBMKSceneObjectType ObjectType);
	FRBMKSceneObjectsToolBase*			GetObjectTool		(ERBMKSceneObjectType ObjectType);
	void 								AppendObject		(const TSharedPtr<FRBMKSceneObjectBase>& Object);
	bool 								RemoveObject		(const TSharedPtr<FRBMKSceneObjectBase>& Object);
	TSharedPtr<FRBMKSceneObjectBase>	FindObjectByName	(const FString& Name, ERBMKSceneObjectType ObjectType);
	TSharedPtr<FRBMKSceneObjectBase>	FindObjectByName	(const FString& Name);
	TSharedPtr<FRBMKSceneObjectBase>	ReadObjectStream	(IReader& F);
	TSharedPtr<FRBMKSceneObjectBase>	ReadObjectLTX		(CInifile& INIFile, const FString& SectionName);
	bool 								ReadObjectsStream	(IReader& F, u32 ChunkID, const FRMBKSceneAppendObjectDelegate& AppendObjectDelegate);
	bool 								ReadObjectsLTX		(CInifile& INIFile, const FString& SectionNameParent,  const FString& SectionNamePrefix, const FRMBKSceneAppendObjectDelegate& AppendObjectDelegate);

	bool								LoadLevelPart		(FRBMKSceneToolBase* SceneTool, const FString&FileName);
	bool								LoadLevelPartStream	(FRBMKSceneToolBase* SceneTool, const FString&FileName);
	bool								LoadLevelPartLTX	(FRBMKSceneToolBase* SceneTool, const FString&FileName);
	static bool							IsLTXFile			(const FString&FileName);
private:
	
	xrGUID														SceneXRGUID;
	TMap<ERBMKSceneObjectType,TSharedPtr<FRBMKSceneToolBase>>   SceneTools;
};

extern FRBMKScene*GRBMKScene;