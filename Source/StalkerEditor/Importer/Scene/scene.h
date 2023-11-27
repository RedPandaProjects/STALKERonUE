#pragma once

//refs

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
	TMap<EXRayObjectClassID,TSharedPtr<FXRaySceneToolBase>>   SceneTools;
protected:
	bool 			OnLoadAppendObject(FXRayCustomObject* O);
	bool 			OnLoadSelectionAppendObject(FXRayCustomObject* O);
protected:
	void			RegisterSceneTools(TSharedPtr<FXRaySceneToolBase> SceneToolBase);
	void			CreateSceneTools();
	void			DestroySceneTools();

public:
	typedef  fastdelegate::FastDelegate1<FXRayCustomObject*, bool> TAppendObject;

	bool 			ReadObjectStream(IReader& F, FXRayCustomObject*& O);
	bool 			ReadObjectLTX(CInifile& ini, LPCSTR sect_name, FXRayCustomObject*& O);
	bool 			ReadObjectsStream(IReader& F, u32 chunk_id, TAppendObject on_append);
	bool 			ReadObjectsLTX(CInifile& ini, LPCSTR sect_name_parent, LPCSTR sect_name_prefix, TAppendObject on_append);


	xr_string		LevelPartPath(LPCSTR map_name);
	xr_string		LevelPartName(LPCSTR map_name, EXRayObjectClassID cls);

	BOOL			LoadLevelPart(FXRaySceneToolBase* M, LPCSTR map_name);
	BOOL			LoadLevelPartStream(FXRaySceneToolBase* M, LPCSTR map_name);
	BOOL			LoadLevelPartLTX(FXRaySceneToolBase* M, LPCSTR map_name);

	BOOL			LoadLevelPart(LPCSTR map_name, EXRayObjectClassID cls);
	BOOL		 	UnloadLevelPart(FXRaySceneToolBase* M);
	BOOL			UnloadLevelPart(LPCSTR map_name, EXRayObjectClassID cls);

	FXRayCustomObject*  FindObjectByName(LPCSTR name, EXRayObjectClassID classfilter);
	FXRayCustomObject*	FindObjectByName(LPCSTR name, FXRayCustomObject* pass_object);
	void			GenObjectName(EXRayObjectClassID cls_id, char* buffer, const char* pref);
	void 			FindObjectByNameCB(LPCSTR new_name, bool& res) { res = !!FindObjectByName(new_name, (FXRayCustomObject*)0); }
public:
	bool			ExportGame(SExportStreams* F);

	bool 			Load(LPCSTR map_name, bool bUndo);
	bool 			LoadLTX(LPCSTR map_name, bool bUndo);


	bool 			LoadSelection(LPCSTR fname);

	bool			LoadToolLTX(EXRayObjectClassID clsid, LPCSTR fn);

	
	void 			Clear();
	void			Reset();

	IC ObjectList&  ListObj(EXRayObjectClassID cat) { VERIFY(GetOTool(cat)); return GetOTool(cat)->GetObjects(); }
	IC ObjectIt 	FirstObj(EXRayObjectClassID cat) { VERIFY(GetOTool(cat)); return ListObj(cat).begin(); }
	IC ObjectIt 	LastObj(EXRayObjectClassID cat) { VERIFY(GetOTool(cat)); return ListObj(cat).end(); }
	IC int 			ObjCount(EXRayObjectClassID cat) { return ListObj(cat).size(); }
	int 			ObjCount();

	IC FXRaySceneToolBase*	  GetTool(EXRayObjectClassID cat) { return SceneTools[cat].Get(); }
	IC int32				  ToolCount() { return SceneTools.Num(); }
	IC FXRaySceneCustomOTool* GetOTool(EXRayObjectClassID cat) { return GetTool(cat)->CastToSceneCustomOTool(); }


	virtual void 	AppendObject(FXRayCustomObject* object, bool bExecUnd = true);
	virtual bool 	RemoveObject(FXRayCustomObject* object, bool bExecUndo, bool bDeleting);


	xr_string		LevelPath();
	shared_str 		LevelPrefix() { return m_LevelOp.m_LevelPrefix; }

	bool            GetSubstObjectName(const xr_string& from, xr_string& to) const;
	
	void			GetObjects(EXRayObjectClassID InType,ObjectList&OutObjects);

};

extern EScene* Scene;
