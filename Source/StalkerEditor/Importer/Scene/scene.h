#pragma once

DECLARE_DELEGATE_OneParam( FRMBKSceneAppendObjectDelegate, FXRayCustomObject*);

struct st_LevelOptions
{
	shared_str		m_FNLevelPath;
    shared_str		m_LevelPrefix;
	shared_str 		m_BOPText;
	shared_str 		m_map_version;
    

    GameTypeChooser	m_mapUsage;
    st_LevelOptions	();

    

	void 			Read			(IReader&);
	void 			ReadLTX			(CInifile&);
    void			Reset			();
};

class EScene 
{
public:
	EScene();
	~EScene();
	st_LevelOptions	m_LevelOp;
	xrGUID			m_GUID;
	shared_str		m_OwnerName;
	time_t			m_CreateTime;
	TMap<ERBMKSceneObjectType,TSharedPtr<FRBMKSceneToolBase>>   SceneTools;
protected:
	bool 			OnLoadAppendObject(FXRayCustomObject* O);
	bool 			OnLoadSelectionAppendObject(FXRayCustomObject* O);
protected:
	void			RegisterSceneTools(TSharedPtr<FRBMKSceneToolBase> SceneToolBase);
	void			CreateSceneTools();
	void			DestroySceneTools();

public:

	bool 			ReadObjectStream(IReader& F, FXRayCustomObject*& O);
	bool 			ReadObjectLTX(CInifile& ini, LPCSTR sect_name, FXRayCustomObject*& O);
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

	FXRayCustomObject*  FindObjectByName(LPCSTR name, ERBMKSceneObjectType classfilter);
	FXRayCustomObject*	FindObjectByName(LPCSTR name, FXRayCustomObject* pass_object);
	void			GenObjectName(ERBMKSceneObjectType cls_id, char* buffer, const char* pref);
	void 			FindObjectByNameCB(LPCSTR new_name, bool& res) { res = !!FindObjectByName(new_name, (FXRayCustomObject*)0); }
public:
	bool			ExportGame(SExportStreams* F);

	bool 			Load(LPCSTR map_name, bool bUndo);
	bool 			LoadLTX(LPCSTR map_name, bool bUndo);


	bool 			LoadSelection(LPCSTR fname);

	bool			LoadToolLTX(ERBMKSceneObjectType clsid, LPCSTR fn);

	
	void 			Clear();
	void			Reset();

	IC ObjectList&  ListObj(ERBMKSceneObjectType cat) { VERIFY(GetOTool(cat)); return GetOTool(cat)->GetObjects(); }
	IC ObjectIt 	FirstObj(ERBMKSceneObjectType cat) { VERIFY(GetOTool(cat)); return ListObj(cat).begin(); }
	IC ObjectIt 	LastObj(ERBMKSceneObjectType cat) { VERIFY(GetOTool(cat)); return ListObj(cat).end(); }
	IC int 			ObjCount(ERBMKSceneObjectType cat) { return ListObj(cat).size(); }
	int 			ObjCount();

	IC FRBMKSceneToolBase*	  GetTool(ERBMKSceneObjectType cat) { return SceneTools[cat].Get(); }
	IC int32				  ToolCount() { return SceneTools.Num(); }
	IC FXRaySceneCustomOTool* GetOTool(ERBMKSceneObjectType cat) { return GetTool(cat)->CastToSceneCustomOTool(); }


	virtual void 	AppendObject(FXRayCustomObject* object, bool bExecUnd = true);
	virtual bool 	RemoveObject(FXRayCustomObject* object, bool bExecUndo, bool bDeleting);


	xr_string		LevelPath();
	shared_str 		LevelPrefix() { return m_LevelOp.m_LevelPrefix; }

	bool            GetSubstObjectName(const xr_string& from, xr_string& to) const;
	
	void			GetObjects(ERBMKSceneObjectType InType,ObjectList&OutObjects);

};

extern EScene* Scene;
