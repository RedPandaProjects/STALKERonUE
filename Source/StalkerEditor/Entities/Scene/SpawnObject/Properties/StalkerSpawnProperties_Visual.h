#pragma once
#include "StalkerSpawnProperties_Visual.generated.h"

UCLASS(DefaultToInstanced)
class  UStalkerSpawnProperties_Visual : public UObject
{
	GENERATED_BODY()
public:
	void SetEntity		(ISE_Abstract* InEntity);
	void FillProperties	();

	UPROPERTY(EditAnywhere, Category = "Base")
	class USkeletalMesh*			Kinematics;
	UPROPERTY(EditAnywhere, Category = "Base")
	class UAnimSequence*			Anim;
	UPROPERTY(EditAnywhere,Category = "Base")
	bool Obstacle;
	void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

protected:
	void		SetAnim			();
	ISE_Abstract* Entity = nullptr;
	ISE_Visual* EntityVisual = nullptr;
};