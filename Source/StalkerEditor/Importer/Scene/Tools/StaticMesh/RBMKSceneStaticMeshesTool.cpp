#include "RBMKSceneStaticMeshesTool.h"

#include "FoliageType_InstancedStaticMesh.h"
#include "InstancedFoliage.h"
#include "InstancedFoliageActor.h"
#include "StalkerEditorManager.h"
#include "Importer/FRBMKEngineFactory.h"
#include "Importer/Scene/Entitys/StaticMesh//RBMKSceneStaticMesh.h"
#include "Kernel/Unreal/GameSettings/StalkerGameSettings.h"
#include "Resources/StaticMesh/StalkerStaticMeshAssetUserData.h"

FRBMKSceneStaticMeshesTool::FRBMKSceneStaticMeshesTool():FRBMKSceneObjectsToolBase(ERBMKSceneObjectType::StaticMesh)
{
}


FRBMKSceneStaticMeshesTool::~FRBMKSceneStaticMeshesTool()
{

}

bool FRBMKSceneStaticMeshesTool::LoadLTX(CInifile& INIFile)
{
	uint32 Version = INIFile.r_u32("main", "version");
    if(Version != ToolVersion )
    {
        UE_LOG(LogXRayImporter,Error,TEXT("%s tools: Unsupported version."),ClassName());
    	return false;
    }
	FRBMKSceneObjectsToolBase::LoadLTX		(INIFile);
    return true;
}

bool FRBMKSceneStaticMeshesTool::LoadStream(IReader& F)
{
	uint16 Version 	= 0;
    if(F.r_chunk(ChunkVersion,&Version))
    {
        if( Version!=ToolVersion )
        {
        	UE_LOG(LogXRayImporter,Error,TEXT("%s tools: Unsupported version."),ClassName());
            return false;
        }
    }

    return FRBMKSceneObjectsToolBase::LoadStream(F);
}

bool FRBMKSceneStaticMeshesTool::LoadSelection(IReader& F)
{
	uint16 Version 	= 0;
    if(!ensure(F.r_chunk(ChunkVersion,&Version)))
    {
	    return false;
    }
    if( Version!=ToolVersion)
    {
		UE_LOG(LogXRayImporter,Error,TEXT("%s tools: Unsupported version."),ClassName());
        return false;
    }

	return FRBMKSceneObjectsToolBase::LoadSelection(F);
}

TSharedPtr<FRBMKSceneObjectBase> FRBMKSceneStaticMeshesTool::CreateObject(const FString& Name)
{
    return MakeShared<FRBMKSceneStaticMesh>(Name);
}

void FRBMKSceneStaticMeshesTool::ExportToWorld(UWorld*World,EObjectFlags InFlags,const URBMKLevelImportOptions&LevelImportOptions)
{
	FRBMKEngineFactory EngineFactory(nullptr,InFlags);


	TMap<CEditableObject*,TArray<FTransform>> MeshesToFoliage;
	for (TSharedPtr<FRBMKSceneObjectBase>& Object : Objects)
	{
		if(FRBMKSceneStaticMesh* SceneObject = reinterpret_cast<FRBMKSceneStaticMesh*>(Object->QueryInterface(ERBMKSceneObjectType::StaticMesh)))
		{
			CEditableObject* EditableObject = SceneObject->GetReferenceObject();
			if (EditableObject)
			{
				if(EditableObject->IsMUStatic()&&LevelImportOptions.ImportMultipleUsageMeshesAsFoliage)
				{
					MeshesToFoliage.FindOrAdd(EditableObject).Add(SceneObject->GetTransform());
				}
				else
				{
					bool NeedDivideSubObject = false;

					for(const FString&Path: GetDefault<UStalkerGameSettings>()->MeshesDivideSubObjectWhenImportLevel)
					{
						if(SceneObject->ReferenceName.Find(Path) == 0)
						{
							NeedDivideSubObject = true;
						}
					}

					if(LevelImportOptions.DivideSubObject&&NeedDivideSubObject)
					{
						TArray<UStaticMesh*> StaticMeshes = EngineFactory.ImportObjectAsStaticMeshWithDivideSubObject(EditableObject);
						int32 Index = 0;
						for(UStaticMesh* StaticMesh:StaticMeshes)
						{
							AStaticMeshActor* StaticMeshActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(),SceneObject->GetTransform());
							StaticMeshActor->GetStaticMeshComponent()->SetStaticMesh(StaticMesh);
							StaticMeshActor->SetFolderPath(TEXT("StaticMeshes"));
							FString Label = EditableObject->GetName();
							Label.ReplaceCharInline(TEXT('\\'), TEXT('/'));
							Label += FString::Printf(TEXT("_part%d"),Index++);
							StaticMeshActor->SetActorLabel(Label);
							if(UStalkerStaticMeshAssetUserData* StalkerStaticMeshAssetUserData = StaticMesh->GetAssetUserData<UStalkerStaticMeshAssetUserData>())
							{
								StaticMeshActor->GetStaticMeshComponent()->SetVisibility(!StalkerStaticMeshAssetUserData->IsOnlyCollision);
							}
						}
					}
					else
					{
						UStaticMesh* StaticMesh = EngineFactory.ImportObjectAsStaticMesh(EditableObject, true);
						if (StaticMesh)
						{
							AStaticMeshActor* StaticMeshActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(),SceneObject->GetTransform());
							StaticMeshActor->GetStaticMeshComponent()->SetStaticMesh(StaticMesh);
							StaticMeshActor->SetFolderPath(TEXT("StaticMeshes"));
							FString Label = EditableObject->GetName();
							Label.ReplaceCharInline(TEXT('\\'), TEXT('/'));
							StaticMeshActor->SetActorLabel(Label);
							if(UStalkerStaticMeshAssetUserData* StalkerStaticMeshAssetUserData = StaticMesh->GetAssetUserData<UStalkerStaticMeshAssetUserData>())
							{
								StaticMeshActor->GetStaticMeshComponent()->SetVisibility(!StalkerStaticMeshAssetUserData->IsOnlyCollision);
							}
						}
					}
					
				}
			}
		}
		
	}
	 auto AddInstances = [](UWorld* World, const UFoliageType* InFoliageType, const TArray<FTransform>& InTransforms)
	{
		TMap<AInstancedFoliageActor*, TArray<const FFoliageInstance*>> InstancesToAdd;
		TArray<FFoliageInstance> FoliageInstances;
		FoliageInstances.Reserve(InTransforms.Num()); // Reserve 

		for (const FTransform& InstanceTransform : InTransforms)
		{
			AInstancedFoliageActor* IFA = AInstancedFoliageActor::Get(World, true, World->PersistentLevel, InstanceTransform.GetLocation());
			FFoliageInstance FoliageInstance;
			FoliageInstance.Location = InstanceTransform.GetLocation();
			FoliageInstance.Rotation = InstanceTransform.GetRotation().Rotator();
			FoliageInstance.DrawScale3D = FVector3f(InstanceTransform.GetScale3D());

			FoliageInstances.Add(FoliageInstance);
			InstancesToAdd.FindOrAdd(IFA).Add(&FoliageInstances[FoliageInstances.Num() - 1]);
		}

		for (const auto& Pair : InstancesToAdd)
		{
			FFoliageInfo* TypeInfo = nullptr;
			if (const UFoliageType* FoliageType = Pair.Key->AddFoliageType(InFoliageType, &TypeInfo))
			{
				TypeInfo->AddInstances(FoliageType, Pair.Value);
			}
		}
	};

	for(auto&[Mesh,Transforms] :MeshesToFoliage)
	{
		if(UStaticMesh* StaticMesh = EngineFactory.ImportObjectAsStaticMesh(Mesh, true))
		{
			FString FileName = Mesh->GetName();
			FileName.ReplaceCharInline(TEXT('\\'), TEXT('/'));
			FString NewObjectPath = UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("Maps") / TEXT("Meshes") / FPaths::GetBaseFilename(FileName, false))+TEXT("_FoliageType");
			UFoliageType_InstancedStaticMesh *FoliageType = nullptr;
			if (FRBMKEngineFactory::LoadOrCreateOrOverwriteAsset(NewObjectPath,InFlags,FoliageType))
			{
			    FAssetRegistryModule::AssetCreated(FoliageType);
				FoliageType->BodyInstance.SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
				FoliageType->SetStaticMesh(StaticMesh);
				FoliageType->Modify();
				FoliageType->PostEditChange();
			}
			AddInstances(World,FoliageType, Transforms);
		}
	}
}
