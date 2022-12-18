#include "StalkerDebugRender.h"

AStalkerDebugRender::AStalkerDebugRender()
{
	PrimaryActorTick.bCanEverTick = true;
	LineBatchComponent = CreateDefaultSubobject<ULineBatchComponent>(TEXT("LineBatchComponent"));
	LineBatchComponent->SetupAttachment(GetRootComponent());
	PrimaryActorTick.TickInterval = 1.f/30.f;
}

void AStalkerDebugRender::BeginPlay()
{
	CurrentFrame = Device->dwFrame;
	Super::BeginPlay();
}

void AStalkerDebugRender::AddTriangle(const FVector& InP1, const FVector& InP2, const FVector& InP3, const FColor& Color)
{
	auto InFaces = Faces.Find(Color);
	if (!InFaces)
	{
		InFaces = &Faces.Add(Color);
	}
	InFaces->Add(InP1);
	InFaces->Add(InP2);
	InFaces->Add(InP3);
}

void AStalkerDebugRender::Tick(float DeltaTime)
{
	if (CurrentFrame < Device->dwFrame)
	{
		LineBatchComponent->Flush();
		for (auto& [Key, Value] : Faces)
		{
			Value.Empty(Value.Num());
		}
		Device->seqRenderDebug.Process(rp_RenderDebug);
		for (auto& [Key, Value] : Faces)
		{
			if(Value.Num()==0)continue;
			TArray<int32> MeshIndices;
			for(int32 i=0;i<Value.Num();i++)
			{
				MeshIndices.Add(i);
			}
			LineBatchComponent->DrawMesh(Value, MeshIndices,Key,0,1.f);

			Value.Empty(Value.Num());
		}
		CurrentFrame = Device->dwFrame;
	}
	Super::Tick(DeltaTime);
}

