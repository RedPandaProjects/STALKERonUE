#include "XRayParticlesFactory.h"
#include "Engine/EngineTypes.h"
#include "ImportUtils/StaticMeshImportUtils.h"
#include "../StalkerEditorManager.h"
#include "Kernel/StalkerEngineManager.h"
THIRD_PARTY_INCLUDES_START
#include "Editors/XrECore/Particles/PSLibrary.h"
#include "Editors/XrECore/Particles/ParticleEffectDef.h"
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
#include "XRayEngineFactory.h"
#include "NiagaraScriptSourceBase.h"
#include "NiagaraDataInterfaceSpriteRendererInfo.h"
#include "NiagaraNodeFunctionCall.h"
#include "ViewModels/Stack/NiagaraStackFunctionInput.h"
#include "ViewModels/Stack/NiagaraStackEntry.h"
#include "NiagaraSystemEditorData.h"
#include "ViewModels/Stack/NiagaraStackInputCategory.h"

XRayParticlesFactory::XRayParticlesFactory()
{

}

XRayParticlesFactory::~XRayParticlesFactory()
{

}

void XRayParticlesFactory::ImportParticles()
{
	UNiagaraEmitter;
	CPSLibrary PSLibrary;
	PSLibrary.OnCreate();
	for (auto Iterator = PSLibrary.FirstPED(); Iterator != PSLibrary.LastPED(); Iterator++)
	{
		PS::CPEDef* PEDef = (*Iterator);
		if (FCStringAnsi::Strrstr(PEDef->Name(), "campfire_01_flame"))
		{
			ImportParticle(PEDef);
		}
		 //PEDef->m_Name.
	}
	PSLibrary.OnDestroy();
}


UNiagaraSystem* XRayParticlesFactory::ImportParticle(PS::CPEDef* PEDef)
{

	FString Name =  PEDef->m_Name.c_str();
	Name.ReplaceCharInline(TEXT('\\'), TEXT('/'));
	const FString PackageName = UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("Particles")/ Name);
	const FString NewObjectPath = PackageName + TEXT(".") + FPaths::GetBaseFilename(PackageName);

	UNiagaraSystem* NiagaraSystem = LoadObject<UNiagaraSystem>(nullptr, *NewObjectPath, nullptr, LOAD_NoWarn);
	if (NiagaraSystem)
		return NiagaraSystem;
		
	UPackage* AssetPackage = CreatePackage(*PackageName);
	NiagaraSystem = NewObject<UNiagaraSystem>(AssetPackage, *FPaths::GetBaseFilename(PackageName), RF_Standalone|RF_Public);
	UNiagaraSystemFactoryNew::InitializeSystem(NiagaraSystem,true);
	FAssetRegistryModule::AssetCreated(NiagaraSystem);
	{
		TSharedPtr<FNiagaraSystemViewModel> SystemViewModel = MakeShared<FNiagaraSystemViewModel>();
		FNiagaraSystemViewModelOptions SystemViewModelOptions = FNiagaraSystemViewModelOptions();
		SystemViewModelOptions.bCanAutoCompile = false;
		SystemViewModelOptions.bCanSimulate = false;
		SystemViewModelOptions.EditMode = ENiagaraSystemViewModelEditMode::SystemAsset;
		SystemViewModelOptions.MessageLogGuid = NiagaraSystem->GetAssetGuid();
		SystemViewModel->Initialize(*NiagaraSystem, SystemViewModelOptions);
		ImportToEmitter(PackageName,PEDef, SystemViewModel->AddEmptyEmitter());
		SystemViewModel->RefreshAll();
	}
	NiagaraSystem->Modify();
	return NiagaraSystem;
}


void XRayParticlesFactory::ImportToEmitter(const FString&PackageName ,PS::CPEDef* InParticleEffect, TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter)
{
	auto AddModuleFromAssetPath = [](FString AssetPath, UNiagaraNodeOutput& TargetOutputNode)-> UNiagaraNodeFunctionCall*
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
			UE_LOG(LogNiagaraEditor, Error, TEXT("Failed to create default modules for emitter.  Missing %s"), *AssetRef.ToString());
			return nullptr;
		}
	};

	XRayEngineFactory EngineFactory(nullptr,RF_Standalone|RF_Public);
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

	{
		UNiagaraSpriteRendererProperties*NiagaraSpriteRendererProperties =  NewObject<UNiagaraSpriteRendererProperties>(Emitter, "Renderer");
		NiagaraSpriteRendererProperties->Material = EngineFactory.ImportSurface(PackageName+TEXT("_Mat"),InParticleEffect->m_ShaderName,InParticleEffect->m_TextureName,"");
		Emitter->AddRenderer(NiagaraSpriteRendererProperties, EmitterData->Version.VersionGuid);
		if(InParticleEffect->m_Flags.is(PS::CPEDef::dfAlignToPath))
		{
			NiagaraSpriteRendererProperties->Alignment = ENiagaraSpriteAlignment::VelocityAligned;
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
					SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->UpdateScriptProps.Script, *SubUVNode, "Frames Per Second",FMath::RoundToInt32(InParticleEffect->m_Frame.m_fSpeed));
					SetModuleParameter(InEmiter,*SubUVNode,TEXT("Random Start Frame"),InParticleEffect->m_Flags.is(PS::CPEDef::dfRandomFrame));
				}
				else
				{
					SetModuleBlueprintEnum(InEmiter, *SubUVNode, TEXT("SubUV Animation Mode"), TEXT("/Niagara/Enums/ENiagara_SubUVLookupModeV2.ENiagara_SubUVLookupModeV2"), TEXT("Random"));
				}
			}
		}

		{
			UNiagaraNodeFunctionCall* EmitterStateNode = AddModuleFromAssetPath(TEXT("/Niagara/Modules/Emitter/EmitterState.EmitterState"), *EmitterUpdateOutputNode);
			if (UNiagaraNodeFunctionCall* SpawnRateNode = AddModuleFromAssetPath(TEXT("/Niagara/Modules/Emitter/SpawnRate.SpawnRate"), *EmitterUpdateOutputNode))
			{
				SetModuleParameter(Emitter->GetUniqueEmitterName(), *EmitterData->EmitterUpdateScriptProps.Script, *SpawnRateNode,"SpawnRate",10.f);
			}
		}
	}

}

FNiagaraVariable XRayParticlesFactory::CreateRapidIterationParameter(const FString& UniqueEmitterName, ENiagaraScriptUsage ScriptUsage, const FName& AliasedInputName, const FNiagaraTypeDefinition& InputType)
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
void XRayParticlesFactory::SetRapidIterationParameter(FString UniqueEmitterName, UNiagaraScript& TargetScript, UNiagaraNodeFunctionCall& TargetFunctionCallNode,FName InputName, FNiagaraTypeDefinition InputType, ValueType Value)
{
	static_assert(!TIsUECoreVariant<ValueType, double>::Value, "Double core variant. Must be float type!");
	FNiagaraParameterHandle InputHandle = FNiagaraParameterHandle::CreateModuleParameterHandle(InputName);
	FNiagaraParameterHandle AliasedInputHandle = FNiagaraParameterHandle::CreateAliasedModuleParameterHandle(InputHandle, &TargetFunctionCallNode);
	FNiagaraVariable RapidIterationParameter = XRayParticlesFactory::CreateRapidIterationParameter(UniqueEmitterName, TargetScript.GetUsage(),		AliasedInputHandle.GetParameterHandleString(), InputType);
	bool bAddParameterIfMissing = true;
	{
		RapidIterationParameter.SetValue(Value);
		TargetScript.RapidIterationParameters.SetParameterData(RapidIterationParameter.GetData(), RapidIterationParameter, bAddParameterIfMissing);
	}
}
void XRayParticlesFactory::SetModuleParameter(FString UniqueEmitterName, UNiagaraScript& TargetScript, UNiagaraNodeFunctionCall& TargetFunctionCallNode, FName InputName, float Value)
{
	SetRapidIterationParameter<float>(UniqueEmitterName,TargetScript,TargetFunctionCallNode,InputName,FNiagaraTypeDefinition::GetFloatDef(),Value);
}

void XRayParticlesFactory::SetModuleParameter(FString UniqueEmitterName, UNiagaraScript& TargetScript, UNiagaraNodeFunctionCall& TargetFunctionCallNode, FName InputName, int32 Value)
{
	SetRapidIterationParameter<int32>(UniqueEmitterName,TargetScript,TargetFunctionCallNode,InputName,FNiagaraTypeDefinition::GetIntDef(),Value);
}

void XRayParticlesFactory::SetModuleParameter(TSharedPtr<class FNiagaraEmitterHandleViewModel> Emitter, class UNiagaraNodeFunctionCall& TargetFunctionCallNode, FString Name, bool Value)
{
	UNiagaraStackFunctionInput* Input = NewObject<UNiagaraStackFunctionInput>();
	UNiagaraStackEntry::FRequiredEntryData RequiredEntryData(Emitter->GetOwningSystemViewModel(), Emitter->GetEmitterViewModel(), NAME_None, NAME_None, Emitter->GetOwningSystemViewModel()->GetEditorData().GetStackEditorData());
	FNiagaraTypeDefinition InputType = FNiagaraTypeDefinition::GetBoolDef();
	
	Name = TEXT("Module.")+Name;

	Input->Initialize(RequiredEntryData, TargetFunctionCallNode, TargetFunctionCallNode, FName(Name), InputType, EStackParameterBehavior::Dynamic, TEXT("None"));
	Input->RefreshChildren();
	TSharedPtr<FStructOnScope> StructValue = MakeShared<FStructOnScope>(InputType.GetStruct());
	FNiagaraBool NiagaraBool;
	NiagaraBool.SetValue(Value);
	FMemory::Memcpy(StructValue->GetStructMemory(), &NiagaraBool, InputType.GetSize());
	Input->SetLocalValue(StructValue.ToSharedRef());
	Input->Finalize();
}

void XRayParticlesFactory::SetModuleParameter(TSharedPtr<class FNiagaraEmitterHandleViewModel> Emitter, class UNiagaraNodeFunctionCall& TargetFunctionCallNode, FString Name, UNiagaraDataInterface* DataInterface)
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

void XRayParticlesFactory::SetModuleBlueprintEnum(TSharedPtr<class FNiagaraEmitterHandleViewModel> Emitter, class UNiagaraNodeFunctionCall& TargetFunctionCallNode, FString Name, FString EnumPath, FString DisplayEnumValue)
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

			Input->Initialize(RequiredEntryData, TargetFunctionCallNode, TargetFunctionCallNode, FName(Name), InputType, EStackParameterBehavior::Dynamic, TEXT("None"));
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
