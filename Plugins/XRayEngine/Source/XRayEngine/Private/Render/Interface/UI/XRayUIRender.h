#pragma once
#include "XrRender/Public/UIShader.h"
#include "XrRender/Public/UIRender.h"
class XRayUIShader :public IUIShader
{
public:
	XRayUIShader();
	~XRayUIShader() override;
	 void Copy(IUIShader& _in) override;
	 void create(LPCSTR sh, LPCSTR tex = 0) override;
	 bool inited() override;
	 void destroy() override;
	 FName MaterialName;
	 FName TextureName;
	 TObjectPtr<USlateBrushAsset> Brush;
};
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

	 void	StartPrimitive				(u32 iMaxVerts, ePrimitiveType primType, ePointType pointType) override;
	 void	FlushPrimitive				() override;
	 void	Flush						() ;
	 LPCSTR	UpdateShaderName			(LPCSTR tex_name, LPCSTR sh_name) override;
	 void	CacheSetXformWorld			(const Fmatrix& M) override;
	 void	CacheSetCullMode			(CullMode) override;


	 XRayUIShader	  CurrentShader;
	 TArray<FVector4f> Scissors;
	 struct Item
	 {
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