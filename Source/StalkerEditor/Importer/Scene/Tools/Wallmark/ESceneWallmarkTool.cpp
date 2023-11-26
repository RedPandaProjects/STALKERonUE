#include "ESceneWallmarkTool.h"
#define WM_VERSION  				0x0004

#define WM_CHUNK_VERSION			0x0001       
#define WM_CHUNK_FLAGS				0x0002
#define WM_CHUNK_PARAMS				0x0003
#define WM_CHUNK_ITEMS				0x0004
#define WM_CHUNK_ITEMS2				0x0005

ESceneWallmarkTool::ESceneWallmarkTool() :ESceneToolBase(OBJCLASS_WM)
{

}

ESceneWallmarkTool::~ESceneWallmarkTool()
{
	{
		for (WMSVecIt p_it=marks.begin(); p_it!=marks.end(); p_it++){
			for (WMVecIt m_it=(*p_it)->items.begin(); m_it!=(*p_it)->items.end(); m_it++)
				delete	(*m_it);
			delete(*p_it);
		}
		marks.clear	();
	}
}


bool ESceneWallmarkTool::LoadStream(IReader&F)
{
    inherited::LoadStream	(F);

	u16 version = 0;

    R_ASSERT(F.r_chunk(WM_CHUNK_VERSION,&version));


    if(version!=0x0003 && version!=WM_VERSION)
    {
        ELog.Msg( mtError, "Static Wallmark: Unsupported version.");
        return false;
    }


    if ( IReader* OBJ 	= F.open_chunk(WM_CHUNK_ITEMS2))
    {
        IReader* O  = OBJ->open_chunk(0);
        for (int count=1; O; count++)
        {
            u32 item_count	= O->r_u32();	
            if (item_count)
            {
                shared_str		tex_name,sh_name;
                O->r_stringZ	(sh_name);
                O->r_stringZ	(tex_name);
                wm_slot* slot	= AppendSlot(sh_name,tex_name);
                if (slot)
                {
                    slot->items.resize(item_count);
                    for (WMVecIt w_it=slot->items.begin(); w_it!=slot->items.end(); w_it++){
                        *w_it	= WMAllocate();
                        wallmark* W	= *w_it;
                        O->r	    (&W->flags,sizeof(W->flags));
                        O->r	    (&W->bbox,sizeof(W->bbox));
                        O->r	    (&W->bounds,sizeof(W->bounds));
	                    W->parent	= slot;
                        W->w 	    = O->r_float();
                        W->h 	    = O->r_float();
                        W->r 	    = O->r_float();
                        W->verts.resize(O->r_u32());
                        O->r	(&*W->verts.begin(),sizeof(WMVertex)*W->verts.size());
                    }
                }
            }
            O->close();
            O = OBJ->open_chunk(count);
        }
        OBJ->close();
    }
    return true;
}

bool ESceneWallmarkTool::LoadLTX(CInifile&)
{
	return false;
}

bool ESceneWallmarkTool::LoadSelection(IReader&F)
{
	Clear();
	return LoadStream(F);
}

void ESceneWallmarkTool::Clear()
{
	{
		for (WMSVecIt p_it=marks.begin(); p_it!=marks.end(); p_it++){
			for (WMVecIt m_it=(*p_it)->items.begin(); m_it!=(*p_it)->items.end(); m_it++)
				delete	(*m_it);
			delete(*p_it);
		}
		marks.clear	();
	}
}

ESceneWallmarkTool::wm_slot* ESceneWallmarkTool::AppendSlot(shared_str sh_name, shared_str tx_name)
{
	wm_slot* slot			= xr_new<wm_slot>(sh_name,tx_name);
    marks.push_back	(slot);
    return slot;
}

ESceneWallmarkTool::wallmark* ESceneWallmarkTool::WMAllocate()
{
	return new wallmark;
}
