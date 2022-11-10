// Fill out your copyright notice in the Description page of Project Settings.


#include "Render/XRayUIWidget.h"
#include "Interface/UI/XRayUIRender.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/IGame_Persistent.h"
THIRD_PARTY_INCLUDES_END
ENGINE_API BOOL g_bRendering;
bool UXRayUIWidget::Initialize()
{
	return Super::Initialize();
}

int32 UXRayUIWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	if(Device->dwFrame!=LastFrame)
	{
		LastFrame = Device->dwFrame;
		g_bRendering = true;
		GXRayUIRender.Flush();
		Device->seqRenderUI.Process(rp_RenderUI);
		if (g_pGamePersistent)	g_pGamePersistent->OnRenderPPUI_main();
		g_bRendering = false;
	}

	for (XRayUIRender::Item& Item : GXRayUIRender.Items)
	{
		if (Item.StartVertex == Item.EndVertex)
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
			continue;
		case IUIRender::ptLineList:
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
			XRayUIRender::Vertex Vertex = GXRayUIRender.Vertices[i];
			VerticesCahce.AddDefaulted();
			FSlateVertex& NewVert = VerticesCahce.Last();
			NewVert.Position = FVector2f(AllottedGeometry.LocalToAbsolute(FVector2D(Vertex.Position)));
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

			XY = AllottedGeometry.LocalToAbsolute(XY);
			ZW = AllottedGeometry.LocalToAbsolute(ZW);

			FSlateRect ClipRect(XY,ZW);
			FSlateClippingZone Clip(ClipRect);
			OutDrawElements.PushClip(Clip);
		}
		FSlateDrawElement::MakeCustomVerts(OutDrawElements, LayerId, Handle, VerticesCahce, IndexesCahce, nullptr, 0, 0);
		if (Item.ScissorsID >= 0)
		{
			OutDrawElements.PopClip();
		}
	}

	return LayerId;
}
