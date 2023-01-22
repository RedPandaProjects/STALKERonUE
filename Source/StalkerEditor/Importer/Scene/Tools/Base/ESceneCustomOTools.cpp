#include "ESceneCustomOTools.h"

ESceneCustomOTool::ESceneCustomOTool(ObjClassID cls):ESceneToolBase(cls)
{
}


ESceneCustomOTool::~ESceneCustomOTool()
{
	for (CCustomObject *Obj : m_Objects)
	{
		delete			Obj;
	}
}

CCustomObject* ESceneCustomOTool::FindObjectByName(LPCSTR name, CCustomObject* pass)
{
	ObjectIt _I = m_Objects.begin();
	ObjectIt _E = m_Objects.end();
	for (; _I != _E; _I++)
	{
		CCustomObject* CO = (*_I);
		LPCSTR _name = CO->GetName();
		R_ASSERT3(_name, "Invalid object name, position:", (std::to_string(CO->GetPosition().x) + ", " + std::to_string(CO->GetPosition().y) + ", " + std::to_string(CO->GetPosition().z)).c_str());
		if ((pass != *_I) && (0 == FCStringAnsi::Strcmp(_name, name)))
			return (*_I);
	}
	return 0;
}


BOOL ESceneCustomOTool::_AppendObject(CCustomObject* object)
{
    m_Objects.push_back(object);
    object->FParentTools = this;
    return TRUE;
}


BOOL ESceneCustomOTool::_RemoveObject(CCustomObject* object)
{
	m_Objects.remove(object);
    return FALSE;
}



