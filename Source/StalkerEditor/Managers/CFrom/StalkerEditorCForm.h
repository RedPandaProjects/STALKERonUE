#pragma once
#include "StalkerEditorCForm.generated.h"
UCLASS()
class UStalkerEditorCForm : public UObject
{
	GENERATED_BODY()
public:
	void	Initialize				();
	void	Destroy					();
private:
	void	OnGetOnScreenMessages	(FCoreDelegates::FSeverityMessageMap& Out);
	void	Build					();
	void	OnPreBeginPIE			(const bool);
	TSharedPtr< FUICommandList>		CFormCommands;

	UPROPERTY()
	TMap<class UStalkerPhysicalMaterial*,int32> 	PhysicalMaterial2ID;
};
