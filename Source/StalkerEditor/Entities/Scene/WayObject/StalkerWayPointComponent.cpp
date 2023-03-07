#include "StalkerWayPointComponent.h"
#include "StalkerWayObject.h"
#include "../../../UI/EdMode/WayPoints/StalkerWayObjectEditMode.h"
#include "UObject/GCObjectScopeGuard.h"
#include "Kernel/Unreal/WorldSettings/StalkerWorldSettings.h"
#include "Resources/Spawn/StalkerLevelSpawn.h"

TCustomShowFlag<> StalkerShowWayPoint(TEXT("StalkerShowWayPoint"), true /*DefaultEnabled*/, SFG_Normal, FText::FromString(TEXT("Way point")));

constexpr float WayPointSize = 1.5f * 100;
constexpr float WayPointRadius = WayPointSize * .5f;

FPrimitiveSceneProxy* UStalkerWayPointComponent::CreateSceneProxy()
{
	class FStalkerWayPointRenderSceneProxy final : public FPrimitiveSceneProxy
	{
	public:
		SIZE_T GetTypeHash() const override
		{
			static size_t UniquePointer;
			return reinterpret_cast<size_t>(&UniquePointer);
		}
		FStalkerWayPointRenderSceneProxy(const UStalkerWayPointComponent& InComponent)
			: FPrimitiveSceneProxy(&InComponent), StalkerWayPointComponent(&InComponent)
		{
			ViewFlagIndex = uint32(FEngineShowFlags::FindIndexByName(TEXT("StalkerShowWayPoint")));

		}

		FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
		{
			const bool bVisibleMe = View->Family->EngineShowFlags.GetSingleFlag(ViewFlagIndex) || GLevelEditorModeTools().GetActiveMode(FStalkerWayObjectEditMode::EM_WayObject);
			FPrimitiveViewRelevance Result;
			Result.bDrawRelevance = bVisibleMe && IsShown(View);
			Result.bDynamicRelevance = true;
			// ideally the TranslucencyRelevance should be filled out by the material, here we do it conservative
			Result.bSeparateTranslucency = Result.bNormalTranslucency = bVisibleMe;
			return Result;
		}
		uint32 GetMemoryFootprint(void) const override { return sizeof * this + GetAllocatedSize(); }
		uint32 GetAllocatedSize(void) const { return (uint32)FPrimitiveSceneProxy::GetAllocatedSize(); }

		void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, class FMeshElementCollector& Collector) const override
		{
			UWorld* World = GetScene().GetWorld();
			for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
			{
				if (VisibilityMap & (1 << ViewIndex))
				{
					const FSceneView* View = Views[ViewIndex];
					FVector Position = GetLocalToWorld().GetOrigin() + FVector(0, 0, WayPointSize * 0.85f);

			
					
					FColor PointColor = IsSelected()? FColor::Red : FColor(0x0000ff00);
					Collector.GetPDI(ViewIndex)->DrawLine(Position, Position + FVector(WayPointRadius, 0, 0), PointColor, SDPG_World, 3.f);
					Collector.GetPDI(ViewIndex)->DrawLine(Position, Position + FVector(0, WayPointRadius, 0), PointColor, SDPG_World, 3.f);
					Collector.GetPDI(ViewIndex)->DrawLine(Position, Position + FVector(-WayPointRadius, 0, 0), PointColor, SDPG_World, 3.f);
					Collector.GetPDI(ViewIndex)->DrawLine(Position, Position + FVector(0, -WayPointRadius, 0), PointColor, SDPG_World, 3.f);
					Collector.GetPDI(ViewIndex)->DrawLine(Position, Position + FVector(0, 0, -WayPointSize * 0.85f), PointColor, SDPG_World, 3.f);
					Collector.GetPDI(ViewIndex)->DrawLine(Position, Position + FVector(0, 0, WayPointSize * 0.15f), PointColor, SDPG_World, 3.f);
					Collector.GetPDI(ViewIndex)->SetHitProxy(nullptr);
				
					const UStalkerWayPointComponent*InStalkerWayPointComponent = StalkerWayPointComponent;
					if (InStalkerWayPointComponent->bIsSelected)
					{
						const ESceneDepthPriorityGroup DrawBoundsDPG = SDPG_World;

						FBox BBox(ForceInit);
						{
							BBox += GetLocalToWorld().GetOrigin() + FVector(WayPointRadius, WayPointRadius, WayPointSize);
							BBox += GetLocalToWorld().GetOrigin() + FVector(-WayPointRadius, -WayPointRadius, 0);
						}
						DrawWireBox(Collector.GetPDI(ViewIndex), BBox, FColor::White, DrawBoundsDPG);

					}
					for (const FStalkerWayPointLink& Link : InStalkerWayPointComponent->Links)
					{
						
						FVector ToPosition =  Link.Point->GetComponentLocation() + FVector(0, 0, WayPointSize * 0.85f);;
						if (!IsSelected())
						{
							Collector.GetPDI(ViewIndex)->DrawLine(Position, ToPosition, InStalkerWayPointComponent->bIsSelected ? FColor::Red : FColor(0x0000ff00), SDPG_World, 1.f);
						}
						else
						{
							FVector ArrowTangetX = (ToPosition - Position);
							ArrowTangetX.Normalize();

							FVector ArrowTangetY = ArrowTangetX ^ FVector(0, 1, 0);
							ArrowTangetY.Normalize();

							FVector ArrowTangetZ = ArrowTangetX ^ ArrowTangetY;
							ArrowTangetZ.Normalize();

							float Distance = FVector::Distance(Position, ToPosition);

							float LerpAplha = 1.f - (50.f / Distance);
							FVector ArrowPosition(FMath::Lerp(Position.X, ToPosition.X, LerpAplha), FMath::Lerp(Position.Y, ToPosition.Y, LerpAplha), FMath::Lerp(Position.Z, ToPosition.Z, LerpAplha));
							FMatrix PlaneMatrix = FMatrix(ArrowTangetX, ArrowTangetY, ArrowTangetZ, ArrowPosition);

							if (Distance > 50)
							{
								ToPosition -= ArrowTangetX * 50.f;
							}

							Collector.GetPDI(ViewIndex)->DrawLine(Position, ToPosition, InStalkerWayPointComponent->bIsSelected ? FColor::Yellow: FColor::Red , SDPG_World, 1.f);

							DrawDirectionalArrow(Collector.GetPDI(ViewIndex), PlaneMatrix, InStalkerWayPointComponent->bIsSelected ? FColor::Cyan : FColor::Orange, 50.f, 50.f * 0.35f, SDPG_World, 1.f);
						}
						
						//Draw
					}
				}
			}

		}
		const UStalkerWayPointComponent* StalkerWayPointComponent;
		int32 ViewFlagIndex;
	};

	return new FStalkerWayPointRenderSceneProxy(*this);
}

FBoxSphereBounds UStalkerWayPointComponent::CalcBounds(const FTransform& InLocalToWorld) const 
{

	FBox BBox(ForceInit);
	FVector Position = InLocalToWorld.GetLocation();
	BBox += Position + FVector(WayPointRadius, WayPointRadius, WayPointSize);
	BBox += Position + FVector(-WayPointRadius, -WayPointRadius, 0);
	for (const FStalkerWayPointLink& Link : Links)
	{
		BBox += Link.Point->GetComponentLocation() + FVector(0, 0, WayPointSize * 0.85f);
	}


	if (BBox.IsValid)
	{
		return FBoxSphereBounds(BBox);
	}
	else
	{
		const FVector BoxExtent(1.f);
		return FBoxSphereBounds(InLocalToWorld.GetLocation(), BoxExtent, 1.f);
	}
}

void UStalkerWayPointComponent::AddLink(UStalkerWayPointComponent* Point, float Probability)
{
	if (!Point)
	{
		return;
	}
	if (Point==this)
	{
		return;
	}
	if (Point->GetOuter() != GetOuter())
	{
		return;
	}
	if (CheckLink(Point))
	{
		return;
	}
	Links.Add(FStalkerWayPointLink());
	Links.Last().Point = Point;
	Links.Last().Probability = Probability;
	UpdateBounds();
	MarkRenderTransformDirty();
}

void UStalkerWayPointComponent::AddDoubleLink(UStalkerWayPointComponent* Point)
{
	AddLink(Point);
	Point->AddLink(this);
}

bool UStalkerWayPointComponent::CheckLink(UStalkerWayPointComponent* Point, float* Probability)
{
	for (FStalkerWayPointLink& Link : Links)
	{
		if (Link.Point == Point)
		{
			if (Probability)
			{
				*Probability = Link.Probability;
			}
			return true;
		}
	}
	return false;
}

void UStalkerWayPointComponent::RemoveLink(UStalkerWayPointComponent* Point)
{
	for (int32 i = 0; i < Links.Num(); i++)
	{
		if (Links[i].Point==Point)
		{
			Links.RemoveAt(i);
			i = 0;
		}
	}
}

void UStalkerWayPointComponent::InvertLink(UStalkerWayPointComponent* PointB)
{
	float ProbabilityA = 1.f, ProbabilityB = 1.f;
	bool LinkA = CheckLink(PointB, &ProbabilityA);
	bool LinkB = PointB->CheckLink(this, &ProbabilityB);

	if (LinkA)
	{
		RemoveLink(PointB);
	}
	if (LinkB)
	{
		PointB->RemoveLink(this);
	}

	if (LinkB)
	{
		AddLink(PointB, ProbabilityB);
	}

	if (LinkA)
	{
		PointB->AddLink(this, ProbabilityA);
	}
}

void UStalkerWayPointComponent::Convert1Link(UStalkerWayPointComponent* PointB)
{
	float ProbabilityA =1.f, ProbabilityB = 1.f;
	bool LinkA = CheckLink(PointB, &ProbabilityA);
	bool LinkB = PointB->CheckLink(this, &ProbabilityB);
	if ((LinkA && !LinkB) || (!LinkA && LinkB) || (!LinkA && !LinkB)) return;
	if(LinkA)
	{
		RemoveLink(PointB);
	}
	if (LinkB)
	{
		PointB->RemoveLink(this);
	}
	AddLink(PointB, ProbabilityA);
}

void UStalkerWayPointComponent::Convert2Link(UStalkerWayPointComponent* PointB)
{
	float ProbabilityA = 1.f, ProbabilityB = 1.f;
	bool LinkA = CheckLink(PointB, &ProbabilityA);
	bool LinkB = PointB->CheckLink(this, &ProbabilityB);
	if ((LinkA && LinkB) || (!LinkA && !LinkB)) return;
	if (LinkA)
	{
		RemoveLink(PointB);
	}
	if (LinkB)
	{
		PointB->RemoveLink(this);
	}

	AddLink(PointB, ProbabilityA);
	PointB->AddLink(this, ProbabilityB);
}

void UStalkerWayPointComponent::RemoveSelectLink()
{
	for(int32 i=0;i< Links.Num();i++)
	{
		if(!Links[i].Point->bIsMain&& Links[i].Point->bIsSelected)
		{
			Links.RemoveAt(i);
			i=0;
		}
	}
}

FBox UStalkerWayPointComponent::GetPointBox()
{
	FBox BBox(ForceInit);
	FVector Position = GetComponentLocation();
	BBox += Position + FVector(WayPointRadius, WayPointRadius, WayPointSize);
	BBox += Position + FVector(-WayPointRadius, -WayPointRadius, 0);
	return BBox;
}

void UStalkerWayPointComponent::PreEditChange(FProperty* PropertyThatWillChange)
{
	Super::PreEditChange(PropertyThatWillChange);
	if ((PropertyThatWillChange->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerWayPointComponent, PointName)))
	{
		TempPointName = PointName;
	}
}

void UStalkerWayPointComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FProperty* EditedProperty = PropertyChangedEvent.Property;

	if (EditedProperty && EditedProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerWayPointComponent, PointName))
	{	
		AStalkerWayObject* WayObject = Cast<AStalkerWayObject>(GetOuter());
		if (WayObject->CheckName(PointName))
		{
			PointName = TempPointName ;
		}
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

bool UStalkerWayPointComponent::Modify(bool bAlwaysMarkDirty /*= true*/)
{
	bool bResult = Super::Modify(bAlwaysMarkDirty);

	if (!IsValid(GetWorld()) || GetWorld()->IsGameWorld()||!bAlwaysMarkDirty)
	{
		return bResult;
	}
	if (AActor* Actor = GetAttachParentActor())
	{
		if (!Actor->IsA<AStalkerWayObject>())
		{
			return bResult;
		}
	}
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(GetWorld()->GetWorldSettings());
	if (IsValid(StalkerWorldSettings))
	{
		UStalkerLevelSpawn* Spawn = StalkerWorldSettings->GetSpawn();
		if (IsValid(Spawn))
		{
			StalkerWorldSettings->Modify();
			StalkerWorldSettings->NeedRebuildSpawn = true;
			Spawn->NeedRebuild = true;
			Spawn->Modify();
		}
	}
	return bResult;
}

//
//bool UStalkerWayPointComponent::CanEditChange(const FProperty* InProperty) const
//{
//	bool ParentVal = Super::CanEditChange(InProperty);
//
//	if ((InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UStalkerWayPointComponent, PointName)))
//	{
//		AStalkerWayObject* WayObject =Cast<AStalkerWayObject>(GetOuter());
//		return ParentVal && !WayObject->CheckName(PointName);
//	}
//	return ParentVal;
//}
