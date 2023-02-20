#pragma once
#include "StalkerEditorAIMap.generated.h"
UCLASS()
class UStalkerEditorAIMap : public UObject
{
	GENERATED_BODY()
public:
	void						Initialize				();
	void						Destroy					();
	bool						CreateNode				(struct FStalkerAIMapNode& Result, UWorld*InWorld, const FVector& InPossition,bool bIgnoreConstraints=false);
	bool						CanTravel				(UWorld* InWorld, const FVector& From, const FVector& To, const FPlane4f InPlaneFrom, const FPlane4f InPlaneTo);
	struct FStalkerAIMapNode*	FindNeighbour			(UWorld* InWorld, struct FStalkerAIMapNode* Node,int32 ID, bool bIgnoreConstraints = false);
	struct FStalkerAIMapNode*	FindNode				(UWorld* InWorld, const FVector& InPosition);
	void						AutoLink				(UWorld* InWorld, struct FStalkerAIMapNode* Node, bool bIgnoreConstraints = false);
	void						Generate				(UWorld* InWorld, bool bSelectedOnly = false);
	struct FStalkerAIMapNode*	BuildNode				(UWorld* InWorld, struct FStalkerAIMapNode* Node, int32 ID, bool bSelectedOnly);
	void						Smooth					(UWorld* InWorld, bool bSelectedOnly = false);
	void						Reset					(UWorld* InWorld, bool bSelectedOnly = false);
	void						Build					();
	void						BuildIfNeeded			();
private:
	TSharedPtr< FUICommandList>	AIMapCommands;
	FBox3f						GenerateAABB;
	TArray<FBox3f>				GenerateAABBs;
};
