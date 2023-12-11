#include "RBMKSceneParticleSystemsTool.h"

#include "NiagaraActor.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "StalkerEditor/StalkerEditorManager.h"
#include "StalkerEditor/Importer/Scene/Entitys/ParticleSystem//RBMKSceneParticleSystem.h"

FRBMKSceneParticleSystemsTool::FRBMKSceneParticleSystemsTool():FRBMKSceneObjectsToolBase(ERBMKSceneObjectType::ParticleSystem)
{
}

FRBMKSceneParticleSystemsTool::~FRBMKSceneParticleSystemsTool()
{

}

TSharedPtr<FRBMKSceneObjectBase> FRBMKSceneParticleSystemsTool::CreateObject(const FString&Name)
{
	return MakeShared<FRBMKSceneParticleSystem>(Name);
}

bool FRBMKSceneParticleSystemsTool::LoadLTX(CInifile& INIFile)
{
	const uint32 Version = INIFile.r_u32("main","version");
    if(Version != ToolVersion )
    {
    	return false;
    }
	FRBMKSceneObjectsToolBase::LoadLTX(INIFile);
	return true;
}

bool FRBMKSceneParticleSystemsTool::LoadStream(IReader& F)
{
	uint16 Version 	= 0;
    if(F.r_chunk(ChunkVersion,&Version))
    {
    	if( Version!=ToolVersion )
	    {
	        return false;
	    }
    }
    return FRBMKSceneObjectsToolBase::LoadStream(F);
}

bool FRBMKSceneParticleSystemsTool::LoadSelection(IReader& F)
{
	uint16 Version 	= 0;
    if(F.r_chunk(ChunkVersion,&Version))
    {
    	if( Version!=ToolVersion )
	    {
	        return false;
	    }
    }
    return FRBMKSceneObjectsToolBase::LoadSelection(F);
}

void FRBMKSceneParticleSystemsTool::ExportToWorld(UWorld* World, EObjectFlags InFlags,const URBMKLevelImportOptions&LevelImportOptions)
{
	for (const TSharedPtr<FRBMKSceneObjectBase>& Object : Objects)
	{
		const FRBMKSceneParticleSystem* SceneParticleSystem = reinterpret_cast<FRBMKSceneParticleSystem*>(Object->QueryInterface(ERBMKSceneObjectType::ParticleSystem));
		UNiagaraSystem * NiagaraSystem = nullptr;
		{
			FString ParentPackageName = GStalkerEditorManager->GetGamePath()/TEXT("Particles")/ SceneParticleSystem->ReferenceName;
			ParentPackageName.ReplaceCharInline(TEXT('\\'), TEXT('/'));
			const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
			NiagaraSystem = LoadObject<UNiagaraSystem>(nullptr, *ParentObjectPath, nullptr, LOAD_NoWarn);
		}
		if (NiagaraSystem)
		{
			ANiagaraActor* NiagaraActor = World->SpawnActor<ANiagaraActor>(ANiagaraActor::StaticClass(),SceneParticleSystem->GetTransform());
			NiagaraActor->GetNiagaraComponent()->SetAsset(NiagaraSystem);
			NiagaraActor->SetFolderPath(TEXT("Particles"));
			FString Label = SceneParticleSystem->GetName();
			Label.ReplaceCharInline(TEXT('\\'), TEXT('/'));
			NiagaraActor->SetActorLabel(Label);
		}
		else
		{
			UE_LOG(LogXRayImporter,Error,TEXT("FRBMKSceneParticleSystem:  '%s' not found in library."),*SceneParticleSystem->ReferenceName);
		}
	}
}

