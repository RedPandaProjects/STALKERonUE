#pragma once
#include "DetailFormat.h"
#include "EDetailModel.h"

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
    
	FDetailSlot&					QueryDB			        (int32 X,int32 Z);
	void							RenderSlot              (int32 X,int32 Z,TArray<TArray<FTransform>>&OutInstances);

	static constexpr int32			DetailSize				= 24;	
	static constexpr int32			DetailObjectsInSlot		= 4;
	static constexpr float			DetailSlotSize		    = 2;

	int32							Dither[16][16];
	FDetailHeader					DetailHeader;
	TArray<FDetailSlot>				DetailSlots;		// note: pointer into VFS
	FDetailSlot						DetailSlotEmpty;

	TArray<TSharedPtr<FRBMKDetail>>	Objects;
    ColorIndexMap					ColorIndices;
    ObjectList						SnapObjects;
};
