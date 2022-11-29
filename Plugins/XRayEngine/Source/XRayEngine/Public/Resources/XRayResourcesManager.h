#pragma once
#include "XRayResourcesManager.generated.h"

UCLASS()
class UXRayResourcesManager : public UObject
{
	GENERATED_BODY()
public:
	USlateBrushAsset*			GetBrush(FName NameMaterial, FName NameTexture);
	UFont*						GetFont(FName Name);
	void						Free(USlateBrushAsset* Brush);
	USlateBrushAsset*			Copy(USlateBrushAsset* Brush);
	void						CheckLeak();
	void						Reload();

	class AXRaySkeletonMesh*	SpawnSkeletonMesh(class XRayKinematics* Kinematics);
	void						Destroy(class AXRaySkeletonMesh*Mesh);
	UPROPERTY(Transient)
	TObjectPtr < class UWorld>	GameWorld;
private:

	UPROPERTY(Transient)
	TMap<FName, UFont*> Fonts;

	UPROPERTY(Transient)
	TMap<USlateBrushAsset*,int32> BrushesCounter;
	UPROPERTY(Transient)
	TMap<USlateBrushAsset*, UMaterialInstanceDynamic*> BrushesMaterials;
#if WITH_EDITORONLY_DATA
	TSet< USlateBrushAsset*>	 BrushesNeedReloading;
#endif
	TMap<FName, TMap<FName, USlateBrushAsset*>> Brushes;
	struct  BrushInfo
	{
		FName Matrrial;
		FName Texture;
	};
	TMap<USlateBrushAsset*, BrushInfo> BrushesInfo;
	
	UPROPERTY(Transient)
	TSet<AXRaySkeletonMesh*> Meshes;
};