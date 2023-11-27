#include "ESceneDOTools.h"
#include "DetailFormat.h"

static const u32 DETMGR_VERSION = 0x0003ul;
enum{
    DETMGR_CHUNK_VERSION		= 0x1000ul,
    DETMGR_CHUNK_HEADER 		= 0x0000ul,
    DETMGR_CHUNK_OBJECTS 		= 0x0001ul,
    DETMGR_CHUNK_SLOTS			= 0x0002ul,
    DETMGR_CHUNK_BBOX			= 0x1001ul,
    DETMGR_CHUNK_BASE_TEXTURE	= 0x1002ul,
    DETMGR_CHUNK_COLOR_INDEX 	= 0x1003ul,
    DETMGR_CHUNK_SNAP_OBJECTS 	= 0x1004ul,
    DETMGR_CHUNK_DENSITY	 	= 0x1005ul,
    DETMGR_CHUNK_FLAGS			= 0x1006ul,
};


EDetailManager::EDetailManager():FXRaySceneToolBase(OBJCLASS_DO)
{
	dtSlots				= 0;
    ZeroMemory			(&dtH,sizeof(dtH));
    InitRender			();
    m_Flags.assign		(flObjectsDraw);
}

EDetailManager::~EDetailManager()
{
	Clear	();
}

void EDetailManager::ClearColorIndices()
{
	inherited::Clear	();
    m_ColorIndices.clear();
}
void EDetailManager::ClearSlots()
{
    ZeroMemory			(&dtH,sizeof(DetailHeader));
    xr_free				(dtSlots);
    InvalidateCache		();
}
void EDetailManager::ClearBase()
{
    m_SnapObjects.clear	();
}
void EDetailManager::Clear()
{
	ClearBase			();
	ClearColorIndices	();
    ClearSlots			();
    m_Flags.zero		();
    m_RTFlags.zero		();
}

void EDetailManager::InvalidateCache()
{
	// resize visible
	m_visibles[0].resize	(objects.size());	// dump(visible[0]);
	m_visibles[1].resize	(objects.size());	// dump(visible[1]);
	m_visibles[2].resize	(objects.size());	// dump(visible[2]);
	// Initialize 'vis' and 'cache'
	cache_Initialize	();
}

extern void bwdithermap	(int levels, int magic[16][16] );
void EDetailManager::InitRender()
{
	// inavlidate cache
	InvalidateCache		();
	// Make dither matrix
	bwdithermap		(2,dither);

//	soft_Load	();
}

void EDetailManager::ExportColorIndices(LPCSTR fname)
{
	IWriter* F 	= FS.w_open(fname);
    if (F){
	    SaveColorIndices(*F);
    	FS.w_close	(F);
    }
}

bool EDetailManager::ImportColorIndices(LPCSTR fname)
{
	IReader* F=FS.r_open(fname);
    if (F){
        ClearColorIndices	();
        LoadColorIndices	(*F);
        FS.r_close			(F);
        return true;
    }else{
    	ELog.DlgMsg			(mtError,"Can't open file '%s'.",fname);
        return false;
    }
}

void EDetailManager::SaveColorIndices(IWriter& F)
{
	// objects
	F.open_chunk		(DETMGR_CHUNK_OBJECTS);
    for (DetailIt it=objects.begin(); it!=objects.end(); it++){
		F.open_chunk	(it-objects.begin());
        ((EDetail*)(*it))->Save		(F);
	    F.close_chunk	();
    }
    F.close_chunk		();
    // color index map
	F.open_chunk		(DETMGR_CHUNK_COLOR_INDEX);
    F.w_u8				((u8)m_ColorIndices.size());
    ColorIndexPairIt S 	= m_ColorIndices.begin();
    ColorIndexPairIt E 	= m_ColorIndices.end();
    ColorIndexPairIt i_it= S;
	for(; i_it!=E; i_it++){
		F.w_u32		(i_it->first);
        F.w_u8			((u8)i_it->second.size());
	    for (DOIt d_it=i_it->second.begin(); d_it!=i_it->second.end(); d_it++)
        	F.w_stringZ	((*d_it)->GetName());
    }
    F.close_chunk		();
}

bool EDetailManager::LoadColorIndices(IReader& F)
{
	VERIFY				(objects.empty());
    VERIFY  			(m_ColorIndices.empty());

    bool bRes			= true;
    // objects
    IReader* OBJ 		= F.open_chunk(DETMGR_CHUNK_OBJECTS);
    if (OBJ){
        IReader* O   	= OBJ->open_chunk(0);
        for (int count=1; O; count++) {
            EDetail* DO	= xr_new<EDetail>();
            if (DO->Load(*O)) 	objects.push_back(DO);
            else				bRes = false;
            O->close();
            O = OBJ->open_chunk(count);
        }
        OBJ->close();
    }
    // color index map
    R_ASSERT			(F.find_chunk(DETMGR_CHUNK_COLOR_INDEX));
    int cnt				= F.r_u8();
    string256			buf;
    u32 index;
    int ref_cnt;
    for (int k=0; k<cnt; k++){
		index			= F.r_u32();
        ref_cnt			= F.r_u8();
		for (int j=0; j<ref_cnt; j++){
        	F.r_stringZ	(buf,sizeof(buf));
            EDetail* DO	= FindDOByName(buf);
            if (DO) 	m_ColorIndices[index].push_back(DO);    
            else		bRes=false;
        }
    }
	InvalidateCache		();

    return bRes;
}
bool EDetailManager::LoadLTX(CInifile& ini)
{
	R_ASSERT2			(0, "not_implemented");
    return true;
}

void EDetailManager::SaveLTX(CInifile& ini, int id)
{
	R_ASSERT2			(0, "not_implemented");
/*
	inherited::SaveLTX	(ini);

    ini.w_u32			("main", "version", DETMGR_VERSION);

    ini.w_u32			("main", "flags", m_Flags.get());

	// header

    ini.w_u32			("detail_header", "version", dtH.version);
    ini.w_u32			("detail_header", "object_count", dtH.object_count);
    ini.w_ivector2		("detail_header", "offset", Ivector2().set(dtH.offs_x, dtH.offs_z) );
    ini.w_ivector2		("detail_header", "size", Ivector2().set(dtH.size_x, dtH.size_z) );

    // objects
    SaveColorIndicesLTX	(F);

    // slots
	F.open_chunk		(DETMGR_CHUNK_SLOTS);
    F.w_u32				(dtH.size_x*dtH.size_z);
	F.w					(dtSlots,dtH.size_x*dtH.size_z*sizeof(DetailSlot));
    F.close_chunk		();

    // internal
    // bbox
    ini.w_fvector3		("main", "bbox_min", m_BBox.min);
    ini.w_fvector3		("main", "bbox_max", m_BBox.max);

	// base texture
    if (m_Base.Valid())
    {
    	ini.w_string	("main", "base_texture", m_Base.GetName());
    }
    ini.w_float			("main", "detail_density", ps_r__Detail_density);

	// snap objects
    for (ObjectIt o_it=m_SnapObjects.begin(); o_it!=m_SnapObjects.end(); ++o_it)
    	ini.w_string	("snap_objects", (*o_it)->Name, NULL);
*/        
}

bool EDetailManager::LoadStream(IReader& F)
{
	inherited::LoadStream	(F);

    string256 buf;
    R_ASSERT			(F.find_chunk(DETMGR_CHUNK_VERSION));
	u32 version			= F.r_u32();

    if (version!=DETMGR_VERSION){
    	ELog.Msg(mtError,"EDetailManager: unsupported version.");
        return false;
    }

    if (F.find_chunk(DETMGR_CHUNK_FLAGS)) m_Flags.assign(F.r_u32());
    
	// header
    R_ASSERT			(F.r_chunk(DETMGR_CHUNK_HEADER,&dtH));

    // slots
    R_ASSERT			(F.find_chunk(DETMGR_CHUNK_SLOTS));
    int slot_cnt		= F.r_u32();
	if (slot_cnt) dtSlots = xr_alloc<DetailSlot>(slot_cnt);
	F.r					(dtSlots,slot_cnt*sizeof(DetailSlot));

    // objects
    if (!LoadColorIndices(F)){
        ELog.DlgMsg		(mtError,"EDetailManager: Some objects removed. Reinitialize objects.",buf);
        InvalidateSlots	();
    }

    // internal
    // bbox
    R_ASSERT			(F.r_chunk(DETMGR_CHUNK_BBOX,&m_BBox));

    InvalidateCache		();

    return true;
}

bool EDetailManager::LoadSelection(IReader& F)
{
	Clear();
	return LoadStream			(F);
}

void EDetailManager::OnDensityChange(PropValue* prop)
{
	InvalidateCache		();
}	



void EDetailManager::InvalidateSlots()
{
	int slot_cnt = dtH.size_x*dtH.size_z;
	for (int k=0; k<slot_cnt; k++){
    	DetailSlot* it = &dtSlots[k];
    	it->w_id(0,DetailSlot::ID_Empty);
    	it->w_id(1,DetailSlot::ID_Empty);
    	it->w_id(2,DetailSlot::ID_Empty);
    	it->w_id(3,DetailSlot::ID_Empty);
    }
    InvalidateCache();
}

int EDetailManager::RemoveDOs()
{
    for(u32 i=0;i<objects.size();i++)
    {
	    EDetail* Detail = static_cast<EDetail*>(objects[i]);
        delete Detail;
    }
	u32 cnt = objects.size();
    objects.clear();
    return cnt;
}

EDetail* EDetailManager::FindDOByName(LPCSTR name)
{
    for(u32 i=0;i<objects.size();i++)
    {
	    EDetail* Detail = static_cast<EDetail*>(objects[i]);
	    if (FCStringAnsi::Stricmp(Detail->GetName(), name) == 0)
	    {
		    return Detail;
	    }
    }
    return nullptr;
}