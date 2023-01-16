#include "StalkerAIMap.h"

#if WITH_EDITORONLY_DATA

void UStalkerAIMap::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	if (Ar.IsLoading() || Ar.IsSaving())
	{
		int32 CurrentVersion = Version;
		Ar << CurrentVersion;
		check(CurrentVersion == 0);

		int32 Count = Nodes.Num();
		Ar << Count;
		if (Ar.IsLoading())
		{
			ClearNodes();
			Nodes.Empty(Count);
			Nodes.AddDefaulted(Count);
			for (FStalkerAIMapNode*& Node : Nodes)
			{
				Node = new FStalkerAIMapNode;
			}
		}
		if (Ar.IsSaving())
		{
			CalculateIndex();
		}
		for (FStalkerAIMapNode* Node : Nodes)
		{
			Node->Serialize(Ar, this);
		}
		if (Ar.IsLoading())
		{
			HashFill();
		}
	}

}

void UStalkerAIMap::InvalidAIMap()
{

}

void UStalkerAIMap::AutoLink(FStalkerAIMapNode* Node)
{
	FVector3f Position = Node->Position;
	FStalkerAIMapNode* LeftNode = FindNode(Position + FVector3f(NodeSize, 0, 0));
	FStalkerAIMapNode* ForwardNode = FindNode(Position + FVector3f(0, NodeSize, 0));
	FStalkerAIMapNode* RightNode = FindNode(Position + FVector3f(-NodeSize, 0, 0));
	FStalkerAIMapNode* BackwardNode = FindNode(Position + FVector3f(0, -NodeSize, 0));
	if (LeftNode == Node)
	{
		LeftNode = nullptr;
	}

	if (ForwardNode == Node)
	{
		ForwardNode = nullptr;
	}

	if (RightNode == Node)
	{
		RightNode = nullptr;
	}

	if (BackwardNode == Node)
	{
		BackwardNode = nullptr;
	}


	if (LeftNode)
	{
		if (LeftNode->NodeRight == nullptr)
		{
			Node->NodeLeft = LeftNode;
			LeftNode->NodeRight = Node;
		}
	}
	if (RightNode)
	{
		if (RightNode->NodeLeft == nullptr)
		{
			Node->NodeRight = RightNode;
			RightNode->NodeLeft = Node;
		}

	}
	if (BackwardNode)
	{
		if (BackwardNode->NodeForward == nullptr)
		{
			Node->NodeBackward = BackwardNode;
			BackwardNode->NodeForward = Node;
		}
	}
	if (ForwardNode)
	{
		if (ForwardNode->NodeBackward == nullptr)
		{
			Node->NodeForward = ForwardNode;
			ForwardNode->NodeBackward = Node;
		}
	}
}

void UStalkerAIMap::BeginDestroy()
{
	ClearNodes();
	Super::BeginDestroy();
}

void UStalkerAIMap::ClearAIMap()
{
	InvalidAIMap();
	ClearNodes();
	HashClear();
	AABB = FBox3f(ForceInit);
}

void UStalkerAIMap::CalculateAABB(const FBox3f& NewAABB)
{
	AABB = NewAABB;
	for (FStalkerAIMapNode* Node : Nodes)
	{
		AABB += Node->Position + FVector3f(NodeSize * 0.5f, NodeSize * 0.5f, 2.f);
		AABB += Node->Position + FVector3f(-NodeSize * 0.5f, -NodeSize * 0.5f, -2.f);
	}
	if (AABB.GetSize().X <= NodesHashSize * NodeSize)
	{
		AABB.Max.X += NodesHashSize * NodeSize;

	}
	if (AABB.GetSize().Y <= NodesHashSize * NodeSize)
	{
		AABB.Max.Y += NodesHashSize * NodeSize;

	}
}

void UStalkerAIMap::HashFill(const FBox3f& NewAABB)
{
	HashClear();
	CalculateAABB(NewAABB);
	for (FStalkerAIMapNode* Node : Nodes)
	{
		TArray<FStalkerAIMapNode*>* NodesHashList = GetEditorHashMap(Node->Position);
		check(NodesHashList);
		NodesHashList->Add(Node);
	}
}

void UStalkerAIMap::HashClear()
{
	for (int32 x = 0; x <= NodesHashSize; x++)
	{
		for (int32 y = 0; y <= NodesHashSize; y++)
		{
			NodesHash[x][y].Reset(0);
		}
	}

}

void UStalkerAIMap::RemoveSelect()
{
	for (FStalkerAIMapNode* Node : Nodes)
	{
		if (EnumHasAnyFlags(Node->Flags, EStalkerAIMapNodeFlags::Selected))
		{
			TArray<FStalkerAIMapNode*>* NodesHashList = GetEditorHashMap(Node->Position);
			if (NodesHashList)NodesHashList->Remove(Node);
		}
	}
	for (int32 i = 0; i < Nodes.Num(); i++)
	{
		FStalkerAIMapNode* Node = Nodes[i];
		for (int32 link = 0; link < 4; link++)
		{
			if (Node->Nodes[link] && EnumHasAnyFlags(Node->Nodes[link]->Flags, EStalkerAIMapNodeFlags::Selected))
			{
				Node->Nodes[link] = nullptr;
			}
		}
	}
	for (int32 i = 0; i < Nodes.Num(); i++)
	{
		FStalkerAIMapNode* Node = Nodes[i];
		if (EnumHasAnyFlags(Node->Flags, EStalkerAIMapNodeFlags::Selected))
		{
			Nodes.Remove(Node);
			delete Node;
			i = 0;
		}
	}
}

TArray<FStalkerAIMapNode*>* UStalkerAIMap::GetEditorHashMap(const FVector3f& InPosition)
{
	if (!AABB.IsValid)
	{
		return nullptr;
	}
	FVector3f	VMscale, Scale, Position = FVector3f(FMath::Floor(InPosition.X / NodeSize) * NodeSize, FMath::Floor(InPosition.Y / NodeSize) * NodeSize, InPosition.Z);

	VMscale.Set(AABB.Max.X - AABB.Min.X, AABB.Max.Y - AABB.Min.Y, AABB.Max.Z - AABB.Min.Z);
	Scale.Set(float(NodesHashSize), float(NodesHashSize), 0);
	Scale /= VMscale;

	int32 ix = VMscale.X == 0 ? 0 : static_cast<int32>(FMath::Floor((Position.X - AABB.Min.X) * Scale.X));
	int32 iy = VMscale.Y == 0 ? 0 : static_cast<int32>(FMath::Floor((Position.Y - AABB.Min.Y) * Scale.Y));

	return (ix <= NodesHashSize && ix >= 0 && iy <= NodesHashSize && iy >= 0) ? &NodesHash[ix][iy] : 0;
}

FStalkerAIMapNode* UStalkerAIMap::FindOrCreateNode(const FVector3f& InPosition, float ErrorToleranceForZ, bool NotFind, bool bAutoLink)
{
	FVector3f Position = FVector3f(FMath::Floor(InPosition.X / NodeSize) * NodeSize, FMath::Floor(InPosition.Y / NodeSize) * NodeSize, InPosition.Z);
	TArray<FStalkerAIMapNode*>* NodesHashList = GetEditorHashMap(InPosition);
	if (NodesHashList)
	{
		FStalkerAIMapNode* Result = nullptr;
		for (FStalkerAIMapNode* Node : *NodesHashList)
		{
			if (FMath::IsNearlyEqual(Node->Position.X, Position.X, 1.f) &&
				FMath::IsNearlyEqual(Node->Position.Y, Position.Y, 1.f) &&
				FMath::IsNearlyEqual(Node->Position.Z, Position.Z, ErrorToleranceForZ))
			{
				if (Result)
				{
					if (FMath::Abs(InPosition.Z - Result->Position.Z) > FMath::Abs(InPosition.Z - Node->Position.Z))
					{
						Result = Node;
					}
				}
				else
				{
					Result = Node;
				}
			}
		}
		if (Result)
		{
			if (NotFind)
			{
				return nullptr;
			}
			return Result;
		}
	}
	Nodes.Add(new FStalkerAIMapNode);
	Nodes.Last()->Position = Position;
	Nodes.Last()->Plane = FPlane4f(Position, FVector3f(0, 0, 1));
	if (!NodesHashList)
	{
		HashFill();
	}
	else
	{
		NodesHashList->Add(Nodes.Last());
	}
	if (bAutoLink)
	{
		AutoLink(Nodes.Last());
	}
	return Nodes.Last();
}

FStalkerAIMapNode* UStalkerAIMap::FindNode(const FVector3f& InPosition, float ErrorToleranceForZ)
{
	TArray<FStalkerAIMapNode*>* NodesHashList = GetEditorHashMap(InPosition);
	if (NodesHashList)
	{
		FVector3f Position = FVector3f(FMath::Floor(InPosition.X / NodeSize) * NodeSize, FMath::Floor(InPosition.Y / NodeSize) * NodeSize, InPosition.Z);
		FStalkerAIMapNode* Result = nullptr;
		for (FStalkerAIMapNode* Node : *NodesHashList)
		{
			if (FMath::IsNearlyEqual(Node->Position.X, Position.X, 1.f) &&
				FMath::IsNearlyEqual(Node->Position.Y, Position.Y, 1.f) &&
				FMath::IsNearlyEqual(Node->Position.Z, Position.Z, ErrorToleranceForZ))
			{
				if (Result)
				{
					if (FMath::Abs(InPosition.Z - Result->Position.Z) > FMath::Abs(InPosition.Z - Node->Position.Z))
					{
						Result = Node;
					}
				}
				else
				{
					Result = Node;
				}
			}
		}
		return Result;
	}
	return nullptr;
}


FStalkerAIMapNode* UStalkerAIMap::FindNeighbour(FStalkerAIMapNode* Node, int32 ID)
{
	FVector3f Position = Node->Position;
	if (ID == 0)
	{
		FStalkerAIMapNode* LeftNode = FindNode(Position + FVector3f(NodeSize, 0, 0));
		return LeftNode == Node ? nullptr : LeftNode;

	}
	else if (ID == 1)
	{
		FStalkerAIMapNode* ForwardNode = FindNode(Position + FVector3f(0, NodeSize, 0));
		return ForwardNode == Node ? nullptr : ForwardNode;
	}
	else if (ID == 2)
	{
		FStalkerAIMapNode* RightNode = FindNode(Position + FVector3f(-NodeSize, 0, 0));
		return RightNode == Node ? nullptr : RightNode;
	}
	else if (ID == 3)
	{

		FStalkerAIMapNode* BackwardNode = FindNode(Position + FVector3f(0, -NodeSize, 0));
		return BackwardNode == Node ? nullptr : BackwardNode;
	}
	return nullptr;
}

void UStalkerAIMap::CalculateIndex()
{
	int32 Index = 0;
	for (FStalkerAIMapNode* Node : Nodes)
	{
		Node->Index = Index++;
	}

}

void UStalkerAIMap::ClearNodes()
{
	for (FStalkerAIMapNode* Node : Nodes)
	{
		delete Node;
	}
	Nodes.Empty();
}
#endif