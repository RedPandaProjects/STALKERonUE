#pragma once

class ESceneWallmarkTool : public FRBMKSceneToolBase
{
	typedef FRBMKSceneToolBase inherited;
public:
	ESceneWallmarkTool();
	~ESceneWallmarkTool() override;
	// definition
	LPCSTR				ClassName() override { return "wallmark"; }
	// IO
	bool 				CanUseInifile() override { return false; }

	bool   				LoadStream(IReader&) override;
	bool				LoadLTX(CInifile&) override;
	bool				LoadSelection(IReader&) override;
	void				Clear()				override;
    struct wm_slot;
#pragma pack(push,4)
	struct WMVertex 
    {
		Fvector		p;
		u32			color;
		Fvector2	t;
	};
#pragma pack(pop)
	struct wallmark 
	{
    	enum{
        	flSelected	= (1<<0)
        };
        wm_slot*				parent;
        float					w,h,r;
        Flags8					flags;
        Fbox					bbox;
		Fsphere					bounds;
		xr_vector<WMVertex>		verts;
        wallmark				(){flags.zero();parent=0;w=0;h=0;r=0;}
	};
	DEFINE_VECTOR		(wallmark*,WMVec,WMVecIt);
	struct wm_slot
	{
		shared_str		sh_name;
		shared_str		tx_name;
		WMVec			items;
						wm_slot	(shared_str sh, shared_str tx):sh_name(sh),tx_name(tx)		{;items.reserve(256);}
	};
	DEFINE_VECTOR		(wm_slot*,WMSVec,WMSVecIt);
	WMSVec				marks;
private:
	wm_slot*			AppendSlot				(shared_str sh_name, shared_str tx_name);
	wallmark*			WMAllocate				();

};