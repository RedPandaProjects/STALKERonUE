#include "StalkerDebugRender.h"
#include "Kernel/StalkerEngineManager.h"

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
//	Fvector du_box_vertices[] =
//	{
//		{-0.5f, -0.5f, -0.5f},
//		{-0.5f, +0.5f, -0.5f},
//		{+0.5f, +0.5f, -0.5f},
//		{+0.5f, -0.5f, -0.5f},
//		{-0.5f, +0.5f, +0.5f},
//		{-0.5f, -0.5f, +0.5f},
//		{+0.5f, +0.5f, +0.5f},
//		{+0.5f, -0.5f, +0.5f},
//	};
//	WORD du_box_lines[] = {
//		0, 1,	1, 2,	2, 3,	3, 0,
//		4, 5,	5, 7,	6, 7,	6, 4,
//		0, 5,	1, 4,	2, 6,	3, 7
//	};
//	for (auto& shape : GXRayEngineManager->DebugShapes)
//	{
//		for (int32 i = 0; i < _countof(du_box_lines);)
//		{
//			Fmatrix FTransform = shape.data.box;
//			Fvector V1 = du_box_vertices[du_box_lines[i++]];
//			FTransform.transform_tiny(V1);
//			Fvector V2 = du_box_vertices[du_box_lines[i++]];
//			FTransform.transform_tiny(V2);
//			DrawDebugLine(GetWorld(), FVector(StalkerMath::RBMKLocationToUnreal(V1)), FVector(StalkerMath::RBMKLocationToUnreal(V2)), FColor::Red,false, 10000.f);
//		}
//	}
//	GXRayEngineManager->DebugShapes.Empty();
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

