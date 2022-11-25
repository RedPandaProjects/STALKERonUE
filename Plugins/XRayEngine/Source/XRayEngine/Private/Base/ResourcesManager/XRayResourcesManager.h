#pragma once
#include "XRayResourcesManager.generated.h"

UCLASS()
class UXRayResourcesManager : public UObject
{
	GENERATED_BODY()
public:
	USlateBrushAsset* GetBrush(FName NameMaterial, FName NameTexture);
	UFont*			  GetFont(FName Name);
	void			  Free(USlateBrushAsset* Brush);
	USlateBrushAsset* Copy(USlateBrushAsset* Brush);
	void			  CheckLeak();
private:

	UPROPERTY(Transient)
	TMap<FName, UFont*> Fonts;

	UPROPERTY(Transient)
	TMap<USlateBrushAsset*,int32> BrushesCounter;
	UPROPERTY(Transient)
	TMap<USlateBrushAsset*, UMaterialInstanceDynamic*> BrushesMaterials;

	TMap<FName, TMap<FName, USlateBrushAsset*>> Brushes;
	struct  BrushInfo
	{
		FName Matrrial;
		FName Texture;
	};
	TMap<USlateBrushAsset*, BrushInfo> BrushesInfo;
};