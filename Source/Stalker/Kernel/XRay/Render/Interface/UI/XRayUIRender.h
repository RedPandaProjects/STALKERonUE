#pragma once

#include "Kernel/XRay/Render/Interface/Shader/XRayUIShader.h"

#include "XrRender/Public/UIRender.h"

class XRayUIRender:public IUIRender
{
public:
			XRayUIRender				() ;
			~XRayUIRender				() ;
	 void	CreateUIGeom				() override;
	 void	DestroyUIGeom				() override;

	 void	SetShader					(IUIShader &shader) override;
	 void	SetAlphaRef					(int aref) override;
	 void	SetScissor					(Irect* rect = NULL) override;
	 void	GetActiveTextureResolution	(Fvector2 &res) override;
	 void	PushPoint					(float x, float y, float z, u32 C, float u, float v) override;
	 void	PushText					(float x, float y, float Scale, u32 C, UFont* Font,float FontSize, const TCHAR* String);

	 void	StartPrimitive				(u32 iMaxVerts, ePrimitiveType primType, ePointType pointType) override;
	 void	FlushPrimitive				() override;
	 void	Flush						() ;
	 LPCSTR	UpdateShaderName			(LPCSTR tex_name, LPCSTR sh_name) override;
	 void	CacheSetXformWorld			(const Fmatrix& M) override;
	 void	CacheSetCullMode			(CullMode) override;


	 XRayUIShader	  CurrentShader;
	 TArray<FVector4f> Scissors;
	 struct Text
	 {
		 FString	Data;
		 FVector2f	Position;
		 FColor		Color;
		 float		Scale;
		 float		FontSize;
		 UFont*		Font;

	 };
	 TArray<Text> Texts;
	 struct Item
	 {
		int32							TextID = -1;
		TObjectPtr<USlateBrushAsset>	Brush;
		uint32							StartVertex;
		uint32							EndVertex;
		int32							ScissorsID;
		ePrimitiveType					PrimitiveType;
		ePointType						PointType;
	 };
	 TArray<Item> Items;
	 struct Vertex
	 {
		FVector2f						Position;
		FColor							Color;
		FVector2f						UV;
	 };

	 TArray<Vertex> Vertices;
private:
	int32 CurrentScissor;
};

extern XRayUIRender GXRayUIRender;