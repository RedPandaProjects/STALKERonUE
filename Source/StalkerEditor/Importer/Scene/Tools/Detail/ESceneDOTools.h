#pragma once
#include "DetailManager.h"
#include "DetailModel.h"
#include "EDetailModel.h"

class CFrustum;
class CEditableObject;
class Shader;

struct SIndexDist{
    u32 	index;
    float 	dist;
    float	dens[4];
    float	cnt[4];
	SIndexDist(){ZeroMemory(this,sizeof(SIndexDist));}
};
DEFINE_SVECTOR		(SIndexDist,4,SIndexDistVec,SIndexDistIt);

DEFINE_MAP			(u32,DOVec,ColorIndexMap,ColorIndexPairIt);

#define DETAIL_SLOT_SIZE_2 	DETAIL_SLOT_SIZE*0.5f
#define DETAIL_SLOT_RADIUS	DETAIL_SLOT_SIZE*0.7071f

class EDetailManager:
	public CDetailManager,
    public FXRaySceneToolBase
//	,public pureDeviceCreate,
//	public pureDeviceDestroy
{
	friend class TfrmDOShuffle;
	typedef FXRaySceneToolBase inherited;

	enum{
    	flBaseTextureDraw		= (1<<31),
        flBaseTextureBlended	= (1<<30),
        flSlotBoxesDraw			= (1<<29),
        flObjectsDraw			= (1<<28),
    };
    Flags32				m_Flags;

    enum{
    	flRTGenerateBaseMesh	= (1<<0)
    };
    Flags32				m_RTFlags;

    ObjectList			m_SnapObjects;

    Fbox				m_BBox;

	IC u32				toSlotX			(float x)	{return (x/DETAIL_SLOT_SIZE+0.5f)+dtH.offs_x;}
	IC u32				toSlotZ			(float z)	{return (z/DETAIL_SLOT_SIZE+0.5f)+dtH.offs_z;}
	IC float			fromSlotX		(int x)		{return (x-dtH.offs_x)*DETAIL_SLOT_SIZE+DETAIL_SLOT_SIZE_2;}
	IC float			fromSlotZ		(int z)		{return (z-dtH.offs_z)*DETAIL_SLOT_SIZE+DETAIL_SLOT_SIZE_2;}

    void				UpdateSlotBBox	(int x, int z, DetailSlot& slot);

    void				GetSlotRect		(Frect& rect, int sx, int sz);
    void				GetSlotTCRect	(Irect& rect, int sx, int sz);
    u8					GetRandomObject	(u32 color_index);
    u8					GetObject		(ColorIndexPairIt& CI, u8 id);

	void 				CalcClosestCount(int part, const Fcolor& C, SIndexDistVec& best);
	void 				FindClosestIndex(const Fcolor& C, SIndexDistVec& best);

    DetailSlot&			GetSlot			(u32 sx, u32 sz);

    void  		OnDensityChange		(PropValue* prop);
    void  		OnBaseTextureChange	(PropValue* prop);
protected:
public:
// render part -----------------------------------------------------------------
    void 				InitRender				();
    void				RenderTexture			(float alpha);
    void				InvalidateCache			();
// render part -----------------------------------------------------------------
public:
    ColorIndexMap		m_ColorIndices;

    void				SaveColorIndices		(IWriter&);
    bool				LoadColorIndices		(IReader&);
public:
						EDetailManager			();
    virtual 			~EDetailManager			() override;

    virtual void		Clear					() override;

	// definition
    IC LPCSTR			ClassName				() override {return "detail_object";}



    // IO
    virtual bool		LoadStream         		(IReader&) override;
    virtual bool		LoadLTX            		(CInifile&) override;
    virtual void		SaveLTX            		(CInifile&, int id);
    virtual bool		CanUseInifile			()				{return false;}


     bool		LoadSelection      		(IReader&) override;

    
    // other
    void				InvalidateSlots			();

    int					RemoveDOs				();
    EDetail*			FindDOByName			(LPCSTR name);

    void				ExportColorIndices		(LPCSTR fname);
    bool				ImportColorIndices		(LPCSTR fname);

    void				ClearColorIndices		();
    void				ClearSlots				();
    void				ClearBase				();

};
