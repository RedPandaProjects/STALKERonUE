#pragma once

DECLARE_DELEGATE_OneParam( FRMBKSceneAppendObjectDelegate,TSharedPtr<FXRayCustomObject>);

class EScene 
{
public:
	EScene();
	~EScene();
	xrGUID			m_GUID;
	TMap<ERBMKSceneObjectType,TSharedPtr<FRBMKSceneToolBase>>   SceneTools;
protected:
	void			RegisterSceneTools(TSharedPtr<FRBMKSceneToolBase> SceneToolBase);
	void			CreateSceneTools();
	void			DestroySceneTools();

public:

	TSharedPtr<FXRayCustomObject>	ReadObjectStream(IReader& F);
	TSharedPtr<FXRayCustomObject>	ReadObjectLTX(CInifile& ini, LPCSTR sect_name);
	bool 			ReadObjectsStream(IReader& F, u32 chunk_id, FRMBKSceneAppendObjectDelegate on_append);
	bool 			ReadObjectsLTX(CInifile& ini, LPCSTR sect_name_parent, LPCSTR sect_name_prefix, FRMBKSceneAppendObjectDelegate on_append);


	xr_string		LevelPartPath(LPCSTR map_name);
	xr_string		LevelPartName(LPCSTR map_name, ERBMKSceneObjectType cls);

	BOOL			LoadLevelPart(FRBMKSceneToolBase* M, LPCSTR map_name);
	BOOL			LoadLevelPartStream(FRBMKSceneToolBase* M, LPCSTR map_name);
	BOOL			LoadLevelPartLTX(FRBMKSceneToolBase* M, LPCSTR map_name);

	BOOL			LoadLevelPart(LPCSTR map_name, ERBMKSceneObjectType cls);
	BOOL		 	UnloadLevelPart(FRBMKSceneToolBase* M);
	BOOL			UnloadLevelPart(LPCSTR map_name, ERBMKSceneObjectType cls);

	TSharedPtr<FXRayCustomObject>  FindObjectByName(LPCSTR name, ERBMKSceneObjectType classfilter);
	TSharedPtr<FXRayCustomObject>	FindObjectByName(LPCSTR name);
	void			GenObjectName(ERBMKSceneObjectType cls_id, char* buffer, const char* pref);
public:

	bool 			Load(LPCSTR map_name, bool bUndo);
	bool 			LoadLTX(LPCSTR map_name, bool bUndo);


	bool 			LoadSelection(LPCSTR fname);

	bool			LoadToolLTX(ERBMKSceneObjectType clsid, LPCSTR fn);

	
	void 			Clear();

	int 			ObjCount();

	IC FRBMKSceneToolBase*	  GetTool(ERBMKSceneObjectType cat) { return SceneTools[cat].Get(); }
	IC int32				  ToolCount() { return SceneTools.Num(); }
	IC FRBMKSceneObjectsToolBase* GetOTool(ERBMKSceneObjectType cat) { return GetTool(cat)->CastToSceneCustomOTool(); }


	 void 	AppendObject(TSharedPtr<FXRayCustomObject> Object);
	 bool 	RemoveObject(TSharedPtr<FXRayCustomObject> Object);


	bool            GetSubstObjectName(const xr_string& from, xr_string& to) const;
	

};

extern EScene* Scene;
