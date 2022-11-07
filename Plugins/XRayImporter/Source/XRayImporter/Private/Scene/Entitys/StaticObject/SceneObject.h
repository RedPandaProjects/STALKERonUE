#pragma once
class CSceneObject : public CCustomObject
{
	shared_str		m_ReferenceName;
	CEditableObject*m_pReference;
public:

	SurfaceVec m_Surfaces;
	enum {
		//    	flDynamic	= (1<<0),
		flUseSurface= (1 << 0),
		flFORCE32	= u32(-1)
    };
private:
	Fbox			m_TBBox;
	// options
    Flags32			m_Flags;
public:
    virtual void 	SetScale				(const Fvector& scale)
    {
    	if (m_pReference && m_pReference->IsDynamic()){
        	ELog.Msg(mtError,"Dynamic object %s - can't scale.", GetName());
        }else{
			FScale.set(scale);
            UpdateTransform();
        }
    }
protected:
	typedef CCustomObject inherited;
    int				m_iBlinkTime;
    CSurface*		m_BlinkSurf;
	void 			RenderBlink				();
public:
    // constructor/destructor methods
					CSceneObject			(LPVOID data, LPCSTR name);
	virtual 		~CSceneObject			();
	
	void 			Construct				(LPVOID data);

    // get object properties methods
	IC bool 		RefCompare				(CEditableObject *to){return m_pReference != nullptr ?!!(m_pReference==to):false; }
	IC bool 		RefCompare				(LPCSTR ref){return ref&&m_pReference != nullptr ?(FCStringAnsi::Strcmp(ref,m_pReference->GetName())==0):false; }
	IC CEditableObject*	GetReference		()	{return m_pReference; }
	CEditableObject*SetReference			(LPCSTR ref_name);
	CEditableObject*UpdateReference			();
	IC EditMeshVec* Meshes					() {return m_pReference != nullptr ?&m_pReference->Meshes():0;}
    virtual LPCSTR	RefName					() {return m_pReference != nullptr ?m_pReference->GetName():0;}
    virtual bool	CanAttach				() {return true;}

    // statistics methods
	IC bool 		IsDynamic     			()	{return (m_pReference != nullptr ? m_pReference->IsDynamic()>0:false); }
	IC bool 		IsStatic     			()	{return (m_pReference != nullptr ?m_pReference->IsStatic() > 0:false); }
	IC bool 		IsMUStatic     			()	{return (m_pReference != nullptr ?m_pReference->IsMUStatic() > 0:false); }
    int 			GetFaceCount			();
	int 			GetVertexCount			();
    int 			GetSurfFaceCount		(const char* surf_name);

    // render methods
    virtual void	OnUpdateTransform		();

    // misc
	void		    EvictObject				();

    // pick methods
  

    // get orintation/bounding volume methods
	virtual bool 	GetBox					(Fbox& box) ;
	virtual bool 	GetUTBox				(Fbox& box);
    void 			GetFullTransformToWorld	(Fmatrix& m);
    void 			GetFullTransformToLocal	(Fmatrix& m);


    // load/save methods
	virtual bool 	LoadStream				(IReader&);
	virtual bool 	LoadLTX					(CInifile& ini, LPCSTR sect_name);


    void			Blink					(CSurface* surf=0);


	void ClearSurface();

	void* QueryInterface(ObjClassID InClassID) override;
};

