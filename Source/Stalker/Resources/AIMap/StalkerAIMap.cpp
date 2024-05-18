#include "StalkerAIMap.h"

UStalkerAIMap::UStalkerAIMap():NeedRebuild(true)
{
}

void UStalkerAIMap::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	if (Ar.IsLoading() || Ar.IsSaving())
	{
		int32 CurrentVersion = Version;
		Ar << CurrentVersion;
		check(CurrentVersion >= 0&& CurrentVersion <= 1);

		if (CurrentVersion == 1)
		{
			Ar << LevelGraphHeader.count << LevelGraphHeader.size << LevelGraphHeader.size_y << static_cast<hdrNODES&>(LevelGraphHeader).version;
			if (LevelGraphHeader.count)
			{
				Ar <<LevelGraphHeader.aabb.min.x<<LevelGraphHeader.aabb.min.y<<LevelGraphHeader.aabb.min.z;
				Ar <<LevelGraphHeader.aabb.max.x<<LevelGraphHeader.aabb.max.y<<LevelGraphHeader.aabb.max.z;
			}
			{
				static_assert(sizeof(FGuid)==sizeof(xrGUID));
				FMemory::Memcpy(&static_cast<hdrNODES&>(LevelGraphHeader).guid,&AIMapGuid,sizeof(FGuid));
			}
			int32 Count = LevelGraphVertices.Num();
			Ar << Count;
			if (Ar.IsLoading())
			{
				LevelGraphVertices.Empty(Count);
				LevelGraphVertices.AddDefaulted(Count);
			}
			for (CVertex& Node : LevelGraphVertices)
			{
				Ar.Serialize(&Node,sizeof(CVertex));
			}
			if (Ar.IsLoading())
			{
				RefreshAIMapMetadata();
			}
		}
		
#if WITH_EDITORONLY_DATA
		else if (Ar.IsLoading())
		{
			InvalidAIMap();
		}
		if (!Ar.IsCooking())
		{
			int32 Count = Nodes.Num();
			Ar << Count;
			if (Ar.IsLoading())
			{
				bool SavedNeedRebuild = NeedRebuild;
				ClearNodes();
				Nodes.Empty(Count);
				Nodes.AddDefaulted(Count);
				for (FStalkerAIMapNode*& Node : Nodes)
				{
					Node = new FStalkerAIMapNode;
				}
				NeedRebuild			= SavedNeedRebuild;
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
#endif
		
	}

}

void UStalkerAIMap::PostInitProperties()
{
	Super::PostInitProperties();
#if WITH_EDITOR
	NodesHash.SetNum(NodesHashSize+1);
	NodesHashSelected.SetNum(NodesHashSize+1);
	for(int32 i = 0;i<=NodesHashSize;i++)
	{
		NodesHash[i].SetNum(NodesHashSize+1);
		NodesHashSelected[i].SetNum(NodesHashSize+1);
	}
#endif
}


const ILevelGraph::CHeader& UStalkerAIMap::header() const
{
	return LevelGraphHeader;
}

const ILevelGraph::CVertex* UStalkerAIMap::get_nodes() const
{
	return LevelGraphVertices.GetData();
}

void UStalkerAIMap::RefreshAIMapMetadata()
{
	if (!LevelGraphHeader.count)
	{
		m_row_length = 0;
		m_column_length = 0;
		m_level_id = -1;
		m_max_x = 0;
		m_max_z = 0;
		m_access_mask.clear();
		return;
	}
	m_row_length = FMath::FloorToInt((header().box().max.z - header().box().min.z) / header().cell_size() + EPS_L + 1.5f);
	m_column_length = FMath::FloorToInt((header().box().max.x - header().box().min.x) / header().cell_size() + EPS_L + 1.5f);
	m_access_mask.assign(header().vertex_count(), true);
	unpack_xz(vertex_position(header().box().max), m_max_x, m_max_z);
	m_level_id = -1;
}


#if WITH_EDITORONLY_DATA
void UStalkerAIMap::InvalidAIMap()
{
	NeedRebuild = true;
	LevelGraphVertices.Empty();
	FMemory::Memzero(LevelGraphHeader);
	AIMapGuid.Invalidate();
	Modify();
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
		bool* NodesHashSelect = GetEditorHashSelected(Node->Position);
		check(NodesHashList);
		check(NodesHashSelect);
		NodesHashList->Add(Node);
		*NodesHashSelect |= EnumHasAnyFlags(Node->Flags, EStalkerAIMapNodeFlags::Selected);
	}
}

void UStalkerAIMap::HashClear()
{
	for (int32 x = 0; x <= NodesHashSize; x++)
	{
		for (int32 y = 0; y <= NodesHashSize; y++)
		{
			NodesHash[x][y].Reset(0);
			NodesHashSelected[x][y] = false;
		}
	}

}

void UStalkerAIMap::RemoveSelect()
{
	NeedRebuild = true;
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
	HashFill();
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

bool* UStalkerAIMap::GetEditorHashSelected(const FVector3f& InPosition)
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

	return (ix <= NodesHashSize && ix >= 0 && iy <= NodesHashSize && iy >= 0) ? &NodesHashSelected[ix][iy] : 0;
}

int32 UStalkerAIMap::GetCountSelected()
{
	int32 Counter = 0;
	for (int32 x = 0; x <= NodesHashSize; x++)
	{
		for (int32 y = 0; y <= NodesHashSize; y++)
		{
			if (NodesHashSelected[x][y])
			{
				for (FStalkerAIMapNode* Node : NodesHash[x][y])
				{
					if (EnumHasAnyFlags(Node->Flags, EStalkerAIMapNodeFlags::Selected))
					{
						Counter++;
					}
				}
			}
		
		}
	}
	return Counter;
}

bool UStalkerAIMap::HasSelected()
{
	for (int32 x = 0; x <= NodesHashSize; x++)
	{
		for (int32 y = 0; y <= NodesHashSize; y++)
		{
			if (NodesHashSelected[x][y])
			{
				return true;
			}
		}
	}
	return false;
}

void UStalkerAIMap::RefreshHashSelected()
{
	for (int32 x = 0; x <= NodesHashSize; x++)
	{
		for (int32 y = 0; y <= NodesHashSize; y++)
		{
			NodesHashSelected[x][y] = false;
			for (FStalkerAIMapNode* Node : NodesHash[x][y])
			{
				NodesHashSelected[x][y] |= EnumHasAnyFlags(Node->Flags, EStalkerAIMapNodeFlags::Selected);
			}
		}
	}
}

void UStalkerAIMap::SelectNode(FStalkerAIMapNode* Node)
{
	bool*Select = GetEditorHashSelected(Node->Position);
	check(Select);
	*Select = true;
	Node->Flags |= EStalkerAIMapNodeFlags::Selected;

}

void UStalkerAIMap::UnSelectNode(FStalkerAIMapNode* InNode)
{
	EnumRemoveFlags(InNode->Flags, EStalkerAIMapNodeFlags::Selected);
	TArray<FStalkerAIMapNode*>* NodesHashList = GetEditorHashMap(InNode->Position);
	check(NodesHashList);
	bool* Select = GetEditorHashSelected(InNode->Position);
	check(Select);
	*Select = false;
	for (FStalkerAIMapNode* Node : *NodesHashList)
	{
		*Select |= EnumHasAnyFlags(Node->Flags, EStalkerAIMapNodeFlags::Selected);
	}
}

void UStalkerAIMap::ClearSelected()
{
	for (int32 x = 0; x <= NodesHashSize; x++)
	{
		for (int32 y = 0; y <= NodesHashSize; y++)
		{
			if (NodesHashSelected[x][y])
			{
				for (FStalkerAIMapNode* Node : NodesHash[x][y])
				{
					EnumRemoveFlags(Node->Flags, EStalkerAIMapNodeFlags::Selected);
				}
			}
		}
	}
}

void UStalkerAIMap::GetSelectedNodes(TArray<FStalkerAIMapNode*>& Result)
{
	for (int32 x = 0; x <= NodesHashSize; x++)
	{
		for (int32 y = 0; y <= NodesHashSize; y++)
		{
			if (NodesHashSelected[x][y])
			{
				for (FStalkerAIMapNode* Node : NodesHash[x][y])
				{
					if (EnumHasAnyFlags(Node->Flags, EStalkerAIMapNodeFlags::Selected))
					{
						Result.Add(Node);
					}
				}
			}
		}
	}
}

bool UStalkerAIMap::GetFirstSelectedNode(FStalkerAIMapNode*& Result)
{
	for (int32 x = 0; x <= NodesHashSize; x++)
	{
		for (int32 y = 0; y <= NodesHashSize; y++)
		{
			if (NodesHashSelected[x][y])
			{
				for (FStalkerAIMapNode* Node : NodesHash[x][y])
				{
					if (EnumHasAnyFlags(Node->Flags, EStalkerAIMapNodeFlags::Selected))
					{
						Result = Node;
						return true;
					}
				}
			}
		}
	}
	return false;
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

	NeedRebuild = true;
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



void UStalkerAIMap::PostEditUndo()
{
	Super::PostEditUndo();
	NeedRebuild = true;
}

void UStalkerAIMap::ClearNodes()
{
	NeedRebuild = true;
	for (FStalkerAIMapNode* Node : Nodes)
	{
		delete Node;
	}
	Nodes.Empty();
}
#endif
