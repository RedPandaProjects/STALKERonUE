#include "StalkerAIMapNode.h"
#include "StalkerAIMap.h"
#if WITH_EDITORONLY_DATA
FStalkerAIMapNode::FStalkerAIMapNode()
{
	Flags = EStalkerAIMapNodeFlags::None;
	Index = 0xFFFFFFFF;
	NodeLeft = nullptr;
	NodeForward = nullptr;
	NodeRight = nullptr;
	NodeBackward = nullptr;

}

void FStalkerAIMapNode::Serialize(FArchive& Ar, UStalkerAIMap* Parent)
{
	if (!Ar.IsLoading()&& !Ar.IsSaving())
		return;

	uint32 UnkownIndex = 0xFFFFFFFF;
	int32 LoadVersion = Version;
	Ar<< LoadVersion;
	check (LoadVersion == 0);
	Ar<< Position;
	Ar<< Plane;
	for (int32 i = 0; i < 4; i++)
	{

		if (Ar.IsLoading())
		{
			uint32 NodeIndex = 0xFFFFFFFF;
			Ar<< NodeIndex;
			Nodes[i] = NodeIndex == UnkownIndex?nullptr : Parent->Nodes[NodeIndex];

		}
		else if (Ar.IsSaving())
		{
			if (Nodes[i])
			{
				Ar << Nodes[i]->Index;

			}
			else
			{
				Ar << UnkownIndex;
			}
		}
		
	}
	if (Ar.IsLoading())
	{
		Flags = EStalkerAIMapNodeFlags::None;
	}
}
#endif