#pragma once
#include "EdMode.h"
class  FStalkerAIMapEditMode : public FEdMode
{
public:
	
	const static FEditorModeID EM_AIMap;
	FStalkerAIMapEditMode();
	~FStalkerAIMapEditMode();
	//class UTurnOneNavigationSystem* GetTurnOneNavigationSystem();
	void Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI) override;

	bool HandleClick(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click) override;


	void Enter() override;
	void RemoveSelectNodes();

	void Exit() override;

	bool IsAddMode();
	void SetAddMode();
	void SetSelectionMode();
	void ClearAIMap();
	void ClearSelectionNodes();

	bool BoxSelect(FBox& InBox, bool InSelect = true) override;


	bool InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event) override;


	bool FrustumSelect(const FConvexVolume& InFrustum, FEditorViewportClient* InViewportClient, bool InSelect = true) override;

	bool InputDelta(FEditorViewportClient* InViewportClient, FViewport* InViewport, FVector& InDrag, FRotator& InRot, FVector& InScale) override;


	FVector GetWidgetLocation() const override;


	bool UsesTransformWidget() const override;


	bool ShowModeWidgets() const override;


	bool ShouldDrawWidget() const override;
	void LinkLeft();
	void LinkRight();
	void LinkForward();
	void LinkBackward();
	void Link(int32 ID);
	void LinkAll();
	bool StartTracking(FEditorViewportClient* InViewportClient, FViewport* InViewport) override;
	bool EndTracking(FEditorViewportClient* InViewportClient, FViewport* InViewport) override;
	void GenerateSelect();
	void GenerateFull();
	void SmoothFull();
	void SmoothSelect();
	void ResetSelect();

	enum ELinkMode
	{
		LM_Add,
		LM_Remove,
		LM_Invert
	};
	ELinkMode LinkMode;
	bool bIgnoreConstraints;
	bool bAutoLink;


private:
	int32 ConvertV2L(int32 Side);
	bool CanRemoveSelectNodes();
	bool bIsAddMode;
	TSharedPtr<FUICommandList> AIMapEdModeActions;
};
