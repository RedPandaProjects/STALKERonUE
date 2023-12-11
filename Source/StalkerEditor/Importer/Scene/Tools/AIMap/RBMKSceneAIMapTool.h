#pragma once

struct FRBMKSceneAIMapParameters
{
	float PatchSize = 0.7f;		
	float TestHeight = 1.0f;
	float CanUP = 1.5f;	
	float CanDOWN = 4.0f;
};

#pragma pack(push,1)
struct FRBMKSceneAINodePosition
{
	int16	X;
	uint16	Y;
	int16	Z;
};

struct FRBMKSceneAINode					// definition of "patch" or "node"
{
				FRBMKSceneAINode	();
	void   		LoadStream			(IReader&, const class FRBMKSceneAIMapTool*);
	union
	{
		struct
		{
			int32 Node1;		// Left
			int32 Node2;		// Forward
			int32 Node3;		// Right
			int32 Node4;		// Backward
		};
		int32 Nodes[4];
	};
	FPlane4f	Plane;					
	FVector3f	Position;				

};
#pragma pack(pop)

class FRBMKSceneAIMapTool : public FRBMKSceneToolBase
{
public:
								FRBMKSceneAIMapTool			();
								~FRBMKSceneAIMapTool		() override;

	const TCHAR*				ClassName					() override { return TEXT("ai_map"); }
	bool 						CanUseInifile				() override { return false; }

	bool   						LoadStream					(IReader&) override;
	bool						LoadLTX						(CInifile&) override;
	bool						LoadSelection				(IReader&) override;

    void						ExportToWorld               (UWorld*World,EObjectFlags InFlags,const URBMKLevelImportOptions&LevelImportOptions) override;
	
	void 						UnpackPosition				(const FRBMKSceneAINodePosition& InNodePosition,FVector3f& Position) const;
	Flags32						Flags;
	Fbox						AIBBox;
	TArray<FRBMKSceneAINode>	Nodes;
	FRBMKSceneAIMapParameters	AIMapParameters;
};