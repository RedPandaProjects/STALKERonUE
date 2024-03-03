#include "XRayUIRender.h"
#include "Resources/StalkerResourcesManager.h"
#include "Kernel/StalkerEngineManager.h"

XRayUIShader::XRayUIShader()
{
	Brush = nullptr;
}

XRayUIShader::~XRayUIShader()
{
	destroy();
}

void XRayUIShader::Copy(IUIShader& _in)
{
	destroy();
	MaterialName = static_cast<XRayUIShader&>(_in).MaterialName;
	TextureName = static_cast<XRayUIShader&>(_in).TextureName; 
	if (static_cast<XRayUIShader&>(_in).Brush)
	{
		Brush = GStalkerEngineManager->GetResourcesManager()->Copy(static_cast<XRayUIShader&>(_in).Brush);
	}
}

void XRayUIShader::create(LPCSTR sh, LPCSTR tex)
{
	MaterialName = sh;
	check(MaterialName != NAME_None);
	TextureName = tex? FName(tex) : NAME_None;
	if (MaterialName != NAME_None)
	{
		Brush = GStalkerEngineManager->GetResourcesManager()->GetBrush(MaterialName, TextureName);
	}
}

bool XRayUIShader::inited()
{
	return !!Brush;
}

void XRayUIShader::destroy()
{
	if (Brush)
	{
		GStalkerEngineManager->GetResourcesManager()->Free(Brush);
		Brush = nullptr;
	}
}

FRBMKUIRender::FRBMKUIRender()
{
	CurrentScissor = -1;
	CurrentLayerType = ERBMKUILayer::Main;
}

FRBMKUIRender::~FRBMKUIRender()
{
}

void FRBMKUIRender::CreateUIGeom()
{
}

void FRBMKUIRender::DestroyUIGeom()
{
	

}

void FRBMKUIRender::SetShader(IUIShader& shader)
{
	CurrentShader.Copy(shader);
}

void FRBMKUIRender::SetAlphaRef(int aref)
{
}

void FRBMKUIRender::SetScissor(Irect* rect)
{
	if (rect)
	{
		FVector4f Scale( 1,1,1,1);
		CurrentScissor = Scissors.AddUnique(FVector4f(rect->x1, rect->y1, rect->x2, rect->y2)*Scale);
	}
	else
	{
		CurrentScissor = -1;
	}
}

void FRBMKUIRender::GetActiveTextureResolution(Fvector2& res)
{
	if(!CurrentShader.Brush)return;
	UTexture**Texture =  GStalkerEngineManager->GetResourcesManager()->BrushesTextures .Find(CurrentShader.Brush);
	if (!Texture)
	{
		res.x =32;
		res.y =32;
		return;
	}
	res.x = (*Texture)->GetSurfaceWidth();
	res.y =  (*Texture)->GetSurfaceHeight();
}

void FRBMKUIRender::PushPoint(float x, float y, float z, u32 C, float u, float v)
{
	FRBMKUIRenderVertex InVertex;
	InVertex.Position.Set(x,y);
	InVertex.Color = FColor(color_rgba(color_get_R(C), color_get_G(C), color_get_B(C), color_get_A(C)));
	InVertex.UV.Set(u,v);
	Vertices.Push(InVertex);
	
}

void FRBMKUIRender::PushText(float x, float y, float Scale, u32 C, UFont* Font, float FontSize, const TCHAR* String)
{
	if(ensure(CurrentLayer))
	{
		FRBMKUIRenderText InText;
		InText.Position.Set(FMath::Floor(x), FMath::Floor(y));
		InText.Color = FColor(color_rgba(color_get_R(C), color_get_G(C), color_get_B(C), color_get_A(C)));
		InText.Data = String;
		InText.Scale = Scale;
		InText.Font = Font;
		InText.FontSize = FontSize;
		CurrentLayer->Texts.Push(InText);

		CurrentLayer->Items.AddDefaulted();
		CurrentLayer->Items.Last().StartVertex = Vertices.Num();
		CurrentLayer->Items.Last().EndVertex = Vertices.Num();
		CurrentLayer->Items.Last().ScissorsID = CurrentScissor;
		CurrentLayer->Items.Last().TextID = CurrentLayer->Texts.Num()-1;
	}
}

void FRBMKUIRender::StartPrimitive(u32 iMaxVerts, ePrimitiveType primType, ePointType pointType)
{
	if(ensure(CurrentLayer))
	{
		CurrentLayer->Items.AddDefaulted();
		CurrentLayer->Items.Last().StartVertex = Vertices.Num();
		CurrentLayer->Items.Last().EndVertex = Vertices.Num();
		CurrentLayer->Items.Last().ScissorsID = CurrentScissor;
		CurrentLayer->Items.Last().PrimitiveType = primType;
		CurrentLayer->Items.Last().PointType = pointType;
		CurrentLayer->Items.Last().Brush = CurrentShader.Brush ? GStalkerEngineManager->GetResourcesManager()->Copy(CurrentShader.Brush) : nullptr;
	}
}
void FRBMKUIRender::FlushPrimitive()
{
	if(ensure(CurrentLayer))
	{
		check(CurrentLayer->Items.Last().TextID==-1);
		CurrentLayer->Items.Last().EndVertex = Vertices.Num();
	}
	CurrentShader.destroy();
}

void FRBMKUIRender::Flush()
{
	for(auto&[Key,Value]:Layers)
	{
		Value->Items.Empty(Value->Items.Num());
		Value->Texts.Empty(Value->Texts.Num());
	}
	Scissors.Empty(Scissors.Num());
	Vertices.Empty(Vertices.Num());

	if(TUniquePtr<FRBMKUIRenderLayer>*Layer = Layers.Find(ERBMKUILayer::Main))
	{
		CurrentLayer = Layer->Get();
	}
	else
	{
		CurrentLayer = Layers.Add(ERBMKUILayer::Main, MakeUnique<FRBMKUIRenderLayer>()).Get();
	}
	CurrentLayerType = ERBMKUILayer::Main;
	if(!ensure(!LayersStack.Num()))
	{
		LayersStack.Empty(LayersStack.Num());
	}
	CurrentScissor = -1;
	CurrentShader.destroy();
}

LPCSTR FRBMKUIRender::UpdateShaderName(LPCSTR tex_name, LPCSTR sh_name)
{
	string_path buff;
	return  FS.exist(buff, "$game_textures$", tex_name, ".ogm") ? "hud\\movie" : sh_name;
}

void FRBMKUIRender::PushLayer(ERBMKUILayer NewLayer)
{
	CurrentScissor = -1;
	CurrentShader.destroy();
	LayersStack.Add(CurrentLayerType);
	if(TUniquePtr<FRBMKUIRenderLayer>*Layer = Layers.Find(NewLayer))
	{
		CurrentLayer = Layer->Get();
	}
	else
	{
		CurrentLayer = Layers.Add(NewLayer, MakeUnique<FRBMKUIRenderLayer>()).Get();
	}
	CurrentLayerType = NewLayer;
}

void FRBMKUIRender::PopLayer()
{
	CurrentLayerType = LayersStack.Pop();
	if(TUniquePtr<FRBMKUIRenderLayer>*Layer = Layers.Find(CurrentLayerType))
	{
		CurrentLayer = Layer->Get();
	}
	else
	{
		ensure(false);
		CurrentLayer = Layers.Add(CurrentLayerType, MakeUnique<FRBMKUIRenderLayer>()).Get();
	}
}

FRBMKUIRender GXRayUIRender;
