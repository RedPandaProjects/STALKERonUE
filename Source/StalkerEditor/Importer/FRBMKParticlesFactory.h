#pragma once
#include "FRBMKEngineFactory.h"
#include "Resources/Particle/StalkerParticleDomain.h"
namespace PS
{
	class CPEDef;
	class CPGDef;
}
namespace PAPI
{
	struct PASource;
	struct PABounce;
	struct PAKillOld;
	struct pDomain;
	struct PATargetColor;
	struct PAMove;
	struct PAGravity;
	struct PATurbulence;
	struct PATargetSize;
	struct PATargetRotate;
	struct PATargetVelocity;
	struct PAScatter;
	struct PASpeedLimit;
	struct PASink;
	struct PARandomAccel;
	struct PAVortex;
	struct PAOrbitPoint;
	struct PADamping;
	struct PAGravitate;
	struct PARandomDisplace;
	struct PARandomVelocity;
	struct PAJet;
	struct PAFollow;
	enum PActionEnum;
};

enum class ENiagaraScriptUsage : uint8;

class FRBMKParticlesFactory
{
public:
	FRBMKParticlesFactory			(UObject* InParentPackage, EObjectFlags InFlags);
	~FRBMKParticlesFactory			();	
	void							ImportParticles					();
	class UNiagaraSystem*			ImportParticle					(PS::CPEDef*PEDef);		
	class UNiagaraSystem*			ImportParticle					(PS::CPGDef*PGDef);		
private:
	enum class EImportToEmitterAs
	{
		None,
		Birth,
		Play,
		Dead
	};
	void							ImportToEmitter					(const FString&PackageName ,PS::CPEDef*InParticleEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect = nullptr,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter = nullptr,EImportToEmitterAs ImportToEmitterAs = EImportToEmitterAs::None);
	void							ImportActionToEmitter			(PS::CPEDef*InParticleEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs ,class UNiagaraEmitter*	Emitter,struct FVersionedNiagaraEmitterData* EmitterData,class UNiagaraNodeOutput* EmitterSpawnOutputNode,class UNiagaraNodeOutput* EmitterUpdateOutputNode,class UNiagaraNodeOutput* ParticleSpawnOutputNode,class UNiagaraNodeOutput* ParticleUpdateOutputNode,  PAPI::PASource*InSourceAction);
	void							ImportActionToEmitter			(PS::CPEDef*InParticleEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs ,class UNiagaraEmitter*	Emitter,struct FVersionedNiagaraEmitterData* EmitterData,class UNiagaraNodeOutput* EmitterSpawnOutputNode,class UNiagaraNodeOutput* EmitterUpdateOutputNode,class UNiagaraNodeOutput* ParticleSpawnOutputNode,class UNiagaraNodeOutput* ParticleUpdateOutputNode,  PAPI::PAKillOld*InSourceAction);
	void							ImportActionToEmitter			(PS::CPEDef*InParticleEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs ,class UNiagaraEmitter*	Emitter,struct FVersionedNiagaraEmitterData* EmitterData,class UNiagaraNodeOutput* EmitterSpawnOutputNode,class UNiagaraNodeOutput* EmitterUpdateOutputNode,class UNiagaraNodeOutput* ParticleSpawnOutputNode,class UNiagaraNodeOutput* ParticleUpdateOutputNode,  PAPI::PATargetColor*InSourceAction);
	void							ImportActionToEmitter			(PS::CPEDef*InParticleEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs ,class UNiagaraEmitter*	Emitter,struct FVersionedNiagaraEmitterData* EmitterData,class UNiagaraNodeOutput* EmitterSpawnOutputNode,class UNiagaraNodeOutput* EmitterUpdateOutputNode,class UNiagaraNodeOutput* ParticleSpawnOutputNode,class UNiagaraNodeOutput* ParticleUpdateOutputNode,  PAPI::PAMove*InSourceAction);
	void							ImportActionToEmitter			(PS::CPEDef*InParticleEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs ,class UNiagaraEmitter*	Emitter,struct FVersionedNiagaraEmitterData* EmitterData,class UNiagaraNodeOutput* EmitterSpawnOutputNode,class UNiagaraNodeOutput* EmitterUpdateOutputNode,class UNiagaraNodeOutput* ParticleSpawnOutputNode,class UNiagaraNodeOutput* ParticleUpdateOutputNode,  PAPI::PAGravity*InSourceAction);
	void							ImportActionToEmitter			(PS::CPEDef*InParticleEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs ,class UNiagaraEmitter*	Emitter,struct FVersionedNiagaraEmitterData* EmitterData,class UNiagaraNodeOutput* EmitterSpawnOutputNode,class UNiagaraNodeOutput* EmitterUpdateOutputNode,class UNiagaraNodeOutput* ParticleSpawnOutputNode,class UNiagaraNodeOutput* ParticleUpdateOutputNode,  PAPI::PATurbulence*InSourceAction);
	void							ImportActionToEmitter			(PS::CPEDef*InParticleEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs ,class UNiagaraEmitter*	Emitter,struct FVersionedNiagaraEmitterData* EmitterData,class UNiagaraNodeOutput* EmitterSpawnOutputNode,class UNiagaraNodeOutput* EmitterUpdateOutputNode,class UNiagaraNodeOutput* ParticleSpawnOutputNode,class UNiagaraNodeOutput* ParticleUpdateOutputNode,  PAPI::PATargetSize*InSourceAction);
	void							ImportActionToEmitter			(PS::CPEDef*InParticleEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs ,class UNiagaraEmitter*	Emitter,struct FVersionedNiagaraEmitterData* EmitterData,class UNiagaraNodeOutput* EmitterSpawnOutputNode,class UNiagaraNodeOutput* EmitterUpdateOutputNode,class UNiagaraNodeOutput* ParticleSpawnOutputNode,class UNiagaraNodeOutput* ParticleUpdateOutputNode,  PAPI::PATargetVelocity*InSourceAction);
	void							ImportActionToEmitter			(PS::CPEDef*InParticleEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs ,class UNiagaraEmitter*	Emitter,struct FVersionedNiagaraEmitterData* EmitterData,class UNiagaraNodeOutput* EmitterSpawnOutputNode,class UNiagaraNodeOutput* EmitterUpdateOutputNode,class UNiagaraNodeOutput* ParticleSpawnOutputNode,class UNiagaraNodeOutput* ParticleUpdateOutputNode,  PAPI::PATargetRotate*InSourceAction);
	void							ImportActionToEmitter			(PS::CPEDef*InParticleEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs ,class UNiagaraEmitter*	Emitter,struct FVersionedNiagaraEmitterData* EmitterData,class UNiagaraNodeOutput* EmitterSpawnOutputNode,class UNiagaraNodeOutput* EmitterUpdateOutputNode,class UNiagaraNodeOutput* ParticleSpawnOutputNode,class UNiagaraNodeOutput* ParticleUpdateOutputNode,  PAPI::PAScatter*InSourceAction);
	void							ImportActionToEmitter			(PS::CPEDef*InParticleEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs ,class UNiagaraEmitter*	Emitter,struct FVersionedNiagaraEmitterData* EmitterData,class UNiagaraNodeOutput* EmitterSpawnOutputNode,class UNiagaraNodeOutput* EmitterUpdateOutputNode,class UNiagaraNodeOutput* ParticleSpawnOutputNode,class UNiagaraNodeOutput* ParticleUpdateOutputNode,  PAPI::PASpeedLimit*InSourceAction);
	void							ImportActionToEmitter			(PS::CPEDef*InParticleEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs ,class UNiagaraEmitter*	Emitter,struct FVersionedNiagaraEmitterData* EmitterData,class UNiagaraNodeOutput* EmitterSpawnOutputNode,class UNiagaraNodeOutput* EmitterUpdateOutputNode,class UNiagaraNodeOutput* ParticleSpawnOutputNode,class UNiagaraNodeOutput* ParticleUpdateOutputNode,  PAPI::PASink*InSourceAction);
	void							ImportActionToEmitter			(PS::CPEDef*InParticleEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs ,class UNiagaraEmitter*	Emitter,struct FVersionedNiagaraEmitterData* EmitterData,class UNiagaraNodeOutput* EmitterSpawnOutputNode,class UNiagaraNodeOutput* EmitterUpdateOutputNode,class UNiagaraNodeOutput* ParticleSpawnOutputNode,class UNiagaraNodeOutput* ParticleUpdateOutputNode,  PAPI::PARandomAccel*InSourceAction);
	void							ImportActionToEmitter			(PS::CPEDef*InParticleEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs ,class UNiagaraEmitter*	Emitter,struct FVersionedNiagaraEmitterData* EmitterData,class UNiagaraNodeOutput* EmitterSpawnOutputNode,class UNiagaraNodeOutput* EmitterUpdateOutputNode,class UNiagaraNodeOutput* ParticleSpawnOutputNode,class UNiagaraNodeOutput* ParticleUpdateOutputNode,  PAPI::PAOrbitPoint*InSourceAction);
	void							ImportActionToEmitter			(PS::CPEDef*InParticleEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs ,class UNiagaraEmitter*	Emitter,struct FVersionedNiagaraEmitterData* EmitterData,class UNiagaraNodeOutput* EmitterSpawnOutputNode,class UNiagaraNodeOutput* EmitterUpdateOutputNode,class UNiagaraNodeOutput* ParticleSpawnOutputNode,class UNiagaraNodeOutput* ParticleUpdateOutputNode,  PAPI::PABounce*InSourceAction);
	void							ImportActionToEmitter			(PS::CPEDef*InParticleEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs ,class UNiagaraEmitter*	Emitter,struct FVersionedNiagaraEmitterData* EmitterData,class UNiagaraNodeOutput* EmitterSpawnOutputNode,class UNiagaraNodeOutput* EmitterUpdateOutputNode,class UNiagaraNodeOutput* ParticleSpawnOutputNode,class UNiagaraNodeOutput* ParticleUpdateOutputNode,  PAPI::PAVortex*InSourceAction);
	void							ImportActionToEmitter			(PS::CPEDef*InParticleEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs ,class UNiagaraEmitter*	Emitter,struct FVersionedNiagaraEmitterData* EmitterData,class UNiagaraNodeOutput* EmitterSpawnOutputNode,class UNiagaraNodeOutput* EmitterUpdateOutputNode,class UNiagaraNodeOutput* ParticleSpawnOutputNode,class UNiagaraNodeOutput* ParticleUpdateOutputNode,  PAPI::PADamping*InSourceAction);
	void							ImportActionToEmitter			(PS::CPEDef*InParticleEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs ,class UNiagaraEmitter*	Emitter,struct FVersionedNiagaraEmitterData* EmitterData,class UNiagaraNodeOutput* EmitterSpawnOutputNode,class UNiagaraNodeOutput* EmitterUpdateOutputNode,class UNiagaraNodeOutput* ParticleSpawnOutputNode,class UNiagaraNodeOutput* ParticleUpdateOutputNode,  PAPI::PAGravitate*InSourceAction);
	void							ImportActionToEmitter			(PS::CPEDef*InParticleEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs ,class UNiagaraEmitter*	Emitter,struct FVersionedNiagaraEmitterData* EmitterData,class UNiagaraNodeOutput* EmitterSpawnOutputNode,class UNiagaraNodeOutput* EmitterUpdateOutputNode,class UNiagaraNodeOutput* ParticleSpawnOutputNode,class UNiagaraNodeOutput* ParticleUpdateOutputNode,  PAPI::PARandomDisplace*InSourceAction);
	void							ImportActionToEmitter			(PS::CPEDef*InParticleEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs ,class UNiagaraEmitter*	Emitter,struct FVersionedNiagaraEmitterData* EmitterData,class UNiagaraNodeOutput* EmitterSpawnOutputNode,class UNiagaraNodeOutput* EmitterUpdateOutputNode,class UNiagaraNodeOutput* ParticleSpawnOutputNode,class UNiagaraNodeOutput* ParticleUpdateOutputNode,  PAPI::PARandomVelocity*InSourceAction);
	void							ImportActionToEmitter			(PS::CPEDef*InParticleEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs ,class UNiagaraEmitter*	Emitter,struct FVersionedNiagaraEmitterData* EmitterData,class UNiagaraNodeOutput* EmitterSpawnOutputNode,class UNiagaraNodeOutput* EmitterUpdateOutputNode,class UNiagaraNodeOutput* ParticleSpawnOutputNode,class UNiagaraNodeOutput* ParticleUpdateOutputNode,  PAPI::PAJet*InSourceAction);
	void							ImportActionToEmitter			(PS::CPEDef*InParticleEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter,void*GroupEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> OwnerEmitter ,EImportToEmitterAs ImportToEmitterAs ,class UNiagaraEmitter*	Emitter,struct FVersionedNiagaraEmitterData* EmitterData,class UNiagaraNodeOutput* EmitterSpawnOutputNode,class UNiagaraNodeOutput* EmitterUpdateOutputNode,class UNiagaraNodeOutput* ParticleSpawnOutputNode,class UNiagaraNodeOutput* ParticleUpdateOutputNode,  PAPI::PAFollow*InSourceAction);


	void							SetModuleParameter				(FString UniqueEmitterName,class UNiagaraScript& TargetScript,class UNiagaraNodeFunctionCall& TargetFunctionCallNode,FName InputName, float Value);
	void							SetModuleParameter				(FString UniqueEmitterName,class UNiagaraScript& TargetScript,class UNiagaraNodeFunctionCall& TargetFunctionCallNode,FName InputName, int32 Value);
	//void							SetModuleParameter				(FString UniqueEmitterName,class UNiagaraScript& TargetScript,class UNiagaraNodeFunctionCall& TargetFunctionCallNode,FName InputName, const FStalkerParticleDomain&Domain);
	void							SetModuleParameter				(FString UniqueEmitterName,class UNiagaraScript& TargetScript,class UNiagaraNodeFunctionCall& TargetFunctionCallNode,FName InputName, const FLinearColor&Color);
	void							SetModuleParameter				(FString UniqueEmitterName,class UNiagaraScript& TargetScript,class UNiagaraNodeFunctionCall& TargetFunctionCallNode,FName InputName, const FVector3f&Vector);
	void							SetModuleBlueprintEnum			(TSharedPtr<class FNiagaraEmitterHandleViewModel> Emitter,class UNiagaraNodeFunctionCall& TargetFunctionCallNode,FString Name,FString EnumPath,FString DisplayEnumValue,bool IsStatic = true);
	void							SetModuleBlueprintEnum			(TSharedPtr<class FNiagaraSystemViewModel>  System,class UNiagaraNodeFunctionCall& TargetFunctionCallNode,FString Name,FString EnumPath,FString DisplayEnumValue,bool IsStatic = true);
	
	void							SetModuleEnum					(TSharedPtr<class FNiagaraEmitterHandleViewModel> Emitter,class UNiagaraNodeFunctionCall& TargetFunctionCallNode,FString Name,UEnum*EnumClass,int32 Value,bool IsStatic = true);
	void							SetModuleParameter				(TSharedPtr<class FNiagaraEmitterHandleViewModel> Emitter,class UNiagaraNodeFunctionCall& TargetFunctionCallNode,FString Name,bool Value,bool IsStatic = true);
	void							SetModuleParameter				(TSharedPtr<class FNiagaraEmitterHandleViewModel> Emitter,class UNiagaraNodeFunctionCall& TargetFunctionCallNode,FString Name,class UNiagaraDataInterface*DataInterface);
	void							SetModuletLinkedParameter		(TSharedPtr<class FNiagaraEmitterHandleViewModel> Emitter,class UNiagaraNodeFunctionCall& TargetFunctionCallNode,FString Name, const struct FNiagaraTypeDefinition&InputType,const class FNiagaraParameterHandle&NiagaraParameterHandle);
	static struct FNiagaraVariable	CreateRapidIterationParameter	(const FString& UniqueEmitterName, ENiagaraScriptUsage ScriptUsage, const FName& AliasedInputName, const struct FNiagaraTypeDefinition& InputType);
	template<typename ValueType>
	inline static void				SetRapidIterationParameter		(FString UniqueEmitterName, UNiagaraScript& TargetScript, UNiagaraNodeFunctionCall& TargetFunctionCallNode,FName InputName, FNiagaraTypeDefinition InputType, ValueType Value);
	class UNiagaraNodeFunctionCall*	AddModuleFromAssetPath			(FString AssetPath,class UNiagaraNodeOutput& TargetOutputNode);
	class UNiagaraNodeFunctionCall*	FindModuleInScript				(UNiagaraScript* SystemUpdateScript,FSoftObjectPath FunctionPath);
	void							XRayDomainToUnreal				(const PAPI::pDomain&InDomain,EStalkerParticleDomainType&OutTypeDomain,FStalkerParticleDomain&OutDomain,bool LocationSpace = false,bool UnrealTangent = true);
	
	void							Flush							();
	TArray<PAPI::PActionEnum>		TypesNotImplemented;
	class CPSLibrary				*MyLibrary = nullptr;
	FScopedSlowTask*				CurrentSlowTask = nullptr;
	FRBMKEngineFactory				EngineFactory;
	TArray<TSharedPtr<FNiagaraSystemViewModel>> NiagaraSystemViewModels;
};