#pragma once


class CGroupObject: public CCustomObject
{
    typedef CCustomObject inherited;

public:
					        CGroupObject	    (LPVOID data, LPCSTR name);
	virtual 		        ~CGroupObject	    ();
	void 			        Construct		    (LPVOID data);
    u32				        GetObjects		    (ObjectList& lst);
	IC int			        ObjectInGroupCount  () { return m_ObjectsInGroup.size(); }
	virtual LPCSTR          RefName             () { return m_GroupReferenceName.size() ? m_GroupReferenceName.c_str() : "group"; }
	void 			        SetRefName		    (LPCSTR nm);
    virtual bool	        CanAttach		    () {return false;}
    virtual void 	        OnUpdateTransform   ();
  	virtual bool 	        LoadStream			(IReader&);
  	virtual bool 	        LoadLTX				(CInifile& ini, LPCSTR sect_name);
	void*                   QueryInterface      (ObjClassID InClassID) override;
private:

    void			        ClearInternal		(xr_list<CCustomObject*>& v);
    bool			        AppendObjectLoadCB	(CCustomObject* object);
    xr_list<CCustomObject*> m_ObjectsInGroup;
    shared_str		        m_GroupReferenceName;
};