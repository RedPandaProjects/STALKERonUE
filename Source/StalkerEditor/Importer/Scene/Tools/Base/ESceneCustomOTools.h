#pragma once
#include "RBMKSceneToolBase.h"
struct SRayPickInfo;
struct mesh_build_data;
class FXRaySceneCustomOTool: public FRBMKSceneToolBase
{
	typedef FRBMKSceneToolBase inherited;
protected:
	ObjectList			m_Objects;
public:
						FXRaySceneCustomOTool		(ERBMKSceneObjectType cls);
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