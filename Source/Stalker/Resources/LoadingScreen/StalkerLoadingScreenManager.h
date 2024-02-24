#pragma once
#include "Framework/Application/IInputProcessor.h"

enum class EStalkerLoadingUpdateDataType
{
	NextStatus,
	SetMapImage,
	EndLoad,
	PressAnyKey,
};
struct FStalkerLoadingUpdateData
{
	EStalkerLoadingUpdateDataType	Type = EStalkerLoadingUpdateDataType::NextStatus;
	FString							Text;
	UTexture2D*						MapImage;
	int32							IndexStatus = 0;
};


class FStalkerLoadingInputProcessor : public IInputProcessor
{
public:
			FStalkerLoadingInputProcessor	(class FStalkerLoadingScreenManager*Owner);
			~FStalkerLoadingInputProcessor	();
	bool	HandleKeyDownEvent				( FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent ) override;
	bool	HandleMouseButtonDownEvent		(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
	void	Tick							(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override;
private:
	class FStalkerLoadingScreenManager*Owner;
};


class FStalkerLoadingScreenManager: public FGCObject,public  IRBMKLoadingScreenManager
{
public:
												FStalkerLoadingScreenManager	();
												~FStalkerLoadingScreenManager	();
		
	void										SetTitle						(const char* Title) override;
		
	void										Initialize						();
	void										Play							(bool IsNewGame,int32 MaxStatus);
	void										PlayDefault						();
	void										Wait							();
		
	void										SetMapImage						(FString Name);
		
	void										AddReferencedObjects			(FReferenceCollector& Collector) override;
	FString										GetReferencerName				() const override;
		
		
	UTexture2D*									BackgroundImage;
	UTexture2D*									MapImage;
	UFont*										MainFont;
	TQueue<FStalkerLoadingUpdateData>			UpdateData;
	int32										StatusIndex = 0;
	TSharedPtr<FStalkerLoadingInputProcessor>	LoadingInputProcessor;
private:
	void								LoadingTick						(float DeltaTime);
};
