#pragma once

class CEditableObject;

class ESceneSpawnTool: public ESceneCustomOTool
{
	typedef ESceneCustomOTool inherited;
    friend class 		CSpawnPoint;
protected:
	enum{
    	flPickSpawnType	= (1<<30),
    	flShowSpawnType	= (1<<31),
    };
    Flags32				m_Flags;

public:
						ESceneSpawnTool		();
	virtual				~ESceneSpawnTool		();

	// definition
    IC LPCSTR			ClassName				(){return "spawn";}
    IC LPCSTR			ClassDesc				(){return "Spawn";}

    virtual void		Clear					(){inherited::Clear();m_Flags.zero();}
    // IO
    virtual bool   		IsNeedSave				(){return true;}
    virtual bool   		LoadStream            		(IReader&);
    virtual bool   		LoadLTX            		(CInifile&);
    virtual bool		can_use_inifile			()				{return true;}
    virtual bool		LoadSelection      		(IReader&);
/*
    virtual void		GetStaticDesc			(int& v_cnt, int& f_cnt, bool b_selected_only);
    virtual bool		ExportStatic			(SceneBuilder* B, bool b_selected_only);
*/
    virtual CCustomObject* CreateObject			(LPVOID data, LPCSTR name);
};