#include "StalkerEditorStyle.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Styling/SlateStyleRegistry.h"
#include "Interfaces/IPluginManager.h"
#include "EditorViewportClient.h"

TSharedPtr< FSlateStyleSet > FStalkerEditorStyle::StyleInstance = NULL;

void FStalkerEditorStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FStalkerEditorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FStalkerEditorStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("StalkerEditorStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);

TSharedRef< FSlateStyleSet > FStalkerEditorStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("StalkerEditorStyle"));
	Style->SetContentRoot(FPaths::ProjectDir() / TEXT("Resources"));

	Style->Set("StalkerEditor.BuildCForm", new IMAGE_BRUSH(*FPaths::Combine(TEXT("Icon40"), TEXT("cform")), Icon40x40));
	Style->Set("StalkerEditor.BuildAIMap", new IMAGE_BRUSH(*FPaths::Combine(TEXT("Icon40"), TEXT("aimap")), Icon40x40));
	Style->Set("StalkerEditor.BuildLevelSpawn", new IMAGE_BRUSH(*FPaths::Combine(TEXT("Icon40"), TEXT("level_spawn")), Icon40x40));
	Style->Set("StalkerEditor.BuildGameSpawn", new IMAGE_BRUSH(*FPaths::Combine(TEXT("Icon40"), TEXT("game_spawn")), Icon40x40));
	Style->Set("StalkerEditor.Reload", new IMAGE_BRUSH(*FPaths::Combine(TEXT("Icon40"), TEXT("reload")),			 Icon40x40));
	Style->Set("StalkerEditor.Stalker40x", new IMAGE_BRUSH(*FPaths::Combine(TEXT("Icon40"), TEXT("stalker_40x")), Icon40x40));
	Style->Set("StalkerEditor.Stalker20x", new IMAGE_BRUSH(*FPaths::Combine(TEXT("Icon20"), TEXT("stalker_20x")), Icon20x20));
	Style->Set("StalkerEditor.Select", new IMAGE_BRUSH(*FPaths::Combine(TEXT("Icon40"), TEXT("Select_40x")), Icon20x20));
	Style->Set("StalkerEditor.Paint", new IMAGE_BRUSH(*FPaths::Combine(TEXT("Icon40"), TEXT("Paint_40x")), Icon20x20));
	Style->Set("StalkerEditor.COP", new IMAGE_BRUSH(*FPaths::Combine(TEXT("Icon40"), TEXT("stalker_cop")), Icon40x40));
	Style->Set("StalkerEditor.CS", new IMAGE_BRUSH(*FPaths::Combine(TEXT("Icon40"), TEXT("stalker_cs")), Icon40x40));
	Style->Set("StalkerEditor.SOC", new IMAGE_BRUSH(*FPaths::Combine(TEXT("Icon40"), TEXT("stalker_soc")), Icon40x40));
	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT

void FStalkerEditorStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FStalkerEditorStyle::Get()
{
	return *StyleInstance;
}

