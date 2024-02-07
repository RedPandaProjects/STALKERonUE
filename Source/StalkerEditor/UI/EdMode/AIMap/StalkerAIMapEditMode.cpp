// Fill out your copyright notice in the Description page of Project Settings.


#include "StalkerAIMapEditMode.h"

#include "StalkerAIMapEditorCommands.h"
#include "StalkerAIMapEdModeToolkit.h"
#include "EditorViewportClient.h"
#include "Kernel/Unreal/WorldSettings/StalkerWorldSettings.h"
#include "Resources/AIMap/StalkerAIMap.h"
#include "StalkerAIMapNodeProxy.h"
#include "LevelEditorViewport.h"
#include "../../../StalkerEditorManager.h"
#include "../../../Managers/AIMap/StalkerEditorAIMap.h"

const FEditorModeID FStalkerAIMapEditMode::EM_AIMap(TEXT("EM_AIMap"));


FStalkerAIMapEditMode::FStalkerAIMapEditMode()
{
	FStalkerAIMapEditorCommands::Register();
	bIsAddMode = false;
	bIgnoreConstraints = false;
	bAutoLink = true;
	AIMapEdModeActions = MakeShareable(new FUICommandList);
	LinkMode = LM_Add;
}

FStalkerAIMapEditMode::~FStalkerAIMapEditMode()
{
	FStalkerAIMapEditorCommands::Unregister();
}

void FStalkerAIMapEditMode::Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return;
	}
	
	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetAIMap();
	if (!StalkerAIMap)
	{
		return;
	}
	const float NodeSize = (StalkerAIMap->NodeSize * 0.9f) * 0.5f;
	const double DistanceRenderAIMap = (StalkerWorldSettings->DistanceRenderAIMap) * 0.5f;
	auto NodeToUV = [](int32 Node)
	{
		return FVector2f(static_cast<float>(Node % 4) * 0.25f, static_cast<float>(Node / 4) * 0.25f);
	};
	int32 CountNode =0;
	auto DrawNode = [View, Viewport, PDI, this, NodeToUV, StalkerWorldSettings, NodeSize, &CountNode](FDynamicMeshBuilder&MeshBuilder,FPlane4f&NodePlane,int32 Node, float x, float y, float z, bool IsSelected)
	{ 
		const FVector3f PlaneNormalRender = FVector3f(0,0,1);
		FVector3f Vertex1 = FMath::RayPlaneIntersection(FVector3f(x - NodeSize, y - NodeSize, z), PlaneNormalRender, NodePlane);
		FVector3f Vertex2 = FMath::RayPlaneIntersection(FVector3f(x - NodeSize, y + NodeSize, z), PlaneNormalRender, NodePlane);
		FVector3f Vertex3 = FMath::RayPlaneIntersection(FVector3f(x + NodeSize, y - NodeSize, z), PlaneNormalRender, NodePlane);
		FVector3f Vertex4 = FMath::RayPlaneIntersection(FVector3f(x + NodeSize, y + NodeSize, z), PlaneNormalRender, NodePlane);
		Vertex1.Z += 4.f;
		Vertex2.Z += 4.f;
		Vertex3.Z += 4.f;
		Vertex4.Z += 4.f;
		MeshBuilder.AddVertex(FDynamicMeshVertex(Vertex1, FVector3f(1, 0, 0), FVector3f(0, 1, 0), FVector2f(0, 0) + NodeToUV(Node), IsSelected ? FColor::White : FColor(127, 127, 127, 255)));
		MeshBuilder.AddVertex(FDynamicMeshVertex(Vertex2, FVector3f(1, 0, 0), FVector3f(0, 1, 0), FVector2f(0, 0.25f) + NodeToUV(Node), IsSelected ? FColor::White : FColor(127, 127, 127, 255)));
		MeshBuilder.AddVertex(FDynamicMeshVertex(Vertex3, FVector3f(1, 0, 0), FVector3f(0, 1, 0), FVector2f(0.25f, 0) + NodeToUV(Node), IsSelected ? FColor::White : FColor(127, 127, 127, 255)));
		MeshBuilder.AddVertex(FDynamicMeshVertex(Vertex4, FVector3f(1, 0, 0), FVector3f(0, 1, 0), FVector2f(0.25f, 0.25f) + NodeToUV(Node), IsSelected ? FColor::White : FColor(127, 127, 127, 255)));
		MeshBuilder.AddTriangle(0+ CountNode*4, 1 + CountNode * 4, 2 + CountNode * 4);
		MeshBuilder.AddTriangle(1 + CountNode * 4, 3 + CountNode * 4, 2 + CountNode * 4);
		CountNode++;
	};

	FDynamicMeshBuilder MeshBuilder(View->GetFeatureLevel());
	MeshBuilder.ReserveTriangles(70 * 70 * 2);
	MeshBuilder.ReserveVertices(70 * 70 * 4);
	FMeshBatch Mesh;
	for (FStalkerAIMapNode* Node : StalkerAIMap->Nodes)
	{
		int32 NodeLink = 0;
		if (Node->NodeLeft)
		{
			NodeLink |= 1 << 2;
		}
		if (Node->NodeForward)
		{
			NodeLink |= 1 << 3;
		}
		if (Node->NodeRight)
		{
			NodeLink |= 1 << 0;
		}
		if (Node->NodeBackward)
		{
			NodeLink |= 1 << 1;
		}
		
		
		if (FVector::DistXY(View->ViewLocation,FVector(Node->Position)) < DistanceRenderAIMap * 100.0)
		{
			DrawNode(MeshBuilder, Node->Plane, NodeLink, Node->Position.X, Node->Position.Y, Node->Position.Z, EnumHasAnyFlags(Node->Flags, EStalkerAIMapNodeFlags::Selected));
		}
		
	}

	PDI->SetHitProxy(new HStalkerAIMapNodeProxy(nullptr));
	MeshBuilder.Draw(PDI, FMatrix::Identity, StalkerWorldSettings->EditorMaterialAIMap->GetRenderProxy(), SDPG_World, true,false);
	PDI->SetHitProxy(nullptr);

	FEdMode::Render(View, Viewport, PDI);
}

bool FStalkerAIMapEditMode::HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click)
{
	if (!IsEnabled())
	{
		return false;
	}
	UWorld* World = GetWorld();
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return false;
	}
	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetOrCreateAIMap();
	if (!StalkerAIMap)
	{ 
		return false;
	}

	bool bIsSelected = StalkerAIMap->HasSelected();

	const double DistanceRenderAIMap = (StalkerWorldSettings->DistanceRenderAIMap) * 0.5f;

	const float NodeSize = (StalkerAIMap->NodeSize) * 0.5f;

	auto GetBox = [NodeSize](FStalkerAIMapNode* Node)
	{
		FBox AABB = FBox(ForceInit);
		const FVector3f PlaneNormalRender = FVector3f(0, 0, 1);
		FVector3f Vertex1 = FMath::RayPlaneIntersection(Node->Position + FVector3f(-NodeSize, -NodeSize, -2.f), PlaneNormalRender, Node->Plane);
		FVector3f Vertex2 = FMath::RayPlaneIntersection(Node->Position + FVector3f(-NodeSize, +NodeSize, 2.f), PlaneNormalRender, Node->Plane);
		FVector3f Vertex3 = FMath::RayPlaneIntersection(Node->Position + FVector3f(+NodeSize, -NodeSize, -2.f), PlaneNormalRender, Node->Plane);
		FVector3f Vertex4 = FMath::RayPlaneIntersection(Node->Position + FVector3f(+NodeSize, +NodeSize, 2.f), PlaneNormalRender, Node->Plane);
		AABB += FVector(Vertex1);
		AABB += FVector(Vertex2);
		AABB += FVector(Vertex3);
		AABB += FVector(Vertex4);
		return AABB;
	};


	FStalkerAIMapNode*ClickNode = nullptr;
	if (HitProxy && HitProxy->IsA(HStalkerAIMapNodeProxy::StaticGetType()))
	{
		ClickNode = StalkerAIMap->FindNode(FVector3f(GEditor->ClickLocation+FVector(StalkerAIMap->NodeSize*0.5f, StalkerAIMap->NodeSize * 0.5f,0)),50.f);
		if (false)
		{
			FVector ResultLocation, ResultNormal; float HitTime;
			FVector EndRay = Click.GetOrigin() + Click.GetDirection() * WORLD_MAX;
			
			TArray<FStalkerAIMapNode*>* Nodes = StalkerAIMap->GetEditorHashMap(FVector3f(GEditor->ClickLocation + FVector(StalkerAIMap->NodeSize * 0.5f, StalkerAIMap->NodeSize * 0.5f, 0)));
			if (Nodes)
			{
				for (FStalkerAIMapNode* Node : *Nodes)
				{
					if (FMath::LineSphereIntersection(Click.GetOrigin(), Click.GetDirection(), WORLD_MAX, FVector(Node->Position), double(NodeSize)))
					{
						if (FMath::LineExtentBoxIntersection(GetBox(Node), Click.GetOrigin(), EndRay, FVector(0, 0, 0), ResultLocation, ResultNormal, HitTime))
						{
							EndRay = ResultLocation;
							ClickNode = Node;
						}
					}
				}
			}
			
		}
	}
	if ((!bIsAddMode && Click.GetKey() == EKeys::LeftMouseButton) || (!bIsSelected  && !Click.IsControlDown() && Click.GetKey() == EKeys::RightMouseButton))
	{
		if (ClickNode)
		{
			if (!Click.IsControlDown())
			{
				 ClearSelectionNodes();
			}
			if (EnumHasAnyFlags(ClickNode->Flags, EStalkerAIMapNodeFlags::Selected))
			{
				StalkerAIMap->UnSelectNode(ClickNode);
			}
			else
			{
				StalkerAIMap->SelectNode(ClickNode);
			}
			if (bIsAddMode && Click.GetKey() == EKeys::LeftMouseButton)
			{
				return true;
			}
		}
		else
		{
			if (!Click.IsControlDown())
			{
				ClearSelectionNodes();
			}
		}

	}

	if (bIsAddMode && Click.GetKey() == EKeys::LeftMouseButton)
	{
		ClearSelectionNodes();
	
		{
		
			FStalkerAIMapNode TempNode;
			if (GStalkerEditorManager->EditorAIMap->CreateNode(TempNode, World, FVector(GEditor->ClickLocation + FVector(StalkerAIMap->NodeSize * 0.5f, StalkerAIMap->NodeSize * 0.5f, 0)), bIgnoreConstraints))
			{
				int32 Index = GEditor->BeginTransaction(FText::FromString(TEXT("Add AI Map Node")));
				StalkerAIMap->PreEditChange(nullptr);
				FStalkerAIMapNode* NewNode = StalkerAIMap->FindNode(TempNode.Position, 1.f);
				if (!NewNode)
				{
					NewNode = StalkerAIMap->FindOrCreateNode(TempNode.Position, 1.f,false,false);
					check(NewNode);
					StalkerAIMap->MarkPackageDirty();
					NewNode->Plane = TempNode.Plane;
					if(bAutoLink)GStalkerEditorManager->EditorAIMap->AutoLink(World, NewNode,bIgnoreConstraints);
					StalkerAIMap->PostEditChange();
					GEditor->EndTransaction();
				}
				else
				{
					StalkerAIMap->PostEditChange();
					StalkerAIMap->MarkPackageDirty();
					if (Index != INDEX_NONE)
					{
						GEditor->CancelTransaction(Index);
					}
					
				}
			}
			
		
		}
		return true;
	}
	bIsSelected = StalkerAIMap->HasSelected();
	if (bIsSelected &&Click.GetKey() == EKeys::RightMouseButton)
	{
		FMenuBuilder MenuBuilder(true, NULL);
		MenuBuilder.PushCommandList(AIMapEdModeActions.ToSharedRef());
		MenuBuilder.BeginSection("AIMap Section");
		MenuBuilder.AddMenuEntry(FStalkerAIMapEditorCommands::Get().DeleteNode);
		MenuBuilder.EndSection();
		MenuBuilder.PopCommandList();
		FSlateApplication::Get().PushMenu(
			Owner->GetToolkitHost()->GetParentWidget(),
			FWidgetPath(),
			MenuBuilder.MakeWidget(),
			FSlateApplication::Get().GetCursorPos(),
			FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));
		return true;

	}

	return true;
}

void FStalkerAIMapEditMode::Enter()
{
	ClearSelectionNodes();
	FEdMode::Enter();
	if (!Toolkit.IsValid())
	{
		Toolkit = MakeShareable(new FStalkerAIMapEdModeToolkit);
		Toolkit->Init(Owner->GetToolkitHost());
	}
	{
		const auto& Commands = FStalkerAIMapEditorCommands::Get();
		AIMapEdModeActions->MapAction(Commands.DeleteNode,FExecuteAction::CreateSP(this, &FStalkerAIMapEditMode::RemoveSelectNodes),FCanExecuteAction::CreateSP(this, &FStalkerAIMapEditMode::CanRemoveSelectNodes));
	}
}



void FStalkerAIMapEditMode::Exit()
{
	FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
	Toolkit.Reset();
	FEdMode::Exit();

}

bool FStalkerAIMapEditMode::IsAddMode()
{
	return bIsAddMode;
}

void FStalkerAIMapEditMode::SetAddMode()
{
	ClearSelectionNodes();
	bIsAddMode = true;
}

void FStalkerAIMapEditMode::SetSelectionMode()
{
	bIsAddMode = false;
}

void FStalkerAIMapEditMode::ClearAIMap()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return;
	}

	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetAIMap();
	if (!StalkerAIMap)
	{
		return;
	}
	if (FMessageDialog::Open(EAppMsgType::Type::YesNo, EAppReturnType::Type::No, FText::FromString(TEXT("Are you sure you want to clear clear ai map?"))) == EAppReturnType::Yes)
	{
		
		ClearSelectionNodes();
		GEditor->BeginTransaction(FText::FromString(TEXT("Clear AI Map")));
		StalkerAIMap->PreEditChange(nullptr);
		StalkerAIMap->ClearAIMap();
		StalkerAIMap->PostEditChange();
		StalkerAIMap->MarkPackageDirty();
		GEditor->EndTransaction();
	}
}

void FStalkerAIMapEditMode::ClearSelectionNodes()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return;
	}

	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetAIMap();
	if (!StalkerAIMap)
	{
		return;
	}
	StalkerAIMap->ClearSelected();

}


bool FStalkerAIMapEditMode::InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event)
{
	bool isHandled = false;

	if (!isHandled && Event == IE_Pressed)
	{
		isHandled = AIMapEdModeActions->ProcessCommandBindings(Key, FSlateApplication::Get().GetModifierKeys(), false);
	}
	return isHandled;
}

bool FStalkerAIMapEditMode::BoxSelect(FBox& InBox, bool InSelect /*= true*/)
{
	bool bStrictDragSelection = GetDefault<ULevelEditorViewportSettings>()->bStrictBoxSelection;
 	UWorld* World = GetWorld();
	if (FEdMode::BoxSelect(InBox, InSelect))
	{
		return true;
	}
	if (!IsEnabled())
	{
		return false;
	}
	if(InSelect) 
	{
		AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
		if (!StalkerWorldSettings)
		{
			return false;
		}

		UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetAIMap();
		if (!StalkerAIMap)
		{
			return false;
		}
		const float NodeSize = StalkerAIMap->NodeSize;
		auto GetBox = [NodeSize](FStalkerAIMapNode* Node)
		{
			FBox AABB = FBox(ForceInit);
			AABB +=FVector(Node->Position + FVector3f(NodeSize * 0.5f, NodeSize * 0.5f, 2.f));
			AABB += FVector(Node->Position + FVector3f(-NodeSize * 0.5f, -NodeSize * 0.5f, -2.f));
			return AABB;
		};

		bIsAddMode = false;
		
		ClearSelectionNodes();
		for (FStalkerAIMapNode* Node : StalkerAIMap->Nodes)
		{
			if (bStrictDragSelection)
			{
				if (InBox.IsInside(GetBox(Node)))
				{
					Node->Flags |= EStalkerAIMapNodeFlags::Selected;
				}
			}
			else
			{
				if (InBox.Intersect(GetBox(Node)))
				{
					Node->Flags |= EStalkerAIMapNodeFlags::Selected;
				}
			}
			
		}
		StalkerAIMap->RefreshHashSelected();
		return true;
	}
	return false;
}


bool FStalkerAIMapEditMode::FrustumSelect(const FConvexVolume& InFrustum, FEditorViewportClient* InViewportClient, bool InSelect /*= true*/)
{
	UWorld* World = GetWorld();
	if (FEdMode::FrustumSelect(InFrustum, InViewportClient, InSelect))
	{
		return true;
	}
	if (!IsEnabled())
	{
		return false;
	}
	bool bStrictDragSelection = GetDefault<ULevelEditorViewportSettings>()->bStrictBoxSelection;
	if ( InSelect)
	{
		bIsAddMode = false;
		AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
		if (!StalkerWorldSettings)
		{
			return false;
		}

		UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetAIMap();
		if (!StalkerAIMap)
		{
			return false;
		}
		const float NodeSize = StalkerAIMap->NodeSize;
		auto GetBox = [NodeSize](FStalkerAIMapNode* Node)
		{
			FBox AABB = FBox(ForceInit);
			AABB += FVector(Node->Position + FVector3f(NodeSize * 0.5f, NodeSize * 0.5f, 2.f));
			AABB += FVector(Node->Position + FVector3f(-NodeSize * 0.5f, -NodeSize * 0.5f, -2.f));
			return AABB;
		};
		ClearSelectionNodes();
		for (FStalkerAIMapNode* Node : StalkerAIMap->Nodes)
		{
			FBox Box = GetBox(Node);
			bool IsInside = false;
			bool Intersect = InFrustum.IntersectBox(Box.GetCenter(), Box.GetExtent(), IsInside);
			if (Intersect && (!bStrictDragSelection || IsInside))
			{
				Node->Flags |= EStalkerAIMapNodeFlags::Selected;
			}
		}
		StalkerAIMap->RefreshHashSelected();
		return true;
	}
	return false;
}


bool FStalkerAIMapEditMode::InputDelta(FEditorViewportClient* InViewportClient, FViewport* InViewport, FVector& InDrag, FRotator& InRot, FVector& InScale)
{
	if (InViewportClient->GetCurrentWidgetAxis() == EAxisList::None)
	{
		return false;
	}
	if (!IsEnabled())
	{
		return false;
	}
	UWorld* World = GetWorld();
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return false;
	}

	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetAIMap();
	if (!StalkerAIMap)
	{
		return false;
	}
	TArray< FStalkerAIMapNode*> Nodes;
	StalkerAIMap->GetSelectedNodes(Nodes);
	for (FStalkerAIMapNode* Node : Nodes)
	{
		StalkerAIMap->NeedRebuild = true;
		Node->Position.Z += InDrag.Z;
		FVector3f NewNormal = Node->Plane.GetNormal() + (FVector3f(InRot.Quaternion().GetUpVector()) - FVector3f(0, 0, 1));
		NewNormal.Normalize();
		if (FMath::RadiansToDegrees(FMath::Acos(NewNormal | FVector3f(0, 0, 1))) > 75.f)
		{
			NewNormal = Node->Plane.GetNormal();
		}
		Node->Plane = FPlane4f(Node->Position, NewNormal);
	}
	return true;
}

FVector FStalkerAIMapEditMode::GetWidgetLocation() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return  FEdMode::GetWidgetLocation();
	}
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return  FEdMode::GetWidgetLocation();
	}

	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetAIMap();
	if (!StalkerAIMap)
	{
		return  FEdMode::GetWidgetLocation();
	}
	FStalkerAIMapNode* Node;
	if (StalkerAIMap->GetFirstSelectedNode(Node))
	{
		return FVector(Node->Position);
	}
	return FEdMode::GetWidgetLocation();
}

bool FStalkerAIMapEditMode::UsesTransformWidget() const
{
	return true;
}

bool FStalkerAIMapEditMode::ShowModeWidgets() const
{
	return true;
}

bool FStalkerAIMapEditMode::ShouldDrawWidget() const
{
	
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return  false;
	}

	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetAIMap();
	if (!StalkerAIMap)
	{
		return false;
	}
	FStalkerAIMapNode* Node;
	if (StalkerAIMap->GetFirstSelectedNode(Node))
	{
		return true;
	}
	return false;
}

void FStalkerAIMapEditMode::LinkLeft()
{
	UWorld* World = GetWorld();
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return ;
	}

	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetAIMap();
	if (!StalkerAIMap)
	{
		return ;
	}
	GEditor->BeginTransaction(FText::FromString(TEXT("AIMap Link Left")));
	StalkerAIMap->PreEditChange(nullptr);
	Link(2);
	StalkerAIMap->PostEditChange();
	StalkerAIMap->MarkPackageDirty();
	GEditor->EndTransaction();
}

void FStalkerAIMapEditMode::LinkRight()
{
	UWorld* World = GetWorld();
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return;
	}

	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetAIMap();
	if (!StalkerAIMap)
	{
		return;
	}
	GEditor->BeginTransaction(FText::FromString(TEXT("AIMap Link Left")));
	StalkerAIMap->PreEditChange(nullptr);
	Link(0);
	StalkerAIMap->PostEditChange();
	StalkerAIMap->MarkPackageDirty();
	GEditor->EndTransaction();
}

void FStalkerAIMapEditMode::LinkForward()
{
	UWorld* World = GetWorld();
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return;
	}

	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetAIMap();
	if (!StalkerAIMap)
	{
		return;
	}
	GEditor->BeginTransaction(FText::FromString(TEXT("AIMap Link Left")));
	StalkerAIMap->PreEditChange(nullptr);
	Link(3);
	StalkerAIMap->PostEditChange();
	StalkerAIMap->MarkPackageDirty();
	GEditor->EndTransaction();
}

void FStalkerAIMapEditMode::LinkBackward()
{
	UWorld* World = GetWorld();
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return;
	}

	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetAIMap();
	if (!StalkerAIMap)
	{
		return;
	}
	GEditor->BeginTransaction(FText::FromString(TEXT("AIMap Link Left")));
	StalkerAIMap->PreEditChange(nullptr);
	Link(1);
	StalkerAIMap->PostEditChange();
	StalkerAIMap->MarkPackageDirty();
	GEditor->EndTransaction();
}

void FStalkerAIMapEditMode::Link(int32 ID)
{
	UWorld* World = GetWorld();
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return;
	}

	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetAIMap();
	if (!StalkerAIMap)
	{
		return;
	}
	
	int32 Link = ConvertV2L(ID);

	if (LinkMode == LM_Remove)
	{
		for (FStalkerAIMapNode* Node : StalkerAIMap->Nodes)
		{
			if (EnumHasAnyFlags(Node->Flags, EStalkerAIMapNodeFlags::Selected))
			{
				StalkerAIMap->NeedRebuild = true;
				Node->Nodes[Link] = nullptr;
			}
		}
	}
	if (LinkMode == LM_Add)
	{
		for (FStalkerAIMapNode* Node : StalkerAIMap->Nodes)
		{

			if (EnumHasAnyFlags(Node->Flags, EStalkerAIMapNodeFlags::Selected))
			{
				StalkerAIMap->NeedRebuild = true;
				if(!Node->Nodes[Link]) 
					Node->Nodes[Link] = StalkerAIMap->FindNeighbour(Node, Link);
			}
		}
	}
	if (LinkMode == LM_Invert)
	{
		static const int32 InvertLink[4] = { 2,3,0,1 };
		for (FStalkerAIMapNode* Node : StalkerAIMap->Nodes)
		{
			if (EnumHasAnyFlags(Node->Flags, EStalkerAIMapNodeFlags::Selected))
			{
				FStalkerAIMapNode* Neighbour = StalkerAIMap->FindNeighbour(Node, Link);
				if (!Neighbour)
				{
					continue;
				}
				if (!Node->Nodes[Link] || !Neighbour->Nodes[InvertLink[Link]])
				{
					StalkerAIMap->NeedRebuild = true;
					FStalkerAIMapNode* SwapNode1 = Node->Nodes[Link];
					FStalkerAIMapNode* SwapNode2 = Neighbour->Nodes[InvertLink[Link]];
					Node->Nodes[Link] = SwapNode2 ? Neighbour : nullptr;
					Neighbour->Nodes[InvertLink[Link]] = SwapNode1 ? Node : nullptr;

				}
			}
		}
	}
}

void FStalkerAIMapEditMode::LinkAll()
{
	UWorld* World = GetWorld();
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return;
	}

	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetAIMap();
	if (!StalkerAIMap)
	{
		return;
	}
	GEditor->BeginTransaction(FText::FromString(TEXT("AIMap Link All")));
	StalkerAIMap->PreEditChange(nullptr);
	for (int32 i = 0; i < 4; i++)
	{
		Link(i);
	}
	StalkerAIMap->PostEditChange();
	StalkerAIMap->MarkPackageDirty();
	GEditor->EndTransaction();
}

bool FStalkerAIMapEditMode::StartTracking(FEditorViewportClient* InViewportClient, FViewport* InViewport)
{
	/*UWorld* World = GetWorld();
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return  false;
	}

	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetAIMap();
	if (!StalkerAIMap)
	{
		return  false;
	}
	bool NeedBeginTransaction = false;
	FStalkerAIMapNode* Node;
	if (StalkerAIMap->GetFirstSelectedNode(Node))
	{
		NeedBeginTransaction = true;
	}
	if (NeedBeginTransaction)
	{
		GEditor->BeginTransaction(FText::FromString(TEXT(" AI Map Move Node's")));
		StalkerAIMap->PreEditChange(nullptr);
	}*/

	return FEdMode::StartTracking(InViewportClient, InViewport);
}

bool FStalkerAIMapEditMode::EndTracking(FEditorViewportClient* InViewportClient, FViewport* InViewport)
{/*
	UWorld* World = GetWorld();
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return  false;
	}

	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetAIMap();
	if (!StalkerAIMap)
	{
		return  false;
	}
	bool NeedBeginTransaction = false;
	FStalkerAIMapNode* Node;
	if (StalkerAIMap->GetFirstSelectedNode(Node))
	{
		NeedBeginTransaction = true;
	}
	if (NeedBeginTransaction)
	{
		StalkerAIMap->Modify();
		StalkerAIMap->MarkPackageDirty();
		GEditor->EndTransaction();
	}*/

	return FEdMode::EndTracking(InViewportClient,InViewport);
}

void FStalkerAIMapEditMode::GenerateSelect()
{
	UWorld* World = GetWorld();	
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return  ;
	}

	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetAIMap();
	if (!StalkerAIMap)
	{
		return  ;
	}
	GEditor->BeginTransaction(FText::FromString(TEXT("AIMap Generate Select")));
	StalkerAIMap->PreEditChange(nullptr);
	GStalkerEditorManager->EditorAIMap->Generate(World, true);
	StalkerAIMap->PostEditChange();
	StalkerAIMap->MarkPackageDirty();
	GEditor->EndTransaction();
}

void FStalkerAIMapEditMode::GenerateFull()
{
	UWorld* World = GetWorld();
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return;
	}

	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetAIMap();
	if (!StalkerAIMap)
	{
		return;
	}
	GEditor->BeginTransaction(FText::FromString(TEXT("AIMap Generate Full")));
	StalkerAIMap->PreEditChange(nullptr);
	GStalkerEditorManager->EditorAIMap->Generate(World);
	StalkerAIMap->PostEditChange();
	StalkerAIMap->MarkPackageDirty();
	GEditor->EndTransaction();
}

int32 FStalkerAIMapEditMode::ConvertV2L(int32 Side)
{
	static const int OutIndex[5][4] = {
		{0,1,2,3},
		{1,2,3,0},
		{2,3,0,1},
		{3,0,1,2},
		{4,4,4,4},
	};

	UWorld* World = GetWorld();
	FRotator CameraRotation = FRotator::ZeroRotator;
	for (FLevelEditorViewportClient* LevelVC : GEditor->GetLevelViewportClients())
	{
		if (LevelVC && LevelVC->IsPerspective())
		{
			CameraRotation = LevelVC->GetViewRotation();
			break;
		}
	}
	CameraRotation.Yaw = FMath::Fmod(CameraRotation.Yaw,360);
	CameraRotation.Yaw = CameraRotation.Yaw<0 ?360 + CameraRotation.Yaw: CameraRotation.Yaw;
	int32 AngleID = (static_cast<int32>(FMath::Floor((CameraRotation.Yaw - 90 + 45) / 90.1f))+2)%4;
	return OutIndex[AngleID][Side];
}

bool FStalkerAIMapEditMode::CanRemoveSelectNodes()
{
	UWorld* World = GetWorld();
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return  false;
	}

	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetAIMap();
	if (!StalkerAIMap)
	{
		return false;
	}
	for (FStalkerAIMapNode* Node : StalkerAIMap->Nodes)
	{
		if (EnumHasAnyFlags(Node->Flags, EStalkerAIMapNodeFlags::Selected))
		{
			return true;
		}
	}
	return false;
}

void FStalkerAIMapEditMode::SmoothFull()
{
	UWorld* World = GetWorld();
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return;
	}

	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetAIMap();
	if (!StalkerAIMap)
	{
		return;
	}
	GEditor->BeginTransaction(FText::FromString(TEXT("AIMap Smooth Full")));
	StalkerAIMap->PreEditChange(nullptr);
	GStalkerEditorManager->EditorAIMap->Smooth(World);
	StalkerAIMap->PostEditChange();
	StalkerAIMap->MarkPackageDirty();
	GEditor->EndTransaction();
}

void FStalkerAIMapEditMode::SmoothSelect()
{
	UWorld* World = GetWorld();
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return;
	}

	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetAIMap();
	if (!StalkerAIMap)
	{
		return;
	}
	GEditor->BeginTransaction(FText::FromString(TEXT("AIMap Smooth Select")));
	StalkerAIMap->PreEditChange(nullptr);
	GStalkerEditorManager->EditorAIMap->Smooth(World,true);
	StalkerAIMap->PostEditChange();
	StalkerAIMap->MarkPackageDirty();
	GEditor->EndTransaction();
}

void FStalkerAIMapEditMode::ResetSelect()
{
	UWorld* World = GetWorld();
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return;
	}

	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetAIMap();
	if (!StalkerAIMap)
	{
		return;
	}
	GEditor->BeginTransaction(FText::FromString(TEXT("AIMap Reset Select")));
	StalkerAIMap->PreEditChange(nullptr);
	GStalkerEditorManager->EditorAIMap->Reset(World, true);
	StalkerAIMap->PostEditChange();
	StalkerAIMap->MarkPackageDirty();
	GEditor->EndTransaction();
}

void FStalkerAIMapEditMode::RemoveSelectNodes()
{
	UWorld* World = GetWorld();
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return ;
	}

	UStalkerAIMap* StalkerAIMap = StalkerWorldSettings->GetAIMap();
	if (!StalkerAIMap)
	{
		return;
	}
	GEditor->BeginTransaction(FText::FromString(TEXT(" AI Map Remove Select Node")));
	StalkerAIMap->PreEditChange(nullptr);
	StalkerAIMap->RemoveSelect();
	StalkerAIMap->PostEditChange();
	StalkerAIMap->MarkPackageDirty();
	GEditor->EndTransaction();

}
bool FStalkerAIMapEditMode::Select(AActor* InActor, bool bInSelected)
{
	return !bInSelected;
}

bool FStalkerAIMapEditMode::IsSelectionAllowed(AActor* InActor, bool bInSelection) const
{
	return !bInSelection;
}

bool FStalkerAIMapEditMode::IsEnabled()
{
	UWorld* World = GetWorld();

	if (GEditor->bIsSimulatingInEditor)
	{
		return false;
	}
	else if (GEditor->PlayWorld != nullptr)
	{
		return false;
	}
	else if (World == nullptr)
	{
		return false;
	}
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	if (!StalkerWorldSettings)
	{
		return false;
	}
	if (StalkerWorldSettings->NotForXRay)
	{
		return false;
	}
	return true;
}

bool FStalkerAIMapEditMode::IsEnabledAction()
{
	return IsEnabled()&& bIsAddMode==false;
}
