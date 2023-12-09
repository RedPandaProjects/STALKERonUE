#include "FRBMKLevelFactory.h"

#include "InstancedFoliageActor.h"
#include "ImporterFactory/XRayLevelImportOptions.h"


FRBMKLevelFactory::FRBMKLevelFactory(UObject* InParentPackage, EObjectFlags InFlags):EngineFactory(InParentPackage, InFlags),ParentPackage(InParentPackage),ObjectFlags(InFlags)
{

}

FRBMKLevelFactory::~FRBMKLevelFactory()
{

}

bool FRBMKLevelFactory::ImportLevel(const FString& FileName,UXRayLevelImportOptions&LevelImportOptions)
{
	FWorldContext* WorldContext = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
	if (!WorldContext)
		return false;
	UWorld* World = WorldContext->World();
	if (!IsValid(World))
		return false;

	GXRayObjectLibrary->AngleSmooth = LevelImportOptions.AngleNormalSmoth;
	switch (LevelImportOptions.ObjectImportGameFormat)
	{
	default:
		GXRayObjectLibrary->LoadAsGame = EGame::NONE;
		break;
	case EXRayObjectImportGameFormat::SOC:
		GXRayObjectLibrary->LoadAsGame = EGame::SHOC;
		break;
	case EXRayObjectImportGameFormat::CS_COP:
		GXRayObjectLibrary->LoadAsGame = EGame::COP;
		break;
	}

	GXRayObjectLibrary->ReloadObjects();

	FRBMKScene CurrentScene;
	GRBMKScene = &CurrentScene;

	if (!CurrentScene.Load(FileName))
	{
		return false;
	}

	auto  ExportToWorldLamda = [&CurrentScene,World,this,&LevelImportOptions](bool NeedExport,ERBMKSceneObjectType ObjectType)
	{
		if (NeedExport)
		{
			if(FRBMKSceneToolBase* Tool = CurrentScene.GetTool(ObjectType))
			{
				Tool->ExportToWorld(World,ObjectFlags,LevelImportOptions);
			}
		}
	};


	ExportToWorldLamda(LevelImportOptions.ImportStaticMeshes,ERBMKSceneObjectType::StaticMesh);
	ExportToWorldLamda(LevelImportOptions.ImportWayObjects,ERBMKSceneObjectType::Way);
	ExportToWorldLamda(LevelImportOptions.ImportAIMap,ERBMKSceneObjectType::AIMap);
	ExportToWorldLamda(LevelImportOptions.ImportSpawnObjects,ERBMKSceneObjectType::SpawnPoint);
	ExportToWorldLamda(LevelImportOptions.ImportParticles,ERBMKSceneObjectType::ParticleSystem);
	ExportToWorldLamda(LevelImportOptions.ImportWallmark,ERBMKSceneObjectType::Wallmark);
	ExportToWorldLamda(LevelImportOptions.ImportDetails,ERBMKSceneObjectType::DetailObject);

	if(LevelImportOptions.ImportDetails||(LevelImportOptions.ImportStaticMeshes&&LevelImportOptions.ImportMultipleUsageMeshesAsFoliage))
	{
		for (TActorIterator<AInstancedFoliageActor> It(World); It; ++It)
		{
			AInstancedFoliageActor* IFA = *It;
			for(auto&[Key,Value]:IFA->GetAllInstancesFoliageType())
			{
				Value->Refresh(true,false);
			}
		}
	}


	GRBMKScene = nullptr;
	return true;
}
