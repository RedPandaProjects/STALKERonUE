#include "StalkerWayObjectEditMode.h"
#include "StalkerWayObjectEditorCommands.h"
#include "StalkerWayObjectEdModeToolkit.h"
#include "EditorViewportClient.h"
#include "../../../Entities/Scene/WayObject/StalkerWayObject.h"
#include "../../../Entities/Scene/WayObject/StalkerWayPointComponent.h"

const FEditorModeID FStalkerWayObjectEditMode::EM_WayObject(TEXT("EM_WayObject"));

void FStalkerWayObjectEditMode::ClearSelectedPoint()
{
	if (!WayObject)
	{
		return;
	}
	for (UStalkerWayPointComponent* Point : WayObject->Points)
	{
		Point->bIsSelected = false;
	}

	UpdateSelectedPoint();
}

void FStalkerWayObjectEditMode::UpdateSelectedPoint()
{
	TArray<UObject*> PointsSelectable;
	if (WayObject)
	{
		for (UStalkerWayPointComponent* Point : WayObject->Points)
		{
			if (Point->bIsSelected)
			{
				PointsSelectable.Add(Point);
			}
		}
	}

	static_cast<FStalkerWayObjectEdModeToolkit*>(Toolkit.Get())->SetPropertyEditor(PointsSelectable);
}

void FStalkerWayObjectEditMode::RemoveSelectPoints()
{
	if (!WayObject)
	{
		return;
	}
	if (!IsEnabled())
	{
		return;
	}
	WayObject->RemoveSelect();
}

bool FStalkerWayObjectEditMode::CanRemoveSelectPoints()
{
	if (!WayObject)
	{
		return false;
	}
	if (!IsEnabled())
	{
		return false;
	}
	for (UStalkerWayPointComponent* Point : WayObject->Points)
	{
		if (Point->bIsSelected)
		{
			return true;
		}
	}
	return false;
}

void FStalkerWayObjectEditMode::SelectALLPoints()
{
	if (!WayObject)
	{
		return;
	}
	for (UStalkerWayPointComponent* Point : WayObject->Points)
	{
		Point->bIsSelected = true;
	}
	UpdateSelectedPoint();
	bIsAddWayPointMode = false;
}

bool FStalkerWayObjectEditMode::CanSelectALLPoints()
{
	return WayObject!=nullptr;
}

FStalkerWayObjectEditMode::FStalkerWayObjectEditMode()
{
	FStalkerWayObjectEditorCommands::Register();
	bIsAddWayPointMode = false;
	bAutoLink = true;
	WayObject = nullptr;
	WayObjectEdModeActions = MakeShareable(new FUICommandList);
}

FStalkerWayObjectEditMode::~FStalkerWayObjectEditMode()
{
	FStalkerWayObjectEditorCommands::Unregister();
}
void FStalkerWayObjectEditMode::Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI)
{
	UWorld* World = GetWorld();

	return;
}

bool FStalkerWayObjectEditMode::HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click)
{
	UWorld* World = GetWorld();

	if (bIsAddWayPointMode&&IsEnabled()&& !Click.IsControlDown())
	{
		UStalkerWayPointComponent* NewPoint = WayObject->CreatePoint(GEditor->ClickLocation, bAutoLink);
		ClearSelectedPoint();
		NewPoint->bIsSelected = true;
		UpdateSelectedPoint();
	}
	else
	{
		bIsAddWayPointMode =  false;
		if(Click.GetKey() == EKeys::LeftMouseButton)
		{
			if (!Click.IsControlDown())
			{
				ClearSelectedPoint();
			}

			UStalkerWayPointComponent* ResultPoint = nullptr;

			if (HitProxy->IsA(HActor::StaticGetType()))
			{

				if (static_cast<HActor*>(HitProxy)->Actor == WayObject &&
					static_cast<HActor*>(HitProxy)->PrimComponent->IsA<UStalkerWayPointComponent>()
					)
				{

					ResultPoint = const_cast<UStalkerWayPointComponent*>(Cast<const UStalkerWayPointComponent>(static_cast<HActor*>(HitProxy)->PrimComponent));

				}

			}
			if (WayObject && !ResultPoint)
			{
				FVector ResultLocation, ResultNormal; float HitTime;
				FVector EndRay = Click.GetOrigin() + Click.GetDirection() * WORLD_MAX;
				for (UStalkerWayPointComponent* Point : WayObject->Points)
				{
					if (FMath::LineExtentBoxIntersection(Point->GetPointBox(), Click.GetOrigin(), EndRay, FVector(0, 0, 0), ResultLocation, ResultNormal, HitTime))
					{
						EndRay = ResultLocation;
						ResultPoint = Point;
					}
				}
			}
			if (ResultPoint)
			{
				ResultPoint->bIsSelected = Click.IsControlDown() ? !ResultPoint->bIsSelected : true;
				UpdateSelectedPoint();
				return true;
			}
			if (Click.IsControlDown())
			{
				return true;
			}
		}
		if (Click.GetKey() == EKeys::RightMouseButton&&WayObject)
		{
			bool IsSelected = false;
			for (UStalkerWayPointComponent* Point : WayObject->Points)
			{
				IsSelected |= Point->bIsSelected;
			}
			if (IsSelected)
			{
				FMenuBuilder MenuBuilder(true, NULL);
				MenuBuilder.PushCommandList(WayObjectEdModeActions.ToSharedRef());
				MenuBuilder.BeginSection("Way Section");
				MenuBuilder.AddMenuEntry(FStalkerWayObjectEditorCommands::Get().SelectALLWayPoint);
				MenuBuilder.AddMenuEntry(FStalkerWayObjectEditorCommands::Get().DeleteWay);
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
		}
	}
	return FEdMode::HandleClick(InViewportClient,HitProxy,Click);
}

void FStalkerWayObjectEditMode::Enter()
{

	FEdMode::Enter();
	GEngine->OnLevelActorDeleted().AddSP(this, &FStalkerWayObjectEditMode::OnLevelActorRemoved);
	if (!Toolkit.IsValid())
	{
		Toolkit = MakeShareable(new FStalkerWayObjectEdModeToolkit);
		Toolkit->Init(Owner->GetToolkitHost());
	}
	{
		const auto& Commands = FStalkerWayObjectEditorCommands::Get();
		WayObjectEdModeActions->MapAction(Commands.DeleteWay, FExecuteAction::CreateSP(this, &FStalkerWayObjectEditMode::RemoveSelectPoints), FCanExecuteAction::CreateSP(this, &FStalkerWayObjectEditMode::CanRemoveSelectPoints));
		WayObjectEdModeActions->MapAction(Commands.SelectALLWayPoint, FExecuteAction::CreateSP(this, &FStalkerWayObjectEditMode::SelectALLPoints), FCanExecuteAction::CreateSP(this, &FStalkerWayObjectEditMode::CanSelectALLPoints));
	}

	AStalkerWayObject* InWayObject = GEditor->GetSelectedActors()->GetTop<AStalkerWayObject>();
	if (InWayObject)
	{
		GEditor->SelectNone(true, true);
		GEditor->SelectActor(InWayObject, true,true);
	}
	else
	{
		GEditor->SelectNone(true, true);
	}
}



void FStalkerWayObjectEditMode::Exit()
{
	ClearSelectedPoint();
	WayObject = nullptr;
	FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
	Toolkit.Reset();
	GEngine->OnLevelActorDeleted().RemoveAll(this);
	FEdMode::Exit();

}

bool FStalkerWayObjectEditMode::InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event)
{
	bool isHandled = false;

	if (!isHandled && Event == IE_Pressed)
	{
		isHandled = WayObjectEdModeActions->ProcessCommandBindings(Key, FSlateApplication::Get().GetModifierKeys(), false);
	}
	return isHandled;
}

bool FStalkerWayObjectEditMode::BoxSelect(FBox& InBox, bool InSelect /*= true*/)
{
	bool bStrictDragSelection = GetDefault<ULevelEditorViewportSettings>()->bStrictBoxSelection;
	if (!IsEnabled())
	{
		return false;
	}
	if(InSelect) 
	{
		bIsAddWayPointMode = false;
		ClearSelectedPoint();
		if (!WayObject)
		{
			return false;
		}

		for (UStalkerWayPointComponent* Point : WayObject->Points)
		{
			FBox Box = Point->GetPointBox();
			if (bStrictDragSelection)
			{
				if (InBox.IsInside(Box))
				{
					Point->bIsSelected = true;
				}
			}
			else
			{
				if (InBox.Intersect(Box))
				{
					Point->bIsSelected = true;
				}
			}
		}
		return true;
	}
	if (FEdMode::BoxSelect(InBox, InSelect))
	{
		return true;
	}
	return false;
}


bool FStalkerWayObjectEditMode::FrustumSelect(const FConvexVolume& InFrustum, FEditorViewportClient* InViewportClient, bool InSelect /*= true*/)
{
	UWorld* World = GetWorld();
	if (!IsEnabled())
	{
		return false;
	}

	bool bStrictDragSelection = GetDefault<ULevelEditorViewportSettings>()->bStrictBoxSelection;
	if ( InSelect)
	{
		bIsAddWayPointMode = false;
		ClearSelectedPoint();
		if (!WayObject)
		{
			return false;
		}

		for (UStalkerWayPointComponent* Point : WayObject->Points)
		{
			FBox Box = Point->GetPointBox();
			bool IsInside = false;
			bool Intersect = InFrustum.IntersectBox(Box.GetCenter(), Box.GetExtent(), IsInside);
			if (Intersect && (!bStrictDragSelection || IsInside))
			{
				Point->bIsSelected = true;
			}
		}
		return true;
	}
	if (FEdMode::FrustumSelect(InFrustum, InViewportClient, InSelect))
	{
		return true;
	}
	return false;
}


bool FStalkerWayObjectEditMode::InputDelta(FEditorViewportClient* InViewportClient, FViewport* InViewport, FVector& InDrag, FRotator& InRot, FVector& InScale)
{
	if (InViewportClient->GetCurrentWidgetAxis() == EAxisList::None)
	{
		return false;
	}
	if (!IsEnabled())
	{
		return false;
	}
	if (!WayObject)
	{
		return false;
	}

	for (UStalkerWayPointComponent* Point : WayObject->Points)
	{
		if (Point->bIsSelected)
		{
			Point->AddRelativeLocation(InDrag);
		}
	}
	return true;
}

FVector FStalkerWayObjectEditMode::GetWidgetLocation() const
{
	UWorld* World = GetWorld();
	if (!WayObject)
	{
		return FEdMode::GetWidgetLocation();
	}
	for (UStalkerWayPointComponent* Point : WayObject->Points)
	{
		if (Point->bIsSelected)
		{
			return Point->GetComponentLocation();
		}
	}
	return FEdMode::GetWidgetLocation();
}

bool FStalkerWayObjectEditMode::UsesTransformWidget() const
{
	return true;
}

bool FStalkerWayObjectEditMode::ShowModeWidgets() const
{
	return true;
}

bool FStalkerWayObjectEditMode::ShouldDrawWidget() const
{
	UWorld* World = GetWorld();

	if (!WayObject)
	{
		return false;
	}
	for (UStalkerWayPointComponent* Point : WayObject->Points)
	{
		if (Point->bIsSelected)
		{
			return true;
		}
	}	
	return false;
}

void FStalkerWayObjectEditMode::SetSelectMode()
{
	bIsAddWayPointMode = false;
}

void FStalkerWayObjectEditMode::SetAddPointMode()
{
	if (!IsEnabled())
	{
		return;
	}
	bIsAddWayPointMode = true;
}

bool FStalkerWayObjectEditMode::Select(AActor* InActor, bool bInSelected)
{
	if (!bInSelected&& WayObject == InActor)
	{
		ClearSelectedPoint();
		bIsAddWayPointMode = false;
		WayObject = nullptr;
	}
	if (!IsSelectionAllowed(InActor, bInSelected) || (WayObject&& WayObject->IsSelected()))
	{
		return true;
	}
	if (bInSelected)
	{
		WayObject = Cast<AStalkerWayObject>(InActor);
		ClearSelectedPoint();
		WayObject->Points[0]->bIsSelected = true;
		UpdateSelectedPoint();
	}
	return false;
}

bool FStalkerWayObjectEditMode::IsSelectionAllowed(AActor* InActor, bool bInSelection) const
{
	if (!bInSelection)
	{
		return true;
	}
	if (InActor != WayObject && bIsAddWayPointMode)
	{
		return false;
	}
	if (InActor->IsA<AStalkerWayObject>())
	{
		return true;
	}
	return false;
}

void FStalkerWayObjectEditMode::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdMode::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(WayObject);
}

FString FStalkerWayObjectEditMode::GetReferencerName() const
{
	return TEXT("FStalkerWayObjectEditMode");
}

void FStalkerWayObjectEditMode::OnLevelActorRemoved(AActor*InActor)
{
	if (InActor->IsA<AStalkerWayObject>()&& Cast<AStalkerWayObject>(InActor)== WayObject)
	{
		WayObject = nullptr;
		bIsAddWayPointMode = false;
	}
}

void FStalkerWayObjectEditMode::InvertLinkSelect()
{

	if (!IsEnabled())
	{
		return;
	}
	if (WayObject)
	{
		const FScopedTransaction Transaction(FText::FromString(TEXT("Way: Convert to 1 link")));
		WayObject->Modify();
		WayObject->InvertLinkSelect();
	}
}

void FStalkerWayObjectEditMode::RemoveLinkSelect()
{
	if (!IsEnabled())
	{
		return;
	}
	if (WayObject)
	{
		const FScopedTransaction Transaction(FText::FromString(TEXT("Way: Remove links")));
		WayObject->Modify();
		WayObject->RemoveLinkSelect();
	}
}

void FStalkerWayObjectEditMode::Convert1LinkSelect()
{
	if (!IsEnabled())
	{
		return;
	}
	if (WayObject)
	{
		const FScopedTransaction Transaction(FText::FromString(TEXT("Way: Convert to 1 link")));
		WayObject->Modify();
		WayObject->Convert1LinkSelect();
	}
}

void FStalkerWayObjectEditMode::Convert2LinkSelect()
{
	if (!IsEnabled())
	{
		return;
	}
	if (WayObject)
	{
		const FScopedTransaction Transaction(FText::FromString(TEXT("Way: Convert to 2 links")));
		WayObject->Modify();
		WayObject->Convert2LinkSelect();
	}
}

void FStalkerWayObjectEditMode::Add1LinkSelect()
{
	if (!IsEnabled())
	{
		return;
	}
	if (WayObject)
	{
		const FScopedTransaction Transaction(FText::FromString(TEXT("Way: Add 1 link")));
		WayObject->Modify();
		WayObject->Add1LinkSelect();
	}
}

void FStalkerWayObjectEditMode::Add2LinkSelect()
{
	if (!IsEnabled())
	{
		return;
	}
	if (WayObject)
	{
		const FScopedTransaction Transaction(FText::FromString(TEXT("Way: Add 2 link")));
		WayObject->Modify();
		WayObject->Add2LinkSelect();
	}
}

bool FStalkerWayObjectEditMode::IsEnabled()
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
	
	return true;
}

bool FStalkerWayObjectEditMode::IsEnabledAction()
{
	return IsEnabled()&&!bIsAddWayPointMode&&WayObject;
}

