#include "SceneObject.h"

#define BLINK_TIME 300.f


CSceneObject::CSceneObject(LPVOID data, LPCSTR name):CCustomObject(data,name)
{
	Construct	(data);
}

void CSceneObject::Construct(LPVOID data)
{
	FClassID		= OBJCLASS_SCENEOBJECT;

    m_ReferenceName = "";
	m_pReference 	= 0;

    m_TBBox.invalidate();
    m_iBlinkTime	= 0;
    m_BlinkSurf		= 0;

    m_Flags.zero	();
}

CSceneObject::~CSceneObject()
{
    for (CSurface* i : m_Surfaces) { i->OnDeviceDestroy(); delete i; }
	GRayObjectLibrary->RemoveEditObject(m_pReference);
}


int CSceneObject::GetFaceCount()
{
	return m_pReference?m_pReference->GetFaceCount():0;
}

int CSceneObject::GetSurfFaceCount(const char* surf_name)
{
	return m_pReference?m_pReference->GetSurfFaceCount(surf_name):0;
}

int CSceneObject::GetVertexCount()
{
	return m_pReference?m_pReference->GetVertexCount():0;
}

void CSceneObject::OnUpdateTransform()
{
	inherited::OnUpdateTransform();
    // update bounding volume
    if (m_pReference){
    	m_TBBox.set		(m_pReference->GetBox());
    	m_TBBox.xform	(_Transform());
    }
}

bool CSceneObject::GetBox( Fbox& box ) 
{
	if (!m_pReference) return false;
	box.set(m_TBBox);
	return true;
}

bool CSceneObject::GetUTBox( Fbox& box )
{
	if (!m_pReference) return false;
    box.set(m_pReference->GetBox());
	return true;
}

void CSceneObject::GetFullTransformToWorld( Fmatrix& m )
{
    m.set(_Transform());
}

void CSceneObject::GetFullTransformToLocal( Fmatrix& m )
{
    m.set(_ITransform());
}

void* CSceneObject::QueryInterface(ObjClassID InClassID)
{
    if (InClassID == OBJCLASS_SCENEOBJECT)
        return this;
    return inherited::QueryInterface(InClassID);
}

CEditableObject* CSceneObject::UpdateReference()
{
    for (CSurface* i : m_Surfaces) { i->OnDeviceDestroy(); delete i; }
    m_Surfaces.clear();
    GRayObjectLibrary->RemoveEditObject(m_pReference);
	m_pReference		= (m_ReferenceName.size())? GRayObjectLibrary->CreateEditObject(*m_ReferenceName):0;
    UpdateTransform		();
    if (m_pReference)
    {
        for (size_t i = 0; i < m_pReference->SurfaceCount(); i++)
        {
            CSurface* surf = xr_new< CSurface>();
            surf->CopyFrom(m_pReference->Surfaces()[i]);
            m_Surfaces.push_back(surf);
        }
    } 
    return m_pReference;
}

CEditableObject* CSceneObject::SetReference(LPCSTR ref_name)
{
	m_ReferenceName	= ref_name;
    return UpdateReference();
}
