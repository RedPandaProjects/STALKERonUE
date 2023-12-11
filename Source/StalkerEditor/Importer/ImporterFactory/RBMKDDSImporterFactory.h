#pragma once
#include "RBMKDDSImporterFactory.generated.h"
UCLASS(transient)
class URBMKDDSImporterFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

public:
	UObject*			FactoryCreateFile	(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;
	void				CleanUp				() override;
	bool				FactoryCanImport	(const FString& Filename) override;

	TArray<FString>		GetFormats			() const override;

};
