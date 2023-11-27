#pragma once
#include "ESceneCustomMTools.h"
struct SRayPickInfo;
struct mesh_build_data;
class FXRaySceneCustomOTool: public FXRaySceneToolBase
{
	typedef FXRaySceneToolBase inherited;
protected:
	ObjectList			m_Objects;
    
	bool 				OnLoadSelectionAppendObject(FXRayCustomObject* obj);
	bool 				OnLoadAppendObject		(FXRayCustomObject* obj);
public:
						FXRaySceneCustomOTool		(EXRayObjectClassID cls);
	virtual				~FXRaySceneCustomOTool		();

    virtual bool   		LoadStream            		(IReader&);
    virtual bool   		LoadLTX            		(CInifile&);
	ObjectList&			GetObjects() { return m_Objects; }
	IC int				ObjCount() { return m_Objects.size(); }
    virtual bool		LoadSelection      		(IReader&);

    virtual FXRayCustomObject* CreateObject			(LPVOID data, LPCSTR name)=0;

	virtual BOOL _AppendObject(FXRayCustomObject* object);
	virtual BOOL _RemoveObject(FXRayCustomObject* object);

	virtual FXRayCustomObject* FindObjectByName(LPCSTR name, FXRayCustomObject* pass = 0);

	class FXRaySceneCustomOTool* CastToSceneCustomOTool() override { return this; }



};