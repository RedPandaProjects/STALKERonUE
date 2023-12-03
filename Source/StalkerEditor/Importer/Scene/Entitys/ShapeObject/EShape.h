#pragma once

enum eShapeUsage{eShapeCommon=0, eShapeLevelBound};

class CEditShape: public FXRayCustomObject,protected CShapeData
{
	typedef FXRayCustomObject inherited;
public:
	u8				m_shape_type;
	u32				m_DrawTranspColor;
	u32				m_DrawEdgeColor;

	void			SetDrawColor	(u32 transp, u32 edge){m_DrawTranspColor=transp;m_DrawEdgeColor=edge;}
	void			ApplyScale		();
	void			add_sphere		(const Fsphere& S);
	void			add_box			(const Fmatrix& B);
    const shape_def&get_shape		(int idx){R_ASSERT(idx<(int)shapes.size());return shapes[idx];}
    
protected:
	virtual void 	SetScale		(const Fvector& val);
    virtual void	OnUpdateTransform();
public:
					CEditShape 		(LPVOID data, LPCSTR name);
	void 			Construct		(LPVOID data);
	virtual 		~CEditShape		();
    virtual bool	CanAttach		() {return true;}
    
    // change position/orientation methods
//	virtual void 	Scale		(Fvector& amount){;}

    // file system function
  	virtual bool 		LoadStream			(IReader&);
  	virtual bool 		LoadLTX				(CInifile& ini, LPCSTR sect_name);
    
    // tools
    //void			Attach		(CEditShape* from);
    //void			Detach		();

    ShapeVec&		GetShapes	(){return shapes;}


    virtual void	OnDetach	();

	void*           QueryInterface(ERBMKSceneObjectType InClassID) override;
};