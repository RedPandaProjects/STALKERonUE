#include "StalkerRBMKUIWidget.h"
#include "Kernel/RBMK/Render/Interface/UI/XRayUIRender.h"

int32 UStalkerRBMKUIWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	FVector2D ScreenSize(Device->dwWidth, Device->dwHeight);
	FVector2D MultiplerSize = AllottedGeometry.GetLocalSize() / ScreenSize;

	TUniquePtr<FRBMKUIRenderLayer> *Layer =  GXRayUIRender.Layers.Find(static_cast<ERBMKUILayer>(IndexLayer));
	if(!Layer)
	{
		return LayerId;
	}
	for (FRBMKUIRenderPrimitive& Item : Layer->Get()->Items)
	{
		if (Item.TextID >= 0)
		{
			FRBMKUIRenderText& TextItem = Layer->Get()->Texts[Item.TextID];
			FSlateFontInfo FontInfo = TextItem.Font->GetLegacySlateFontInfo();
			FontInfo.OutlineSettings.OutlineColor = TextItem.Color.ReinterpretAsLinear();

			FontInfo.Size = TextItem.FontSize;
			if (Item.ScissorsID >= 0)
			{
				FVector2D XY(GXRayUIRender.Scissors[Item.ScissorsID].X, GXRayUIRender.Scissors[Item.ScissorsID].Y);
				FVector2D ZW(GXRayUIRender.Scissors[Item.ScissorsID].Z, GXRayUIRender.Scissors[Item.ScissorsID].W);

				XY = AllottedGeometry.LocalToAbsolute(XY * MultiplerSize);
				ZW = AllottedGeometry.LocalToAbsolute(ZW * MultiplerSize);

				FSlateRect ClipRect(XY, ZW);
				FSlateClippingZone Clip(ClipRect);
				OutDrawElements.PushClip(Clip);
			}
			FSlateDrawElement::MakeText(OutDrawElements, LayerId++, AllottedGeometry.ToPaintGeometry(FVector2D(TextItem.Position.X, TextItem.Position.Y ) / TextItem.Scale, AllottedGeometry.GetLocalSize(), FMath::Sqrt(MultiplerSize.X * MultiplerSize.Y) * TextItem.Scale), TextItem.Data, FontInfo, ESlateDrawEffect::NoGamma, TextItem.Color.ReinterpretAsLinear());
			if (Item.ScissorsID >= 0)
			{
				OutDrawElements.PopClip();
			}
			continue;
		}
		if (Item.StartVertex == Item.EndVertex)
		{
			continue;
		}
		if (Item.Brush == nullptr)
		{
			continue;
		}
		if (Item.PointType == IUIRender::pttLIT )
		{
			continue;
		}
		switch (Item.PrimitiveType)
		{
		case IUIRender::ptTriList:
		case IUIRender::ptTriStrip:
		break;
		case IUIRender::ptLineStrip:
		{
			if (Item.EndVertex - Item.StartVertex)
			{
				for (uint32 i = Item.StartVertex; i < Item.EndVertex-1; i++)
				{
					VerticesLineCahce.Empty(2);
					FRBMKUIRenderVertex Vertex = GXRayUIRender.Vertices[i];
					VerticesLineCahce.Add(FVector2f(AllottedGeometry.LocalToAbsolute(FVector2D(Vertex.Position) * MultiplerSize)));
					Vertex = GXRayUIRender.Vertices[i+1];
					VerticesLineCahce.Add(FVector2f(AllottedGeometry.LocalToAbsolute(FVector2D(Vertex.Position) * MultiplerSize)));
					FSlateDrawElement::MakeLines(OutDrawElements, LayerId++, AllottedGeometry.ToPaintGeometry(), VerticesLineCahce);
				}
			}
			
		}
			continue;
		case IUIRender::ptLineList:
		{
			for (uint32 i = Item.StartVertex; i < Item.EndVertex; i++)
			{
				VerticesLineCahce.Empty(2);
				FRBMKUIRenderVertex Vertex = GXRayUIRender.Vertices[i++];
				VerticesLineCahce.Add(FVector2f(FVector2D(Vertex.Position) * MultiplerSize));
				Vertex = GXRayUIRender.Vertices[i];
				VerticesLineCahce.Add(FVector2f(FVector2D(Vertex.Position) * MultiplerSize));
				FSlateDrawElement::MakeLines(OutDrawElements, LayerId++, AllottedGeometry.ToPaintGeometry(FVector2D(0,0), AllottedGeometry.GetLocalSize(),1.f), VerticesLineCahce);
			}
		}
			continue;
		default:
			continue;
		}


		FSlateResourceHandle Handle = Item.Brush->Brush.GetRenderingResource();
		const FSlateShaderResourceProxy* ResourceProxy = Handle.GetResourceProxy();
		FVector2f StartUV = FVector2f(0,0);
		FVector2f SizeUV = FVector2f(1,1);
		if (ResourceProxy)
		{
			StartUV = ResourceProxy->StartUV;
			SizeUV = ResourceProxy->SizeUV;
		}
		VerticesCahce.Empty();
		for (uint32 i = Item.StartVertex; i < Item.EndVertex; i++)
		{
			FRBMKUIRenderVertex Vertex = GXRayUIRender.Vertices[i];
			VerticesCahce.AddDefaulted();
			FSlateVertex& NewVert = VerticesCahce.Last();
			NewVert.Position = FVector2f(AllottedGeometry.LocalToAbsolute(FVector2D(Vertex.Position) * MultiplerSize));
			NewVert.Color = Vertex.Color;
			NewVert.TexCoords[0] = StartUV.X + Vertex.UV.X* SizeUV.X;
			NewVert.TexCoords[1] = StartUV.Y + Vertex.UV.Y * SizeUV.Y;
			NewVert.TexCoords[2] = NewVert.TexCoords[3] = 1.0f;
		}
		IndexesCahce.Empty();
		switch (Item.PrimitiveType)
		{
		case IUIRender::ptTriList:
		{	
			for (uint32 i = 0; i < Item.EndVertex - Item.StartVertex; i++)
			{
				IndexesCahce.Add(i);
			}
		}
		break;
		case IUIRender::ptTriStrip:
		{
			for (uint32 i = 0; i < Item.EndVertex - Item.StartVertex-2; i++)
			{
				for (int32 a = 0; a < 3; a++)
				{
					IndexesCahce.Add(i + a);
				}
			}
		}
		break;
		}
		if (Item.ScissorsID >= 0)
		{
			FVector2D XY(GXRayUIRender.Scissors[Item.ScissorsID].X, GXRayUIRender.Scissors[Item.ScissorsID].Y);
			FVector2D ZW(GXRayUIRender.Scissors[Item.ScissorsID].Z, GXRayUIRender.Scissors[Item.ScissorsID].W);

			XY = AllottedGeometry.LocalToAbsolute(XY * MultiplerSize) ;
			ZW = AllottedGeometry.LocalToAbsolute(ZW * MultiplerSize) ;

			FSlateRect ClipRect(XY,ZW);
			FSlateClippingZone Clip(ClipRect);
			OutDrawElements.PushClip(Clip);
		}
		FSlateDrawElement::MakeCustomVerts(OutDrawElements, LayerId++, Handle, VerticesCahce, IndexesCahce, nullptr, 0, 0);
		if (Item.ScissorsID >= 0)
		{
			OutDrawElements.PopClip();
		}
	}
	return LayerId;
}
