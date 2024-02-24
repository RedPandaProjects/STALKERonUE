#include "StalkerSOCLoadingScreen.h"

#include <Widgets/Layout/SConstraintCanvas.h>

#include "MoviePlayer.h"
#include "Widgets/Images/SThrobber.h"

#include "Kernel/StalkerEngineManager.h"
#include "Resources/StalkerResourcesManager.h"

#define LOCTEXT_NAMESPACE "StalkerModule"
void SStalkerSOCLoadingScreen::Construct(const FArguments& InArgs,int32 InMaxStatus)
{
	MaxStatus = InMaxStatus;
	StatusTextColor = FColor::White;
	SetCursor(EMouseCursor::None);
	ChildSlot
	[
		SNew( SConstraintCanvas )
		+SConstraintCanvas::Slot()
		.Anchors( FAnchors( 0.329266f, 0.888095f, 0.672522f, 0.938384f ) )
		.Offset( FMargin() )
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
		   .HAlign(HAlign_Center)
			[
				
			   SNew(SHorizontalBox)
			   +SHorizontalBox::Slot()
			   .AutoWidth()
			   .VAlign(VAlign_Bottom)
			   [
				    SNew(SHorizontalBox)
				   +SHorizontalBox::Slot()
				   .FillWidth(80)
				   .VAlign(VAlign_Bottom)
				   [
						SNew(SCircularThrobber)
						.Visibility_Raw(this, &SStalkerSOCLoadingScreen::GetThrobberVisibility)
				   ]
				   +SHorizontalBox::Slot()
				    .AutoWidth()
				   .VAlign(VAlign_Bottom)
				   [
						SNew(STextBlock)
						.Text_Raw(this, &SStalkerSOCLoadingScreen::GetLoadingStatusText)
						.ColorAndOpacity_Raw(this, &SStalkerSOCLoadingScreen::GetLoadingStatusColor)
						.Font(FSlateFontInfo(GStalkerEngineManager->GetResourcesManager()->LoadingScreenManager.MainFont,26))
				   ]

			   ]
			]
		]
	];
	BackgroundSlateBrush =  FDeferredCleanupSlateBrush::CreateBrush(GStalkerEngineManager->GetResourcesManager()->LoadingScreenManager.BackgroundImage);
}

int32 SStalkerSOCLoadingScreen::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	auto AddPlane = [&AllottedGeometry](const FVector4f&Rect,const FVector4f&RectUV,TArray<FSlateVertex>&VerticesCahce,TArray<SlateIndex>&IndexesCahce)
	{
		auto AddVertex = [&VerticesCahce,&AllottedGeometry](const FVector2D&Position,const FVector2f&UV)
		{
			FSlateVertex& NewVert = VerticesCahce.AddDefaulted_GetRef();
			NewVert.Position = FVector2f(AllottedGeometry.LocalToAbsolute(Position));
			NewVert.Color = FColor::White;
			NewVert.TexCoords[0] = UV.X;
			NewVert.TexCoords[1] = UV.Y;
			NewVert.TexCoords[2] = NewVert.TexCoords[3] = 1.0f;
		};

		const int32 IndexStart = VerticesCahce.Num();
		AddVertex({Rect.X,Rect.Y},{RectUV.X,RectUV.Y});
		AddVertex({Rect.Z,Rect.Y},{RectUV.Z,RectUV.Y});
		AddVertex({Rect.X,Rect.W},{RectUV.X,RectUV.W});
		AddVertex({Rect.Z,Rect.W},{RectUV.Z,RectUV.W});
		IndexesCahce.Add(IndexStart+0);IndexesCahce.Add(IndexStart+1);IndexesCahce.Add(IndexStart+2);
		IndexesCahce.Add(IndexStart+1);IndexesCahce.Add(IndexStart+2);IndexesCahce.Add(IndexStart+3);
	};

	{
		const FSlateResourceHandle BackgroundHandle = BackgroundSlateBrush->GetSlateBrush()->GetRenderingResource();
		TArray<FSlateVertex> VerticesCache; TArray<SlateIndex> IndexesCache;
		AddPlane(FVector4f(0.f,0.f,AllottedGeometry.GetLocalSize().X,AllottedGeometry.GetLocalSize().Y),FVector4f(0,0,1,0.75f),VerticesCache,IndexesCache);
		FSlateDrawElement::MakeCustomVerts(OutDrawElements, LayerId++,BackgroundHandle,VerticesCache,IndexesCache,nullptr, 0, 0);
	}
	if(MapSlateBrush)
	{
		const FSlateResourceHandle MapImageHandle = MapSlateBrush->GetSlateBrush()->GetRenderingResource();
		TArray<FSlateVertex> VerticesCache; TArray<SlateIndex> IndexesCache;
		FVector4f Rect;
		Rect.X = 257.f/1024;
		Rect.Y = 369.f/768.f;
		Rect.Z = (257.f+512)/1024;
		Rect.W = (369.f+256)/768.f;
		Rect*=FVector4f(AllottedGeometry.GetLocalSize().X,AllottedGeometry.GetLocalSize().Y,AllottedGeometry.GetLocalSize().X,AllottedGeometry.GetLocalSize().Y);
		AddPlane(Rect,FVector4f(0,0,1,1.f),VerticesCache,IndexesCache);
		FSlateDrawElement::MakeCustomVerts(OutDrawElements, LayerId++,MapImageHandle,VerticesCache,IndexesCache,nullptr, 0, 0);
	}
	{
		const FSlateResourceHandle BackgroundHandle = BackgroundSlateBrush->GetSlateBrush()->GetRenderingResource();
		TArray<FSlateVertex> VerticesCache; TArray<SlateIndex> IndexesCache;

		auto AddVertex = [&VerticesCache,&AllottedGeometry](const FVector2D&Position,const FVector2f&UV,const FColor&Color)
		{
			FSlateVertex& NewVert = VerticesCache.AddDefaulted_GetRef();
			NewVert.Position = FVector2f(AllottedGeometry.LocalToAbsolute(Position));
			NewVert.Color = Color;
			NewVert.TexCoords[0] = UV.X;
			NewVert.TexCoords[1] = UV.Y;
			NewVert.TexCoords[2] = NewVert.TexCoords[3] = 1.0f;
		};
		auto CalculateProgressColor = [this](int32 Index,int32 Count)
		{
			if(Index>(Count/2)) 
			{
				Index	= Count-Index;
			}

			const float IndexStatusPerCount = (static_cast<float>(IndexStatus + 1)/static_cast<float>(MaxStatus))*(Count/2.0f);
			const float Alpha = 1/(exp((static_cast<float>(Index)-IndexStatusPerCount)*0.5f)+1.0f);
			return FColor(255,252,255,FMath::Clamp(Alpha,0,1)*255.f);
		};

		FVector4f Rect;
		Rect.X = 379.f/1024;
		Rect.Y = 726.f/768.f;
		Rect.Z = Rect.X + 268.f/1024;
		Rect.W = Rect.Y + 37.f/768.f;
		
		FVector4f RectUV;
		RectUV.X = 0.f/1024.f;
		RectUV.Y = 768.f/1024.f;
		RectUV.Z = RectUV.X + 268.f/1024.f;
		RectUV.W = RectUV.Y + 37.f/1024.f;

		Rect*=FVector4f(AllottedGeometry.GetLocalSize().X,AllottedGeometry.GetLocalSize().Y,AllottedGeometry.GetLocalSize().X,AllottedGeometry.GetLocalSize().Y);

		constexpr int32 Count = 40;
		const float PositionDelta = (Rect.Z-Rect.X)/Count;
		const float PositionUVDelta = (RectUV.Z-RectUV.X)/Count;
		for(int32 i = 0;i<=Count;i++)
		{

			AddVertex(FVector2D(Rect.X+PositionDelta*i,Rect.Y),FVector2f(RectUV.X+PositionUVDelta*i,RectUV.Y),CalculateProgressColor(i,Count));
			AddVertex(FVector2D(Rect.X+PositionDelta*i,Rect.W),FVector2f(RectUV.X+PositionUVDelta*i,RectUV.W),CalculateProgressColor(i,Count));
			const int32 IndexStart = i*2;
			if(i<Count)
			{
				IndexesCache.Add(IndexStart+0);IndexesCache.Add(IndexStart+1);IndexesCache.Add(IndexStart+2);
				IndexesCache.Add(IndexStart+1);IndexesCache.Add(IndexStart+2);IndexesCache.Add(IndexStart+3);
			}
		}
		FSlateDrawElement::MakeCustomVerts(OutDrawElements, LayerId++,BackgroundHandle,VerticesCache,IndexesCache,nullptr, 0, 0);
	}

	LayerId = SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	return LayerId;
	
}

void SStalkerSOCLoadingScreen::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime,const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	FStalkerLoadingUpdateData UpdateData ;
	if(GStalkerEngineManager->GetResourcesManager()->LoadingScreenManager.UpdateData.Dequeue(UpdateData))
	{
		switch (UpdateData.Type)
		{
		case EStalkerLoadingUpdateDataType::NextStatus:
			StatusText = FText::FromString(UpdateData.Text);
			IndexStatus = UpdateData.IndexStatus;
			break;
		case EStalkerLoadingUpdateDataType::SetMapImage:
			MapSlateBrush = FDeferredCleanupSlateBrush::CreateBrush(UpdateData.MapImage);
			break;
		case EStalkerLoadingUpdateDataType::EndLoad:
			StatusText = LOCTEXT("SStalkerLoadingScreen","Press any key for enter to game");
			IsLoadEnd = true;
			break;
		}
	}
	if(IsLoadEnd)
	{
		StatusTextColor.A = FMath::Abs(FMath::Sin(InCurrentTime*UE_PI))*255;
	}
	
}

FText SStalkerSOCLoadingScreen::GetLoadingStatusText() const
{
	return StatusText;
}

EVisibility SStalkerSOCLoadingScreen::GetThrobberVisibility() const
{
	if(IsLoadEnd)
	{
		return EVisibility::Collapsed;
	}
	return EVisibility::Visible;
}

FSlateColor SStalkerSOCLoadingScreen::GetLoadingStatusColor() const
{
	return StatusTextColor;
}

#undef LOCTEXT_NAMESPACE
