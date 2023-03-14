#pragma once
#if WITH_EDITORONLY_DATA
enum class EStalkerAIMapNodeFlags
{
	None = 0,
	Selected,
};
ENUM_CLASS_FLAGS(EStalkerAIMapNodeFlags);
struct STALKER_API FStalkerAIMapNode 
{
	FPlane4f	Plane;
	FVector3f	Position;
	EStalkerAIMapNodeFlags		Flags;
	
	union 
	{
		struct 
		{
			FStalkerAIMapNode* NodeLeft;		// Left
			FStalkerAIMapNode* NodeForward;		// Forward
			FStalkerAIMapNode* NodeRight;		// Right
			FStalkerAIMapNode* NodeBackward;	// Backward
		};
		FStalkerAIMapNode* Nodes[4];
	};
	FStalkerAIMapNode();
	void Serialize(FArchive& Ar, class UStalkerAIMap* Parent);

	uint32		Index;
	int32		Version = 0;
};
#endif
