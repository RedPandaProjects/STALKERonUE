#include "StalkerNiagaraSystem.h"

void UStalkerNiagaraSystem::PreSave(FObjectPreSaveContext ObjectSaveContext)
{
	UFXSystemAsset::PreSave(ObjectSaveContext);
	EnsureFullyLoaded();
#if WITH_EDITORONLY_DATA
	WaitForCompilationComplete();
#endif
}
