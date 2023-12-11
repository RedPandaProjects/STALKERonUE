#include "RBMKSceneAIMapTool.h"

#include "Kernel/Unreal/WorldSettings/StalkerWorldSettings.h"
#include "Resources/AIMap/StalkerAIMap.h"


FRBMKSceneAIMapTool::FRBMKSceneAIMapTool() :FRBMKSceneToolBase(ERBMKSceneObjectType::AIMap)
{

}

FRBMKSceneAIMapTool::~FRBMKSceneAIMapTool()
{
}


bool FRBMKSceneAIMapTool::LoadStream(IReader&F)
{
	enum
	{
		AIMAP_VERSION  				= 0x0002,
		AIMAP_CHUNK_VERSION			= 0x0001,       
		AIMAP_CHUNK_FLAGS			= 0x0002,
		AIMAP_CHUNK_BOX				= 0x0003,
		AIMAP_CHUNK_PARAMS			= 0x0004,
		AIMAP_CHUNK_NODES			= 0x0006,
		AIMAP_CHUNK_SNAP_OBJECTS	= 0x0007,
		AIMAP_CHUNK_INTERNAL_DATA	= 0x0008,
		AIMAP_CHUNK_INTERNAL_DATA2	= 0x0009,
	};

	FRBMKSceneToolBase::LoadStream(F);

	uint16 version = 0;

	if(!ensure(F.r_chunk(AIMAP_CHUNK_VERSION, &version)))
	{
		return false;
	}
	if (version != AIMAP_VERSION) 
	{
		UE_LOG(LogXRayImporter,Error,TEXT("FRBMKSceneAIMapTool: unsupported version."));
		return false;
	}

	if(!ensure(F.find_chunk(AIMAP_CHUNK_FLAGS)))
	{
		return false;
	}
	F.r(&Flags, sizeof(Flags));

	if(!ensure(F.find_chunk(AIMAP_CHUNK_BOX)))
	{
		return false;
	}
	F.r(&AIBBox, sizeof(AIBBox));

	if(!ensure(F.find_chunk(AIMAP_CHUNK_PARAMS)))
	{
		return false;
	}
	F.r(&AIMapParameters, sizeof(AIMapParameters));

	
	if(!ensure(F.find_chunk(AIMAP_CHUNK_NODES)))
	{
		return false;
	}
	Nodes.SetNum(F.r_u32());
	for (FRBMKSceneAINode&Node:Nodes) 
	{
		Node.LoadStream(F, this);
	}
	return true;
}

void FRBMKSceneAIMapTool::UnpackPosition(const FRBMKSceneAINodePosition& InNodePosition,FVector3f& OutPosition) const
{
	Fvector3 InRBMKPosition;
	InRBMKPosition.x = static_cast<float>(InNodePosition.X) * AIMapParameters.PatchSize;
	InRBMKPosition.y = (static_cast<float>(InNodePosition.Y) / 65535) * (AIBBox.max.y - AIBBox.min.y) + AIBBox.min.y;
	InRBMKPosition.z = static_cast<float>(InNodePosition.Z) * AIMapParameters.PatchSize;
	OutPosition = StalkerMath::RBMKLocationToUnreal(InRBMKPosition);
}

bool FRBMKSceneAIMapTool::LoadLTX(CInifile&)
{
	return false;
}

bool FRBMKSceneAIMapTool::LoadSelection(IReader&)
{
	return false;
}

FRBMKSceneAINode::FRBMKSceneAINode()
{
	Node1 = INDEX_NONE;
	Node2 = INDEX_NONE;
	Node3 = INDEX_NONE;
	Node4 = INDEX_NONE;
}

void FRBMKSceneAINode::LoadStream(IReader&F, const FRBMKSceneAIMapTool* SceneAIMapTool)
{
	static constexpr int32 InvalidNode = (1 << 24) - 1;
	auto UnpackLinkLambda = [](const int32 & ID)->int32
	{
		const int32 Index = ID&0x00FFFFFF;
		if(InvalidNode == Index)
		{
			return INDEX_NONE;
		}
		return Index;
	};

	int32 			InID;

	F.r(&InID, 3); 			Node1 = UnpackLinkLambda(InID);
	F.r(&InID, 3); 			Node2 = UnpackLinkLambda(InID);
	F.r(&InID, 3); 			Node3 = UnpackLinkLambda(InID);
	F.r(&InID, 3); 			Node4 = UnpackLinkLambda(InID);

	Fvector3 InPlaneNormal;
	const uint16 InPlane = F.r_u16();
	pvDecompress(InPlaneNormal, InPlane);
	
	FRBMKSceneAINodePosition 	InAINodePosition;
	F.r(&InAINodePosition, sizeof(InAINodePosition));
	SceneAIMapTool->UnpackPosition(InAINodePosition,Position);
	Plane = FPlane4f(Position,StalkerMath::RBMKNormalToUnreal(InPlaneNormal));
	F.r_u8();
}



void FRBMKSceneAIMapTool::ExportToWorld(UWorld* World, EObjectFlags InFlags,const URBMKLevelImportOptions&LevelImportOptions)
{
	if (AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings()))
	{
		if(UStalkerAIMap* InAIMap = StalkerWorldSettings->GetOrCreateAIMap())
		{
			InAIMap->ClearAIMap();
			InAIMap->Nodes.AddDefaulted(Nodes.Num());
			for (int32 i = 0; i < Nodes.Num(); i++)
			{
				InAIMap->Nodes[i] = new FStalkerAIMapNode;
			}
			for (int32 i = 0; i < Nodes.Num(); i++)
			{
				InAIMap->Nodes[i]->Position = Nodes[i].Position;
				InAIMap->Nodes[i]->Position.X = InAIMap->NodeSize * FMath::RoundToDouble(InAIMap->Nodes[i]->Position.X / InAIMap->NodeSize);
				InAIMap->Nodes[i]->Position.Y = InAIMap->NodeSize * FMath::RoundToDouble(InAIMap->Nodes[i]->Position.Y / InAIMap->NodeSize);
				InAIMap->Nodes[i]->Plane = Nodes[i].Plane;
				for (int32 Link = 0; Link < 4; Link++)
				{
					if (Nodes[i].Nodes[Link] != INDEX_NONE)
					{
						InAIMap->Nodes[i]->Nodes[Link] = InAIMap->Nodes[Nodes[i].Nodes[Link]];
					}
				}
			}
			InAIMap->HashFill();
			InAIMap->Modify();
		}
	}
}