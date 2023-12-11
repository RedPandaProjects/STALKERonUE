#pragma once
#include "RBMKTextureTHMImporterFactory.generated.h"
UCLASS(transient)
class URBMKTextureTHMImporterFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

public:
	virtual UObject*	FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;
	virtual void		CleanUp() override;
	virtual bool		FactoryCanImport(const FString& Filename) override;


};
