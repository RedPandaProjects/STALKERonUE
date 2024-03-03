#pragma once
#include "XrEngine/Render/UIShader.h"
#include "XrEngine/Render/UIRender.h"
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
struct FRBMKUIRenderText
{
	FString	Data;
	FVector2f	Position;
	FColor		Color;
	float		Scale;
	float		FontSize;
	UFont*		Font;

};
struct FRBMKUIRenderPrimitive
{
	int32							TextID = -1;
	TObjectPtr<USlateBrushAsset>	Brush;
	uint32							StartVertex;
	uint32							EndVertex;
	int32							ScissorsID;
	IUIRender::ePrimitiveType		PrimitiveType;
	IUIRender::ePointType			PointType;
};

struct FRBMKUIRenderVertex
{
	FVector2f						Position;
	FColor							Color;
	FVector2f						UV;
};
struct FRBMKUIRenderLayer
{
	TArray<FRBMKUIRenderText>		Texts;
	TArray<FRBMKUIRenderPrimitive>	Items;
};
class FRBMKUIRender:public IUIRender
{
public:
														FRBMKUIRender				() ;
														~FRBMKUIRender				() ;
	void												CreateUIGeom				() override;
	void												DestroyUIGeom				() override;
											
	void												SetShader					(IUIShader &shader) override;
	void												SetAlphaRef					(int aref) override;
	void												SetScissor					(Irect* rect = NULL) override;
	void												GetActiveTextureResolution	(Fvector2 &res) override;
	void												PushPoint					(float x, float y, float z, u32 C, float u, float v) override;
	void												PushText					(float x, float y, float Scale, u32 C, UFont* Font,float FontSize, const TCHAR* String);
											
	void												StartPrimitive				(u32 iMaxVerts, ePrimitiveType primType, ePointType pointType) override;
	void												FlushPrimitive				() override;
	void												Flush						() ;
	LPCSTR												UpdateShaderName			(LPCSTR tex_name, LPCSTR sh_name) override;
	void												PushLayer					(ERBMKUILayer NewLayer) override;
	void												PopLayer					() override;
			
			
	XRayUIShader										CurrentShader;
	TArray<FVector4f>									Scissors;
	TArray<FRBMKUIRenderVertex>							Vertices;
	FRBMKUIRenderLayer*									CurrentLayer;
	ERBMKUILayer										CurrentLayerType;
	TMap<ERBMKUILayer,TUniquePtr<FRBMKUIRenderLayer>>	Layers;
	TArray<ERBMKUILayer>								LayersStack;
private:
	int32												CurrentScissor;
};

extern FRBMKUIRender GXRayUIRender;