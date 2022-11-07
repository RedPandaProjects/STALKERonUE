#pragma once
#include "ESceneCustomMTools.h"
struct SRayPickInfo;
struct mesh_build_data;
class ESceneCustomOTool: public ESceneToolBase
{
	typedef ESceneToolBase inherited;
protected:
	ObjectList			m_Objects;
    
	bool 				OnLoadSelectionAppendObject(CCustomObject* obj);
	bool 				OnLoadAppendObject		(CCustomObject* obj);
public:
						ESceneCustomOTool		(ObjClassID cls);
	virtual				~ESceneCustomOTool		();

    virtual bool   		LoadStream            		(IReader&);
    virtual bool   		LoadLTX            		(CInifile&);
	ObjectList&			GetObjects() { return m_Objects; }
	IC int				ObjCount() { return m_Objects.size(); }
    virtual bool		LoadSelection      		(IReader&);

    virtual CCustomObject* CreateObject			(LPVOID data, LPCSTR name)=0;

	virtual BOOL _AppendObject(CCustomObject* object);
	virtual BOOL _RemoveObject(CCustomObject* object);

	virtual CCustomObject* FindObjectByName(LPCSTR name, CCustomObject* pass = 0);

	class ESceneCustomOTool* CastToESceneCustomOTool() override { return this; }



};