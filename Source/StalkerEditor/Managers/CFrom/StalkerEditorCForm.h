#pragma once
#include "StalkerEditorCForm.generated.h"
UCLASS()
class UStalkerEditorCForm : public UObject
{
	GENERATED_BODY()
public:
	void	Initialize				();
	void	Destroy					();
	void	Build					();
private:
	//void	OnGetOnScreenMessages	(FCoreDelegates::FSeverityMessageMap& Out);

	UPROPERTY()
	TMap<class UStalkerPhysicalMaterial*,int32> 	PhysicalMaterial2ID;
};
