#include "StalkerEditorAIMap.h"
#include "Kernel/Unreal/WorldSettings/StalkerWorldSettings.h"
#include "Resources/AIMap/StalkerAIMap.h"
#include "Resources/AIMap/StalkerAIMapNode.h"
#include "../../Entities/Tools/AIMapBoundsVolume/StalkerAIMapBoundsVolume.h"
#include "../../UI/Commands/StalkerEditorCommands.h"
#include "../../StalkerEditorManager.h"
#include "Resources/Spawn/StalkerLevelSpawn.h"

void UStalkerEditorAIMap::Initialize()
{
	GStalkerEditorManager->UICommandList->MapAction(StalkerEditorCommands::Get().BuildAIMap, FExecuteAction::CreateUObject(this, &UStalkerEditorAIMap::Build));
}

void UStalkerEditorAIMap::Destroy()
{
}

bool UStalkerEditorAIMap::CreateNode(FStalkerAIMapNode& Result,UWorld* InWorld, const FVector& InPosition, bool bIgnoreConstraints )
{
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(InWorld->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return false;
	}

	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetOrCreateAIMap();
	if (!StalkerAIMap)
	{
		return false;
	}

	StalkerAIMap->NeedRebuild = true;

	const float NodeSize = StalkerAIMap->NodeSize;
	const int32	RCAST_MaxTris = (2 * 1024);
	const int32	RCAST_Count = 4;
	const int32	RCAST_Total = (2 * RCAST_Count + 1) * (2 * RCAST_Count + 1);
	const float	RCAST_Depth = 1.f * 100.f;
	const float RCAST_VALID = 0.55f * 100.f;

	const FVector3f Position = FVector3f(FMath::Floor(InPosition.X / NodeSize) * NodeSize, FMath::Floor(InPosition.Y / NodeSize) * NodeSize, InPosition.Z);


	TArray<FVector> Points;
	TArray<FVector> Normals;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.bTraceComplex = true;
	float RCASTDelta = 0.5f * NodeSize/ float(RCAST_Count);

	for (int32 x = -RCAST_Count; x <= RCAST_Count; x++)
	{
		float RayX = Position.X + RCASTDelta * float(x);
		for (int32 y = -RCAST_Count; y <= RCAST_Count; y++)
		{
			float RayY = Position.Y + RCASTDelta * float(y);
			FVector StartRay = FVector(RayX, RayY, Position.Z + RCAST_Depth);
			FVector EndRay = FVector(RayX, RayY, Position.Z - RCAST_Depth);
			FHitResult HitResult;
			if (InWorld->LineTraceSingleByChannel(HitResult, StartRay, EndRay, ECC_WorldStatic,CollisionQueryParams,FCollisionResponseParams(ECR_Block)))
			{
				Points.Add(HitResult.Location);
				Normals.Add(HitResult.Normal);
			}
		}
	}

	if (Points.Num() < 3)
	{
		return false;
	}
	
	float RCASTLimit = bIgnoreConstraints ? 0.015f : 0.7f;
	if (float(Points.Num()) / float(RCAST_Total) < RCASTLimit)
	{
		return false;
	}

	FVector NodeNormal = FVector(0,0,0);
	for (const FVector& InNormal : Normals)
	{
		NodeNormal += InNormal;
	}

	NodeNormal.Normalize();

	FVector NodePosition(0,0, Position.Y - 1000 * 100);
	{
		FPlane NodePlane(NodePosition, NodeNormal);
		for (const FVector& InPoint : Points)
		{
			if (NodePlane.PlaneDot(InPoint) > 0)
			{
				NodePlane = FPlane(InPoint, NodeNormal);
				NodePosition = InPoint;
			}
		}
	}

	Result.Plane = FPlane4f(FVector3f( NodePosition), FVector3f(NodeNormal));
	Result.Position = FMath::RayPlaneIntersection(FVector3f(Position) - FVector3f(0, 0, -RCAST_Depth), FVector3f(0, 0, 1), Result.Plane);
	
	if (FMath::RadiansToDegrees(FMath::Acos(Result.Plane.GetNormal() | FVector3f(0, 0, 1))) > 60.f)
	{
			return false;
	}
	if (Position.Z > Result.Position.Z) 
	{
		if (Position.Z - Result.Position.Z > StalkerWorldSettings->AIMapCanDOWN)
			return false;
	}
	else 
	{
		if (Result.Position.Z - Position.Z > StalkerWorldSettings->AIMapCanUP)
			return false;
	}

	if (!bIgnoreConstraints)
	{
		float MaxZ = FMath::RayPlaneIntersection(FVector3f(Position) - FVector3f(NodeSize * .5f, NodeSize * .5f, 0), FVector3f(0, 0, -1), Result.Plane).Z;
		MaxZ = FMath::Max(FMath::RayPlaneIntersection(FVector3f(Position) - FVector3f(NodeSize * .5f, -NodeSize * .5f, 0), FVector3f(0, 0, -1), Result.Plane).Z, MaxZ);
		MaxZ = FMath::Max(FMath::RayPlaneIntersection(FVector3f(Position) - FVector3f(-NodeSize * .5f, NodeSize * .5f, 0), FVector3f(0, 0, -1), Result.Plane).Z, MaxZ);
		MaxZ = FMath::Max(FMath::RayPlaneIntersection(FVector3f(Position) - FVector3f(-NodeSize * .5f, -NodeSize * .5f, 0), FVector3f(0, 0, -1), Result.Plane).Z, MaxZ);

		FBox TestHeightBox(ForceInit);
		TestHeightBox += FVector(Position.X - NodeSize * 0.5f, Position.Y - NodeSize * 0.5f, MaxZ + 6.f);
		TestHeightBox += FVector(Position.X + NodeSize * 0.5f, Position.Y + NodeSize * 0.5f, MaxZ + StalkerWorldSettings->AIMapTestHeight);
		TArray<FHitResult> HitResults;
		InWorld->SweepMultiByChannel(HitResults, TestHeightBox.GetCenter(), TestHeightBox.GetCenter(), FQuat::Identity, ECC_WorldStatic, FCollisionShape::MakeBox(TestHeightBox.GetExtent()), CollisionQueryParams, FCollisionResponseParams(ECR_Block));
		if (HitResults.Num())
		{
			for (FHitResult& HitResult : HitResults)
			{
				if (HitResult.bBlockingHit)
				{
					//DrawDebugBox(InWorld, TestHeightBox.GetCenter(), TestHeightBox.GetExtent(), FColor::Red, true, 10.f);
					return false;
				}
			}
		}
		//DrawDebugBox(InWorld, TestHeightBox.GetCenter(), TestHeightBox.GetExtent(), FColor::Green, true, 10.f);
	}

	int32 NumSuccessedRays = 0;
	for (int32 x = -RCAST_Count; x <= RCAST_Count; x++)
	{
		float RayX = Position.X + RCASTDelta * float(x);
		for (int32 y = -RCAST_Count; y <= RCAST_Count; y++)
		{
			float RayY = Position.Y + RCASTDelta * float(y);
			FVector StartRay = FVector(RayX, RayY, Position.Z + RCAST_Depth);
			FVector EndRay = FVector(RayX, RayY, Position.Z - RCAST_Depth);
			StartRay.Z = FMath::RayPlaneIntersection(FVector3f(StartRay) - FVector3f(0, 0, RCAST_VALID * 0.01f), FVector3f(0, 0, -1), Result.Plane).Z+6.f;
			FVector EndCheckRay = StartRay + FVector(0,0,RCAST_Depth);
			FHitResult HitResult;
		
			if (InWorld->LineTraceSingleByChannel(HitResult, StartRay, EndRay, ECC_WorldStatic, CollisionQueryParams, FCollisionResponseParams(ECR_Block)))
			{
				if (FVector::Distance(HitResult.Location, StartRay) < RCAST_VALID)
				{
					NumSuccessedRays++;
				}
				
			}
		}
	}

	if (float(NumSuccessedRays) / float(RCAST_Total) < (bIgnoreConstraints ? 0.015f : 0.5f ))
	{
		return	false;
	}

	return true;
}

bool UStalkerEditorAIMap::CanTravel(UWorld* InWorld, const FVector& InFrom, const FVector& InTo, const FPlane4f InPlaneFrom, const FPlane4f InPlaneTo)
{
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(InWorld->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return false;
	}

	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetOrCreateAIMap();
	if (!StalkerAIMap)
	{
		return false;
	}

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.bTraceComplex = true;

	const float NodeSize = StalkerAIMap->NodeSize;
	const FVector3f From = FVector3f(FMath::Floor(InFrom.X / NodeSize) * NodeSize, FMath::Floor(InFrom.Y / NodeSize) * NodeSize, InFrom.Z);
	const FVector3f To = FVector3f(FMath::Floor(InTo.X / NodeSize) * NodeSize, FMath::Floor(InTo.Y / NodeSize) * NodeSize, InTo.Z);

	

	if (!FMath::IsNearlyEqual(FVector3f::DistXY(From, To),NodeSize,0.1f))
	{
		return false;
	}

	auto GetMaxZ = [NodeSize, StalkerAIMap](const FPlane4f Plane, const FVector& Position)
	{
		float MaxZ = FMath::RayPlaneIntersection(FVector3f(Position) - FVector3f(NodeSize * .5f, NodeSize * .5f, 0), FVector3f(0, 0, -1),Plane).Z;
		MaxZ = FMath::Max(FMath::RayPlaneIntersection(FVector3f(Position) - FVector3f(NodeSize * .5f, -NodeSize * .5f, 0), FVector3f(0, 0, -1),Plane).Z, MaxZ);
		MaxZ = FMath::Max(FMath::RayPlaneIntersection(FVector3f(Position) - FVector3f(-NodeSize * .5f, NodeSize * .5f, 0), FVector3f(0, 0, -1),Plane).Z, MaxZ);
		MaxZ = FMath::Max(FMath::RayPlaneIntersection(FVector3f(Position) - FVector3f(-NodeSize * .5f, -NodeSize * .5f, 0), FVector3f(0, 0, -1), Plane).Z, MaxZ);
		return MaxZ;
	};

	float MaxZ1 = GetMaxZ(InPlaneFrom, InFrom);
	float MaxZ2 = GetMaxZ(InPlaneTo, InTo);

	{
		float TestZFrom = FMath::RayPlaneIntersection((FVector3f(InFrom) + FVector3f(InTo)) / 2.f, FVector3f(0, 0, -1), InPlaneFrom).Z;
		float TestZTo = FMath::RayPlaneIntersection((FVector3f(InFrom) + FVector3f(InTo)) / 2.f, FVector3f(0, 0, -1), InPlaneTo).Z;
		if (TestZFrom > TestZTo)
		{
			if (StalkerWorldSettings->AIMapCanDOWN < TestZFrom - TestZTo)
			{
				return false;
			}
		}
		else
		{
			if (StalkerWorldSettings->AIMapCanUP < TestZTo - TestZFrom)
			{
				return false;
			}
		}

	}


	FVector3f BoxStart1 = (FVector3f(From.X, From.Y, MaxZ1 + 6.f) + FVector3f(To.X, To.Y, MaxZ1 + 6.f)) * 0.5;
	FVector3f BoxStart2 = (FVector3f(From.X, From.Y, MaxZ2 + 6.f) + FVector3f(To.X, To.Y, MaxZ2 + 6.f)) * 0.5;
	FBox TestBox1(ForceInit), TestBox2(ForceInit);
	if (FMath::Abs(FVector3f(1, 0, 0) | (FVector3f(From.X, From.Y, 0) - FVector3f(To.X, To.Y, 0))) > 0.9f)
	{
		TestBox1 += FVector(BoxStart1) + FVector(NodeSize*0.1f, NodeSize * 0.5f * 0.9f, 0);
		TestBox1 += FVector(BoxStart1) + FVector(-NodeSize * 0.1f, -NodeSize * 0.5f * 0.9f, 0);
		TestBox1 += FVector(BoxStart1) + FVector(0,0,FMath::Max( (MaxZ2> MaxZ1)?(MaxZ2- MaxZ1) :0 ,StalkerWorldSettings->AIMapTestHeight));

		TestBox2 += FVector(BoxStart2) + FVector(NodeSize * 0.1f, NodeSize * 0.5f * 0.9f, 0);
		TestBox2 += FVector(BoxStart2) + FVector(NodeSize * 0.1f, -NodeSize * 0.5f * 0.9f, 0);
		TestBox2 += FVector(BoxStart2) + FVector(0, 0, FMath::Max((MaxZ1 > MaxZ2) ? (MaxZ1 - MaxZ2) : 0, StalkerWorldSettings->AIMapTestHeight));
	}
	else
	{
		TestBox1 += FVector(BoxStart1) + FVector( NodeSize * 0.5f * 0.9f, NodeSize * 0.1f,  0);
		TestBox1 += FVector(BoxStart1) + FVector( -NodeSize * 0.5f * 0.9f, NodeSize * 0.1f, 0);
		TestBox1 += FVector(BoxStart1) + FVector(0, 0, FMath::Max((MaxZ2 > MaxZ1) ? (MaxZ2 - MaxZ1) : 0, StalkerWorldSettings->AIMapTestHeight));

		TestBox2 += FVector(BoxStart2) + FVector(NodeSize * 0.5f * 0.9f, NodeSize * 0.1f, 0);
		TestBox2 += FVector(BoxStart2) + FVector(-NodeSize * 0.5f * 0.9f, NodeSize * 0.1f, 0);
		TestBox2 += FVector(BoxStart2) + FVector(0, 0, FMath::Max((MaxZ1 > MaxZ2) ? (MaxZ1 - MaxZ2) : 0, StalkerWorldSettings->AIMapTestHeight));

	}

	FBox CheckBox(ForceInit);
	if (MaxZ2 > MaxZ1)
	{
		CheckBox += FVector(From.X - NodeSize * 0.5f * 0.9f, From.Y - NodeSize * 0.5f * 0.9f, MaxZ1 + 6.f);
		CheckBox += FVector(From.X + NodeSize * 0.5f * 0.9f, From.Y + NodeSize * 0.5f * 0.9f, MaxZ1 + 6.f + (MaxZ2 - MaxZ1) + StalkerWorldSettings->AIMapTestHeight);
	}
	else
	{
		CheckBox += FVector(To.X - NodeSize * 0.5f * 0.9f, To.Y - NodeSize * 0.5f * 0.9f, MaxZ2 + 6.f);
		CheckBox += FVector(To.X + NodeSize * 0.5f * 0.9f, To.Y + NodeSize * 0.5f * 0.9f, MaxZ2 + 6.f + (MaxZ1 - MaxZ2) + StalkerWorldSettings->AIMapTestHeight);
	}


	TArray<FHitResult> HitResults;
	InWorld->SweepMultiByChannel(HitResults, CheckBox.GetCenter(), CheckBox.GetCenter(), FQuat::Identity, ECC_WorldStatic, FCollisionShape::MakeBox(TestBox1.GetExtent()), CollisionQueryParams, FCollisionResponseParams(ECR_Block));
	for (FHitResult& HitResult : HitResults)
	{
		if (HitResult.bBlockingHit)
		{
			DrawDebugBox(InWorld, CheckBox.GetCenter(), CheckBox.GetExtent(), FColor::Red, true, 10.f);
			return false;
		}
	}
	//DrawDebugBox(InWorld, CheckBox.GetCenter(), CheckBox.GetExtent(), FColor::Green, true, 10.f);
	HitResults.Empty();

	InWorld->SweepMultiByChannel(HitResults, TestBox1.GetCenter(), TestBox1.GetCenter(), FQuat::Identity, ECC_WorldStatic, FCollisionShape::MakeBox(TestBox1.GetExtent()), CollisionQueryParams, FCollisionResponseParams(ECR_Block));
	bool bResult = true;
	for (FHitResult& HitResult : HitResults)
	{
		if (HitResult.bBlockingHit)
		{
			bResult = false;
			break;
		}
	}
	if (bResult)
	{
		//DrawDebugBox(InWorld, TestBox1.GetCenter(), TestBox1.GetExtent(), FColor::Green, true, 10.f);
		return true;
	}
	

	//DrawDebugBox(InWorld, TestBox2.GetCenter(), TestBox2.GetExtent(), FColor::Red, true, 10.f);
	HitResults.Empty();
	InWorld->SweepMultiByChannel(HitResults, TestBox2.GetCenter(), TestBox2.GetCenter(), FQuat::Identity, ECC_WorldStatic, FCollisionShape::MakeBox(TestBox2.GetExtent()), CollisionQueryParams, FCollisionResponseParams(ECR_Block));
	bResult = true;
	for (FHitResult& HitResult : HitResults)
	{
		if (HitResult.bBlockingHit)
		{
			bResult = false;
			break;
		}
	}
	if (bResult)
	{
		//DrawDebugBox(InWorld, TestBox2.GetCenter(), TestBox2.GetExtent(), FColor::Green, true, 10.f);
		return true;
	}
	DrawDebugBox(InWorld, TestBox2.GetCenter(), TestBox2.GetExtent(), FColor::Red, true, 10.f);
	return false;
}

struct FStalkerAIMapNode* UStalkerEditorAIMap::FindNeighbour(UWorld* InWorld, struct FStalkerAIMapNode* Node, int32 ID, bool bIgnoreConstraints)
{
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(InWorld->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return nullptr;
	}

	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetOrCreateAIMap();
	if (!StalkerAIMap)
	{
		return nullptr;
	}
	const float NodeSize = StalkerAIMap->NodeSize;
	FVector Position = FVector(Node->Position);
	if (ID == 0)
	{
		FStalkerAIMapNode* LeftNode = FindNode(InWorld,Position + FVector(NodeSize, 0, 0));
		if (LeftNode&& LeftNode != Node&&( bIgnoreConstraints||CanTravel(InWorld,FVector(Node->Position), FVector(LeftNode->Position), Node->Plane, LeftNode->Plane)))
		{
			return LeftNode;
		}
		return nullptr ;

	}
	else if (ID == 1)
	{
		FStalkerAIMapNode* ForwardNode = FindNode(InWorld, Position + FVector(0, NodeSize, 0));
		if (ForwardNode && ForwardNode != Node && (bIgnoreConstraints || CanTravel(InWorld, FVector(Node->Position), FVector(ForwardNode->Position), Node->Plane, ForwardNode->Plane)))
		{
			return ForwardNode;
		}
		return nullptr;
	}
	else if (ID == 2)
	{
		FStalkerAIMapNode* RightNode = FindNode(InWorld, Position + FVector(-NodeSize, 0, 0));
		if (RightNode && RightNode != Node && (bIgnoreConstraints || CanTravel(InWorld, FVector(Node->Position), FVector(RightNode->Position), Node->Plane, RightNode->Plane)))
		{
			return RightNode;
		}
	}
	else if (ID == 3)
	{
		FStalkerAIMapNode* BackwardNode = FindNode(InWorld, Position + FVector(0, -NodeSize, 0));
		if (BackwardNode && BackwardNode != Node && (bIgnoreConstraints || CanTravel(InWorld, FVector(Node->Position), FVector(BackwardNode->Position), Node->Plane, BackwardNode->Plane)))
		{
			return BackwardNode;
		}
	}
	return nullptr;
}

struct FStalkerAIMapNode* UStalkerEditorAIMap::FindNode(UWorld* InWorld, const FVector& InPosition)
{
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(InWorld->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return nullptr;
	}

	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetOrCreateAIMap();
	if (!StalkerAIMap)
	{
		return nullptr;
	}
	const float NodeSize = StalkerAIMap->NodeSize;
	TArray<FStalkerAIMapNode*>* NodesHashList = StalkerAIMap->GetEditorHashMap(FVector3f(InPosition));
	if (NodesHashList)
	{
		FVector3f Position = FVector3f(FMath::Floor(InPosition.X / StalkerAIMap->NodeSize) * NodeSize, FMath::Floor(InPosition.Y / StalkerAIMap->NodeSize) * NodeSize, InPosition.Z);
		FStalkerAIMapNode* Result = nullptr;
		for (FStalkerAIMapNode* Node : *NodesHashList)
		{
			if (FMath::IsNearlyEqual(Node->Position.X, Position.X, 1.f) &&
				FMath::IsNearlyEqual(Node->Position.Y, Position.Y, 1.f))
			{

				if (Position.Z > Node->Position.Z)
				{
					if (Position.Z - Node->Position.Z > StalkerWorldSettings->AIMapCanDOWN)
						continue;
				}
				else
				{
					if (Node->Position.Z - Position.Z > StalkerWorldSettings->AIMapCanUP)
						continue;
				}
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

void UStalkerEditorAIMap::AutoLink(UWorld* InWorld, struct FStalkerAIMapNode* Node, bool bIgnoreConstraints )
{
	for (int32 i = 0; i < 4; i++)
	{
		if(!Node->Nodes[i])
			Node->Nodes[i] = FindNeighbour(InWorld,Node,i, bIgnoreConstraints);
	
	}
	if (Node->NodeLeft&& !Node->NodeLeft->NodeRight)
	{
		Node->NodeLeft->NodeRight = Node;
	}
	if (Node->NodeRight && !Node->NodeRight->NodeLeft)
	{
		Node->NodeRight->NodeLeft = Node;
	}
	if (Node->NodeForward && !Node->NodeForward->NodeBackward)
	{
		Node->NodeForward->NodeBackward = Node;
	}
	if (Node->NodeBackward && !Node->NodeBackward->NodeForward)
	{
		Node->NodeBackward->NodeForward = Node;
	}
}

void UStalkerEditorAIMap::Generate(UWorld* InWorld, bool bSelectedOnly /*= false*/)
{
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(InWorld->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return;
	}

	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetOrCreateAIMap();
	if (!StalkerAIMap)
	{
		return;
	}

	StalkerAIMap->NeedRebuild = true;
	GenerateAABB = FBox3f(ForceInit);
	GenerateAABBs.Reset();
	for (TActorIterator<AStalkerAIMapBoundsVolume> It(InWorld); It; ++It)
	{
		AStalkerAIMapBoundsVolume * V = (*It);
		if (IsValid(V))
		{
			GenerateAABBs.Add(FBox3f( V->GetComponentsBoundingBox(true)));
			GenerateAABB += FBox3f(GenerateAABBs.Last());
		}
	}
	if (GenerateAABBs.Num() == 0)
	{
		return;
	}
	StalkerAIMap->HashFill(GenerateAABB);

	for (int32 i = 0; i < StalkerAIMap->Nodes.Num(); i++)
	{

		if (GenerateAABB.IsInsideOrOn(StalkerAIMap->Nodes[i]->Position))
		{
			if (GenerateAABBs.Num() > 1)
			{
				bool bCheck = false;
				for (FBox3f& Box : GenerateAABBs)
				{
					bCheck|=Box.IsInsideOrOn(StalkerAIMap->Nodes[i]->Position);
				}
				if (!bCheck)
				{
					continue;
				}
			}
		}
		else
		{
			continue;
		}
		if (!bSelectedOnly || EnumHasAnyFlags(StalkerAIMap->Nodes[i]->Flags,EStalkerAIMapNodeFlags::Selected))
		{
			for (int32 Link = 0; Link < 4; Link++)
			{
				if (!StalkerAIMap->Nodes[i]->Nodes[Link])
				{
					StalkerAIMap->Nodes[i]->Nodes[Link] = BuildNode(InWorld, StalkerAIMap->Nodes[i], Link, bSelectedOnly);
				}
			}
		}
		
	}
}

struct FStalkerAIMapNode* UStalkerEditorAIMap::BuildNode(UWorld* InWorld, struct FStalkerAIMapNode* Node, int32 ID, bool bSelectedOnly)
{
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(InWorld->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return nullptr;
	}

	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetOrCreateAIMap();
	if (!StalkerAIMap)
	{
		return nullptr;
	}
	FVector ToPosition;
	
	if (ID == 0)
	{
		ToPosition = FVector(Node->Position) + FVector(StalkerAIMap->NodeSize, 0, 0);
	}
	else if (ID == 1)
	{
		ToPosition = FVector(Node->Position) + FVector(0, StalkerAIMap->NodeSize, 0);
	}
	else if (ID == 2)
	{

		ToPosition = FVector(Node->Position) + FVector(-StalkerAIMap->NodeSize, 0, 0);
	}
	else if (ID == 3)
	{
		ToPosition = FVector(Node->Position) + FVector(0, -StalkerAIMap->NodeSize, 0);
	}

	if (GenerateAABB.IsInsideOrOn(FVector3f(ToPosition)))
	{
		if (GenerateAABBs.Num() > 1)
		{
			bool bCheck = false;
			for (FBox3f& Box : GenerateAABBs)
			{
				bCheck |= Box.IsInsideOrOn(FVector3f(ToPosition));
			}
			if (!bCheck)
			{
				return nullptr;
			}
		}
	}
	else
	{
		return nullptr;
	}
	FStalkerAIMapNode TempNode;
	if (!CreateNode(TempNode, InWorld, ToPosition))
	{
		return nullptr;
	}
	if (!CanTravel(InWorld, FVector(Node->Position), FVector(TempNode.Position), Node->Plane, TempNode.Plane))
	{
		return nullptr;
	}
	FStalkerAIMapNode* Result = StalkerAIMap->FindNode(TempNode.Position,1);
	if (!Result)
	{
		Result = StalkerAIMap->FindOrCreateNode(TempNode.Position,1,true,false);
		Result->Plane = TempNode.Plane;
		Result->Flags |= EStalkerAIMapNodeFlags::Selected;
		AutoLink(InWorld,Result);
	}
	if (ID == 0&& !Result->NodeRight)
	{
		Result->NodeRight = Node;
	}
	else if (ID == 1 && !Result->NodeBackward)
	{
		Result->NodeBackward = Node;
	}
	else if (ID == 2 && !Result->NodeLeft)
	{
		Result->NodeLeft = Node;
	}
	else if (ID == 3&&!Result->NodeForward)
	{
		Result->NodeForward = Node;
	}
	return Result;
}

void UStalkerEditorAIMap::Smooth(UWorld* InWorld, bool bSelectedOnly /*= false*/)
{
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(InWorld->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return;
	}

	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetOrCreateAIMap();
	if (!StalkerAIMap)
	{
		return;
	}

	StalkerAIMap->NeedRebuild = true;
	const float NodeSize = StalkerAIMap->NodeSize;


	auto PointLF = [NodeSize](FStalkerAIMapNode&Node)
	{
		FVector3f	Ray(0,0, -1);
		FVector3f	Position =Node.Position;;
		Position.X += NodeSize / 2;
		Position.Y += NodeSize / 2;
		return FMath::RayPlaneIntersection(Position,Ray,Node.Plane);
	};
	auto PointRF = [NodeSize](FStalkerAIMapNode& Node)
	{
		FVector3f	Ray(0, 0, -1);
		FVector3f	Position = Node.Position;;
		Position.X -= NodeSize / 2;
		Position.Y += NodeSize / 2;
		return FMath::RayPlaneIntersection(Position, Ray, Node.Plane);
	};
	auto PointRB = [NodeSize](FStalkerAIMapNode& Node)
	{
		FVector3f	Ray(0, 0, -1);
		FVector3f	Position = Node.Position;;
		Position.X -= NodeSize / 2;
		Position.Y -= NodeSize / 2;
		return FMath::RayPlaneIntersection(Position, Ray, Node.Plane);
	};
	auto PointLB = [NodeSize](FStalkerAIMapNode& Node)
	{
		FVector3f	Ray(0, 0, -1);
		FVector3f	Position = Node.Position;;
		Position.X += NodeSize / 2;
		Position.Y -= NodeSize / 2;
		return FMath::RayPlaneIntersection(Position, Ray, Node.Plane);
	};
	auto Merge = [NodeSize](int32&Count,FVector3f&To,const FVector3f&In)
	{
		if (!FMath::IsNearlyZero(To.Y, In.Y))
		{
			To+=In; Count++;

		}
	};

	TArray<FStalkerAIMapNode*> NewNodes;
	NewNodes.Empty(StalkerAIMap->Nodes.Num());
	for (int32 i = 0; i < StalkerAIMap->Nodes.Num(); i++)
	{
		NewNodes.Add(new FStalkerAIMapNode);
		NewNodes.Last()->Index = i;
	}
	StalkerAIMap->CalculateIndex();
	for (int32 i = 0; i < StalkerAIMap->Nodes.Num(); i++)
	{
		
		FStalkerAIMapNode&MainNode = *StalkerAIMap->Nodes[i];
		NewNodes[i]->Plane = MainNode.Plane;
		NewNodes[i]->Position = MainNode.Position;
		for (int32 Link = 0; Link < 4; Link++)
		{
			NewNodes[i]->Nodes[Link] = MainNode.Nodes[Link] ? NewNodes[MainNode.Nodes[Link]->Index]: nullptr;
		}
		if (!bSelectedOnly || EnumHasAnyFlags(StalkerAIMap->Nodes[i]->Flags, EStalkerAIMapNodeFlags::Selected))
		{

			FVector3f Point1,Point2,Point3,Point4;
			// smooth point LF
			{
				bool	bCorner = false;
				int32	Counter = 1;
				Point1 = PointLF(MainNode);
				if (MainNode.NodeLeft)
				{
					Merge(Counter, Point1,PointRF(*MainNode.NodeLeft));
					if (MainNode.NodeLeft->NodeForward)
					{
						bCorner = true;
						Merge(Counter, Point1, PointRB(*MainNode.NodeLeft->NodeForward));
					}
				}
				if (MainNode.NodeForward)
				{
					Merge(Counter, Point1, PointLB(*MainNode.NodeForward));
					if (!bCorner && MainNode.NodeForward->NodeLeft)
					{
						bCorner = true;
						Merge(Counter, Point1, PointRB(*MainNode.NodeForward->NodeLeft));
					}
				}
				check(Counter <= 4);
				Point1/=Counter;
			}

			// smooth point RF
			{
				bool	bCorner = false;
				int32	Counter = 1;
				Point2 = PointRF(MainNode);
				if (MainNode.NodeForward)
				{
					Merge(Counter, Point2, PointRB(*MainNode.NodeForward));
					if (MainNode.NodeForward->NodeRight)
					{
						bCorner = true;
						Merge(Counter, Point2, PointLB(*MainNode.NodeForward->NodeRight));
					}
				}
				if (MainNode.NodeRight)
				{
					Merge(Counter, Point2, PointLF(*MainNode.NodeRight));
					if (!bCorner && MainNode.NodeRight->NodeForward)
					{
						bCorner = true;
						Merge(Counter, Point2, PointLB(*MainNode.NodeRight->NodeForward));
					}
				}
				check(Counter <= 4);
				Point2 /= Counter;
			}

			// smooth point RB
			{
				bool	bCorner = false;
				int32	Counter = 1;
				Point3 = PointRB(MainNode);
				if (MainNode.NodeRight)
				{
					Merge(Counter, Point3, PointLB(*MainNode.NodeRight));
					if (MainNode.NodeRight->NodeBackward)
					{
						bCorner = true;
						Merge(Counter, Point3, PointLF(*MainNode.NodeRight->NodeBackward));
					}
				}
				if (MainNode.NodeBackward)
				{
					Merge(Counter, Point3, PointRF(*MainNode.NodeBackward));
					if (!bCorner && MainNode.NodeBackward->NodeRight)
					{
						bCorner = true;
						Merge(Counter, Point3, PointLF(*MainNode.NodeBackward->NodeRight));
					}
				}
				check(Counter <= 4);
				Point3 /= Counter;
			}

			// smooth point BL
			{
				bool	bCorner = false;
				int32	Counter = 1;
				Point4 = PointLB(MainNode);
				if (MainNode.NodeBackward)
				{
					Merge(Counter, Point4, PointLF(*MainNode.NodeBackward));
					if (MainNode.NodeBackward->NodeLeft)
					{
						bCorner = true;
						Merge(Counter, Point4, PointRF(*MainNode.NodeBackward->NodeLeft));
					}
				}
				if (MainNode.NodeLeft)
				{
					Merge(Counter, Point4, PointRB(*MainNode.NodeLeft));
					if (!bCorner && MainNode.NodeLeft->NodeBackward)
					{
						bCorner = true;
						Merge(Counter, Point4, PointRF(*MainNode.NodeLeft->NodeBackward));
					}
				}
				check(Counter <= 4);
				Point4 /= Counter;
			}	
			FPlane4f Plane1(Point1, Point2, Point3);
			FPlane4f Plane2(Point1, Point4, Point3);
			FPlane4f Plane3(Point1, Point2, Point4);
			FPlane4f Plane4(Point2, Point4, Point3);
			if (Plane1.Z < 0)
			{
				Plane1 = Plane1.Flip();
			}
			if (Plane2.Z < 0)
			{
				Plane2 = Plane2.Flip();
			}
			if (Plane3.Z < 0)
			{
				Plane3 = Plane3.Flip();
			}
			if (Plane4.Z < 0)
			{
				Plane4 = Plane4.Flip();
			}

			FVector3f Normal = Plane1.GetNormal()+ Plane2.GetNormal() + Plane3.GetNormal() + Plane4.GetNormal();
			Normal.Normalize();
			FVector3f Position = (Point1+ Point2+Point3+Point4)/4.f;
			Position.X = MainNode.Position.X;
			Position.Y = MainNode.Position.Y;
			NewNodes[i]->Position = Position;
			NewNodes[i]->Plane = FPlane4f(Position,Normal);
		}
	}
	StalkerAIMap->ClearAIMap();
	StalkerAIMap->Nodes = NewNodes;
	NewNodes.Empty();
	StalkerAIMap->HashFill();
}

void UStalkerEditorAIMap::Reset(UWorld* InWorld, bool bSelectedOnly /*= false*/)
{
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(InWorld->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return;
	}

	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetOrCreateAIMap();
	if (!StalkerAIMap)
	{
		return;
	}
	for (int32 i = 0; i < StalkerAIMap->Nodes.Num(); i++)
	{
		if (!bSelectedOnly || EnumHasAnyFlags(StalkerAIMap->Nodes[i]->Flags, EStalkerAIMapNodeFlags::Selected))
		{
			StalkerAIMap->Nodes[i]->Plane = FPlane4f(StalkerAIMap->Nodes[i]->Position,FVector3f(0,0,1));
		}
	}
}



void UStalkerEditorAIMap::Build()
{
	if (FApp::IsGame())
	{
		return;
	}
	constexpr float high_cover_height = 1.5f;
	constexpr float low_cover_height = 0.6f;
	FWorldContext* WorldContext = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
	if (!WorldContext)
		return;

	UWorld* World = WorldContext->World();
	if (!IsValid(World))
		return;

	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return;
	}
	UStalkerAIMap* AIMap = StalkerWorldSettings->GetOrCreateAIMap();
	if (!AIMap)
	{
		return;
	}
	const float NodeSize = AIMap->NodeSize/100.f;
	auto CNodePositionCompressor = [NodeSize](NodePosition & Pdest,const Fvector & Psrc, hdrNODES & H)
	{
		float sp = 1 / NodeSize;
		int row_length = iFloor((H.aabb.max.z - H.aabb.min.z) / H.size + EPS_L + 1.5f);
		int pxz = iFloor((Psrc.x - H.aabb.min.x) * sp + EPS_L + .5f) * row_length + iFloor((Psrc.z - H.aabb.min.z) * sp + EPS_L + .5f);
		int py = iFloor(65535.f * (Psrc.y - H.aabb.min.y) / (H.size_y) + EPS_L);
		VERIFY(pxz < (1 << MAX_NODE_BIT_COUNT) - 1);
		Pdest.xz(pxz);
		clamp(py, 0, 65535);	Pdest.y(u16(py));
	};
	auto CompressNode  = [](NodeCompressed & Dest, FStalkerAIMapNode* Src)
	{
		constexpr int32 StalkerInvalidNode = (1 << 24) - 1;
		Dest.light(15);
		for (u8 L = 0; L < 4; ++L)
			Dest.link(L, Src->Nodes[L] ? Src->Nodes[L]->Index : StalkerInvalidNode);
	};
	auto CompressCover = [](float c, int max_value)
	{
		int	cover = iFloor(c * float(max_value) + .5f);
		clamp(cover, 0, max_value);
		return BYTE(cover);
	};
	auto	Compress = [low_cover_height, high_cover_height, CompressCover, CompressNode, CNodePositionCompressor](NodeCompressed & Dest, FStalkerAIMapNode * Src, hdrNODES & H)
	{
		Dest.plane = pvCompress(StalkerMath::UnrealNormalToRBMK(Src->Plane.GetNormal()));
		CNodePositionCompressor(Dest.p, StalkerMath::UnrealLocationToRBMK(Src->Position), H);
		CompressNode(Dest, Src);
		Dest.high.cover0 = CompressCover(high_cover_height, 15);
		Dest.high.cover1 = CompressCover(high_cover_height, 15);
		Dest.high.cover2 = CompressCover(high_cover_height, 15);
		Dest.high.cover3 = CompressCover(high_cover_height, 15);
		Dest.low.cover0 = CompressCover(low_cover_height, 15);
		Dest.low.cover1 = CompressCover(low_cover_height, 15);
		Dest.low.cover2 = CompressCover(low_cover_height, 15);
		Dest.low.cover3 = CompressCover(low_cover_height, 15);
	};
	auto CNodeRenumberer = [](
		TArray<ILevelGraph::CVertex>& nodes,
		xr_vector<u32>&sorted,
		xr_vector<u32>&renumbering
	)
	{
		u32					N = (u32)nodes.Num();
		sorted.resize(N);
		renumbering.resize(N);

		for (u32 i = 0; i < N; ++i)
			sorted[i] = i;

		std::sort(sorted.begin(), sorted.end(), [&nodes](u32 vertex_id0, u32 vertex_id1)
		{
				if(nodes[vertex_id0].p.xz() != nodes[vertex_id1].p.xz())
					return		(nodes[vertex_id0].p.xz() < nodes[vertex_id1].p.xz());
				return nodes[vertex_id0].p.y() < nodes[vertex_id1].p.y();
		});

		for (u32 i = 0; i < N; ++i)
			renumbering[sorted[i]] = i;

		for (u32 i = 0; i < N; ++i) {
			for (u32 j = 0; j < 4; ++j) {
				u32			vertex_id = nodes[i].link(u8(j));
				if (vertex_id >= N)
					continue;
				nodes[i].link(u8(j), renumbering[vertex_id]);
			}
		}
		nodes.Sort( [](const NodeCompressed& vertex0, const NodeCompressed& vertex1)
			{
				if(vertex0.p.xz() != vertex1.p.xz())
					return		(vertex0.p.xz() < vertex1.p.xz());
				return vertex0.p.y() < vertex1.p.y();
			});
	};


	AIMap->InvalidAIMap();
	UStalkerLevelSpawn *LevelSpawn =  StalkerWorldSettings->GetSpawn();
	if (LevelSpawn)
	{
		LevelSpawn->NeedRebuild = true;
		LevelSpawn->Modify();
	}
	if (AIMap->Nodes.Num() == 0)
	{
		UE_LOG(LogStalkerEditor,Warning,TEXT("AIMap [%s] is empty !"), *AIMap->GetPathName())
		return;
	}
	AIMap->NeedRebuild = false;
	AIMap->HashFill();
	AIMap->CalculateIndex();
	AIMap->AIMapGuid = FGuid::NewGuid();
	auto CalculateAABB = [&AIMap](Fbox& BB)->float
	{
		BB.invalidate();
		for (int32 i = 0; i < AIMap->Nodes.Num(); i++)
		{
			BB.modify(StalkerMath::UnrealLocationToRBMK(AIMap->Nodes[i]->Position));
		}
		return BB.max.y - BB.min.y + EPS_L;
	};
	static_cast<hdrNODES&>(AIMap->LevelGraphHeader).version = XRAI_CURRENT_VERSION;
	static_cast<hdrNODES&>(AIMap->LevelGraphHeader).count = AIMap->Nodes.Num();
	static_cast<hdrNODES&>(AIMap->LevelGraphHeader).size = NodeSize;
	static_cast<hdrNODES&>(AIMap->LevelGraphHeader).size_y = CalculateAABB(static_cast<hdrNODES&>(AIMap->LevelGraphHeader).aabb) + EPS_L;
	FMemory::Memcpy(&static_cast<hdrNODES&>(AIMap->LevelGraphHeader).guid, &AIMap->AIMapGuid,sizeof(FGuid));
	AIMap->LevelGraphVertices.Empty(AIMap->Nodes.Num());

	for (FStalkerAIMapNode* Node : AIMap->Nodes)
	{
		ILevelGraph::CVertex	NC;
		Compress(NC, Node, static_cast<hdrNODES&>(AIMap->LevelGraphHeader));
		AIMap->LevelGraphVertices.Add(NC);
	}
	xr_vector<u32>	sorted;
	xr_vector<u32>	renumbering;
	CNodeRenumberer(AIMap->LevelGraphVertices, sorted, renumbering);
	AIMap->RefreshAIMapMetadata();
	AIMap->Modify();
}

void UStalkerEditorAIMap::BuildIfNeeded()
{
	FWorldContext* WorldContext = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
	if (!WorldContext)
		return;

	UWorld* World = WorldContext->World();
	if (!IsValid(World))
		return;

	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return;
	}
	UStalkerAIMap* AIMap = StalkerWorldSettings->GetOrCreateAIMap();
	if (!AIMap)
	{
		return;
	}
	if (AIMap->NeedRebuild || !AIMap->AIMapGuid.IsValid())
	{
		Build();
	}
	else
	{
		UE_LOG(LogStalkerEditor, Log, TEXT("This AIMap[%s] not needed rebuild!"), *AIMap->GetPathName());
	}
}
