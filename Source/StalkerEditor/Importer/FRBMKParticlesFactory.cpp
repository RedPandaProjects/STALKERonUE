#include "FRBMKParticlesFactory.h"
#include "Engine/EngineTypes.h"
#include "ImportUtils/StaticMeshImportUtils.h"
#include "../StalkerEditorManager.h"
#include "Kernel/StalkerEngineManager.h"
#include "Kernel/Unreal/GameSettings/StalkerGameSettings.h"
THIRD_PARTY_INCLUDES_START
#include "Editors/XrECore/Particles/PSLibrary.h"
#include "Editors/XrECore/Particles/ParticleEffectDef.h"
#include "Editors/XrECore/Particles/ParticleGroup.h"
#include "Editors/XrECore/Particles/particle_actions.h"
#include "Editors/XrECore/Particles/particle_actions_collection.h"
THIRD_PARTY_INCLUDES_END
#include "NiagaraEmitter.h"
#include "NiagaraScriptSource.h"
#include "ViewModels/Stack/NiagaraStackGraphUtilities.h"
#include "NiagaraGraph.h"
#include "NiagaraNodeOutput.h"
#include "NiagaraNodeInput.h"
#include "NiagaraEmitterFactoryNew.h"
#include "NiagaraSystemFactoryNew.h"
#include "ViewModels/NiagaraSystemViewModel.h"
#include "ViewModels/NiagaraEmitterHandleViewModel.h"
#include "NiagaraEmitterHandle.h"
#include "NiagaraSpriteRendererProperties.h"
#include "FRBMKEngineFactory.h"
#include "NiagaraScriptSourceBase.h"
#include "NiagaraDataInterfaceSpriteRendererInfo.h"
#include "NiagaraNodeFunctionCall.h"
#include "ViewModels/Stack/NiagaraStackFunctionInput.h"
#include "ViewModels/Stack/NiagaraStackEntry.h"
#include "NiagaraSystemEditorData.h"
#include "ViewModels/Stack/NiagaraStackInputCategory.h"
#include "NiagaraDataInterfaceParticleRead.h"
#include "Resources/Particle/StalkerNiagaraSystem.h"

FRBMKParticlesFactory::FRBMKParticlesFactory(UObject* InParentPackage, EObjectFlags InFlags):EngineFactory(InParentPackage,InFlags)
{

}

FRBMKParticlesFactory::~FRBMKParticlesFactory()
{

}
inline const TCHAR*ActionNoSupportEnum2Str(PAPI::PActionEnum ActionEnum)
{
	switch (ActionEnum)
	{
			case PAPI::PAAvoidID:			return TEXT("AvoidID");
			case PAPI::PACopyVertexBID:    	return TEXT("CopyVertexB");
			case PAPI::PAExplosionID:    	return TEXT("Explosion");
			case PAPI::PAMatchVelocityID:   return TEXT("MatchVelocity");
			case PAPI::PAOrbitLineID:    	return TEXT("OrbitLine");
			case PAPI::PARestoreID:    		return TEXT("Restore");
			case PAPI::PASinkVelocityID:    return TEXT("SinkVelocity");
	default:
		return TEXT("Unkown");
	}
		
}
void FRBMKParticlesFactory::ImportParticles()
{
	UE_LOG(LogXRayImporter, Log, TEXT("Start Import Particles"));
	CPSLibrary PSLibrary;
	MyLibrary = &PSLibrary;
	PSLibrary.OnCreate();
	FScopedSlowTask Progress((PSLibrary.LastPED()-PSLibrary.FirstPED()) + (PSLibrary.LastPGD()- PSLibrary.FirstPGD()), FText::FromString(TEXT("Import Particles")), true);
	CurrentSlowTask = &Progress;
	Progress.MakeDialog(true);
	for (auto Iterator = PSLibrary.FirstPED(); Iterator != PSLibrary.LastPED(); Iterator++)
	{
		PS::CPEDef* PEDef = (*Iterator);
		if (PEDef/*&&FCStringAnsi::Strstr(PEDef->m_Name.c_str(),"campfire_smoke")*/)
		{
		
			Progress.EnterProgressFrame(1, FText::FromString(FString::Printf(TEXT("Import PE:%S"),PEDef->Name())));
			UE_LOG(LogXRayImporter, Log, TEXT("Import ParticleEffect:%S"),PEDef->Name());
			ImportParticle(PEDef);
			if (GWarn->ReceivedUserCancel())
			{
				break;
			}
		}
		else
		{
			Progress.EnterProgressFrame(1, FText::FromString(FString::Printf(TEXT("Import PE:%S"),PEDef->Name())));
		}
	}
	for (auto Iterator = PSLibrary.FirstPGD(); Iterator != PSLibrary.LastPGD(); Iterator++)
	{
		PS::CPGDef* PGDef = (*Iterator);
		if (PGDef/*&&FCStringAnsi::Strstr(PGDef->m_Name.c_str(),"generator_accum_thunderbolt")*/)
		{
			Progress.EnterProgressFrame(1,  FText::FromString(FString::Printf(TEXT("Import PG:%S"),PGDef->m_Name.c_str())));
			UE_LOG(LogXRayImporter, Log, TEXT("Import ParticleGroup:%S"),PGDef->m_Name.c_str());
			ImportParticle(PGDef);
			if (GWarn->ReceivedUserCancel())
			{
				break;
			}
		}
		else
		{
			Progress.EnterProgressFrame(1,  FText::FromString(FString::Printf(TEXT("Import PG:%S"),PGDef->m_Name.c_str())));
		}
	}
	if (TypesNotImplemented.Num())
	{
		FString Actions;
		for (PAPI::PActionEnum ActionEnum:TypesNotImplemented)
		{
			if(Actions.Len())Actions.Append(TEXT(","));
			Actions.Append(ActionNoSupportEnum2Str(ActionEnum));
		}
		UE_LOG(LogXRayImporter, Warning, TEXT("Particle actions [%s] not supprt"),*Actions);
	}
	Flush();
	UE_LOG(LogXRayImporter, Log, TEXT("End Import Particles"));
	PSLibrary.OnDestroy();
	MyLibrary = nullptr;
}


UNiagaraSystem* FRBMKParticlesFactory::ImportParticle(PS::CPEDef* PEDef)
{
	FString Name =  PEDef->m_Name.c_str();
	Name.ReplaceCharInline(TEXT('\\'), TEXT('/'));
	const FString NewObjectPath = UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("Particles")/ Name);

	UStalkerNiagaraSystem* NiagaraSystem = nullptr;
	if(!FRBMKEngineFactory::LoadOrCreateOrOverwriteAsset(NewObjectPath,RF_Standalone|RF_Public,NiagaraSystem))
	{
		return NiagaraSystem;
	}

	NiagaraSystem->SetFlags(RF_Transactional);
	UNiagaraSystemFactoryNew::InitializeSystem(NiagaraSystem,true);
	FAssetRegistryModule::AssetCreated(NiagaraSystem);
	{
		TSharedPtr<FNiagaraSystemViewModel> SystemViewModel = MakeShared<FNiagaraSystemViewModel>();
		NiagaraSystemViewModels.Add(SystemViewModel);
		FNiagaraSystemViewModelOptions SystemViewModelOptions = FNiagaraSystemViewModelOptions();
		SystemViewModelOptions.bCanAutoCompile = true;
		SystemViewModelOptions.bCanSimulate = false;
		SystemViewModelOptions.EditMode = ENiagaraSystemViewModelEditMode::SystemAsset;
		SystemViewModelOptions.MessageLogGuid = NiagaraSystem->GetAssetGuid();
		SystemViewModel->Initialize(*NiagaraSystem, SystemViewModelOptions);
		{
			UNiagaraScript* SystemUpdateScript = NiagaraSystem->GetSystemUpdateScript();
			FSoftObjectPath SystemUpdateScriptRef = GetDefault<UNiagaraEditorSettings>()->RequiredSystemUpdateScript;
			UNiagaraNodeFunctionCall* SystemUpdateNode = FindModuleInScript(SystemUpdateScript, SystemUpdateScriptRef);
			if (ensure(SystemUpdateNode))
			{
				if (PEDef->m_Flags.is(PS::CPEDef::dfTimeLimit))
				{
					SetModuleBlueprintEnum(SystemViewModel, *SystemUpdateNode, TEXT("Loop Behavior"), TEXT("/Niagara/Enums/ENiagara_EmitterStateOptions.ENiagara_EmitterStateOptions"), TEXT("Once"));
					SetModuleParameter(TEXT(""),*SystemUpdateScript,*SystemUpdateNode,"Loop Duration",PEDef->m_fTimeLimit);
				}
			}
			
			FNiagaraVariable Velocity(FNiagaraTypeDefinition::GetVec3Def(),"User.Velocity");
			SystemViewModel->GetSystem().GetExposedParameters().AddParameter(Velocity);
			NiagaraSystem->IsLooping = !PEDef->m_Flags.is(PS::CPEDef::dfTimeLimit);
		}
		ImportToEmitter(NewObjectPath,PEDef, SystemViewModel->AddEmptyEmitter());
	}
	if (NiagaraSystemViewModels.Num() >= 100)
	{
		Flush();
	}
	NiagaraSystem->Modify();
	UNiagaraSystemEditorData* SystemEditorData = CastChecked<UNiagaraSystemEditorData>(NiagaraSystem->GetEditorData(), ECastCheckedType::NullChecked);
	SystemEditorData->SynchronizeOverviewGraphWithSystem(*NiagaraSystem);
	return NiagaraSystem;
}


class UNiagaraSystem* FRBMKParticlesFactory::ImportParticle(PS::CPGDef* PGDef)
{
	FString Name =  PGDef->m_Name.c_str();
	Name.ReplaceCharInline(TEXT('\\'), TEXT('/'));
	const FString NewObjectPath = UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("Particles")/ Name);

	UStalkerNiagaraSystem* NiagaraSystem = nullptr;
	if(!FRBMKEngineFactory::LoadOrCreateOrOverwriteAsset(NewObjectPath,RF_Standalone|RF_Public,NiagaraSystem))
	{
		return NiagaraSystem;
	}

	NiagaraSystem->SetFlags(RF_Transactional);

	UNiagaraSystemFactoryNew::InitializeSystem(NiagaraSystem,true);

	FAssetRegistryModule::AssetCreated(NiagaraSystem);

	{
		TSharedPtr<FNiagaraSystemViewModel> SystemViewModel = MakeShared<FNiagaraSystemViewModel>();
		NiagaraSystemViewModels.Add(SystemViewModel);
		FNiagaraSystemViewModelOptions SystemViewModelOptions = FNiagaraSystemViewModelOptions();
		SystemViewModelOptions.bCanAutoCompile = true;
		SystemViewModelOptions.bCanSimulate = false;
		SystemViewModelOptions.EditMode = ENiagaraSystemViewModelEditMode::SystemAsset;
		SystemViewModelOptions.MessageLogGuid = NiagaraSystem->GetAssetGuid();
		SystemViewModel->Initialize(*NiagaraSystem, SystemViewModelOptions);
		{
			UNiagaraScript* SystemUpdateScript = NiagaraSystem->GetSystemUpdateScript();
			FSoftObjectPath SystemUpdateScriptRef = GetDefault<UNiagaraEditorSettings>()->RequiredSystemUpdateScript;
			UNiagaraNodeFunctionCall* SystemUpdateNode = FindModuleInScript(SystemUpdateScript, SystemUpdateScriptRef);
			if (ensure(SystemUpdateNode))
			{
				if (PGDef->m_fTimeLimit>0)
				{
					SetModuleBlueprintEnum(SystemViewModel, *SystemUpdateNode, TEXT("Loop Behavior"), TEXT("/Niagara/Enums/ENiagara_EmitterStateOptions.ENiagara_EmitterStateOptions"), TEXT("Once"));
					SetModuleParameter(TEXT(""),*SystemUpdateScript,*SystemUpdateNode,"Loop Duration",PGDef->m_fTimeLimit);
					SetModuleBlueprintEnum(SystemViewModel, *SystemUpdateNode, TEXT("Inactive Response"), TEXT("/Niagara/Enums/ENiagaraSystemInactiveMode.ENiagaraSystemInactiveMode"), TEXT("Kill (System and Emitters Die Immediately)"));
					
				}
			}
			FNiagaraVariable Velocity(FNiagaraTypeDefinition::GetVec3Def(),"User.Velocity");
			SystemViewModel->GetSystem().GetExposedParameters().AddParameter(Velocity);
			NiagaraSystem->IsLooping = PGDef->m_fTimeLimit<=0;
		}
		for (PS::CPGDef::SEffect* Effect : PGDef->m_Effects)
		{
			if(Effect->m_Flags.is(PS::CPGDef::SEffect::flEnabled))
			{
				PS::CPEDef* InPEDef = MyLibrary->FindPED(Effect->m_EffectName.c_str());
				if (InPEDef)
				{
					ImportToEmitter(NewObjectPath,InPEDef, SystemViewModel->AddEmptyEmitter() ,Effect);
				}
			}
		}

	}
	
	if (NiagaraSystemViewModels.Num() >= 100)
	{
		Flush();
	}
	NiagaraSystem->Modify();
	UNiagaraSystemEditorData* SystemEditorData = CastChecked<UNiagaraSystemEditorData>(NiagaraSystem->GetEditorData(), ECastCheckedType::NullChecked);
	SystemEditorData->SynchronizeOverviewGraphWithSystem(*NiagaraSystem);
	return NiagaraSystem;
}
void FRBMKParticlesFactory::ImportToEmitter(const FString&PackageName ,PS::CPEDef* InParticleEffect, TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*InGroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter,EImportToEmitterAs ImportToEmitterAs )
{
	PS::CPGDef::SEffect* GroupEffect  = reinterpret_cast<PS::CPGDef::SEffect*>(InGroupEffect);

	FNiagaraEmitterHandle*	EmitterHandle	=	InEmiter->GetEmitterHandle();
	if (!ensure(EmitterHandle))
	{
		return;
	}
	UNiagaraEmitter*		Emitter			=	EmitterHandle->GetInstance().Emitter;
	FVersionedNiagaraEmitterData*EmitterData =  EmitterHandle->GetInstance().GetEmitterData();
	if (!ensure(Emitter)&&!ensure(EmitterData))
	{
		return;
	}
	UNiagaraScriptSource* ScriptSource =Cast<UNiagaraScriptSource>( EmitterData->GraphSource);
	if (!ensure(ScriptSource))
	{
		return;
	}
	EmitterData->AllocationMode = EParticleAllocationMode::FixedCount;
	EmitterData->PreAllocationCount = InParticleEffect->m_MaxParticles;

	UNiagaraNodeOutput* EmitterSpawnOutputNode =  ScriptSource->NodeGraph->FindEquivalentOutputNode(ENiagaraScriptUsage::EmitterSpawnScript, EmitterData->EmitterSpawnScriptProps.Script->GetUsageId());
	UNiagaraNodeOutput* EmitterUpdateOutputNode =  ScriptSource->NodeGraph->FindEquivalentOutputNode(ENiagaraScriptUsage::EmitterUpdateScript, EmitterData->EmitterUpdateScriptProps.Script->GetUsageId());
	UNiagaraNodeOutput* ParticleSpawnOutputNode =  ScriptSource->NodeGraph->FindEquivalentOutputNode(ENiagaraScriptUsage::ParticleSpawnScript, EmitterData->SpawnScriptProps.Script->GetUsageId());
	UNiagaraNodeOutput* ParticleUpdateOutputNode =  ScriptSource->NodeGraph->FindEquivalentOutputNode(ENiagaraScriptUsage::ParticleUpdateScript, EmitterData->UpdateScriptProps.Script->GetUsageId());
	
	checkf(EmitterSpawnOutputNode != nullptr && EmitterUpdateOutputNode != nullptr && ParticleSpawnOutputNode != nullptr && ParticleUpdateOutputNode != nullptr,TEXT("Failed to create output nodes for emitter scripts."));
	InEmiter->SetName(FName(FPaths::GetBaseFilename(ANSI_TO_TCHAR(InParticleEffect->m_Name.c_str()))));
	{
		UNiagaraSpriteRendererProperties*NiagaraSpriteRendererProperties =  NewObject<UNiagaraSpriteRendererProperties>(Emitter, "Renderer");
		FStalkerEmiterMaterial StalkerEmiterMaterial;

		FString ParticleEffectName = InParticleEffect->Name();
		bool UseHUD = false;
		for(const FString&Path: GetDefault<UStalkerGameSettings>()->ParticlesUsingHudMode)
		{
			if(ParticleEffectName.Find(Path) == 0)
			{
				UseHUD = true;
			}
		}

		if (GroupEffect&&ImportToEmitterAs == EImportToEmitterAs::None)
		{
			FString Name =  GroupEffect->m_EffectName.c_str();
			Name.ReplaceCharInline(TEXT('\\'), TEXT('/'));
			const FString EffectPackageName = UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("Particles")/ Name);
			NiagaraSpriteRendererProperties->Material = EngineFactory.ImportSurface(EffectPackageName+TEXT("_Mat"),InParticleEffect->m_ShaderName,InParticleEffect->m_TextureName,"",false);
			if(UseHUD)
			{
				StalkerEmiterMaterial.Material =NiagaraSpriteRendererProperties->Material;
				StalkerEmiterMaterial.HudMaterial = EngineFactory.ImportSurface(EffectPackageName+TEXT("_Mat"),InParticleEffect->m_ShaderName,InParticleEffect->m_TextureName,"",true);
			}
		}
		else
		{
			NiagaraSpriteRendererProperties->Material = EngineFactory.ImportSurface(PackageName+TEXT("_Mat"),InParticleEffect->m_ShaderName,InParticleEffect->m_TextureName,"",false);
			if(UseHUD)
			{
				StalkerEmiterMaterial.Material = 			NiagaraSpriteRendererProperties->Material;
				StalkerEmiterMaterial.HudMaterial = EngineFactory.ImportSurface(PackageName+TEXT("_Mat"),InParticleEffect->m_ShaderName,InParticleEffect->m_TextureName,"",true);
			}
		}
		
		if(UseHUD)
		{
			FString MaterialUserParametrName = FString::Printf(TEXT("User.%sMaterial"),*InEmiter->GetName().ToString());
			FNiagaraVariable MaterialUserParametr(FNiagaraTypeDefinition::GetUMaterialDef(), *MaterialUserParametrName);
			if (UStalkerNiagaraSystem* StalkerNiagaraSystem = Cast<UStalkerNiagaraSystem>(&InEmiter->GetOwningSystemViewModel()->GetSystem()))
			{
				StalkerNiagaraSystem->Materials.Add(*MaterialUserParametrName,StalkerEmiterMaterial);
			}
			InEmiter->GetOwningSystemViewModel()->GetSystem().GetExposedParameters().AddParameter( MaterialUserParametr);
			NiagaraSpriteRendererProperties->MaterialUserParamBinding.Parameter = MaterialUserParametr;
		}
		Emitter->AddRenderer(NiagaraSpriteRendererProperties, EmitterData->Version.VersionGuid);
		if(InParticleEffect->m_Flags.is(PS::CPEDef::dfAlignToPath))
		{	
			if (InParticleEffect->m_Flags.is(PS::CPEDef::dfFaceAlign))
			{
				NiagaraSpriteRendererProperties->Alignment = ENiagaraSpriteAlignment::CustomAlignment;
				NiagaraSpriteRendererProperties->FacingMode = ENiagaraSpriteFacingMode::CustomFacingVector;
			}
			else if (InParticleEffect->m_Flags.is(PS::CPEDef::dfWorldAlign))
			{
				NiagaraSpriteRendererProperties->Alignment = ENiagaraSpriteAlignment::VelocityAligned;
			}
			else
			{
				NiagaraSpriteRendererProperties->Alignment = ENiagaraSpriteAlignment::VelocityAligned;
			}
		}
		if(InParticleEffect->m_Flags.is(PS::CPEDef::dfFramed))
		{
			NiagaraSpriteRendererProperties->SubImageSize.Set(FMath::RoundToDouble(1.f/InParticleEffect->m_Frame.m_fTexSize.x),FMath::RoundToDouble(1.f/InParticleEffect->m_Frame.m_fTexSize.y));
			NiagaraSpriteRendererProperties->bSubImageBlend = true;
			if (UNiagaraNodeFunctionCall* SubUVNode = AddModuleFromAssetPath(TEXT("/Niagara/Modules/Update/SubUV/V2/SubUVAnimation.SubUVAnimation"), *ParticleUpdateOutputNode))
			{
				{
					UNiagaraDataInterfaceSpriteRendererInfo* NiagaraDataInterfaceSpriteRendererInfo = NewObject<UNiagaraDataInterfaceSpriteRendererInfo>();
					FObjectPtrProperty* Property = FindFProperty<FObjectPtrProperty>(NiagaraDataInterfaceSpriteRendererInfo->GetClass(), "SpriteRenderer");
					Property->SetPropertyValue_InContainer(NiagaraDataInterfaceSpriteRendererInfo, NiagaraSpriteRendererProperties);
					SetModuleParameter(InEmiter, *SubUVNode, TEXT("Sprite Renderer "), NiagaraDataInterfaceSpriteRendererInfo);
				}
				SetModuleParameter(InEmiter,*SubUVNode,TEXT("UseEndFrame"),true);
				SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *SubUVNode, "End Frame Range Override", InParticleEffect->m_Frame.m_iFrameCount);
				if (InParticleEffect->m_Flags.is(PS::CPEDef::dfAnimated))
				{
					SetModuleBlueprintEnum(InEmiter, *SubUVNode, TEXT("SubUV Animation Mode"), TEXT("/Niagara/Enums/ENiagara_SubUVLookupModeV2.ENiagara_SubUVLookupModeV2"), TEXT("Infinite Loop"));
					SetModuleBlueprintEnum(InEmiter, *SubUVNode, TEXT("Playback Mode"), TEXT("/Niagara/Enums/SpriteRenderer/ENiagara_FPSPlayrate.ENiagara_FPSPlayrate"), TEXT("Frames Per Second"));
					SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *SubUVNode, "Frames Per Second", FMath::RoundToInt32(InParticleEffect->m_Frame.m_fSpeed));
					SetModuleParameter(InEmiter, *SubUVNode, TEXT("Random Start Frame"), InParticleEffect->m_Flags.is(PS::CPEDef::dfRandomFrame));
				}
				else
				{
					if (InParticleEffect->m_Flags.is(PS::CPEDef::dfRandomFrame))
					{
						SetModuleBlueprintEnum(InEmiter, *SubUVNode, TEXT("SubUV Animation Mode"), TEXT("/Niagara/Enums/ENiagara_SubUVLookupModeV2.ENiagara_SubUVLookupModeV2"), TEXT("Random"));
						SetModuleBlueprintEnum(InEmiter, *SubUVNode, TEXT("Evaluation Type"), TEXT("/Niagara/Enums/ENiagaraRandomnessEvaluation.ENiagaraRandomnessEvaluation"), TEXT("Spawn Only"));
					}
					else
					{
						SetModuleBlueprintEnum(InEmiter, *SubUVNode, TEXT("SubUV Animation Mode"), TEXT("/Niagara/Enums/ENiagara_SubUVLookupModeV2.ENiagara_SubUVLookupModeV2"), TEXT("Direct Index"));
					}
				}
			}
		}

		{
			UNiagaraNodeFunctionCall* EmitterStateNode = AddModuleFromAssetPath(TEXT("/Niagara/Modules/Emitter/EmitterState.EmitterState"), *EmitterUpdateOutputNode);
			if (ensure(EmitterStateNode))
			{
				if (GroupEffect&&ImportToEmitterAs == EImportToEmitterAs::None)
				{
					if (!FMath::IsNearlyZero(GroupEffect->m_Time0)||!FMath::IsNearlyZero(GroupEffect->m_Time1)||InParticleEffect->m_Flags.is(PS::CPEDef::dfTimeLimit))
					{
						SetModuleBlueprintEnum(InEmiter, *EmitterStateNode, TEXT("Life Cycle Mode"), TEXT("/Niagara/Enums/ENiagaraEmitterLifeCycleMode.ENiagaraEmitterLifeCycleMode"), TEXT("Self"));
					}

					if (!FMath::IsNearlyZero(GroupEffect->m_Time0))
					{	
						SetModuleParameter(InEmiter,*EmitterStateNode,TEXT("UseLoopDelay"),true);	
						SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->EmitterUpdateScriptProps.Script, *EmitterStateNode, "Loop Delay",GroupEffect->m_Time0);
					}

					if (!FMath::IsNearlyZero(GroupEffect->m_Time1)&&!(InParticleEffect->m_Flags.is(PS::CPEDef::dfTimeLimit)&&GroupEffect->m_Time1 < InParticleEffect->m_fTimeLimit))
					{
						if (!GroupEffect->m_Flags.is(PS::CPGDef::SEffect::flDefferedStop))
						{
							SetModuleBlueprintEnum(InEmiter, *EmitterStateNode, TEXT("Inactive Response"), TEXT("/Niagara/Enums/ENiagaraInactiveMode.ENiagaraInactiveMode"), TEXT("Kill (Emitter and Particles Die Immediately)"));
						}
						SetModuleBlueprintEnum(InEmiter, *EmitterStateNode, TEXT("Loop Behavior"), TEXT("/Niagara/Enums/ENiagara_EmitterStateOptions.ENiagara_EmitterStateOptions"), TEXT("Once"));
						SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->EmitterUpdateScriptProps.Script, *EmitterStateNode, "Loop Duration", GroupEffect->m_Time1-GroupEffect->m_Time0);
					
					}
					else if (InParticleEffect->m_Flags.is(PS::CPEDef::dfTimeLimit))
					{
						SetModuleBlueprintEnum(InEmiter, *EmitterStateNode, TEXT("Loop Behavior"), TEXT("/Niagara/Enums/ENiagara_EmitterStateOptions.ENiagara_EmitterStateOptions"), TEXT("Once"));
						SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->EmitterUpdateScriptProps.Script, *EmitterStateNode, "Loop Duration", InParticleEffect->m_fTimeLimit);
					}
				}
			}
		}
	}
	if (UNiagaraNodeFunctionCall* StalkerBeginNode = AddModuleFromAssetPath(TEXT("/Game/Base/Particles/Modules/StalkerBegin.StalkerBegin"), *ParticleUpdateOutputNode))
	{
	}
	IReader ActionsStreams				(InParticleEffect->m_Actions.pointer(),InParticleEffect->m_Actions.size());

	auto LoadActionNotImplemented	 = [&ActionsStreams,this]<typename T>(T*Action)->void
	{
		Action->Load(ActionsStreams);
		delete Action;
		TypesNotImplemented.AddUnique(Action->type);
		FString Actions;
		Actions.Append(ActionNoSupportEnum2Str(Action->type));
		UE_LOG(LogXRayImporter, Warning, TEXT("Particle action [%s] not supprt"),*Actions);
	};
	auto LoadAction					 = [&]<typename T>(T*Action)->void{Action->Load(ActionsStreams);ImportActionToEmitter(InParticleEffect,InEmiter,GroupEffect,OwnerEmitter,ImportToEmitterAs,Emitter,EmitterData,EmitterSpawnOutputNode,EmitterUpdateOutputNode,ParticleSpawnOutputNode,ParticleUpdateOutputNode,Action); delete Action; };
	if (ActionsStreams.length())
	{
        u32 cnt					= ActionsStreams.r_u32();
        for (u32 k=0; k<cnt; k++)
		{
			PAPI::PActionEnum ActionEnum=	(PAPI::PActionEnum)ActionsStreams.r_u32();
			switch (ActionEnum)
			{
			case PAPI::PAAvoidID:			LoadActionNotImplemented(new PAPI::PAAvoid);			break;
			case PAPI::PABounceID:    		LoadAction				(new PAPI::PABounce);			break;
			case PAPI::PACopyVertexBID:    	LoadActionNotImplemented(new PAPI::PACopyVertexB);		break;
			case PAPI::PADampingID:    		LoadAction				(new PAPI::PADamping);			break;
			case PAPI::PAExplosionID:    	LoadActionNotImplemented(new PAPI::PAExplosion);		break;
			case PAPI::PAFollowID:    		LoadAction				(new PAPI::PAFollow);			break;
			case PAPI::PAGravitateID:    	LoadAction				(new PAPI::PAGravitate);		break;
			case PAPI::PAGravityID:    		LoadAction				(new PAPI::PAGravity);			break;
			case PAPI::PAJetID:    			LoadAction				(new PAPI::PAJet);				break;
			case PAPI::PAKillOldID:    		LoadAction				(new PAPI::PAKillOld);			break;
			case PAPI::PAMatchVelocityID:   LoadActionNotImplemented(new PAPI::PAMatchVelocity);	break;
			case PAPI::PAMoveID:    		LoadAction				(new PAPI::PAMove);				break;
			case PAPI::PAOrbitLineID:    	LoadActionNotImplemented(new PAPI::PAOrbitLine);		break;
			case PAPI::PAOrbitPointID:    	LoadAction				(new PAPI::PAOrbitPoint);		break;
			case PAPI::PARandomAccelID:    	LoadAction				(new PAPI::PARandomAccel);		break;
			case PAPI::PARandomDisplaceID:  LoadAction				(new PAPI::PARandomDisplace);	break;
			case PAPI::PARandomVelocityID:  LoadAction				(new PAPI::PARandomVelocity);	break;
			case PAPI::PARestoreID:    		LoadActionNotImplemented(new PAPI::PARestore);			break;
			case PAPI::PASinkID:    		LoadAction				(new PAPI::PASink);				break;
			case PAPI::PASinkVelocityID:    LoadActionNotImplemented(new PAPI::PASinkVelocity);		break;
			case PAPI::PASourceID:    		LoadAction				(new PAPI::PASource);			break;
			case PAPI::PASpeedLimitID:    	LoadAction				(new PAPI::PASpeedLimit);		break;
			case PAPI::PATargetColorID:    	LoadAction				(new PAPI::PATargetColor);		break;
			case PAPI::PATargetSizeID:    	LoadAction				(new PAPI::PATargetSize);		break;
			case PAPI::PATargetRotateID:    LoadAction				(new PAPI::PATargetRotate);		break;
			case PAPI::PATargetRotateDID:   LoadAction				(new PAPI::PATargetRotate);		break;
			case PAPI::PATargetVelocityID:  LoadAction				(new PAPI::PATargetVelocity); 	break;
			case PAPI::PATargetVelocityDID: LoadAction				(new PAPI::PATargetVelocity);	break;
			case PAPI::PAVortexID:    		LoadAction				(new PAPI::PAVortex);			break;
			case PAPI::PATurbulenceID:		LoadAction				(new PAPI::PATurbulence);		break;
			case PAPI::PAScatterID:  		LoadAction				(new PAPI::PAScatter);			break;
			default: NODEFAULT;
			}
        }
    }
	
	if (UNiagaraNodeFunctionCall* StalkerEndNode = AddModuleFromAssetPath(TEXT("/Game/Base/Particles/Modules/StalkerEnd.StalkerEnd"), *ParticleUpdateOutputNode))
	{
		if (InParticleEffect->m_Flags.is(PS::CPEDef::dfVelocityScale))
		{
			SetModuleParameter(InEmiter,*StalkerEndNode,TEXT("UseVelocityScale"),true);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerEndNode, "VelocityScale",FVector3f(InParticleEffect->m_VelocityScale.x,InParticleEffect->m_VelocityScale.y,InParticleEffect->m_VelocityScale.z));
		}
		if (InParticleEffect->m_Flags.is(PS::CPEDef::dfAlignToPath))
		{
			if (InParticleEffect->m_Flags.is(PS::CPEDef::dfFaceAlign))
			{
				SetModuleParameter(InEmiter,*StalkerEndNode,TEXT("UseFaceAlign"),true);
			}
		}

	}
	if (GroupEffect&&ImportToEmitterAs == EImportToEmitterAs::None)
	{
		if (GroupEffect->m_Flags.is(PS::CPGDef::SEffect::flOnBirthChild))
		{
			PS::CPEDef* InPEDef = MyLibrary->FindPED(GroupEffect->m_OnBirthChildName.c_str());
			if (InPEDef)
			{
				FString Name = InPEDef->Name();
				Name.ReplaceCharInline(TEXT('\\'), TEXT('/'));
				const FString EffectPackageName = UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("Particles") / Name);
				ImportToEmitter(EffectPackageName, InPEDef,InEmiter->GetOwningSystemViewModel()->AddEmptyEmitter(),GroupEffect,InEmiter,EImportToEmitterAs::Birth);
			}
		}
		if (GroupEffect->m_Flags.is(PS::CPGDef::SEffect::flOnPlayChild))
		{		
			PS::CPEDef* InPEDef = MyLibrary->FindPED(GroupEffect->m_OnPlayChildName.c_str());
			if (InPEDef)
			{
				FString Name = InPEDef->Name();
				Name.ReplaceCharInline(TEXT('\\'), TEXT('/'));
				const FString EffectPackageName = UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("Particles") / Name);
				ImportToEmitter(EffectPackageName, InPEDef,InEmiter->GetOwningSystemViewModel()->AddEmptyEmitter(),GroupEffect,InEmiter,EImportToEmitterAs::Play);
			}
		}
		
		if (GroupEffect->m_Flags.is(PS::CPGDef::SEffect::flOnDeadChild))
		{		
			PS::CPEDef* InPEDef = MyLibrary->FindPED(GroupEffect->m_OnDeadChildName.c_str());
			if (InPEDef)
			{
				FString Name = InPEDef->Name();
				Name.ReplaceCharInline(TEXT('\\'), TEXT('/'));
				const FString EffectPackageName = UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("Particles") / Name);
				ImportToEmitter(EffectPackageName, InPEDef,InEmiter->GetOwningSystemViewModel()->AddEmptyEmitter(),GroupEffect,InEmiter,EImportToEmitterAs::Dead);
			}
		}
	}
}

void FRBMKParticlesFactory::ImportActionToEmitter(PS::CPEDef* InParticleEffect, TSharedPtr<FNiagaraEmitterHandleViewModel> InEmiter,void*InGroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs ,UNiagaraEmitter*	Emitter,FVersionedNiagaraEmitterData* EmitterData,UNiagaraNodeOutput* EmitterSpawnOutputNode,UNiagaraNodeOutput* EmitterUpdateOutputNode,UNiagaraNodeOutput* ParticleSpawnOutputNode,UNiagaraNodeOutput* ParticleUpdateOutputNode,PAPI::PASource* InSourceAction)
{
	PS::CPGDef::SEffect* GroupEffect  = reinterpret_cast<PS::CPGDef::SEffect*>(InGroupEffect);
	if (ImportToEmitterAs == EImportToEmitterAs::None)
	{
		UNiagaraNodeFunctionCall* SpawnRateNode = AddModuleFromAssetPath(TEXT("/Niagara/Modules/Emitter/SpawnRate.SpawnRate"), *EmitterUpdateOutputNode);
		if (ensure(SpawnRateNode))
		{
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->EmitterUpdateScriptProps.Script, *SpawnRateNode, "SpawnRate", static_cast<float>(InSourceAction->particle_rate));
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->EmitterUpdateScriptProps.Script, *SpawnRateNode, "Spawn Probability", 0.73f);
			SetModuleParameter(InEmiter, *SpawnRateNode, TEXT("Use Spawn Probability"), true);

		}
	}
	else 	
	{
		float OwnerMaxParticles = 1;
		PS::CPEDef* InOwnerPEDef = MyLibrary->FindPED(GroupEffect->m_EffectName.c_str());
		if (ensure(InOwnerPEDef))
		{
			OwnerMaxParticles = InOwnerPEDef->m_MaxParticles;
			OwnerMaxParticles = FMath::Max(1,OwnerMaxParticles);
		}
		UNiagaraNodeFunctionCall* SpawnRateNode = AddModuleFromAssetPath(TEXT("/Game/Base/Particles/Modules/StalkerSpawnParticlesFromOtherEmitter"), *EmitterUpdateOutputNode);
		if (ensure(SpawnRateNode))
		{
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->EmitterUpdateScriptProps.Script, *SpawnRateNode, "Spawn Rate", static_cast<float>(InSourceAction->particle_rate)*OwnerMaxParticles);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->EmitterUpdateScriptProps.Script, *SpawnRateNode, "Spawn Rate Per Particle Cap", static_cast<float>(InSourceAction->particle_rate));
			{
				UNiagaraDataInterfaceParticleRead* NiagaraDataInterfaceParticleRead = NewObject<UNiagaraDataInterfaceParticleRead>();
				FStrProperty* Property = FindFProperty<FStrProperty>(NiagaraDataInterfaceParticleRead->GetClass(), "EmitterName");
				FString Name = OwnerEmitter->GetName().ToString();
				Property->SetPropertyValue_InContainer(NiagaraDataInterfaceParticleRead,Name);
				SetModuleParameter(InEmiter, *SpawnRateNode, TEXT("Attribute Reader"), NiagaraDataInterfaceParticleRead);
			}
		}
	
	}
	UNiagaraNodeFunctionCall* StalkerTargetSourceNode = AddModuleFromAssetPath(TEXT("/Game/Base/Particles/Modules/StalkerTargetSource.StalkerTargetSource"), *ParticleSpawnOutputNode);
	if (ensure(StalkerTargetSourceNode) )
	{	
		float KillTime = 5;
		if ((ImportToEmitterAs == EImportToEmitterAs::Dead || (ImportToEmitterAs == EImportToEmitterAs::Play && !GroupEffect->m_Flags.is(PS::CPGDef::SEffect::flOnPlayChildRewind))))
		{
			PS::CPEDef* InOwnerPEDef = GroupEffect ? MyLibrary->FindPED(GroupEffect->m_EffectName.c_str()) : nullptr;
			if (ensure(InOwnerPEDef))
			{
				IReader ActionsStreams(InOwnerPEDef->m_Actions.pointer(), InOwnerPEDef->m_Actions.size());
				auto LoadActionNotImplemented = [&ActionsStreams, this]<typename T>(T * Action)->void { Action->Load(ActionsStreams); delete Action; };
				auto LoadAction = [&](PAPI::PAKillOld* Action, float& OutKillTime)->void {Action->Load(ActionsStreams); OutKillTime = FMath::Min(Action->age_limit, OutKillTime); delete Action; };
				if (ActionsStreams.length())
				{
					u32 cnt = ActionsStreams.r_u32();
					for (u32 k = 0; k < cnt; k++)
					{
						PAPI::PActionEnum ActionEnum = (PAPI::PActionEnum)ActionsStreams.r_u32();
						switch (ActionEnum)
						{
						case PAPI::PAAvoidID:			LoadActionNotImplemented(new PAPI::PAAvoid);			break;
						case PAPI::PABounceID:    		LoadActionNotImplemented(new PAPI::PABounce);			break;
						case PAPI::PACopyVertexBID:    	LoadActionNotImplemented(new PAPI::PACopyVertexB);		break;
						case PAPI::PADampingID:    		LoadActionNotImplemented(new PAPI::PADamping);			break;
						case PAPI::PAExplosionID:    	LoadActionNotImplemented(new PAPI::PAExplosion);		break;
						case PAPI::PAFollowID:    		LoadActionNotImplemented(new PAPI::PAFollow);			break;
						case PAPI::PAGravitateID:    	LoadActionNotImplemented(new PAPI::PAGravitate);		break;
						case PAPI::PAGravityID:    		LoadActionNotImplemented(new PAPI::PAGravity);			break;
						case PAPI::PAJetID:    			LoadActionNotImplemented(new PAPI::PAJet);				break;
						case PAPI::PAKillOldID:    		LoadAction(new PAPI::PAKillOld, KillTime);	break;
						case PAPI::PAMatchVelocityID:   LoadActionNotImplemented(new PAPI::PAMatchVelocity);	break;
						case PAPI::PAMoveID:    		LoadActionNotImplemented(new PAPI::PAMove);				break;
						case PAPI::PAOrbitLineID:    	LoadActionNotImplemented(new PAPI::PAOrbitLine);		break;
						case PAPI::PAOrbitPointID:    	LoadActionNotImplemented(new PAPI::PAOrbitPoint);		break;
						case PAPI::PARandomAccelID:    	LoadActionNotImplemented(new PAPI::PARandomAccel);		break;
						case PAPI::PARandomDisplaceID:  LoadActionNotImplemented(new PAPI::PARandomDisplace);	break;
						case PAPI::PARandomVelocityID:  LoadActionNotImplemented(new PAPI::PARandomVelocity);	break;
						case PAPI::PARestoreID:    		LoadActionNotImplemented(new PAPI::PARestore);			break;
						case PAPI::PASinkID:    		LoadActionNotImplemented(new PAPI::PASink);				break;
						case PAPI::PASinkVelocityID:    LoadActionNotImplemented(new PAPI::PASinkVelocity);		break;
						case PAPI::PASourceID:    		LoadActionNotImplemented(new PAPI::PASource);			break;
						case PAPI::PASpeedLimitID:    	LoadActionNotImplemented(new PAPI::PASpeedLimit);		break;
						case PAPI::PATargetColorID:    	LoadActionNotImplemented(new PAPI::PATargetColor);		break;
						case PAPI::PATargetSizeID:    	LoadActionNotImplemented(new PAPI::PATargetSize);		break;
						case PAPI::PATargetRotateID:    LoadActionNotImplemented(new PAPI::PATargetRotate);		break;
						case PAPI::PATargetRotateDID:   LoadActionNotImplemented(new PAPI::PATargetRotate);		break;
						case PAPI::PATargetVelocityID:  LoadActionNotImplemented(new PAPI::PATargetVelocity); 	break;
						case PAPI::PATargetVelocityDID: LoadActionNotImplemented(new PAPI::PATargetVelocity);	break;
						case PAPI::PAVortexID:    		LoadActionNotImplemented(new PAPI::PAVortex);			break;
						case PAPI::PATurbulenceID:		LoadActionNotImplemented(new PAPI::PATurbulence);		break;
						case PAPI::PAScatterID:  		LoadActionNotImplemented(new PAPI::PAScatter);			break;
						default: NODEFAULT;
						}
					}
				}

			}
		}

		if (ImportToEmitterAs != EImportToEmitterAs::None)
		{
			SetModuleParameter(InEmiter,*StalkerTargetSourceNode,TEXT("Spawn Particles From Other Emitter"),true);
			if (ImportToEmitterAs == EImportToEmitterAs::Birth || (ImportToEmitterAs == EImportToEmitterAs::Play&&!GroupEffect->m_Flags.is(PS::CPGDef::SEffect::flOnPlayChildRewind)))
			{
				SetModuleParameter(InEmiter,*StalkerTargetSourceNode,TEXT("Use Maximum Sampled Particle Age"),true);
				SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "Maximum Sampled Particle Age", FMath::Min(KillTime*0.80f,static_cast<float>(InSourceAction->particle_rate/InParticleEffect->m_MaxParticles)));
			}
			else if (ImportToEmitterAs == EImportToEmitterAs::Dead)
			{
				SetModuleParameter(InEmiter,*StalkerTargetSourceNode,TEXT("Use Minimum Sampled Particle Age"),true);
				SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "Minimum Sampled Particle Age",FMath::Max(KillTime/2.f, KillTime- static_cast<float>(InSourceAction->particle_rate/InParticleEffect->m_MaxParticles)));
			}
		
		}
		else
		{
			SetModuletLinkedParameter(InEmiter,*StalkerTargetSourceNode,TEXT("ParentVelocity"),FNiagaraTypeDefinition::GetVec3Def(),FNiagaraParameterHandle("User","Velocity"));
		}
		
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "ParentMotion", static_cast<float>(InSourceAction->parent_motion));

		SetModuleParameter(InEmiter,*StalkerTargetSourceNode,TEXT("AllowRotate"),InSourceAction->m_Flags.test(PAPI::ParticleAction::ALLOW_ROTATE));
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "Age", static_cast<float>(InSourceAction->age));
		if (!FMath::IsNearlyZero(InSourceAction->age_sigma))
		{
			SetModuleParameter(InEmiter,*StalkerTargetSourceNode,TEXT("UseAgeSigma"),true);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "AgeSigma", static_cast<float>(InSourceAction->age_sigma));

		}
		{
			EStalkerParticleDomainType DomainType ;
			FStalkerParticleDomain Domain;
			XRayDomainToUnreal(InSourceAction->position,DomainType,Domain,true);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "Position.Position1", Domain.Position1);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "Position.Position2", Domain.Position2);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "Position.U", Domain.U);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "Position.V", Domain.V);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "Position.Radius1", Domain.Radius1);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "Position.Radius2", Domain.Radius2);
			SetModuleEnum(InEmiter,*StalkerTargetSourceNode,TEXT("PositionType"),StaticEnum<EStalkerParticleDomainType>(),static_cast<int32>(DomainType),false);
		}
		{
			EStalkerParticleDomainType DomainType;
			FStalkerParticleDomain Domain;
			XRayDomainToUnreal(InSourceAction->size, DomainType, Domain, true,false);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "Size.Position1", Domain.Position1);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "Size.Position2", Domain.Position2);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "Size.U", Domain.U);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "Size.V", Domain.V);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "Size.Radius1", Domain.Radius1);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "Size.Radius2", Domain.Radius2);
			SetModuleEnum(InEmiter, *StalkerTargetSourceNode, TEXT("SizeType"), StaticEnum<EStalkerParticleDomainType>(), static_cast<int32>(DomainType),false);
		}
		{
			EStalkerParticleDomainType DomainType;
			FStalkerParticleDomain Domain;
			XRayDomainToUnreal(InSourceAction->rot, DomainType, Domain, false,false);
			Domain.Position1 = FMath::RadiansToDegrees(Domain.Position1);
			Domain.Position2 = FMath::RadiansToDegrees(Domain.Position2);
			Domain.U = FMath::RadiansToDegrees(Domain.U);
			Domain.V = FMath::RadiansToDegrees(Domain.V);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "Rotation.Position1", Domain.Position1);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "Rotation.Position2", Domain.Position2);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "Rotation.U", Domain.U);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "Rotation.V", Domain.V);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "Rotation.Radius1", Domain.Radius1);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "Rotation.Radius2", Domain.Radius2);
			SetModuleEnum(InEmiter, *StalkerTargetSourceNode, TEXT("RotationType"), StaticEnum<EStalkerParticleDomainType>(), static_cast<int32>(DomainType),false);
		}
		{
			EStalkerParticleDomainType DomainType;
			FStalkerParticleDomain Domain;
			XRayDomainToUnreal(InSourceAction->color, DomainType, Domain, false, false);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "RGBColor.Position1", Domain.Position1);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "RGBColor.Position2", Domain.Position2);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "RGBColor.U", Domain.U);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "RGBColor.V", Domain.V);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "RGBColor.Radius1", Domain.Radius1);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "RGBColor.Radius2", Domain.Radius2);

			SetModuleEnum(InEmiter, *StalkerTargetSourceNode, TEXT("ColorType"), StaticEnum<EStalkerParticleDomainType>(), static_cast<int32>(DomainType),false);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "ColorAlpha", static_cast<float>(InSourceAction->alpha));
		}
		{
			EStalkerParticleDomainType DomainType;
			FStalkerParticleDomain Domain;
			XRayDomainToUnreal(InSourceAction->velocity, DomainType, Domain, true,true);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "Velocity.Position1", Domain.Position1);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "Velocity.Position2", Domain.Position2);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "Velocity.U", Domain.U);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "Velocity.V", Domain.V);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "Velocity.Radius1", Domain.Radius1);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->SpawnScriptProps.Script, *StalkerTargetSourceNode, "Velocity.Radius2", Domain.Radius2);
			SetModuleEnum(InEmiter, *StalkerTargetSourceNode, TEXT("VelocityType"), StaticEnum<EStalkerParticleDomainType>(), static_cast<int32>(DomainType),false);
		}
		if (InSourceAction->m_Flags.is(PAPI::PASource::flSingleSize)) 
		{
			SetModuleParameter(InEmiter,*StalkerTargetSourceNode,TEXT("UseSingleSize"),true);
		}
	}
}

void FRBMKParticlesFactory::ImportActionToEmitter(PS::CPEDef* InParticleEffect, TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter, void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs ,class UNiagaraEmitter* Emitter, struct FVersionedNiagaraEmitterData* EmitterData, class UNiagaraNodeOutput* EmitterSpawnOutputNode, class UNiagaraNodeOutput* EmitterUpdateOutputNode, class UNiagaraNodeOutput* ParticleSpawnOutputNode, class UNiagaraNodeOutput* ParticleUpdateOutputNode, PAPI::PAKillOld* InSourceAction)
{
	UNiagaraNodeFunctionCall* StalkerKillOldNode = AddModuleFromAssetPath(TEXT("/Game/Base/Particles/Modules/StalkerKillOld.StalkerKillOld"), *ParticleUpdateOutputNode);
	if (ensure(StalkerKillOldNode))
	{
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerKillOldNode, "AgeLimit", static_cast<float>(InSourceAction->age_limit));
		SetModuleParameter(InEmiter,*StalkerKillOldNode,TEXT("Kill Less Than"),InSourceAction->kill_less_than);
	}
}

void FRBMKParticlesFactory::ImportActionToEmitter(PS::CPEDef* InParticleEffect, TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs , class UNiagaraEmitter* Emitter, struct FVersionedNiagaraEmitterData* EmitterData, class UNiagaraNodeOutput* EmitterSpawnOutputNode, class UNiagaraNodeOutput* EmitterUpdateOutputNode, class UNiagaraNodeOutput* ParticleSpawnOutputNode, class UNiagaraNodeOutput* ParticleUpdateOutputNode, PAPI::PATargetColor* InSourceAction)
{
	UNiagaraNodeFunctionCall* StalkerTargetColorNode = AddModuleFromAssetPath(TEXT("/Game/Base/Particles/Modules/StalkerTargetColor.StalkerTargetColor"), *ParticleUpdateOutputNode);
	if (ensure(StalkerTargetColorNode))
	{
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerTargetColorNode, "Color", FLinearColor(InSourceAction->color.x,InSourceAction->color.y,InSourceAction->color.z,InSourceAction->alpha));
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerTargetColorNode, "TimeFrom", static_cast<float>(InSourceAction->timeFrom));
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerTargetColorNode, "TimeTo", static_cast<float>(InSourceAction->timeTo));
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerTargetColorNode, "Scale", static_cast<float>(InSourceAction->scale));
	}
}

void FRBMKParticlesFactory::ImportActionToEmitter(PS::CPEDef* InParticleEffect, TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs , class UNiagaraEmitter* Emitter, struct FVersionedNiagaraEmitterData* EmitterData, class UNiagaraNodeOutput* EmitterSpawnOutputNode, class UNiagaraNodeOutput* EmitterUpdateOutputNode, class UNiagaraNodeOutput* ParticleSpawnOutputNode, class UNiagaraNodeOutput* ParticleUpdateOutputNode, PAPI::PAMove* InSourceAction)
{
	if (InParticleEffect->m_Flags.is(PS::CPEDef::dfCollision))
	{
		UNiagaraNodeFunctionCall* StalkerCollision = AddModuleFromAssetPath(TEXT("/Game/Base/Particles/Modules/StalkerCollision.StalkerCollision"), *ParticleUpdateOutputNode);
		if (ensure(StalkerCollision))
		{
			if (InParticleEffect->m_Flags.is(PS::CPEDef::dfCollisionDel))
			{
				SetModuleParameter(InEmiter,*StalkerCollision,TEXT("Destroy On Contact"),true);
			}
			else
			{
				SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerCollision, "Friction", static_cast<float>(InParticleEffect->m_fCollideOneMinusFriction));
				SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerCollision, "Resilence", static_cast<float>(InParticleEffect->m_fCollideResilience));
				SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerCollision, "Cutoff", FMath::Sqrt(static_cast<float>(InParticleEffect->m_fCollideSqrCutoff))*100.f);
			}
		
		}
	}
	UNiagaraNodeFunctionCall* StalkerMoveNode = AddModuleFromAssetPath(TEXT("/Game/Base/Particles/Modules/StalkerMove.StalkerMove"), *ParticleUpdateOutputNode);
	if (ensure(StalkerMoveNode))
	{
	}
}

void FRBMKParticlesFactory::ImportActionToEmitter(PS::CPEDef* InParticleEffect, TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs , class UNiagaraEmitter* Emitter, struct FVersionedNiagaraEmitterData* EmitterData, class UNiagaraNodeOutput* EmitterSpawnOutputNode, class UNiagaraNodeOutput* EmitterUpdateOutputNode, class UNiagaraNodeOutput* ParticleSpawnOutputNode, class UNiagaraNodeOutput* ParticleUpdateOutputNode, PAPI::PAGravity* InSourceAction)
{
	UNiagaraNodeFunctionCall* StalkerGravityNode = AddModuleFromAssetPath(TEXT("/Game/Base/Particles/Modules/StalkerGravity.StalkerGravity"), *ParticleUpdateOutputNode);
	if (ensure(StalkerGravityNode))
	{
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerGravityNode, "Direction" ,StalkerMath::RBMKLocationToUnreal(InSourceAction->direction));
	}
}

void FRBMKParticlesFactory::ImportActionToEmitter(PS::CPEDef* InParticleEffect, TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs , class UNiagaraEmitter* Emitter, struct FVersionedNiagaraEmitterData* EmitterData, class UNiagaraNodeOutput* EmitterSpawnOutputNode, class UNiagaraNodeOutput* EmitterUpdateOutputNode, class UNiagaraNodeOutput* ParticleSpawnOutputNode, class UNiagaraNodeOutput* ParticleUpdateOutputNode, PAPI::PATurbulence* InSourceAction)
{
	UNiagaraNodeFunctionCall* StalkerTurbulenceNode = AddModuleFromAssetPath(TEXT("/Game/Base/Particles/Modules/StalkerTurbulence.StalkerTurbulence"), *ParticleUpdateOutputNode);
	if (ensure(StalkerTurbulenceNode))
	{	
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerTurbulenceNode, "Epsilon",InSourceAction->epsilon);
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerTurbulenceNode, "Frequency",InSourceAction->frequency);
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerTurbulenceNode, "Magnitude",InSourceAction->magnitude);
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerTurbulenceNode, "Octaves",InSourceAction->octaves);
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerTurbulenceNode, "Offset",StalkerMath::RBMKNormalToUnreal(InSourceAction->offset));
		
	}
}

void FRBMKParticlesFactory::ImportActionToEmitter(PS::CPEDef* InParticleEffect, TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs , class UNiagaraEmitter* Emitter, struct FVersionedNiagaraEmitterData* EmitterData, class UNiagaraNodeOutput* EmitterSpawnOutputNode, class UNiagaraNodeOutput* EmitterUpdateOutputNode, class UNiagaraNodeOutput* ParticleSpawnOutputNode, class UNiagaraNodeOutput* ParticleUpdateOutputNode, PAPI::PATargetSize* InSourceAction)
{
	UNiagaraNodeFunctionCall* StalkerTargetSizeNode = AddModuleFromAssetPath(TEXT("/Game/Base/Particles/Modules/StalkerTargetSize.StalkerTargetSize"), *ParticleUpdateOutputNode);
	if (ensure(StalkerTargetSizeNode))
	{
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerTargetSizeNode, "Scale" ,FVector3f(InSourceAction->scale.x,InSourceAction->scale.y,InSourceAction->scale.z));
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerTargetSizeNode, "Size" ,FVector3f(InSourceAction->size.x,InSourceAction->size.y,InSourceAction->size.z)*100.f);
	}
}

void FRBMKParticlesFactory::ImportActionToEmitter(PS::CPEDef* InParticleEffect, TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs , class UNiagaraEmitter* Emitter, struct FVersionedNiagaraEmitterData* EmitterData, class UNiagaraNodeOutput* EmitterSpawnOutputNode, class UNiagaraNodeOutput* EmitterUpdateOutputNode, class UNiagaraNodeOutput* ParticleSpawnOutputNode, class UNiagaraNodeOutput* ParticleUpdateOutputNode, PAPI::PATargetVelocity* InSourceAction)
{
	UNiagaraNodeFunctionCall*StalkerTargetVelocityNode = AddModuleFromAssetPath(TEXT("/Game/Base/Particles/Modules/StalkerTargetVelocity.StalkerTargetVelocity"), *ParticleUpdateOutputNode);
	if (ensure(StalkerTargetVelocityNode))
	{
		SetModuleParameter(InEmiter,*StalkerTargetVelocityNode,TEXT("AllowRotate"),InSourceAction->m_Flags.test(PAPI::ParticleAction::ALLOW_ROTATE));
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerTargetVelocityNode, "Scale" ,InSourceAction->scale);
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerTargetVelocityNode, "Velocity" ,StalkerMath::RBMKLocationToUnreal(InSourceAction->velocity));
	}
}

void FRBMKParticlesFactory::ImportActionToEmitter(PS::CPEDef* InParticleEffect, TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs , class UNiagaraEmitter* Emitter, struct FVersionedNiagaraEmitterData* EmitterData, class UNiagaraNodeOutput* EmitterSpawnOutputNode, class UNiagaraNodeOutput* EmitterUpdateOutputNode, class UNiagaraNodeOutput* ParticleSpawnOutputNode, class UNiagaraNodeOutput* ParticleUpdateOutputNode, PAPI::PATargetRotate* InSourceAction)
{	
	UNiagaraNodeFunctionCall* StalkerTargetRotateNode = AddModuleFromAssetPath(TEXT("/Game/Base/Particles/Modules/StalkerTargetRotate.StalkerTargetRotate"), *ParticleUpdateOutputNode);
	if (ensure(StalkerTargetRotateNode))
	{
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerTargetRotateNode, "Scale" ,InSourceAction->scale);
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerTargetRotateNode, "Rotate" ,FMath::RadiansToDegrees(FVector3f(InSourceAction->rot.x,InSourceAction->rot.y,InSourceAction->rot.z)));
	}
}

void FRBMKParticlesFactory::ImportActionToEmitter(PS::CPEDef* InParticleEffect, TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs , class UNiagaraEmitter* Emitter, struct FVersionedNiagaraEmitterData* EmitterData, class UNiagaraNodeOutput* EmitterSpawnOutputNode, class UNiagaraNodeOutput* EmitterUpdateOutputNode, class UNiagaraNodeOutput* ParticleSpawnOutputNode, class UNiagaraNodeOutput* ParticleUpdateOutputNode, PAPI::PAScatter* InSourceAction)
{
	UNiagaraNodeFunctionCall* StalkerScatterNode = AddModuleFromAssetPath(TEXT("/Game/Base/Particles/Modules/StalkerScatter.StalkerScatter"), *ParticleUpdateOutputNode);
	if (ensure(StalkerScatterNode))
	{
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerScatterNode, "Center" ,StalkerMath::RBMKLocationToUnreal(InSourceAction->center));
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerScatterNode, "Epsilon" ,InSourceAction->epsilon);
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerScatterNode, "Magnitude" ,InSourceAction->magnitude);
		if (InSourceAction->max_radius*InSourceAction->max_radius < P_MAXFLOAT)
		{
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerScatterNode, "MaxRadius" ,InSourceAction->max_radius);
			SetModuleParameter(InEmiter,*StalkerScatterNode,TEXT("UseMaxRadius"),true);
		}
	}
}

void FRBMKParticlesFactory::ImportActionToEmitter(PS::CPEDef* InParticleEffect, TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs , class UNiagaraEmitter* Emitter, struct FVersionedNiagaraEmitterData* EmitterData, class UNiagaraNodeOutput* EmitterSpawnOutputNode, class UNiagaraNodeOutput* EmitterUpdateOutputNode, class UNiagaraNodeOutput* ParticleSpawnOutputNode, class UNiagaraNodeOutput* ParticleUpdateOutputNode, PAPI::PASpeedLimit* InSourceAction)
{
	UNiagaraNodeFunctionCall* StalkerSpeedLimitNode = AddModuleFromAssetPath(TEXT("/Game/Base/Particles/Modules/StalkerSpeedLimit.StalkerSpeedLimit"), *ParticleUpdateOutputNode);
	if (ensure(StalkerSpeedLimitNode))
	{
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerSpeedLimitNode, "MaxSpeed" ,InSourceAction->max_speed*100);
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerSpeedLimitNode, "MinSpeed" ,InSourceAction->min_speed*100);
	}

}

void FRBMKParticlesFactory::ImportActionToEmitter(PS::CPEDef* InParticleEffect, TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs , class UNiagaraEmitter* Emitter, struct FVersionedNiagaraEmitterData* EmitterData, class UNiagaraNodeOutput* EmitterSpawnOutputNode, class UNiagaraNodeOutput* EmitterUpdateOutputNode, class UNiagaraNodeOutput* ParticleSpawnOutputNode, class UNiagaraNodeOutput* ParticleUpdateOutputNode, PAPI::PASink* InSourceAction)
{
	UNiagaraNodeFunctionCall* StalkerSinkNode = AddModuleFromAssetPath(TEXT("/Game/Base/Particles/Modules/StalkerSink.StalkerSink"), *ParticleUpdateOutputNode);
	if (ensure(StalkerSinkNode))
	{
		SetModuleParameter(InEmiter,*StalkerSinkNode,TEXT("AllowRotate"),InSourceAction->m_Flags.test(PAPI::ParticleAction::ALLOW_ROTATE));
		{
			EStalkerParticleDomainType DomainType ;
			FStalkerParticleDomain Domain;
			XRayDomainToUnreal(InSourceAction->position,DomainType,Domain,true);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerSinkNode, "Position1", Domain.Position1);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerSinkNode, "Position2", Domain.Position2);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerSinkNode, "U", Domain.U);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerSinkNode, "V", Domain.V);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerSinkNode, "Radius1", Domain.Radius1);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerSinkNode, "Radius2", Domain.Radius2);
			SetModuleEnum(InEmiter,*StalkerSinkNode,TEXT("PositionType"),StaticEnum<EStalkerParticleDomainType>(),static_cast<int32>(DomainType),false);
		}
		SetModuleParameter(InEmiter,*StalkerSinkNode,TEXT("KillInside"),InSourceAction->kill_inside);
		
	}
}

void FRBMKParticlesFactory::ImportActionToEmitter(PS::CPEDef* InParticleEffect, TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs , class UNiagaraEmitter* Emitter, struct FVersionedNiagaraEmitterData* EmitterData, class UNiagaraNodeOutput* EmitterSpawnOutputNode, class UNiagaraNodeOutput* EmitterUpdateOutputNode, class UNiagaraNodeOutput* ParticleSpawnOutputNode, class UNiagaraNodeOutput* ParticleUpdateOutputNode, PAPI::PARandomAccel* InSourceAction)
{
	UNiagaraNodeFunctionCall* StalkerRandomAccelNode = AddModuleFromAssetPath(TEXT("/Game/Base/Particles/Modules/StalkerRandomAccel.StalkerRandomAccel"), *ParticleUpdateOutputNode);
	if (ensure(StalkerRandomAccelNode))
	{
		SetModuleParameter(InEmiter,*StalkerRandomAccelNode,TEXT("AllowRotate"),InSourceAction->m_Flags.test(PAPI::ParticleAction::ALLOW_ROTATE));
		{
			EStalkerParticleDomainType DomainType ;
			FStalkerParticleDomain Domain;
			XRayDomainToUnreal(InSourceAction->gen_acc,DomainType,Domain,true);			
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerRandomAccelNode, "Position1", Domain.Position1);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerRandomAccelNode, "Position2", Domain.Position2);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerRandomAccelNode, "U", Domain.U);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerRandomAccelNode, "V", Domain.V);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerRandomAccelNode, "Radius1", Domain.Radius1);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerRandomAccelNode, "Radius2", Domain.Radius2);
			SetModuleEnum(InEmiter,*StalkerRandomAccelNode,TEXT("GenAccType"),StaticEnum<EStalkerParticleDomainType>(),static_cast<int32>(DomainType),false);
		}
	}

}

void FRBMKParticlesFactory::ImportActionToEmitter(PS::CPEDef* InParticleEffect, TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs , class UNiagaraEmitter* Emitter, struct FVersionedNiagaraEmitterData* EmitterData, class UNiagaraNodeOutput* EmitterSpawnOutputNode, class UNiagaraNodeOutput* EmitterUpdateOutputNode, class UNiagaraNodeOutput* ParticleSpawnOutputNode, class UNiagaraNodeOutput* ParticleUpdateOutputNode, PAPI::PAOrbitPoint* InSourceAction)
{
	UNiagaraNodeFunctionCall* StalkerOrbitPointNode = AddModuleFromAssetPath(TEXT("/Game/Base/Particles/Modules/StalkerOrbitPoint.StalkerOrbitPoint"), *ParticleUpdateOutputNode);
	if (ensure(StalkerOrbitPointNode))
	{
		SetModuleParameter(InEmiter,*StalkerOrbitPointNode,TEXT("AllowRotate"),InSourceAction->m_Flags.test(PAPI::ParticleAction::ALLOW_ROTATE));
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerOrbitPointNode, "Epsilon" ,InSourceAction->epsilon);
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerOrbitPointNode, "Magnitude" ,InSourceAction->magnitude);
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerOrbitPointNode, "Center" ,StalkerMath::RBMKLocationToUnreal(InSourceAction->center));
		if (InSourceAction->max_radius*InSourceAction->max_radius < P_MAXFLOAT)
		{
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerOrbitPointNode, "MaxRadius" ,InSourceAction->max_radius);
			SetModuleParameter(InEmiter,*StalkerOrbitPointNode,TEXT("UseMaxRadius"),true);
		}
	}
}

void FRBMKParticlesFactory::ImportActionToEmitter(PS::CPEDef* InParticleEffect, TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs , class UNiagaraEmitter* Emitter, struct FVersionedNiagaraEmitterData* EmitterData, class UNiagaraNodeOutput* EmitterSpawnOutputNode, class UNiagaraNodeOutput* EmitterUpdateOutputNode, class UNiagaraNodeOutput* ParticleSpawnOutputNode, class UNiagaraNodeOutput* ParticleUpdateOutputNode, PAPI::PABounce* InSourceAction)
{	
	UNiagaraNodeFunctionCall* StalkerBounceNode = AddModuleFromAssetPath(TEXT("/Game/Base/Particles/Modules/StalkerBounce.StalkerBounce"), *ParticleUpdateOutputNode);
	if (ensure(StalkerBounceNode))
	{
		SetModuleParameter(InEmiter,*StalkerBounceNode,TEXT("AllowRotate"),InSourceAction->m_Flags.test(PAPI::ParticleAction::ALLOW_ROTATE));
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerBounceNode, "CutoffSqr" ,InSourceAction->cutoffSqr*100.f*100.f);
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerBounceNode, "OneMinusFriction" ,InSourceAction->oneMinusFriction);
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerBounceNode, "Resilience" ,InSourceAction->resilience);
		{
			EStalkerParticleDomainType DomainType ;
			FStalkerParticleDomain Domain;
			XRayDomainToUnreal(InSourceAction->position,DomainType,Domain,true);
			
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerBounceNode, "Position.Position1", Domain.Position1);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerBounceNode, "Position.Position2", Domain.Position2);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerBounceNode, "Position.U", Domain.U);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerBounceNode, "Position.V", Domain.V);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerBounceNode, "Position.Radius1", Domain.Radius1);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerBounceNode, "Position.Radius2", Domain.Radius2);

			SetModuleEnum(InEmiter,*StalkerBounceNode,TEXT("PositionType"),StaticEnum<EStalkerParticleDomainType>(),static_cast<int32>(DomainType),false);
		}

	}
}

void FRBMKParticlesFactory::ImportActionToEmitter(PS::CPEDef* InParticleEffect, TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs , class UNiagaraEmitter* Emitter, struct FVersionedNiagaraEmitterData* EmitterData, class UNiagaraNodeOutput* EmitterSpawnOutputNode, class UNiagaraNodeOutput* EmitterUpdateOutputNode, class UNiagaraNodeOutput* ParticleSpawnOutputNode, class UNiagaraNodeOutput* ParticleUpdateOutputNode, PAPI::PAVortex* InSourceAction)
{
	UNiagaraNodeFunctionCall* StalkerVortexNode = AddModuleFromAssetPath(TEXT("/Game/Base/Particles/Modules/StalkerVortex.StalkerVortex"), *ParticleUpdateOutputNode);
	if (ensure(StalkerVortexNode))
	{
		SetModuleParameter(InEmiter,*StalkerVortexNode,TEXT("AllowRotate"),InSourceAction->m_Flags.test(PAPI::ParticleAction::ALLOW_ROTATE));
		if (InSourceAction->max_radius*InSourceAction->max_radius < P_MAXFLOAT)
		{
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerVortexNode, "MaxRadius" ,InSourceAction->max_radius);
			SetModuleParameter(InEmiter,*StalkerVortexNode,TEXT("UseMaxRadius"),true);
		}
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerVortexNode, "Magnitude" ,InSourceAction->magnitude);
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerVortexNode, "Epsilon" ,InSourceAction->epsilon);
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerVortexNode, "Axis" ,StalkerMath::RBMKNormalToUnreal(InSourceAction->axis));
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerVortexNode, "Center" ,StalkerMath::RBMKNormalToUnreal(InSourceAction->center));


	}
}

void FRBMKParticlesFactory::ImportActionToEmitter(PS::CPEDef* InParticleEffect, TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs , class UNiagaraEmitter* Emitter, struct FVersionedNiagaraEmitterData* EmitterData, class UNiagaraNodeOutput* EmitterSpawnOutputNode, class UNiagaraNodeOutput* EmitterUpdateOutputNode, class UNiagaraNodeOutput* ParticleSpawnOutputNode, class UNiagaraNodeOutput* ParticleUpdateOutputNode, PAPI::PADamping* InSourceAction)
{
	UNiagaraNodeFunctionCall* StalkerDampingNode = AddModuleFromAssetPath(TEXT("/Game/Base/Particles/Modules/StalkerDamping.StalkerDamping"), *ParticleUpdateOutputNode);
	if (ensure(StalkerDampingNode))
	{
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerDampingNode, "Damping" ,StalkerMath::RBMKNormalToUnreal(InSourceAction->damping));
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerDampingNode, "Low" ,InSourceAction->vlowSqr*100.f);
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerDampingNode, "High" ,InSourceAction->vhighSqr*100.f);
	}
}

void FRBMKParticlesFactory::ImportActionToEmitter(PS::CPEDef* InParticleEffect, TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter, void* GroupEffect, TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter, EImportToEmitterAs ImportToEmitterAs, class UNiagaraEmitter* Emitter, struct FVersionedNiagaraEmitterData* EmitterData, class UNiagaraNodeOutput* EmitterSpawnOutputNode, class UNiagaraNodeOutput* EmitterUpdateOutputNode, class UNiagaraNodeOutput* ParticleSpawnOutputNode, class UNiagaraNodeOutput* ParticleUpdateOutputNode, PAPI::PAGravitate* InSourceAction)
{
	EmitterData->bRequiresPersistentIDs = true;
	UNiagaraNodeFunctionCall* StalkerGravitateConstuctorNode = AddModuleFromAssetPath(TEXT("/Game/Base/Particles/Modules/StalkerGravitateConstuctor.StalkerGravitateConstuctor"), *EmitterSpawnOutputNode);
	if (ensure(StalkerGravitateConstuctorNode))
	{
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->EmitterSpawnScriptProps.Script,*StalkerGravitateConstuctorNode, "MaxParticles",int32(InParticleEffect->m_MaxParticles));
	}
	UNiagaraNodeFunctionCall* StalkerGravitateNode = AddModuleFromAssetPath(TEXT("/Game/Base/Particles/Modules/StalkerGravitate.StalkerGravitate"), *ParticleUpdateOutputNode);
	if (ensure(StalkerGravitateNode))
	{
		if (InSourceAction->max_radius*InSourceAction->max_radius < P_MAXFLOAT)
		{
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerGravitateNode, "MaxRadius" ,InSourceAction->max_radius);
			SetModuleParameter(InEmiter,*StalkerGravitateNode,TEXT("UseMaxRadius"),true);
		}
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerGravitateNode, "Magnitude" ,InSourceAction->magnitude);
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerGravitateNode, "Epsilon" ,InSourceAction->epsilon);
	}
}

void FRBMKParticlesFactory::ImportActionToEmitter(PS::CPEDef* InParticleEffect, TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter, void* GroupEffect, TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter, EImportToEmitterAs ImportToEmitterAs, class UNiagaraEmitter* Emitter, struct FVersionedNiagaraEmitterData* EmitterData, class UNiagaraNodeOutput* EmitterSpawnOutputNode, class UNiagaraNodeOutput* EmitterUpdateOutputNode, class UNiagaraNodeOutput* ParticleSpawnOutputNode, class UNiagaraNodeOutput* ParticleUpdateOutputNode, PAPI::PARandomDisplace* InSourceAction)
{
	UNiagaraNodeFunctionCall* StalkerRandomDisplaceNode = AddModuleFromAssetPath(TEXT("/Game/Base/Particles/Modules/StalkerRandomDisplace.StalkerRandomDisplace"), *ParticleUpdateOutputNode);
	if (ensure(StalkerRandomDisplaceNode))
	{
		SetModuleParameter(InEmiter,*StalkerRandomDisplaceNode,TEXT("AllowRotate"),InSourceAction->m_Flags.test(PAPI::ParticleAction::ALLOW_ROTATE));
		{
			EStalkerParticleDomainType DomainType ;
			FStalkerParticleDomain Domain;
			XRayDomainToUnreal(InSourceAction->gen_disp,DomainType,Domain,true);
		
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerRandomDisplaceNode, "Position1", Domain.Position1);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerRandomDisplaceNode, "Position2", Domain.Position2);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerRandomDisplaceNode, "U", Domain.U);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerRandomDisplaceNode, "V", Domain.V);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerRandomDisplaceNode, "Radius1", Domain.Radius1);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerRandomDisplaceNode, "Radius2", Domain.Radius2);

			SetModuleEnum(InEmiter,*StalkerRandomDisplaceNode,TEXT("PositionType"),StaticEnum<EStalkerParticleDomainType>(),static_cast<int32>(DomainType),false);
		}
		
	}
}

void FRBMKParticlesFactory::ImportActionToEmitter(PS::CPEDef* InParticleEffect, TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter, void* GroupEffect, TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter, EImportToEmitterAs ImportToEmitterAs, class UNiagaraEmitter* Emitter, struct FVersionedNiagaraEmitterData* EmitterData, class UNiagaraNodeOutput* EmitterSpawnOutputNode, class UNiagaraNodeOutput* EmitterUpdateOutputNode, class UNiagaraNodeOutput* ParticleSpawnOutputNode, class UNiagaraNodeOutput* ParticleUpdateOutputNode, PAPI::PARandomVelocity* InSourceAction)
{
	UNiagaraNodeFunctionCall* StalkerRandomVelocityNode = AddModuleFromAssetPath(TEXT("/Game/Base/Particles/Modules/StalkerRandomVelocity.StalkerRandomVelocity"), *ParticleUpdateOutputNode);
	if (ensure(StalkerRandomVelocityNode))
	{
		SetModuleParameter(InEmiter,*StalkerRandomVelocityNode,TEXT("AllowRotate"),InSourceAction->m_Flags.test(PAPI::ParticleAction::ALLOW_ROTATE));
		{
			EStalkerParticleDomainType DomainType ;
			FStalkerParticleDomain Domain;
			XRayDomainToUnreal(InSourceAction->gen_vel,DomainType,Domain,true);
			
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerRandomVelocityNode, "Position1", Domain.Position1);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerRandomVelocityNode, "Position2", Domain.Position2);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerRandomVelocityNode, "U", Domain.U);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerRandomVelocityNode, "V", Domain.V);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerRandomVelocityNode, "Radius1", Domain.Radius1);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerRandomVelocityNode, "Radius2", Domain.Radius2);

			SetModuleEnum(InEmiter,*StalkerRandomVelocityNode,TEXT("PositionType"),StaticEnum<EStalkerParticleDomainType>(),static_cast<int32>(DomainType),false);
		}
		
	}
}

void FRBMKParticlesFactory::ImportActionToEmitter(PS::CPEDef* InParticleEffect, TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter, void* GroupEffect, TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter, EImportToEmitterAs ImportToEmitterAs, class UNiagaraEmitter* Emitter, struct FVersionedNiagaraEmitterData* EmitterData, class UNiagaraNodeOutput* EmitterSpawnOutputNode, class UNiagaraNodeOutput* EmitterUpdateOutputNode, class UNiagaraNodeOutput* ParticleSpawnOutputNode, class UNiagaraNodeOutput* ParticleUpdateOutputNode, PAPI::PAJet* InSourceAction)
{
	UNiagaraNodeFunctionCall* StalkerJetNode = AddModuleFromAssetPath(TEXT("/Game/Base/Particles/Modules/StalkerJet.StalkerJet"), *ParticleUpdateOutputNode);
	if (ensure(StalkerJetNode))
	{
		SetModuleParameter(InEmiter,*StalkerJetNode,TEXT("AllowRotate"),InSourceAction->m_Flags.test(PAPI::ParticleAction::ALLOW_ROTATE));
		{
			EStalkerParticleDomainType DomainType ;
			FStalkerParticleDomain Domain;
			XRayDomainToUnreal(InSourceAction->acc,DomainType,Domain,false);

			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerJetNode, "Acc.Position1", Domain.Position1);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerJetNode, "Acc.Position2", Domain.Position2);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerJetNode, "Acc.U", Domain.U);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerJetNode, "Acc.V", Domain.V);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerJetNode, "Acc.Radius1", Domain.Radius1);
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerJetNode, "Acc.Radius2", Domain.Radius2);

			SetModuleEnum(InEmiter,*StalkerJetNode,TEXT("AccType"),StaticEnum<EStalkerParticleDomainType>(),static_cast<int32>(DomainType),false);
		}
		
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerJetNode, "Center",StalkerMath::RBMKNormalToUnreal(InSourceAction->center));
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerJetNode, "Magnitude" ,InSourceAction->magnitude);
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerJetNode, "Epsilon" ,InSourceAction->epsilon);
		if (InSourceAction->max_radius*InSourceAction->max_radius < P_MAXFLOAT)
		{
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerJetNode, "MaxRadius" ,InSourceAction->max_radius);
			SetModuleParameter(InEmiter,*StalkerJetNode,TEXT("UseMaxRadius"),true);
		}
	}
}

void FRBMKParticlesFactory::ImportActionToEmitter(PS::CPEDef* InParticleEffect, TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter, void* GroupEffect, TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter, EImportToEmitterAs ImportToEmitterAs, class UNiagaraEmitter* Emitter, struct FVersionedNiagaraEmitterData* EmitterData, class UNiagaraNodeOutput* EmitterSpawnOutputNode, class UNiagaraNodeOutput* EmitterUpdateOutputNode, class UNiagaraNodeOutput* ParticleSpawnOutputNode, class UNiagaraNodeOutput* ParticleUpdateOutputNode, PAPI::PAFollow* InSourceAction)
{
	EmitterData->bRequiresPersistentIDs = true;
	UNiagaraNodeFunctionCall* StalkerFollowConstuctorNode = AddModuleFromAssetPath(TEXT("/Game/Base/Particles/Modules/StalkerFollowConstuctor.StalkerFollowConstuctor"), *EmitterSpawnOutputNode);
	if (ensure(StalkerFollowConstuctorNode))
	{
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->EmitterSpawnScriptProps.Script,*StalkerFollowConstuctorNode, "MaxParticles",int32(InParticleEffect->m_MaxParticles));
	}
	UNiagaraNodeFunctionCall* StalkerFollowNode = AddModuleFromAssetPath(TEXT("/Game/Base/Particles/Modules/StalkerFollow.StalkerFollow"), *ParticleUpdateOutputNode);
	if (ensure(StalkerFollowNode))
	{
		if (InSourceAction->max_radius*InSourceAction->max_radius < P_MAXFLOAT)
		{
			SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerFollowNode, "MaxRadius" ,InSourceAction->max_radius);
			SetModuleParameter(InEmiter,*StalkerFollowNode,TEXT("UseMaxRadius"),true);
		}
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerFollowNode, "Magnitude" ,InSourceAction->magnitude);
		SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *StalkerFollowNode, "Epsilon" ,InSourceAction->epsilon);
	}
}

FNiagaraVariable FRBMKParticlesFactory::CreateRapidIterationParameter(const FString& UniqueEmitterName, ENiagaraScriptUsage ScriptUsage, const FName& AliasedInputName, const FNiagaraTypeDefinition& InputType)
{
	FNiagaraVariable InputVariable(InputType, AliasedInputName);
	FNiagaraVariable RapidIterationVariable;
	if (ScriptUsage == ENiagaraScriptUsage::SystemSpawnScript || ScriptUsage == ENiagaraScriptUsage::SystemUpdateScript)
	{
		RapidIterationVariable = FNiagaraUtilities::ConvertVariableToRapidIterationConstantName(InputVariable, nullptr, ScriptUsage); // These names *should* have the emitter baked in...
	}
	else
	{
		RapidIterationVariable = FNiagaraUtilities::ConvertVariableToRapidIterationConstantName(InputVariable, *UniqueEmitterName, ScriptUsage);
	}

	return RapidIterationVariable;
}

template<typename ValueType>
void FRBMKParticlesFactory::SetRapidIterationParameter(FString UniqueEmitterName, UNiagaraScript& TargetScript, UNiagaraNodeFunctionCall& TargetFunctionCallNode,FName InputName, FNiagaraTypeDefinition InputType, ValueType Value)
{
	static_assert(!TIsUECoreVariant<ValueType, double>::Value, "Double core variant. Must be float type!");
	FNiagaraParameterHandle InputHandle = FNiagaraParameterHandle::CreateModuleParameterHandle(InputName);
	FNiagaraParameterHandle AliasedInputHandle = FNiagaraParameterHandle::CreateAliasedModuleParameterHandle(InputHandle, &TargetFunctionCallNode);
	FNiagaraVariable RapidIterationParameter = FRBMKParticlesFactory::CreateRapidIterationParameter(UniqueEmitterName, TargetScript.GetUsage(),		AliasedInputHandle.GetParameterHandleString(), InputType);
	bool bAddParameterIfMissing = true;
	{
		RapidIterationParameter.SetValue(Value);
		TargetScript.RapidIterationParameters.SetParameterData(RapidIterationParameter.GetData(), RapidIterationParameter, bAddParameterIfMissing);
	}
}

UNiagaraNodeFunctionCall* FRBMKParticlesFactory::AddModuleFromAssetPath(FString AssetPath, UNiagaraNodeOutput& TargetOutputNode)
{
	FSoftObjectPath AssetRef(AssetPath);
	UNiagaraScript* AssetScript = Cast<UNiagaraScript>(AssetRef.TryLoad());
	FAssetData ScriptAssetData(AssetScript);
	if (ScriptAssetData.IsValid())
	{
		return FNiagaraStackGraphUtilities::AddScriptModuleToStack(AssetScript, TargetOutputNode);
	}
	else
	{
		UE_LOG(LogXRayImporter, Error, TEXT("Failed to create default modules for emitter.  Missing %s"), *AssetRef.ToString());
		return nullptr;
	}
}

class UNiagaraNodeFunctionCall* FRBMKParticlesFactory::FindModuleInScript(UNiagaraScript* SystemUpdateScript, FSoftObjectPath FunctionPath)
{
	UNiagaraScriptSource* SystemScriptSource = Cast<UNiagaraScriptSource>(SystemUpdateScript->GetLatestSource());
	for (TObjectPtr< UEdGraphNode>& Node : SystemScriptSource->NodeGraph->Nodes)
	{
		if (UNiagaraNodeFunctionCall* NodeFunction = Cast<UNiagaraNodeFunctionCall>(Node))
		{
			if (NodeFunction->FunctionScriptAssetObjectPath == FunctionPath.GetAssetPathName())
			{
				return NodeFunction;
			}
		}
	}
	return nullptr;
}

void FRBMKParticlesFactory::XRayDomainToUnreal(const PAPI::pDomain& InDomain, EStalkerParticleDomainType& OutTypeDomain, FStalkerParticleDomain& OutDomain,bool LocationSpace,bool UnrealTangent)
{
	switch ( InDomain.type)
	{
	case PAPI::PDLine:
		OutTypeDomain = EStalkerParticleDomainType::Line;
		break;
	case PAPI::PDTriangle:
		OutTypeDomain = EStalkerParticleDomainType::Triangle;
		break;
	case PAPI::PDPlane:
		OutTypeDomain = EStalkerParticleDomainType::Plane;
		break;
	case PAPI::PDBox:
		OutTypeDomain = EStalkerParticleDomainType::Box;
		break;
	case PAPI::PDSphere:
		OutTypeDomain = EStalkerParticleDomainType::Sphere;
		break;
	case PAPI::PDCylinder:
		OutTypeDomain = EStalkerParticleDomainType::Cylinder;
		break;
	case PAPI::PDCone:
		OutTypeDomain = EStalkerParticleDomainType::Cone;
		break;
	case PAPI::PDBlob:
		OutTypeDomain = EStalkerParticleDomainType::Blob;
		break;
	case PAPI::PDDisc:
		OutTypeDomain = EStalkerParticleDomainType::Disc;
		break;
	case PAPI::PDRectangle:
		OutTypeDomain = EStalkerParticleDomainType::Rectangle;
		break;
	default:
		OutTypeDomain = EStalkerParticleDomainType::Point;
		break;
	}
	if (UnrealTangent)
	{
		OutDomain.Position1 = StalkerMath::RBMKNormalToUnreal(InDomain.p1);
		OutDomain.Position2 = StalkerMath::RBMKNormalToUnreal(InDomain.p2);
		switch ( InDomain.type)
		{
			case PAPI::PDBox:
			case PAPI::PDCone:
			case PAPI::PDCylinder:
			Swap(OutDomain.Position1.X,OutDomain.Position2.X);
		}
		OutDomain.U = StalkerMath::RBMKNormalToUnreal(InDomain.u);
		OutDomain.V = StalkerMath::RBMKNormalToUnreal(InDomain.v);
	}
	else
	{
		OutDomain.Position1 = FVector3f(InDomain.p1.x,InDomain.p1.y,InDomain.p1.z);
		OutDomain.Position2 = FVector3f(InDomain.p2.x,InDomain.p2.y,InDomain.p2.z);
		OutDomain.U =  FVector3f(InDomain.u.x,InDomain.u.y,InDomain.u.z);
		OutDomain.V = FVector3f(InDomain.v.x,InDomain.v.y,InDomain.v.z);
	}
	OutDomain.Radius1 = InDomain.radius1;
	OutDomain.Radius2 = InDomain.radius2;
	if (LocationSpace)
	{
		OutDomain.Position1*=100;
		OutDomain.Position2*=100;
		OutDomain.U*=100;
		OutDomain.V*=100;
		switch (InDomain.type)
		{
		case PAPI::PDCylinder:
		case PAPI::PDCone:
		case PAPI::PDDisc:
			break;
		default:
			OutDomain.Radius1 *= 100;
			OutDomain.Radius2 *= 100;
			break;
		}
	}

}


void FRBMKParticlesFactory::Flush()
{
	CurrentSlowTask->EnterProgressFrame(0, FText::FromString(FString::Printf(TEXT("WaitForCompilationComplete ..."))));
	//for (TSharedPtr<FNiagaraSystemViewModel>& SystemViewModel : NiagaraSystemViewModels)
	//{
	//	do
	//	{
	//		SystemViewModel->GetSystem().WaitForCompilationComplete(true);
	//		SystemViewModel->RefreshAll();
	//		SystemViewModel->Tick(0);
	//	}
	//	while(SystemViewModel->GetSystem().HasOutstandingCompilationRequests(true));
	//}
	NiagaraSystemViewModels.Empty();
}

void FRBMKParticlesFactory::SetModuleParameter(FString UniqueEmitterName, UNiagaraScript& TargetScript, UNiagaraNodeFunctionCall& TargetFunctionCallNode, FName InputName, float Value)
{
	SetRapidIterationParameter<float>(UniqueEmitterName,TargetScript,TargetFunctionCallNode,InputName,FNiagaraTypeDefinition::GetFloatDef(),Value);
}

void FRBMKParticlesFactory::SetModuleParameter(FString UniqueEmitterName, UNiagaraScript& TargetScript, UNiagaraNodeFunctionCall& TargetFunctionCallNode, FName InputName, int32 Value)
{
	SetRapidIterationParameter<int32>(UniqueEmitterName,TargetScript,TargetFunctionCallNode,InputName,FNiagaraTypeDefinition::GetIntDef(),Value);
}

//void FRBMKParticlesFactory::SetModuleParameter(FString UniqueEmitterName, class UNiagaraScript& TargetScript, class UNiagaraNodeFunctionCall& TargetFunctionCallNode, FName InputName, const FStalkerParticleDomain& Domain)
//{
//	SetRapidIterationParameter<FStalkerParticleDomain>(UniqueEmitterName,TargetScript,TargetFunctionCallNode,InputName,FNiagaraTypeDefinition(FStalkerParticleDomain::StaticStruct()), Domain);
//}

void FRBMKParticlesFactory::SetModuleParameter(TSharedPtr<class FNiagaraEmitterHandleViewModel> Emitter, class UNiagaraNodeFunctionCall& TargetFunctionCallNode, FString Name, bool Value,bool IsStatic)
{
	UNiagaraStackFunctionInput* Input = NewObject<UNiagaraStackFunctionInput>();
	UNiagaraStackEntry::FRequiredEntryData RequiredEntryData(Emitter->GetOwningSystemViewModel(), Emitter->GetEmitterViewModel(), NAME_None, NAME_None, Emitter->GetOwningSystemViewModel()->GetEditorData().GetStackEditorData());
	FNiagaraTypeDefinition InputType = FNiagaraTypeDefinition::GetBoolDef();
	
	Name = TEXT("Module.")+Name;

	Input->Initialize(RequiredEntryData, TargetFunctionCallNode, TargetFunctionCallNode, FName(Name), IsStatic?InputType.ToStaticDef():InputType, IsStatic?EStackParameterBehavior::Static: EStackParameterBehavior::Dynamic, TEXT("None"));
	Input->RefreshChildren();
	TSharedPtr<FStructOnScope> StructValue = MakeShared<FStructOnScope>(InputType.GetStruct());
	FNiagaraBool NiagaraBool;
	NiagaraBool.SetValue(Value);
	FMemory::Memcpy(StructValue->GetStructMemory(), &NiagaraBool, InputType.GetSize());
	Input->SetLocalValue(StructValue.ToSharedRef());
	Input->Finalize();
}

void FRBMKParticlesFactory::SetModuleParameter(TSharedPtr<class FNiagaraEmitterHandleViewModel> Emitter, class UNiagaraNodeFunctionCall& TargetFunctionCallNode, FString Name, UNiagaraDataInterface* DataInterface)
{
	UNiagaraStackFunctionInput* Input = NewObject<UNiagaraStackFunctionInput>();
	UNiagaraStackEntry::FRequiredEntryData RequiredEntryData(Emitter->GetOwningSystemViewModel(), Emitter->GetEmitterViewModel(), NAME_None, NAME_None, Emitter->GetOwningSystemViewModel()->GetEditorData().GetStackEditorData());
	FNiagaraTypeDefinition InputType = FNiagaraTypeDefinition(DataInterface->GetClass());
	
	Name = TEXT("Module.")+Name;
	Input->Initialize(RequiredEntryData, TargetFunctionCallNode, TargetFunctionCallNode, FName(Name), InputType, EStackParameterBehavior::Dynamic, TEXT("None"));
	Input->RefreshChildren();
	if (Input->GetDataValueObject())
	{
		DataInterface->CopyTo(Input->GetDataValueObject());
		DataInterface->OnChanged().Broadcast();
	}
	Input->Finalize();
}

void FRBMKParticlesFactory::SetModuleParameter(FString UniqueEmitterName, class UNiagaraScript& TargetScript, class UNiagaraNodeFunctionCall& TargetFunctionCallNode, FName InputName, const FLinearColor& Color)
{
	SetRapidIterationParameter<FLinearColor>(UniqueEmitterName,TargetScript,TargetFunctionCallNode,InputName,FNiagaraTypeDefinition::GetColorDef(), Color);
}

void FRBMKParticlesFactory::SetModuleParameter(FString UniqueEmitterName, class UNiagaraScript& TargetScript, class UNiagaraNodeFunctionCall& TargetFunctionCallNode, FName InputName, const FVector3f& Vector)
{
	SetRapidIterationParameter<FVector3f>(UniqueEmitterName,TargetScript,TargetFunctionCallNode,InputName,FNiagaraTypeDefinition::GetVec3Def(), Vector);

}

void FRBMKParticlesFactory::SetModuletLinkedParameter(TSharedPtr<class FNiagaraEmitterHandleViewModel> Emitter, class UNiagaraNodeFunctionCall& TargetFunctionCallNode, FString Name, const struct FNiagaraTypeDefinition& InputType, const class FNiagaraParameterHandle& NiagaraParameterHandle)
{
	UNiagaraStackFunctionInput* Input = NewObject<UNiagaraStackFunctionInput>();
	UNiagaraStackEntry::FRequiredEntryData RequiredEntryData(Emitter->GetOwningSystemViewModel(), Emitter->GetEmitterViewModel(), NAME_None, NAME_None, Emitter->GetOwningSystemViewModel()->GetEditorData().GetStackEditorData());
	
	Name = TEXT("Module.")+Name;

	Input->Initialize(RequiredEntryData, TargetFunctionCallNode, TargetFunctionCallNode, FName(Name), InputType, EStackParameterBehavior::Dynamic, TEXT("None"));
	Input->RefreshChildren();
	Input->SetLinkedValueHandle(NiagaraParameterHandle);
	Input->Finalize();
}

void FRBMKParticlesFactory::SetModuleBlueprintEnum(TSharedPtr<class FNiagaraEmitterHandleViewModel> Emitter, class UNiagaraNodeFunctionCall& TargetFunctionCallNode, FString Name, FString EnumPath, FString DisplayEnumValue,bool IsStatic)
{
	UUserDefinedEnum* BlueprintEnum = LoadObject<UUserDefinedEnum>(nullptr,*EnumPath);
	if (ensure(BlueprintEnum))
	{
		int32 Index = INDEX_NONE;
		for (auto& [Key, Value] : BlueprintEnum->DisplayNameMap)
		{
			if (Value.ToString() == DisplayEnumValue)
			{
				Index = BlueprintEnum->GetValueByName(Key);
				break;
			}
		}
		Name = TEXT("Module.")+Name;
		if (ensure(Index != INDEX_NONE))
		{
			UNiagaraStackFunctionInput* Input = NewObject<UNiagaraStackFunctionInput>();
			UNiagaraStackEntry::FRequiredEntryData RequiredEntryData(Emitter->GetOwningSystemViewModel(), Emitter->GetEmitterViewModel(), NAME_None, NAME_None, Emitter->GetOwningSystemViewModel()->GetEditorData().GetStackEditorData());
			FNiagaraTypeDefinition InputType(BlueprintEnum);

			Input->Initialize(RequiredEntryData, TargetFunctionCallNode, TargetFunctionCallNode, FName(Name),  IsStatic?InputType.ToStaticDef():InputType, EStackParameterBehavior::Dynamic, TEXT("None"));
			Input->RefreshChildren();
			TSharedPtr<FStructOnScope> StructValue = MakeShared<FStructOnScope>(InputType.GetStruct());
			FNiagaraInt32 NiagaraInt32;
			NiagaraInt32.Value = Index;
			FMemory::Memcpy(StructValue->GetStructMemory(), &NiagaraInt32, InputType.GetSize());
			Input->SetLocalValue(StructValue.ToSharedRef());
			Input->Finalize();
		}
	}
}

void FRBMKParticlesFactory::SetModuleBlueprintEnum(TSharedPtr<FNiagaraSystemViewModel> System, class UNiagaraNodeFunctionCall& TargetFunctionCallNode, FString Name, FString EnumPath, FString DisplayEnumValue, bool IsStatic /*= true*/)
{
	UUserDefinedEnum* BlueprintEnum = LoadObject<UUserDefinedEnum>(nullptr,*EnumPath);
	if (ensure(BlueprintEnum))
	{
		int32 Index = INDEX_NONE;
		for (auto& [Key, Value] : BlueprintEnum->DisplayNameMap)
		{
			if (Value.ToString() == DisplayEnumValue)
			{
				Index = BlueprintEnum->GetValueByName(Key);
				break;
			}
		}
		Name = TEXT("Module.")+Name;
		if (ensure(Index != INDEX_NONE))
		{
			UNiagaraStackFunctionInput* Input = NewObject<UNiagaraStackFunctionInput>();
			UNiagaraStackEntry::FRequiredEntryData RequiredEntryData(System.ToSharedRef(), nullptr, NAME_None, NAME_None, System->GetEditorData().GetStackEditorData());
			FNiagaraTypeDefinition InputType(BlueprintEnum);

			Input->Initialize(RequiredEntryData, TargetFunctionCallNode, TargetFunctionCallNode, FName(Name),  IsStatic?InputType.ToStaticDef():InputType, EStackParameterBehavior::Dynamic, TEXT("None"));
			Input->RefreshChildren();
			TSharedPtr<FStructOnScope> StructValue = MakeShared<FStructOnScope>(InputType.GetStruct());
			FNiagaraInt32 NiagaraInt32;
			NiagaraInt32.Value = Index;
			FMemory::Memcpy(StructValue->GetStructMemory(), &NiagaraInt32, InputType.GetSize());
			Input->SetLocalValue(StructValue.ToSharedRef());
			Input->Finalize();
		}
	}
}

void FRBMKParticlesFactory::SetModuleEnum(TSharedPtr<class FNiagaraEmitterHandleViewModel> Emitter, class UNiagaraNodeFunctionCall& TargetFunctionCallNode, FString Name, UEnum* EnumClass, int32 Value,bool IsStatic)
{
		Name = TEXT("Module.")+Name;
		UNiagaraStackFunctionInput* Input = NewObject<UNiagaraStackFunctionInput>();
		UNiagaraStackEntry::FRequiredEntryData RequiredEntryData(Emitter->GetOwningSystemViewModel(), Emitter->GetEmitterViewModel(), NAME_None, NAME_None, Emitter->GetOwningSystemViewModel()->GetEditorData().GetStackEditorData());
		FNiagaraTypeDefinition InputType(EnumClass);

		Input->Initialize(RequiredEntryData, TargetFunctionCallNode, TargetFunctionCallNode, FName(Name), IsStatic?InputType.ToStaticDef(): InputType, IsStatic?EStackParameterBehavior::Static: EStackParameterBehavior::Dynamic, TEXT("None"));
		Input->RefreshChildren();
		TSharedPtr<FStructOnScope> StructValue = MakeShared<FStructOnScope>(InputType.GetStruct());
		FNiagaraInt32 NiagaraInt32;
		NiagaraInt32.Value = Value;
		FMemory::Memcpy(StructValue->GetStructMemory(), &NiagaraInt32, InputType.GetSize());
		Input->SetLocalValue(StructValue.ToSharedRef());
		Input->Finalize();
}
