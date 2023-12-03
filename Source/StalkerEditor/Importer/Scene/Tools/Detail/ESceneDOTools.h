#pragma once
#include "RBMKDetailFormat.h"
#include "RBMKDetailModel.h"

class EDetailManager: public FXRaySceneToolBase
{
using ColorIndexMap = TMap<uint32,TArray<FRBMKDetail*>>;
public:
									EDetailManager			();
			 						~EDetailManager			() override;
    void							Clear					() override;
									
    bool							LoadStream         		(IReader&) override;
	bool							LoadSelection      		(IReader&) override;
    bool							LoadLTX            		(CInifile&) override;
    bool							CanUseInifile			() override {return false;}
    const char* 					ClassName				() override {return "detail_object";}
    void							ExportToCurrentWorld    () override;

    void 							InitRender				();

    bool							LoadColorIndices		(IReader&);

    int								RemoveDOs				();
    FRBMKDetail*					FindDOByName			(const TCHAR* Name);
    
    void							InvalidateSlots			();
    void							ClearColorIndices		();
    void							ClearSlots				();
    float							DetailDensity;
private:
    
	FRBMKDetailSlot&					QueryDB			        (int32 X,int32 Z);
	void							RenderSlot              (int32 X,int32 Z,TArray<TArray<FTransform>>&OutInstances);

	static constexpr int32			DetailSize				= 24;	
	static constexpr int32			DetailObjectsInSlot		= 4;
	static constexpr float			DetailSlotSize		    = 2;

	int32							Dither[16][16];
	FRBMKDetailHeader					DetailHeader;
	TArray<FRBMKDetailSlot>				DetailSlots;		// note: pointer into VFS
	FRBMKDetailSlot						DetailSlotEmpty;

	TArray<TSharedPtr<FRBMKDetail>>	Objects;
    ColorIndexMap					ColorIndices;
    ObjectList						SnapObjects;
};
