#include "StalkerEditorSpawn.h"
#include "Kernel/Unreal/WorldSettings/StalkerWorldSettings.h"
#include "Resources/Spawn/StalkerLevelSpawn.h"
#include "Resources/AIMap/StalkerAIMap.h"
#include "../../Entities/Scene/SpawnObject/StalkerSpawnObject.h"
#include "Graph/Builder/game_graph_builder.h"
#include "Kernel/StalkerEngineManager.h"
#include "Resources/StalkerResourcesManager.h"
#include "Resources/Spawn/StalkerGameSpawn.h"
#include "../../Entities/Scene/WayObject/StalkerWayObject.h"
#include "../../Entities/Scene/WayObject/StalkerWayPointComponent.h"
#include "../../Entities/Scene/WayObject/StalkerWayPointLink.h"
#include "Constructor/Game/game_spawn_constructor.h"
#include "../../Entities/Scene/SpawnObject/Components/StalkerSpawnObjectSphereShapeComponent.h"
#include "../../Entities/Scene/SpawnObject/Components/StalkerSpawnObjectBoxShapeComponent.h"
#include "../../StalkerEditorManager.h"
#include "../../UI/Commands/StalkerEditorCommands.h"
#include "Kernel/Unreal/GameSettings/StalkerGameSettings.h"

void UStalkerEditorSpawn::Initialize()
{
	FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &UStalkerEditorSpawn::OnPostWorldInitialization);
	GStalkerEditorManager->UICommandList->MapAction(StalkerEditorCommands::Get().BuildLevelSpawn, FExecuteAction::CreateUObject(this, &UStalkerEditorSpawn::OnBuildLevelSpawn));
	GStalkerEditorManager->UICommandList->MapAction(StalkerEditorCommands::Get().BuildGameSpawn, FExecuteAction::CreateUObject(this, &UStalkerEditorSpawn::OnBuildGameSpawn));
}

void UStalkerEditorSpawn::Destroy()
{
	FWorldDelegates::OnPostWorldInitialization.RemoveAll(this);
}

class UStalkerLevelSpawn* UStalkerEditorSpawn::BuildLevelSpawnIfNeeded()
{
	if (FApp::IsGame())
	{
		return nullptr;
	}
	FWorldContext* WorldContext = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
	if (!WorldContext)
		return nullptr;

	UWorld* World = WorldContext->World();
	if (!IsValid(World))
		return nullptr;

	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return nullptr;
	}
	if (StalkerWorldSettings->NotForXRay)
	{
		return nullptr;
	}
	UStalkerLevelSpawn* Spawn = StalkerWorldSettings->GetSpawn();
	UStalkerAIMap* AIMap = StalkerWorldSettings->GetOrCreateAIMap();
	check(AIMap);
	if (!Spawn ||StalkerWorldSettings->NeedRebuildSpawn|| Spawn->NeedRebuild || !Spawn->SpawnGuid.IsValid() || AIMap->AIMapGuid != Spawn->AIMapGuid)
	{
		return BuildLevelSpawn();
	}
	else
	{
		UE_LOG(LogStalkerEditor, Log, TEXT("This LevelSpawn[%s] not needed rebuild!"), *Spawn->GetPathName());
	}
	return Spawn;
}

class UStalkerLevelSpawn* UStalkerEditorSpawn::BuildLevelSpawn()
{
	if (FApp::IsGame())
	{
		return nullptr;
	}
	FWorldContext* WorldContext = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
	if (!WorldContext)
		return nullptr;

	UWorld* World = WorldContext->World();
	if (!IsValid(World))
		return nullptr;

	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return nullptr;
	}
	if (StalkerWorldSettings->NotForXRay)
	{
		return nullptr;
	}
	UStalkerLevelSpawn* Spawn = StalkerWorldSettings->GetOrCreateSpawn();
	check(Spawn);
	UStalkerAIMap* AIMap = StalkerWorldSettings->GetOrCreateAIMap();
	check(AIMap);
	UE_LOG(LogStalkerEditor,Log,TEXT("Start build level spawn %s"),*Spawn->GetPathName());
	Spawn->InvalidLevelSpawn();
	if (!AIMap->AIMapGuid.IsValid())
	{
		UE_LOG(LogStalkerEditor, Error, TEXT("Failed build level spawn because ai map is empty!"));
		return Spawn;
	}
	if (AIMap->NeedRebuild)
	{
		UE_LOG(LogStalkerEditor, Warning, TEXT("Need rebuild ai map!"));
	}
	Spawn->SpawnGuid = FGuid::NewGuid();
	Spawn->AIMap = AIMap;
	Spawn->AIMapGuid = AIMap->AIMapGuid;
	Spawn->Map = World;
	CGameGraphBuilder GameGraphBuilder(AIMap);
	TArray<UStalkerSpawnObjectSphereShapeComponent*>SphereShapeComponents;
	TArray<UStalkerSpawnObjectBoxShapeComponent*>BoxShapeComponents;
	//GStalkerEngineManager->DebugShapes.Empty();
	
	FSoftObjectPath WorldSoftPath =   UWorld::RemovePIEPrefix(*World->GetPathName());
	for (TActorIterator<AStalkerSpawnObject> AactorItr(World); AactorItr; ++AactorItr)
	{
		if (!AactorItr->XRayEntity)
		{
			continue;
		}
		if (AactorItr->ExcludeFromBuild)
		{
			continue;
		}
		FString Name = AactorItr->GetActorLabel(true);
		Name.ToLowerInline();
		FString ActorLabel =  AactorItr->GetDefaultActorLabel();
		while (ActorLabel.Len() && FChar::IsDigit(ActorLabel[ActorLabel.Len() - 1]))
		{
			ActorLabel.RemoveAt(ActorLabel.Len() - 1);
		}
		if (!ActorLabel.Len()||Name.StartsWith(ActorLabel ))
		{
			Name = WorldSoftPath.GetAssetName() + TEXT("_") + Name;
		}
		AactorItr->XRayEntity->unreal_soft_refence = TCHAR_TO_ANSI(*AactorItr->GetPathName());
		AactorItr->XRayEntity->set_name_replace(TCHAR_TO_ANSI(*Name));
		AactorItr->XRayEntity->position().set(StalkerMath::UnrealLocationToRBMK(AactorItr->GetActorLocation()));
		{
			Fquaternion XRayQuat = StalkerMath::UnrealQuaternionToRBMK(FQuat(AactorItr->GetActorRotation()));
			Fmatrix XRayMatrix;
			XRayMatrix.rotation(XRayQuat);
			XRayMatrix.getHPB(AactorItr->XRayEntity->angle());
			Swap(AactorItr->XRayEntity->angle().x, AactorItr->XRayEntity->angle().y);
		}
		ISE_Shape* ShapeInterface  =  AactorItr->XRayEntity->shape();
		if (ShapeInterface)
		{
			TArray< CShapeData::shape_def> Shapes;
			AactorItr->GetComponents<UStalkerSpawnObjectSphereShapeComponent>(SphereShapeComponents, false);
			for (UStalkerSpawnObjectSphereShapeComponent* SphereShape : SphereShapeComponents)
			{
				CShapeData::shape_def&Shape =  Shapes.AddDefaulted_GetRef();
				FTransform Transform = SphereShape->GetComponentToWorld();
				Shape.type = CShapeData::cfSphere;
				Shape.data.sphere.R = Transform.GetScale3D().X * SphereShape->SphereRadius / 100.f;
				Shape.data.sphere.P = StalkerMath::UnrealLocationToRBMK(Transform.GetLocation()- AactorItr->GetActorLocation());
			}
			AactorItr->GetComponents<UStalkerSpawnObjectBoxShapeComponent>(BoxShapeComponents, false);
			for (UStalkerSpawnObjectBoxShapeComponent* BoxShape : BoxShapeComponents)
			{
				CShapeData::shape_def& Shape = Shapes.AddDefaulted_GetRef();
				FTransform Transform = BoxShape->GetComponentToWorld().GetRelativeTransform(AactorItr->GetActorTransform());
				Shape.type = CShapeData::cfBox;
				{
					FVector InScale = BoxShape->GetComponentToWorld().GetScale3D();
					Fmatrix FTransformR, FTransformS;
					FTransformR.rotation(StalkerMath::UnrealQuaternionToRBMK(Transform.GetRotation()));
					Fvector		Scale;
					Scale.set(InScale.X* BoxShape->BoxExtent.X/100.f*2.f, InScale.Z * BoxShape->BoxExtent.Z / 100.f * 2.f, InScale.Y * BoxShape->BoxExtent.Y / 100.f * 2.f);
					FTransformS.scale(Scale);
					Shape.data.box.mul(FTransformR, FTransformS);
					Shape.data.box.translate_over(StalkerMath::UnrealLocationToRBMK(Transform.GetLocation()));
				}
				{
					Fmatrix xform;
					xform.setXYZ(AactorItr->XRayEntity->o_Angle);
					xform.c.set(AactorItr->XRayEntity->o_Position);

					CShapeData::shape_def ToShape =Shape;
					ToShape.data.box.mul_43(xform, Shape.data.box);
				//	GStalkerEngineManager->DebugShapes.Add(ToShape);
				}
			/*	for (int32 i = 0; i < _countof(du_box_lines);)
				{
					Fmatrix FTransform = Shape.data.box;
					FTransform.c.add(AactorItr->XRayEntity->position());
					Fvector V1 = du_box_vertices[du_box_lines[i++]];
					Shape.data.box.transform(V1);
					Fvector V2 = du_box_vertices[du_box_lines[i++]];
					Shape.data.box.transform(V2);
					DrawDebugLine(World,FVector(StalkerMath::RBMKLocationToUnreal(V1)), FVector(StalkerMath::RBMKLocationToUnreal(V2)), FColor::Red);
				}*/
			}
		
			ShapeInterface->assign_shapes(Shapes.GetData(), Shapes.Num());
		}


		NET_Packet					Packet;
		AactorItr->XRayEntity->Spawn_Write(Packet, TRUE);
		FStalkerLevelSpawnData SpawnData;
		Spawn->Spawns.AddDefaulted();
		Spawn->Spawns.Last().SpawnData.AddUninitialized(Packet.B.count);
		for (u32 i =0;i< Packet.B.count ; i++)
		{
			Spawn->Spawns.Last().SpawnData[i] = Packet.B.data[i];
		}
		GameGraphBuilder.load_graph_point(Spawn,AactorItr->XRayEntity);
	}
	if (!Spawn->Spawns.Num())
	{
		UE_LOG(LogStalkerEditor, Error, TEXT("Failed build level spawn because spawn point not found in world!"));
		Spawn->InvalidLevelSpawn();
		return Spawn;
	}
	if (!GameGraphBuilder.build_graph(Spawn))
	{
		Spawn->InvalidLevelSpawn();
		return Spawn;
	}
	for (TActorIterator<AStalkerWayObject> AactorItr(World); AactorItr; ++AactorItr)
	{
		AactorItr->CalculateIndex();
		FStalkerLevelSpawnWay&Way =  Spawn->Ways.AddDefaulted_GetRef();
		Way.Name = AactorItr->GetActorLabel(true);
		for(UStalkerWayPointComponent*InPoint : AactorItr->Points)
		{
			FStalkerLevelSpawnWayPoint& Point = Way.Points.AddDefaulted_GetRef();
			Point.Position = InPoint->GetComponentToWorld().GetLocation();
			Point.Name = InPoint->PointName;
			Point.Flags = InPoint->Flags;
			for (FStalkerWayPointLink& InLink : InPoint->Links)
			{
				FStalkerLevelSpawnWayPointLink& Link = Point.Links.AddDefaulted_GetRef();
				Link.Probability = InLink.Probability;
				Link.ToPoint = InLink.Point->Index;
			}
			
		}

	}
	StalkerWorldSettings->Modify();
	StalkerWorldSettings->NeedRebuildSpawn = false;
	Spawn->NeedRebuild = false;
	Spawn->MarkPackageDirty();
	UE_LOG(LogStalkerEditor, Log, TEXT("Build level spawn is complete!"));
	return Spawn;
}

bool UStalkerEditorSpawn::BuildGameSpawn(UStalkerLevelSpawn* OnlyIt, bool IfNeededRebuild, bool IgnoreIncludeInBuild)
{
	if (FApp::IsGame())
	{
		return true;
	}
	UStalkerGameSpawn* GameSpawn = GStalkerEngineManager->GetResourcesManager()->GetOrCreateGameSpawn();
	check(GameSpawn);
	if (!ensure(GameSpawn->LevelsInfo.Num() == GameSpawn->GameGraph.header().level_count()))
	{
		GameSpawn->InvalidGameSpawn();
	}
	else if (GameSpawn->LevelsInfo.Num())
	{
		if (!ensure(FMemory::Memcmp(&GameSpawn->GameSpawnGuid, &GameSpawn->GameGraph.header().guid(), sizeof(FGuid)) == 0))
		{
			GameSpawn->InvalidGameSpawn();
		}
		else
		{
			for (int32 i = 0; i < GameSpawn->LevelsInfo.Num(); i++)
			{
				if (!ensure(FMemory::Memcmp(&GameSpawn->LevelsInfo[i].AIMapGuid,&GameSpawn->GameGraph.header().level(i).guid(),sizeof(FGuid)) == 0))
				{
					GameSpawn->InvalidGameSpawn();
					break;
				}
			}
		}
	}
	UE_LOG(LogXRayGameSpawnConstructor, Log, TEXT("Start build spawn"));
	LevelSpawns.Empty();
	const UStalkerGameSettings* SGSettings = GetDefault<UStalkerGameSettings>();
	const TMap<FName, FStalkerLevelInfo>& Levels = SGSettings->GetCurrentLevels();


	TArray<FStalkerGameSpawnLevelInfo>	NewLevelsInfo;

	if (OnlyIt)
	{
		FString LevelName = TEXT("editor");
		for (auto& [Name, Level] : Levels)
		{
			if (Level.Map == OnlyIt->Map)
			{
				LevelName = Name.ToString().ToLower();
				break;
			}
		}
		OnlyIt->LevelID = 0;
		{
			FStalkerGameSpawnLevelInfo& GameSpawnLevelInfo = NewLevelsInfo.AddDefaulted_GetRef();
			GameSpawnLevelInfo.AIMapGuid = OnlyIt->AIMapGuid;
			GameSpawnLevelInfo.LeveID = OnlyIt->LevelID;
			GameSpawnLevelInfo.Name = LevelName;
			GameSpawnLevelInfo.Map = OnlyIt->Map;
			GameSpawnLevelInfo.LevelSpawnGuid = OnlyIt->SpawnGuid;
		}
		LevelSpawns.Add(OnlyIt);
	}
	else
	{
		
		int32 CountLevel = 0;
		for (auto& [Name,Level] : Levels)
		{
			if (!Level.IncludeInBuildSpawn &&! IgnoreIncludeInBuild &&! SGSettings->IgnoreIncludeInBuildSpawn)
			{
				continue;
			}
			const FString ParentPackageName = FPaths::GetPath(Level.Map.ToString()) / FPaths::GetBaseFilename(Level.Map.ToString()) + TEXT("_Spawn");
			const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
			UStalkerLevelSpawn* CurrentLevelSpawn =  LoadObject<UStalkerLevelSpawn>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
			if (CurrentLevelSpawn)
			{
				if (CurrentLevelSpawn->AIMap == nullptr)
				{
					UE_LOG(LogXRayGameSpawnConstructor, Error, TEXT("AI map is empty for level spawn %s"), *ParentPackageName);
					GameSpawn->InvalidGameSpawn();
					LevelSpawns.Empty();
					return false;
				}
				if (CurrentLevelSpawn->AIMap->NeedRebuild)
				{
					UE_LOG(LogStalkerEditor, Warning, TEXT("Need rebuild ai map %s!"), *CurrentLevelSpawn->AIMap->GetPathName());
				}
				if (CurrentLevelSpawn->NeedRebuild)
				{
					UE_LOG(LogStalkerEditor, Warning, TEXT("Need rebuild LevelSpawn %s!"), *ParentPackageName);
				}
				FStalkerGameSpawnLevelInfo& GameSpawnLevelInfo = NewLevelsInfo.AddDefaulted_GetRef();
				GameSpawnLevelInfo.AIMapGuid = CurrentLevelSpawn->AIMapGuid;
				GameSpawnLevelInfo.LevelSpawnGuid = CurrentLevelSpawn->SpawnGuid;
				GameSpawnLevelInfo.LeveID = CurrentLevelSpawn->LevelID = CountLevel++;
				GameSpawnLevelInfo.Name = Name.ToString().ToLower();
				GameSpawnLevelInfo.Map = CurrentLevelSpawn->Map;
				LevelSpawns.Add(CurrentLevelSpawn);
			}
			else
			{
				UE_LOG(LogXRayGameSpawnConstructor, Error, TEXT("No found level spawn %s"), *ParentPackageName);
				GameSpawn->InvalidGameSpawn();
				LevelSpawns.Empty();
				return false;
			}
		}
		
	}
	for (UStalkerLevelSpawn* LevelSpawn : LevelSpawns)
	{
		if (!LevelSpawn->SpawnGuid.IsValid())
		{
			UE_LOG(LogXRayGameSpawnConstructor, Error, TEXT("Level spawn [%s] is empty"), *LevelSpawn->GetPathName());
			LevelSpawns.Empty();
			GameSpawn->InvalidGameSpawn();
			return false;
		}
		if (LevelSpawn->AIMapGuid != LevelSpawn->AIMap->AIMapGuid)
		{
			UE_LOG(LogXRayGameSpawnConstructor, Error, TEXT("Level spawn [%s] is invalid because ai map is already different!"), *LevelSpawn->GetPathName());
			LevelSpawns.Empty();
			GameSpawn->InvalidGameSpawn();
			return false;
		}
	}
	if (IfNeededRebuild)
	{
		if (GameSpawn->GameSpawnGuid.IsValid()&& GameSpawn->LevelsInfo.Num() == NewLevelsInfo.Num())
		{
			bool NeedRebuild = false;
			for (int32 i = 0; i < GameSpawn->LevelsInfo.Num(); i++)
			{
				if (GameSpawn->LevelsInfo[i].LevelSpawnGuid != NewLevelsInfo[i].LevelSpawnGuid)
				{
					NeedRebuild = true;
				}
				else if (GameSpawn->LevelsInfo[i].Name != NewLevelsInfo[i].Name)
				{
					NeedRebuild = true;
				}
				else if (GameSpawn->LevelsInfo[i].Map != NewLevelsInfo[i].Map)
				{
					NeedRebuild = true;
				}
			}
			if (!NeedRebuild)
			{
				UE_LOG(LogXRayGameSpawnConstructor, Log, TEXT("This GameSpawn not needed rebuild!"));
				LevelSpawns.Empty();
				return true;
			}
		}
	}
	GameSpawn->InvalidGameSpawn();
	GameSpawn->LevelsInfo =	NewLevelsInfo;
	GameSpawn->GameSpawnGuid = FGuid::NewGuid();
	BuildGameGraph(GameSpawn);
	{
		CGameSpawnConstructor GameSpawnConstructor;
		if (!GameSpawnConstructor.build(GameSpawn, LevelSpawns))
		{
			GameSpawn->InvalidGameSpawn();
		}
	}
	LevelSpawns.Empty();
	GameSpawn->Modify();
	UE_LOG(LogXRayGameSpawnConstructor, Log, TEXT("Build game spawn is complete!"));
	return true;
}

void UStalkerEditorSpawn::BuildGameGraph(UStalkerGameSpawn* GameSpawn)
{
	UE_LOG(LogXRayGameSpawnConstructor, Log, TEXT("Start merge game graph!"));
	TMap< FStalkerLevelGraphPointConnection* ,int32>				ConnectiontoID;
	TMap<int32, FStalkerLevelGraphPointConnection*>					IDtoConnection;
	TMap<int32, TMap<FString, FStalkerLevelGraphPointConnection*>>	LevelAndNameToConnection;
	GameSpawn->GameGraph.CrossTables.AddDefaulted(LevelSpawns.Num());
	TArray<IGameGraph::CEdge> TempEdges;
	for (UStalkerLevelSpawn* LevelSpawn : LevelSpawns)
	{

		int32 VertexOffset = GameSpawn->GameGraph.Vertices.Num();
		StalkerGameLevelCrossTable& CrossTables = GameSpawn->GameGraph.CrossTables[LevelSpawn->LevelID];
		{	
			IGameLevelCrossTable::CHeader& Header = CrossTables.GetHeader();
			Header.m_level_guid = LevelSpawn->AIMap->header().guid();
			FMemory::Memcpy(&Header.m_game_guid, &GameSpawn->GameSpawnGuid, sizeof(FGuid));
			Header.dwNodeCount = LevelSpawn->GameCrossTableCells.Num();
			Header.dwGraphPointCount = LevelSpawn->GameGraphVertices.Num();
			Header.dwVersion = XRAI_CURRENT_VERSION;
		}
		for (IGameLevelCrossTable::CCell& InCell : LevelSpawn->GameCrossTableCells)
		{
			IGameLevelCrossTable::CCell&Cell = CrossTables.Cells.AddDefaulted_GetRef();
			Cell.tGraphIndex = InCell.tGraphIndex + VertexOffset;
			Cell.fDistance = InCell.fDistance;
		}
		for (int32 InVertexID=0; InVertexID< LevelSpawn->GameGraphVertices.Num(); InVertexID++)
		{
			IGameGraph::CVertex& InVertex = LevelSpawn->GameGraphVertices[InVertexID];
			int32 VertexID = GameSpawn->GameGraph.Vertices.Num();
			ConnectiontoID.Add(&LevelSpawn->GameGraphConnection[InVertexID], VertexID);
			IDtoConnection.Add(VertexID,&LevelSpawn->GameGraphConnection[InVertexID]);
			LevelAndNameToConnection.FindOrAdd(LevelSpawn->LevelID).Add(LevelSpawn->GameGraphConnection[InVertexID].Name,&LevelSpawn->GameGraphConnection[InVertexID]);
			IGameGraph::CVertex&Vertex = GameSpawn->GameGraph.Vertices.AddDefaulted_GetRef();
			Vertex.tLocalPoint = InVertex.tLocalPoint;
			Vertex.tGlobalPoint = InVertex.tGlobalPoint;
			Vertex.tNeighbourCount =  InVertex.tNeighbourCount;
			Vertex.tNodeID = InVertex.tNodeID;
			Vertex.tLevelID = LevelSpawn->LevelID;
			Vertex.dwEdgeOffset = TempEdges.Num();
			Vertex.dwPointOffset = GameSpawn->GameGraph.LevelPoints.Num();
			FMemory::Memcpy(Vertex.tVertexTypes, InVertex.tVertexTypes);
			for (u32 i=0;i < InVertex.tNeighbourCount ; i++)
			{
				IGameGraph::CEdge&Edge = TempEdges.AddDefaulted_GetRef();
				IGameGraph::CEdge&InEdge = LevelSpawn->GameGraphEdges[InVertex.dwEdgeOffset + i];
				Edge.m_vertex_id = VertexOffset + InEdge.m_vertex_id;
				Edge.m_path_distance = InEdge.m_path_distance;
			}
			
			TArray<u32>		Nodes;
			for (u32 i = 0; i < static_cast<u32>(CrossTables.Cells.Num()); i++)
			{
				if(CrossTables.Cells[i].tGraphIndex == VertexID)
				{
					Nodes.Add(i);
				}
			}
			for (int32 i = Nodes.Num() - 1; i > 0; --i)
			{
				Swap(Nodes[i], Nodes[FMath::Rand() % (i + 1)]);
			}
			int32 LevelCount = FMath::Min(Nodes.Num(), 255);
			for (int32 LevelPointID = 0; LevelPointID < LevelCount; LevelPointID++)
			{
				IGameGraph::CLevelPoint& LevelPoint = GameSpawn->GameGraph.LevelPoints.AddDefaulted_GetRef();
				LevelPoint.tPoint = LevelSpawn->AIMap->vertex_position(Nodes[LevelPointID]);
				LevelPoint.tNodeID = Nodes[LevelPointID];
				LevelPoint.fDistance = CrossTables.Cells[Nodes[LevelPointID]].distance();
			}
			Vertex.tDeathPointCount = static_cast<u32>(LevelCount);
		}
	}
	GameSpawn->GameGraph.Edges.Empty(TempEdges.Num());
	for (int32 InVertexID = 0; InVertexID < GameSpawn->GameGraph.Vertices.Num(); InVertexID++)
	{
		IGameGraph::CVertex& InVertex = GameSpawn->GameGraph.Vertices[InVertexID];
		
		int32 OffsetEdges = InVertex.dwEdgeOffset;
		InVertex.dwEdgeOffset = GameSpawn->GameGraph.Edges.Num();
		for (u32 i = 0; i < InVertex.tNeighbourCount; i++)
		{
			IGameGraph::CEdge& Edge   = GameSpawn->GameGraph.Edges.AddDefaulted_GetRef();
			IGameGraph::CEdge& InEdge = TempEdges[OffsetEdges + i];
			Edge.m_vertex_id =			InEdge.m_vertex_id;
			Edge.m_path_distance =		InEdge.m_path_distance;
		}
		if (IDtoConnection[static_cast<uint32>(InVertexID)]->ConnectionLevelName.Len())
		{
			FStalkerGameSpawnLevelInfo* LevelInfo = GameSpawn->LevelsInfo.FindByPredicate([&IDtoConnection, InVertexID](const FStalkerGameSpawnLevelInfo& LevelInfo) {return LevelInfo.Name == IDtoConnection[static_cast<uint32>(InVertexID)]->ConnectionLevelName; });
			if (!LevelInfo)
			{
				UE_LOG(LogXRayGameSpawnConstructor, Warning, TEXT("Invalid level conection %s in graph point %s in level %s!"), *IDtoConnection[static_cast<uint32>(InVertexID)]->ConnectionLevelName, *IDtoConnection[static_cast<uint32>(InVertexID)]->Name, *GameSpawn->LevelsInfo[InVertex.level_id()].Name);
			}
			else
			{
				TMap<FString, FStalkerLevelGraphPointConnection*>& NameToConnection = LevelAndNameToConnection[LevelInfo->LeveID];
				FStalkerLevelGraphPointConnection** Connection = NameToConnection.Find(IDtoConnection[static_cast<uint32>(InVertexID)]->ConnectionPointName);
				if (!Connection)
				{
					UE_LOG(LogXRayGameSpawnConstructor, Warning, TEXT("Invalid point conection %s in graph point %s in level %s!"), *IDtoConnection[static_cast<uint32>(InVertexID)]->ConnectionPointName, *IDtoConnection[static_cast<uint32>(InVertexID)]->Name, *GameSpawn->LevelsInfo[InVertex.level_id()].Name);
				}
				else
				{
					InVertex.tNeighbourCount++;
					IGameGraph::CEdge& Edge = GameSpawn->GameGraph.Edges.AddDefaulted_GetRef();
					Edge.m_vertex_id = ConnectiontoID[*Connection];
					Edge.m_path_distance = 250;
					UE_LOG(LogXRayGameSpawnConstructor, Log, TEXT("Added conection for graph point %s in level %s to point %s in level %s!"), *IDtoConnection[static_cast<uint32>(InVertexID)]->Name, *GameSpawn->LevelsInfo[InVertex.level_id()].Name, *IDtoConnection[static_cast<uint32>(InVertexID)]->ConnectionPointName, *IDtoConnection[static_cast<uint32>(InVertexID)]->ConnectionLevelName);
				}

			}
		}
	}
	{
		IGameGraph::CHeader& Header = GameSpawn->GameGraph.GetHeader();
		Header.m_vertex_count = GameSpawn->GameGraph.Vertices.Num();
		Header.m_edge_count = GameSpawn->GameGraph.Edges.Num();
		Header.m_death_point_count = GameSpawn->GameGraph.LevelPoints.Num();
		Header.m_version = XRAI_CURRENT_VERSION;
		GameSpawn->GameGraph.BuildHeader(GameSpawn);
	}

	UE_LOG(LogXRayGameSpawnConstructor, Log, TEXT("Merge game spawn is complete!"));

}

void UStalkerEditorSpawn::OnBuildLevelSpawn()
{
	BuildLevelSpawn();
}

void UStalkerEditorSpawn::OnBuildGameSpawn()
{
	if (FApp::IsGame())
	{
		return;
	}
	if (!BuildGameSpawn(nullptr,false,true))
	{
		FMessageDialog::Open(EAppMsgType::Type::Ok, EAppReturnType::Type::Ok, FText::FromString(TEXT("Failure build GameSpawn,see log!")));
	}
}

void UStalkerEditorSpawn::OnPostWorldInitialization(UWorld* World, const UWorld::InitializationValues IVS)
{
	if (!IVS.bInitializeScenes)
	{
		return;
	}
	for (TActorIterator<AStalkerSpawnObject> AactorItr(World); AactorItr; ++AactorItr)
	{
		if (!AactorItr->XRayEntity)
		{
			AactorItr->SpawnRead();
		}
	}
}
