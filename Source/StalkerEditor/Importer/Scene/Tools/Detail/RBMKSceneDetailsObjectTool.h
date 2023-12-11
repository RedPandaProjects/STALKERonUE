#pragma once
#include "RBMKDetailFormat.h"
#include "RBMKDetailModel.h"

class FRBMKSceneDetailsObjectTool: public FRBMKSceneToolBase
{
using ColorIndexMap = TMap<uint32,TArray<FRBMKDetail*>>;
public:
									        FRBMKSceneDetailsObjectTool	();
			 						        ~FRBMKSceneDetailsObjectTool	() override;

    const TCHAR* 					        ClassName				    () override {return TEXT("detail_object");}
									
    bool							        LoadStream         		    (IReader&) override;
	bool							        LoadSelection      		    (IReader&) override;
    bool							        LoadLTX            		    (CInifile&) override;

    bool							        CanUseInifile			    () override {return false;}

    void						            ExportToWorld               (UWorld*World,EObjectFlags InFlags,const URBMKLevelImportOptions&LevelImportOptions) override;

    void 							        InitRender				    ();

    bool							        LoadColorIndices		    (IReader&);

    int								        RemoveDOs				    ();
    FRBMKDetail*					        FindDOByName			    (const TCHAR* Name);

    void							        InvalidateSlots			    ();
    void							        ClearColorIndices		    ();
    void							        ClearSlots				    ();
    float							        DetailDensity               = 0.4f;
private:

	FRBMKDetailSlot&				        QueryDB			            (int32 X,int32 Z);
	void							        RenderSlot                  (int32 X,int32 Z,TArray<TArray<FTransform>>&OutInstances);

	static constexpr int32			        DetailSize				    = 24;	
	static constexpr int32			        DetailObjectsInSlot		    = 4;
	static constexpr float			        DetailSlotSize		        = 2;

	int32							        Dither[16][16];
	FRBMKDetailHeader				        DetailHeader;
	TArray<FRBMKDetailSlot>			        DetailSlots;		// note: pointer into VFS
	FRBMKDetailSlot					        DetailSlotEmpty;

	TArray<TSharedPtr<FRBMKDetail>>	        Objects;
    ColorIndexMap					        ColorIndices;
    TArray<TSharedPtr<FRBMKSceneObjectBase>>SnapObjects;
};
