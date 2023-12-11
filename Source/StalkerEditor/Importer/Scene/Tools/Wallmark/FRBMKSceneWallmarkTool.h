#pragma once

struct FRBMKSceneWallmarkItem
{
	FString		ShaderName;
	FString		TextureName;
    float		Width;
	float		Hieght;
	float		Rotate;
	FVector3f	Position;
	FPlane4f	Plane;
};

class FRBMKSceneWallmarkTool : public FRBMKSceneToolBase
{
public:
									FRBMKSceneWallmarkTool	();
									~FRBMKSceneWallmarkTool	() override;
	const TCHAR*					ClassName				() override { return TEXT("wallmark"); }
	// IO
	bool 							CanUseInifile			() override { return false; }

	bool   							LoadStream				(IReader&) override;
	bool							LoadLTX					(CInifile&) override;
	bool							LoadSelection			(IReader&) override;
    void							ExportToWorld			(UWorld*World,EObjectFlags InFlags,const URBMKLevelImportOptions&LevelImportOptions) override;

	TArray<FRBMKSceneWallmarkItem>	Items;
private:
};