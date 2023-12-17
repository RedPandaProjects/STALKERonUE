#include "FRBMKLevelFactory.h"

#include "InstancedFoliageActor.h"
#include "ImporterFactory/RBMKLevelImportOptions.h"


FRBMKLevelFactory::FRBMKLevelFactory(UObject* InParentPackage, EObjectFlags InFlags):EngineFactory(InParentPackage, InFlags),ParentPackage(InParentPackage),ObjectFlags(InFlags)
{

}

FRBMKLevelFactory::~FRBMKLevelFactory()
{

}

bool FRBMKLevelFactory::ImportLevel(const FString& FileName,URBMKLevelImportOptions&LevelImportOptions)
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
	
	FScopedSlowTask Progress(7, FText::FromString(FString::Printf(TEXT("Impot Level:%s"),*FileName)), true);
	Progress.MakeDialog(true);
	
	Progress.EnterProgressFrame(0, FText::FromString(FString::Printf(TEXT("Loading ..."))));
	GXRayObjectLibrary->ReloadObjects();

	FRBMKScene CurrentScene;
	GRBMKScene = &CurrentScene;
	if (!CurrentScene.Load(FileName))
	{
		return false;
	}
	
	auto  ExportToWorldLambda = [&CurrentScene,World,this,&LevelImportOptions](bool NeedExport,ERBMKSceneObjectType ObjectType)
	{
		if (NeedExport)
		{
			if(FRBMKSceneToolBase* Tool = CurrentScene.GetTool(ObjectType))
			{
				Tool->ExportToWorld(World,ObjectFlags,LevelImportOptions);
			}
		}
	};

	
	Progress.EnterProgressFrame(1, FText::FromString(FString::Printf(TEXT("Impoting StaticMeshes ..."))));
	ExportToWorldLambda(LevelImportOptions.ImportStaticMeshes,ERBMKSceneObjectType::StaticMesh);
	Progress.EnterProgressFrame(1, FText::FromString(FString::Printf(TEXT("Impoting Ways ..."))));
	ExportToWorldLambda(LevelImportOptions.ImportWayObjects,ERBMKSceneObjectType::Way);
	Progress.EnterProgressFrame(1, FText::FromString(FString::Printf(TEXT("Impoting AIMap ..."))));
	ExportToWorldLambda(LevelImportOptions.ImportAIMap,ERBMKSceneObjectType::AIMap);
	Progress.EnterProgressFrame(1, FText::FromString(FString::Printf(TEXT("Impoting SpawnPoints ..."))));
	ExportToWorldLambda(LevelImportOptions.ImportSpawnObjects,ERBMKSceneObjectType::SpawnPoint);
	Progress.EnterProgressFrame(1, FText::FromString(FString::Printf(TEXT("Impoting ParticleSystems ..."))));
	ExportToWorldLambda(LevelImportOptions.ImportParticles,ERBMKSceneObjectType::ParticleSystem);
	Progress.EnterProgressFrame(1, FText::FromString(FString::Printf(TEXT("Impoting Decals ..."))));
	ExportToWorldLambda(LevelImportOptions.ImportWallmark,ERBMKSceneObjectType::Wallmark);
	Progress.EnterProgressFrame(1, FText::FromString(FString::Printf(TEXT("Impoting DetailObjects ..."))));
	ExportToWorldLambda(LevelImportOptions.ImportDetails,ERBMKSceneObjectType::DetailObject);

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
