#include "RBMKEngine.h"
#include "RBMKDevice.h"
#include "../Render/Interface/XRayRenderInterface.h"
#include "../Render/Interface/XRayRenderFactory.h"
#include "../Render/Interface/XRayDUInterface.h"
#include "../Render/Interface/XRayDebugRender.h"
#include "../Render/Interface/UI/XRayUIRender.h"
#include "RBMKInput.h"
#include "../../StalkerEngineManager.h"
#include "RBMKConsole.h"
#include "Entities/Levels/Proxy/StalkerProxy.h"
#include "Resources/StalkerResourcesManager.h"
#include "../../Unreal/WorldSettings/StalkerWorldSettings.h"
#include "Entities/Levels/Environment/StalkerEnvironment.h"
#include "Resources/CFrom/StalkerCForm.h"
#include "Resources/PhysicalMaterial/StalkerPhysicalMaterialsManager.h"
#include "Resources/PhysicalMaterial/StalkerPhysicalMaterial.h"
#include "Resources/AIMap/StalkerAIMap.h"
#include "Resources/Spawn/StalkerGameSpawn.h"
#include "Entities/Player/Character/StalkerPlayerCharacter.h"
#include "Resources/Sound/StalkerSoundManager.h"
THIRD_PARTY_INCLUDES_START
#include "XrCDB/xr_area.h"
THIRD_PARTY_INCLUDES_END
XRayRenderFactory GRenderFactory;
XRayDUInterface  GDUInterface;
XRayDebugRender GDebugRender;
FRBMKEngine::FRBMKEngine()
{

}

void FRBMKEngine::Initialize()
{
	Device = new FRBMKDevice;
	Console = new  FRBMKConsole;

	FRBMKInput* MyInput = new FRBMKInput;
	GStalkerEngineManager->SetInput(MyInput);
	GXRayInput = MyInput;
	::Render = &GRenderInterface;
	::RenderFactory = &GRenderFactory;
	::DU = &GDUInterface;
	UIRender = &GXRayUIRender;
	DRender = &GDebugRender;
	IRBMKEngine::Initialize();
}

void FRBMKEngine::Destroy()
{
	GStalkerEngineManager->SetInput(nullptr);
	IRBMKEngine::Destroy();
	delete GXRayInput;
	delete Console;
	delete Device;
	Console	= nullptr;
	Device = nullptr;
	GXRayInput = nullptr;
}

void FRBMKEngine::OnFrame()
{
	IRBMKEngine::OnFrame();
}

void FRBMKEngine::Destroy(class IRBMKUnrealProxy*InProxy)
{
	if (!InProxy)
	{
		return;
	}
	AStalkerProxy * StalkerProxy =  reinterpret_cast<	AStalkerProxy *>(InProxy->CastUnrealObject(ERBMKUnrealObjectType::StalkerProxy));
	StalkerProxy->Destroy();
}

class ILevelGraph* FRBMKEngine::GetLevelGraph(const char* Name)
{
	return GStalkerEngineManager->GetLevelGraph(Name);
}

class IGameGraph* FRBMKEngine::GetGameGraph()
{
	UStalkerGameSpawn*GameSpawn =  GStalkerEngineManager->GetResourcesManager()->GetGameSpawn();
	if (IsValid(GameSpawn))
	{
		GameSpawn->GameGraph.ResetEnabled();
		return &GameSpawn->GameGraph;
	}
	return nullptr;
}

IReader FRBMKEngine::GetGameSpawn()
{
	UStalkerGameSpawn* GameSpawn = GStalkerEngineManager->GetResourcesManager()->GetGameSpawn();
	if (IsValid(GameSpawn))
	{
		return  { GameSpawn->SpawnData.GetData(),GameSpawn->SpawnData.Num() };;
	}
	return {nullptr,0};
}

bool FRBMKEngine::LoadWorld(const char* Name)
{
	return GStalkerEngineManager->LoadWorld(Name);
}

class IRBMKUnrealProxy* FRBMKEngine::CreateUnrealProxy()
{
#if WITH_EDITORONLY_DATA
	check(GWorld&&GWorld->IsGameWorld());
#endif
	FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
	SpawnParameters.ObjectFlags = EObjectFlags::RF_Transient;
	AStalkerProxy* Result = GWorld->SpawnActor< AStalkerProxy>(SpawnParameters);
	return Result;
}
void FRBMKEngine::LoadCFormFormCurrentWorld(class CObjectSpace& ObjectSpace, CDB::build_callback build_callback)
{
#if WITH_EDITORONLY_DATA
	check(GWorld&&GWorld->IsGameWorld());
#endif
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
		if (GStalkerEngineManager->GetPhysicalMaterialsManager()->GetMaterialByID(GlobalIndex)->Flags.is(SGameMtl::flDynamic))
		{
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
		LegacyCFormVertices.Add(StalkerMath::UnrealLocationToRBMK(Vertex));
	}
	hdrCFORM LegacyCFormHeader={};
	LegacyCFormHeader.aabb.invalidate();
	LegacyCFormHeader.aabb.modify(StalkerMath::UnrealLocationToRBMK(CForm->AABB.Max));
	LegacyCFormHeader.aabb.modify(StalkerMath::UnrealLocationToRBMK(CForm->AABB.Min));
	LegacyCFormHeader.vertcount = LegacyCFormVertices.Num();
	LegacyCFormHeader.facecount = LegacyCFormTriangles.Num();
	LegacyCFormHeader.version = CFORM_CURRENT_VERSION;

	ObjectSpace.Create(LegacyCFormVertices.GetData(), LegacyCFormTriangles.GetData(), LegacyCFormHeader, build_callback);
}

ERBMKWorldStatus FRBMKEngine::GetWorldStatus()
{
	switch (GStalkerEngineManager->GetWorldStatus())
	{
	default:
		return ERBMKWorldStatus::Failure;
		break;
	case EStalkerWorldStatus::None:
		return ERBMKWorldStatus::None;
		break;
	case EStalkerWorldStatus::Ready:
		return ERBMKWorldStatus::Ready;
		break;
	case EStalkerWorldStatus::Loading:
		return ERBMKWorldStatus::Loading;
		break;
	case EStalkerWorldStatus::Failure:
		return ERBMKWorldStatus::Failure;
		break;
	} 
}

class IRBMKUnrealProxy* FRBMKEngine::GetUnrealPlayerCharacter()
{
	if (IsValid(GWorld))
	{
#if WITH_EDITORONLY_DATA
		check(GWorld->IsGameWorld());
#endif
		if (APlayerController* PC = GWorld->GetFirstPlayerController())
		{
			return Cast<AStalkerPlayerCharacter>(PC->GetPawn());
		}
	}

	return nullptr;
}

shared_str FRBMKEngine::GetUnrealVersion()
{
	return TCHAR_TO_ANSI(FApp::GetBuildVersion()); 
}

IRBMKSoundManager* FRBMKEngine::GetSoundManager()
{
	return GStalkerEngineManager->GetResourcesManager()->GetSoundManager();
}

IRBMKEnvironment* FRBMKEngine::GetEnvironment()
{
#if WITH_EDITORONLY_DATA
	check(GWorld&&GWorld->IsGameWorld());
#endif
	if(AStalkerWorldSettings*WorldSettings = Cast<AStalkerWorldSettings>(GWorld->GetWorldSettings()))
	{
		if(!WorldSettings->Environment)
		{
			UE_DEBUG_BREAK();
		}
		return WorldSettings->Environment;
	}
	return nullptr;
}

void FRBMKEngine::LoadDefaultWorld()
{	
	GStalkerEngineManager->LoadDefaultWorld();
	
}

void FRBMKEngine::Exit()
{	
	GEngine->Exec(nullptr, TEXT( "Exit" ) );
}
