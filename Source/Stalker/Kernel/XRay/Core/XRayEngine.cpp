#include "XRayEngine.h"
#include "XRayDevice.h"
#include "../Render/Interface/XRayRenderInterface.h"
#include "../Render/Interface/XRayRenderFactory.h"
#include "../Render/Interface/XRayDUInterface.h"
#include "../Render/Interface/XRayDebugRender.h"
#include "../Render/Interface/UI/XRayUIRender.h"
#include "XRayInput.h"
#include "../../StalkerEngineManager.h"
#include "XRayConsole.h"
#include "Entities/Levels/Proxy/StalkerProxy.h"
#include "Resources/StalkerResourcesManager.h"
#include "../../Unreal/WorldSettings/StalkerWorldSettings.h"
#include "Resources/CFrom/StalkerCForm.h"
#include "Resources/PhysicalMaterial/StalkerPhysicalMaterialsManager.h"
#include "Resources/PhysicalMaterial/StalkerPhysicalMaterial.h"
#include "Resources/AIMap/StalkerAIMap.h"
#include "Resources/Spawn/StalkerGameSpawn.h"
#include "Entities/Player/Character/StalkerPlayerCharacter.h"
THIRD_PARTY_INCLUDES_START
#include "XrCDB/xr_area.h"
THIRD_PARTY_INCLUDES_END
XRayRenderFactory GRenderFactory;
XRayDUInterface  GDUInterface;
XRayDebugRender GDebugRender;
XRayEngine::XRayEngine()
{

}

void XRayEngine::Initialize()
{
	Device = new XRayDevice;
	Console = new  XRayConsole;

	XRayInput* MyInput = new XRayInput;
	GStalkerEngineManager->SetInput(MyInput);
	GXRayInput = MyInput;
	::Render = &GRenderInterface;
	::RenderFactory = &GRenderFactory;
	::DU = &GDUInterface;
	UIRender = &GXRayUIRender;
	DRender = &GDebugRender;
	XRayEngineInterface::Initialize();
}

void XRayEngine::Destroy()
{
	GStalkerEngineManager->SetInput(nullptr);
	XRayEngineInterface::Destroy();
	delete GXRayInput;
	delete Console;
	delete Device;
	Console	= nullptr;
	Device = nullptr;
	GXRayInput = nullptr;
}

void XRayEngine::Destroy(class XRayUnrealProxyInterface*InProxy)
{
	if (!InProxy)
	{
		return;
	}
	AStalkerProxy * StalkerProxy = InProxy->CastToStalkerProxy();
	StalkerProxy->Destroy();
}

class ILevelGraph* XRayEngine::GetLevelGraph(const char* Name)
{
	return GStalkerEngineManager->GetLevelGraph(Name);
}

class IGameGraph* XRayEngine::GetGameGraph()
{
	UStalkerGameSpawn*GameSpawn =  GStalkerEngineManager->GetResourcesManager()->GetGameSpawn();
	if (IsValid(GameSpawn))
	{
		GameSpawn->GameGraph.ResetEnabled();
		return &GameSpawn->GameGraph;
	}
	return nullptr;
}

IReader XRayEngine::GetGameSpawn()
{
	UStalkerGameSpawn* GameSpawn = GStalkerEngineManager->GetResourcesManager()->GetGameSpawn();
	if (IsValid(GameSpawn))
	{
		return  { GameSpawn->SpawnData.GetData(),GameSpawn->SpawnData.Num() };;
	}
	return {nullptr,0};
}

bool XRayEngine::LoadWorld(const char* Name)
{
	return GStalkerEngineManager->LoadWorld(Name);
}

class XRayUnrealProxyInterface* XRayEngine::CreateUnrealProxy()
{
	FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
	SpawnParameters.ObjectFlags = EObjectFlags::RF_Transient;
	AStalkerProxy* Result = GWorld->SpawnActor< AStalkerProxy>(SpawnParameters);
	return Result;
}
void XRayEngine::LoadCFormFormCurrentWorld(class CObjectSpace& ObjectSpace, CDB::build_callback build_callback)
{
	UWorld*World = GWorld;
	check(World);
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	check(StalkerWorldSettings);
	UStalkerCForm* CForm = StalkerWorldSettings->GetCForm();
	check(IsValid(CForm)); 
	check (IsValid(GStalkerEngineManager->GetPhysicalMaterialsManager()->DefaultPhysicalMaterial));
	static_assert(sizeof(FVector3f)==sizeof(Fvector));

	TMap<uint32,uint32>		CFormMaterialID2GlobalID;
	for (uint32 Index=0; Index <static_cast<uint32>(CForm->Name2ID.Num()); Index++)
	{
		FString& Name = CForm->Name2ID[Index];
		shared_str LegacyName = TCHAR_TO_ANSI(*Name);
		int32 GlobalIndex =  GStalkerEngineManager->GetPhysicalMaterialsManager()->Names.IndexOfByKey(LegacyName);
		if (GlobalIndex == INDEX_NONE)
		{
			UE_LOG(LogStalker,Error,TEXT("Can't found material %s in time build cform"),*Name);
			GlobalIndex = GStalkerEngineManager->GetPhysicalMaterialsManager()->PhysicalMaterials.IndexOfByKey(GStalkerEngineManager->GetPhysicalMaterialsManager()->DefaultPhysicalMaterial);
			check(GlobalIndex!= INDEX_NONE);
		}
		CFormMaterialID2GlobalID.Add(Index,static_cast<uint32>(GlobalIndex));
	}

	TArray<CDB::TRI>		LegacyCFormTriangles;
	LegacyCFormTriangles.Empty(CForm->Triangles.Num());
	for (FStalkerCFormTriangle& Triangle : CForm->Triangles)
	{
		LegacyCFormTriangles.AddDefaulted();
		CDB::TRI&LegacyTriangle = LegacyCFormTriangles.Last();
		LegacyTriangle.material= CFormMaterialID2GlobalID[Triangle.MaterialIndex];
		LegacyTriangle.verts[0] = Triangle.VertexIndex0;
		LegacyTriangle.verts[1] = Triangle.VertexIndex1;
		LegacyTriangle.verts[2] = Triangle.VertexIndex2;
	}

	TArray<Fvector>		LegacyCFormVertices;
	LegacyCFormVertices.Empty(CForm->Vertices.Num());
	for (FVector3f& Vertex : CForm->Vertices)
	{
		LegacyCFormVertices.Add(StalkerMath::UnrealLocationToXRay(Vertex));
	}
	hdrCFORM LegacyCFormHeader={};
	LegacyCFormHeader.aabb.invalidate();
	LegacyCFormHeader.aabb.modify(StalkerMath::UnrealLocationToXRay(CForm->AABB.Max));
	LegacyCFormHeader.aabb.modify(StalkerMath::UnrealLocationToXRay(CForm->AABB.Min));
	LegacyCFormHeader.vertcount = LegacyCFormVertices.Num();
	LegacyCFormHeader.facecount = LegacyCFormTriangles.Num();
	LegacyCFormHeader.version = CFORM_CURRENT_VERSION;

	ObjectSpace.Create(LegacyCFormVertices.GetData(), LegacyCFormTriangles.GetData(), LegacyCFormHeader, build_callback);
}

EXRayWorldStatus XRayEngine::GetWorldStatus()
{
	switch (GStalkerEngineManager->GetWorldStatus())
	{
	default:
		return EXRayWorldStatus::Failure;
		break;
	case EStalkerWorldStatus::None:
		return EXRayWorldStatus::None;
		break;
	case EStalkerWorldStatus::Ready:
		return EXRayWorldStatus::Ready;
		break;
	case EStalkerWorldStatus::Loading:
		return EXRayWorldStatus::Loading;
		break;
	case EStalkerWorldStatus::Failure:
		return EXRayWorldStatus::Failure;
		break;
	} 
}

class XRayUnrealProxyInterface* XRayEngine::GetUnrealPlayerCharacter()
{
	if (IsValid(GWorld))
	{
		if (APlayerController* PC = GWorld->GetFirstPlayerController())
		{
			return Cast<AStalkerPlayerCharacter>(PC->GetPawn());
		}
	}

	return nullptr;
}

shared_str XRayEngine::GetUnrealVersion()
{
	return TCHAR_TO_ANSI(FApp::GetBuildVersion()); 
}

void XRayEngine::LoadDefaultWorld()
{	
	GStalkerEngineManager->LoadDefaultWorld();
	
}
