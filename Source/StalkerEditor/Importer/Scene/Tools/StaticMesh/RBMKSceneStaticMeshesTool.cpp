#include "RBMKSceneStaticMeshesTool.h"

#include "Importer/FRBMKEngineFactory.h"
#include "Importer/Scene/Entitys/StaticMesh//RBMKSceneStaticMesh.h"
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

void FRBMKSceneStaticMeshesTool::ExportToWorld(UWorld*World,EObjectFlags InFlags,const UXRayLevelImportOptions&LevelImportOptions)
{
	FRBMKEngineFactory EngineFactory(nullptr,InFlags);

	for (TSharedPtr<FRBMKSceneObjectBase>& Object : Objects)
	{
		if(FRBMKSceneStaticMesh* SceneObject = reinterpret_cast<FRBMKSceneStaticMesh*>(Object->QueryInterface(ERBMKSceneObjectType::StaticMesh)))
		{
			CEditableObject* EditableObject = SceneObject->GetReferenceObject();
			if (EditableObject)
			{
				bool UseOnlyCollision = false;
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
