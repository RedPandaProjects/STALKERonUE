#pragma once
namespace PS
{
	class CPEDef;
}
enum class ENiagaraScriptUsage : uint8;
class XRayParticlesFactory
{
public:
	XRayParticlesFactory			();
	~XRayParticlesFactory			();	
	void							ImportParticles					();
	class UNiagaraSystem*			ImportParticle					(PS::CPEDef*PEDef);		
private:
	void							ImportToEmitter					(const FString&PackageName ,PS::CPEDef*InParticleEffect,TSharedPtr<class FNiagaraEmitterHandleViewModel> InEmiter);
	void							SetModuleParameter				(FString UniqueEmitterName,class UNiagaraScript& TargetScript,class UNiagaraNodeFunctionCall& TargetFunctionCallNode,FName InputName, float Value);
	void							SetModuleParameter				(FString UniqueEmitterName,class UNiagaraScript& TargetScript,class UNiagaraNodeFunctionCall& TargetFunctionCallNode,FName InputName, int32 Value);
	void							SetModuleBlueprintEnum			(TSharedPtr<class FNiagaraEmitterHandleViewModel> Emitter,class UNiagaraNodeFunctionCall& TargetFunctionCallNode,FString Name,FString EnumPath,FString DisplayEnumValue);
	void							SetModuleParameter				(TSharedPtr<class FNiagaraEmitterHandleViewModel> Emitter,class UNiagaraNodeFunctionCall& TargetFunctionCallNode,FString Name,bool Value);
	void							SetModuleParameter				(TSharedPtr<class FNiagaraEmitterHandleViewModel> Emitter,class UNiagaraNodeFunctionCall& TargetFunctionCallNode,FString Name,class UNiagaraDataInterface*DataInterface);
	static struct FNiagaraVariable	CreateRapidIterationParameter	(const FString& UniqueEmitterName, ENiagaraScriptUsage ScriptUsage, const FName& AliasedInputName, const struct FNiagaraTypeDefinition& InputType);
	template<typename ValueType>
	inline static void				SetRapidIterationParameter		(FString UniqueEmitterName, UNiagaraScript& TargetScript, UNiagaraNodeFunctionCall& TargetFunctionCallNode,FName InputName, FNiagaraTypeDefinition InputType, ValueType Value);
};