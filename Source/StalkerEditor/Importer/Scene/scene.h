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
	SceneToolsMap   m_SceneTools;
protected:
	bool 			OnLoadAppendObject(CCustomObject* O);
	bool 			OnLoadSelectionAppendObject(CCustomObject* O);
protected:
	void			RegisterSceneTools(ESceneToolBase* mt);
	void			CreateSceneTools();
	void			DestroySceneTools();

public:
	typedef  fastdelegate::FastDelegate1<CCustomObject*, bool> TAppendObject;

	bool 			ReadObjectStream(IReader& F, CCustomObject*& O);
	bool 			ReadObjectLTX(CInifile& ini, LPCSTR sect_name, CCustomObject*& O);
	bool 			ReadObjectsStream(IReader& F, u32 chunk_id, TAppendObject on_append);
	bool 			ReadObjectsLTX(CInifile& ini, LPCSTR sect_name_parent, LPCSTR sect_name_prefix, TAppendObject on_append);


	xr_string		LevelPartPath(LPCSTR map_name);
	xr_string		LevelPartName(LPCSTR map_name, ObjClassID cls);

	BOOL			LoadLevelPart(ESceneToolBase* M, LPCSTR map_name);
	BOOL			LoadLevelPartStream(ESceneToolBase* M, LPCSTR map_name);
	BOOL			LoadLevelPartLTX(ESceneToolBase* M, LPCSTR map_name);

	BOOL			LoadLevelPart(LPCSTR map_name, ObjClassID cls);
	BOOL		 	UnloadLevelPart(ESceneToolBase* M);
	BOOL			UnloadLevelPart(LPCSTR map_name, ObjClassID cls);

	CCustomObject*  FindObjectByName(LPCSTR name, ObjClassID classfilter);
	CCustomObject*	FindObjectByName(LPCSTR name, CCustomObject* pass_object);
	void			GenObjectName(ObjClassID cls_id, char* buffer, const char* pref);
	void 			FindObjectByNameCB(LPCSTR new_name, bool& res) { res = !!FindObjectByName(new_name, (CCustomObject*)0); }
public:
	bool			ExportGame(SExportStreams* F);

	bool 			Load(LPCSTR map_name, bool bUndo);
	bool 			LoadLTX(LPCSTR map_name, bool bUndo);


	bool 			LoadSelection(LPCSTR fname);

	bool			LoadToolLTX(ObjClassID clsid, LPCSTR fn);

	
	void 			Clear();
	void			Reset();

	IC ObjectList&  ListObj(ObjClassID cat) { VERIFY(GetOTool(cat)); return GetOTool(cat)->GetObjects(); }
	IC ObjectIt 	FirstObj(ObjClassID cat) { VERIFY(GetOTool(cat)); return ListObj(cat).begin(); }
	IC ObjectIt 	LastObj(ObjClassID cat) { VERIFY(GetOTool(cat)); return ListObj(cat).end(); }
	IC int 			ObjCount(ObjClassID cat) { return ListObj(cat).size(); }
	int 			ObjCount();


	IC ESceneToolBase*	  GetTool(ObjClassID cat) { return m_SceneTools[cat]; }
	IC u32				  ToolCount() { return m_SceneTools.size(); }
	IC ESceneCustomOTool* GetOTool(ObjClassID cat) { return GetTool(cat)->CastToESceneCustomOTool(); }


	virtual void 	AppendObject(CCustomObject* object, bool bExecUnd = true);
	virtual bool 	RemoveObject(CCustomObject* object, bool bExecUndo, bool bDeleting);


	xr_string		LevelPath();
	shared_str 		LevelPrefix() { return m_LevelOp.m_LevelPrefix; }

	bool            GetSubstObjectName(const xr_string& from, xr_string& to) const;
};

extern EScene* Scene;
