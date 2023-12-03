#pragma once


class CGroupObject: public FXRayCustomObject
{
    typedef FXRayCustomObject inherited;

public:
					        CGroupObject	    (LPVOID data, LPCSTR name);
	virtual 		        ~CGroupObject	    ();
	void 			        Construct		    (LPVOID data);
	virtual LPCSTR          RefName             () { return m_GroupReferenceName.size() ? m_GroupReferenceName.c_str() : "group"; }
	void 			        SetRefName		    (LPCSTR nm);
    virtual bool	        CanAttach		    () {return false;}
    virtual void 	        OnUpdateTransform   ();
  	virtual bool 	        LoadStream			(IReader&);
  	virtual bool 	        LoadLTX				(CInifile& ini, LPCSTR sect_name);
	void*                   QueryInterface      (ERBMKSceneObjectType InClassID) override;
private:

    void			        ClearInternal		(xr_list<FXRayCustomObject*>& v);
    bool			        AppendObjectLoadCB	(FXRayCustomObject* object);
    TArray<TSharedPtr<FXRayCustomObject>> m_ObjectsInGroup;
    shared_str		        m_GroupReferenceName;
};