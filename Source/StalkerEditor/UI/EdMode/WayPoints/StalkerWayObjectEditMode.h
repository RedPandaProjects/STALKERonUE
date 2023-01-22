#pragma once
#include "EdMode.h"
class  FStalkerWayObjectEditMode : public FEdMode
{
public:
	
								FStalkerWayObjectEditMode		();
								~FStalkerWayObjectEditMode		();
	void						Render							(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI) override;
	bool						HandleClick						(FEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FViewportClick& Click) override;
	void						Enter							() override;
	void						Exit							() override;
	bool						BoxSelect						(FBox& InBox, bool InSelect = true) override;
	bool						InputKey						(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event) override;
	bool						FrustumSelect					(const FConvexVolume& InFrustum, FEditorViewportClient* InViewportClient, bool InSelect = true) override;
	bool						InputDelta						(FEditorViewportClient* InViewportClient, FViewport* InViewport, FVector& InDrag, FRotator& InRot, FVector& InScale) override;
	FVector						GetWidgetLocation				() const override;
	bool						UsesTransformWidget				() const override;
	bool						ShowModeWidgets					() const override;
	bool						ShouldDrawWidget				() const override;
	void						SetSelectMode					();
	void						SetAddPointMode					();
	inline bool					IsAddPointMode					(){return bIsAddWayPointMode;}
	bool						Select							(AActor* InActor, bool bInSelected) override;
	bool						IsSelectionAllowed				(AActor* InActor, bool bInSelection) const override;

	void						AddReferencedObjects			(FReferenceCollector& Collector) override;
	FString						GetReferencerName				() const override;
	void						OnLevelActorRemoved				(AActor*);
	void						InvertLinkSelect();
	void						RemoveLinkSelect();
	void						Convert1LinkSelect();
	void						Convert2LinkSelect();
	void						Add1LinkSelect();
	void						Add2LinkSelect();
	bool						IsEnabled();
	bool						IsEnabledAction();


	const static FEditorModeID	EM_WayObject;
	bool						bAutoLink;
private:
	void						ClearSelectedPoint();
	void						UpdateSelectedPoint();
	void						RemoveSelectPoints();
	bool						CanRemoveSelectPoints();
	void						SelectALLPoints();
	bool						CanSelectALLPoints();

	class AStalkerWayObject*	WayObject;
	bool						bIsAddWayPointMode;
	TSharedPtr<FUICommandList>	WayObjectEdModeActions;
};
