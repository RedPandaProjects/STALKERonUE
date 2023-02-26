#pragma once
#include "StalkerAIMapNode.h"
#include "StalkerAIMap.generated.h"

UCLASS()
class STALKER_API UStalkerAIMap : public UObject,public ILevelGraph
{
	GENERATED_BODY()
public:
								UStalkerAIMap		();
	void						Serialize			(FArchive& Ar) override;

	const						CHeader& header		() const override;
	const						CVertex* get_nodes	() const override;
	void						RefreshAIMapMetadata();
#if WITH_EDITORONLY_DATA
	void						AutoLink			(FStalkerAIMapNode* Node);
	void						BeginDestroy		() override;
	void						InvalidAIMap		();
	void						ClearAIMap			();
	void						CalculateAABB		(const FBox3f& NewAABB = FBox3f(ForceInit));
	void						HashFill			(const FBox3f& NewAABB = FBox3f(ForceInit));
	void						HashClear			();
	void						RemoveSelect		();
	TArray<FStalkerAIMapNode*>* GetEditorHashMap	(const FVector3f&Position);

	bool*						GetEditorHashSelected(const FVector3f&Position);
	int32						GetCountSelected	();
	bool						HasSelected			();
	void						RefreshHashSelected ();
	void						SelectNode			(FStalkerAIMapNode*Node);
	void						UnSelectNode		(FStalkerAIMapNode*Node);
	void						ClearSelected		();
	void						GetSelectedNodes	(TArray<FStalkerAIMapNode*>&Result);
	bool						GetFirstSelectedNode(FStalkerAIMapNode*&Result);

	FStalkerAIMapNode*			FindOrCreateNode	(const FVector3f&Position, float ErrorToleranceForZ = 50.f,bool NotFind = false,bool AutLink=true);
	FStalkerAIMapNode*			FindNode			(const FVector3f&Position,float ErrorToleranceForZ = 50.f);
	FStalkerAIMapNode*			FindNeighbour		(FStalkerAIMapNode* Node, int32 ID);
	void						CalculateIndex		();
#endif
	
	UPROPERTY()
	FBox3f	AABB;
	UPROPERTY()
	FGuid	AIMapGuid;
	UPROPERTY()
	bool	NeedRebuild = true;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	float NodeSize = 70.f;

	TArray<FStalkerAIMapNode*>	Nodes;
	static const int32			NodesHashSize = 256;
	TArray<FStalkerAIMapNode*>	NodesHash[NodesHashSize + 1][NodesHashSize + 1];
	bool						NodesHashSelected[NodesHashSize + 1][NodesHashSize + 1];
#endif



	CHeader						LevelGraphHeader;
	TArray< CVertex>			LevelGraphVertices;

	void PostEditUndo() override;

private:
#if WITH_EDITORONLY_DATA
	void						ClearNodes			();
#endif
	const int32					Version = 1;
};
