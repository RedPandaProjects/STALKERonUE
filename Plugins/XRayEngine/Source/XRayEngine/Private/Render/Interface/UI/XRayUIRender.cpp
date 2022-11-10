#include "XRayUIRender.h"
#include "Base/EngineManager/XRayEngineManager.h"
#include "Base/ResourcesManager/XRayResourcesManager.h"

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
		Brush = GXRayEngineManager->GetResourcesManager()->Copy(static_cast<XRayUIShader&>(_in).Brush);
	}
}

void XRayUIShader::create(LPCSTR sh, LPCSTR tex)
{
	MaterialName = sh;
	TextureName = tex? FName(tex) : NAME_None;
}

bool XRayUIShader::inited()
{
	 Brush = GXRayEngineManager->GetResourcesManager()->GetBrush(MaterialName, TextureName);
	 return !!Brush;
}

void XRayUIShader::destroy()
{
	if (Brush)
	{
		GXRayEngineManager->GetResourcesManager()->Free(Brush);
		Brush = nullptr;
	}
}

XRayUIRender::XRayUIRender()
{
	CurrentScissor = -1;
}

XRayUIRender::~XRayUIRender()
{
}

void XRayUIRender::CreateUIGeom()
{
}

void XRayUIRender::DestroyUIGeom()
{
	

}

void XRayUIRender::SetShader(IUIShader& shader)
{
	CurrentShader.Copy(shader);
}

void XRayUIRender::SetAlphaRef(int aref)
{
}

void XRayUIRender::SetScissor(Irect* rect)
{
	if (rect)
	{
		CurrentScissor = Scissors.AddUnique(FVector4f(rect->x1, rect->y1, rect->x2, rect->y2));
	}
	else
	{
		CurrentScissor = -1;
	}
}

void XRayUIRender::GetActiveTextureResolution(Fvector2& res)
{
	if(!CurrentShader.Brush)return;
	res.x = CurrentShader.Brush->Brush.GetImageSize().X;
	res.y = CurrentShader.Brush->Brush.GetImageSize().Y;
}

void XRayUIRender::PushPoint(float x, float y, float z, u32 C, float u, float v)
{
	Vertex InVertex;
	InVertex.Position.Set(x,y);
	InVertex.Color = FColor(C);
	InVertex.UV.Set(u,v);
	Vertices.Push(InVertex);
	
}

void XRayUIRender::StartPrimitive(u32 iMaxVerts, ePrimitiveType primType, ePointType pointType)
{
	check(CurrentShader.Brush);
	Items.AddDefaulted();
	Items.Last().StartVertex = Vertices.Num();
	Items.Last().EndVertex = Vertices.Num();
	Items.Last().ScissorsID = CurrentScissor;
	Items.Last().PrimitiveType = primType;
	Items.Last().PointType = pointType;
	Items.Last().Brush = GXRayEngineManager->GetResourcesManager()->Copy(CurrentShader.Brush);
}
void XRayUIRender::FlushPrimitive()
{
	Items.Last().EndVertex = Vertices.Num();
}

void XRayUIRender::Flush()
{
	Items.Empty(Items.Num());
	Scissors.Empty(Scissors.Num());
	Vertices.Empty(Vertices.Num());
	CurrentScissor = -1;
	CurrentShader.destroy();

}

LPCSTR XRayUIRender::UpdateShaderName(LPCSTR tex_name, LPCSTR sh_name)
{
	string_path buff;

	return  FS.exist(buff, "$game_textures$", tex_name, ".ogm") ? "hud\\movie" : sh_name;
}

void XRayUIRender::CacheSetXformWorld(const Fmatrix& M)
{
}

void XRayUIRender::CacheSetCullMode(CullMode)
{
}
XRayUIRender GXRayUIRender;