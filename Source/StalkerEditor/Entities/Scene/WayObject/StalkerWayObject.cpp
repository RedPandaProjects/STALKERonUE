#include "StalkerWayObject.h"
#include "StalkerWayPointComponent.h"
#include "Components/BillboardComponent.h"
#include "Kernel/Unreal/WorldSettings/StalkerWorldSettings.h"
#include "Resources/Spawn/StalkerLevelSpawn.h"
AStalkerWayObject::AStalkerWayObject(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bIsEditorOnlyActor = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(SceneComponent);

	UStalkerWayPointComponent* MainPoint = CreateDefaultSubobject<UStalkerWayPointComponent>(TEXT("MainPoint"));
	MainPoint->SetupAttachment(SceneComponent);
	MainPoint->bIsMain = true;
	MainPoint->PointName = TEXT("wp_00");
	Points.Add(MainPoint);

	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	Billboard->SetupAttachment(MainPoint);
}

class UStalkerWayPointComponent* AStalkerWayObject::CreatePoint(const FVector& Position, bool AutoLink)
{
	
	UStalkerWayPointComponent* Point = NewObject<UStalkerWayPointComponent>(this,NAME_None,RF_Transactional);
	Point->SetWorldLocation(Position);
	if (Point->AttachToComponent(SceneComponent, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false)))
	{
		for (int32 i = 0;true; i++)
		{
			Point->PointName = FString::Printf(TEXT("wp_%02d"),i);
			if (!CheckName(Point->PointName))
			{
				break;
			}
		}
		Point->RegisterComponent();
		TArray<UStalkerWayPointComponent*> SelectedPoints;
		GetSelectedPoint(SelectedPoints);
		Points.Add(Point);
		if (AutoLink&& SelectedPoints.Num())
		{
			SelectedPoints[0]->AddLink(Point);
		}
		return Point;
	}

	return nullptr;
}

void AStalkerWayObject::RemoveSelect()
{
	for (UStalkerWayPointComponent* Point : Points)
	{
		Point->RemoveSelectLink();
	}
	for (int32 i = 0; i < Points.Num(); i++)
	{
		if (!Points[i]->bIsMain && Points[i]->bIsSelected)
		{
			Points[i]->DestroyComponent();
			Points.RemoveAt(i);
			i = 0;
		}
	}
}

void AStalkerWayObject::InvertLinkSelect()
{
	TArray<UStalkerWayPointComponent*> SelectedPoints;
	GetSelectedPoint(SelectedPoints);
	for (int32 IndexA = 0; IndexA < SelectedPoints.Num() - 1; IndexA++)
	{
		for (int32 IndexB = IndexA + 1; IndexB < SelectedPoints.Num(); IndexB++)
		{
			UStalkerWayPointComponent* A = SelectedPoints[IndexA];
			UStalkerWayPointComponent* B = SelectedPoints[IndexB];
			check(A != B);
			A->InvertLink(B);
		}
	}
}

void AStalkerWayObject::RemoveLinkSelect()
{
	TArray<UStalkerWayPointComponent*> SelectedPoints;
	GetSelectedPoint(SelectedPoints);
	for (int32 IndexA = 0; IndexA < SelectedPoints.Num(); IndexA++)
	{
		for (int32 IndexB = 0; IndexB < SelectedPoints.Num(); IndexB++)
		{
			UStalkerWayPointComponent* A = SelectedPoints[IndexA];
			UStalkerWayPointComponent* B = SelectedPoints[IndexB];
			if (A != B)
			{
				A->RemoveLink(B);
			}
		}
	}
}

void AStalkerWayObject::Convert1LinkSelect()
{
	TArray<UStalkerWayPointComponent*> SelectedPoints;
	GetSelectedPoint(SelectedPoints);

	for (int32 IndexA = 0; IndexA < SelectedPoints.Num()-1; IndexA++)
	{
		for (int32 IndexB = IndexA +1; IndexB < SelectedPoints.Num(); IndexB++)
		{
			UStalkerWayPointComponent* A = SelectedPoints[IndexA];
			UStalkerWayPointComponent* B = SelectedPoints[IndexB];
			check(A!=B);
			A->Convert1Link(B);
		}
	}
}

void AStalkerWayObject::Convert2LinkSelect()
{
	TArray<UStalkerWayPointComponent*> SelectedPoints;
	GetSelectedPoint(SelectedPoints);
	for (int32 IndexA = 0; IndexA < SelectedPoints.Num() - 1; IndexA++)
	{
		for (int32 IndexB = IndexA + 1; IndexB < SelectedPoints.Num(); IndexB++)
		{
			UStalkerWayPointComponent* A = SelectedPoints[IndexA];
			UStalkerWayPointComponent* B = SelectedPoints[IndexB];
			check(A != B);
			A->Convert2Link(B);
		}
	}
}

void AStalkerWayObject::Add1LinkSelect()
{
	TArray<UStalkerWayPointComponent*> SelectedPoints;
	GetSelectedPoint(SelectedPoints);
	for (int32 IndexA = 0; IndexA < SelectedPoints.Num() - 1; IndexA++)
	{
		for (int32 IndexB = IndexA + 1; IndexB < SelectedPoints.Num(); IndexB++)
		{
			UStalkerWayPointComponent* A = SelectedPoints[IndexA];
			UStalkerWayPointComponent* B = SelectedPoints[IndexB];
			check(A != B);
			A->AddLink(B);
		}
	}
}

void AStalkerWayObject::Add2LinkSelect()
{
	TArray<UStalkerWayPointComponent*> SelectedPoints;
	GetSelectedPoint(SelectedPoints);
	for (int32 IndexA = 0; IndexA < SelectedPoints.Num() - 1; IndexA++)
	{
		for (int32 IndexB = IndexA + 1; IndexB < SelectedPoints.Num(); IndexB++)
		{
			UStalkerWayPointComponent* A = SelectedPoints[IndexA];
			UStalkerWayPointComponent* B = SelectedPoints[IndexB];
			check(A != B);
			A->AddDoubleLink(B);
		}
	}
}

bool AStalkerWayObject::CheckName(const FString& Name)
{
	int32 Count = 0;
	for (int32 IndexA = 0; IndexA < Points.Num(); IndexA++)
	{
		UStalkerWayPointComponent* A = Points[IndexA];
		if (A->PointName.Compare(Name, ESearchCase::IgnoreCase) == 0)
		{
			Count++;
		}
	}
	return Count>1;
}

void AStalkerWayObject::CalculateIndex()
{
	for (int32 i = 0; i < Points.Num(); i++)
	{
		Points[i]->Index = i;
	}
}

void AStalkerWayObject::Destroyed()
{
	Super::Destroyed();
	if (!IsValid(GetWorld()) || GetWorld()->IsGameWorld())
	{
		return;
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
}

bool AStalkerWayObject::Modify(bool bAlwaysMarkDirty /*= true*/)
{
	bool bResult = Super::Modify(bAlwaysMarkDirty);
	if (!IsValid(GetWorld())|| GetWorld()->IsGameWorld() || !bAlwaysMarkDirty)
	{
		return bResult;
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

void AStalkerWayObject::PostEditUndo()
{
	Super::PostEditUndo();
	if (!IsValid(GetWorld()))
	{
		return;
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
}

void AStalkerWayObject::GetSelectedPoint(TArray<UStalkerWayPointComponent*>& SelectedPoints)
{
	SelectedPoints.Empty();
	for (int32 i = 0; i < Points.Num(); i++)
	{
		if ( Points[i]->bIsSelected)
		{
			SelectedPoints.AddUnique(Points[i]);
		}
	}
}
