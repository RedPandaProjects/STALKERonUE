#include "StalkerSpawnPropertiesTypeCustomization.h"
#include "../../../../StalkerEditorManager.h"
#include "../../../../Managers/SEFactory/StalkerSEFactoryManager.h"

void FStalkerSpawnData_StorysCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	ValueHandle = PropertyHandle->GetChildHandle(TEXT("Value"));

	HeaderRow.NameContent()
	[
		PropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SAssignNew(StorysComboBox,SComboBox<TSharedPtr<FString>>)
			.OptionsSource(&GStalkerEditorManager->SEFactoryManager->Storys_ForComboBox)
			.OnGenerateWidget(this, &FStalkerSpawnData_StorysCustomization::MakeBasePoseComboWidget)
			.OnSelectionChanged(this, &FStalkerSpawnData_StorysCustomization::SelectionChanged)
			.ContentPadding(2)
			.Content()
				[
					SNew(STextBlock)
					.Text(this, &FStalkerSpawnData_StorysCustomization::GetStorysComboBoxContent)
				]
			]
	];
}

TSharedRef<SWidget> FStalkerSpawnData_StorysCustomization::MakeBasePoseComboWidget(TSharedPtr<FString> InItem)
{
	return 	SNew(STextBlock).Text(FText::FromString(*InItem));
}

void FStalkerSpawnData_StorysCustomization::SelectionChanged(TSharedPtr<FString> PoseName, ESelectInfo::Type SelectionType)
{
	ValueHandle->SetValue(*PoseName);
}

FText FStalkerSpawnData_StorysCustomization::GetStorysComboBoxContent() const
{
	FString	Result;
	ValueHandle->GetValue(Result);
	return FText::FromString(Result);
}



void FStalkerSpawnData_SpawnStorysCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	ValueHandle = PropertyHandle->GetChildHandle(TEXT("Value"));


	HeaderRow.NameContent()
	[
		PropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SAssignNew(StorysComboBox,SComboBox<TSharedPtr<FString>>)
			.OptionsSource(&GStalkerEditorManager->SEFactoryManager->SpawnStorys_ForComboBox)
			.OnGenerateWidget(this, &FStalkerSpawnData_SpawnStorysCustomization::MakeBasePoseComboWidget)
			.OnSelectionChanged(this, &FStalkerSpawnData_SpawnStorysCustomization::SelectionChanged)
			.ContentPadding(2)
			.Content()
					[
						SNew(STextBlock)
						.Text(this, &FStalkerSpawnData_SpawnStorysCustomization::GetStorysComboBoxContent)
					]
			]
	];
}

TSharedRef<SWidget> FStalkerSpawnData_SpawnStorysCustomization::MakeBasePoseComboWidget(TSharedPtr<FString> InItem)
{
	return 	SNew(STextBlock).Text(FText::FromString(*InItem));
}

void FStalkerSpawnData_SpawnStorysCustomization::SelectionChanged(TSharedPtr<FString> PoseName, ESelectInfo::Type SelectionType)
{
	ValueHandle->SetValue(*PoseName);
}

FText FStalkerSpawnData_SpawnStorysCustomization::GetStorysComboBoxContent() const
{
	FString	Result;
	ValueHandle->GetValue(Result);
	return FText::FromString(Result);
}

FStalkerSpawnData_LocationCustomization::FStalkerSpawnData_LocationCustomization(int32 Index):LocationIndex(Index)
{

}


void FStalkerSpawnData_LocationCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	ValueHandle = PropertyHandle->GetChildHandle(TEXT("Value"));

	TArray<TSharedPtr<FString>>*LocationSources = &GStalkerEditorManager->SEFactoryManager->Location1_ForComboBox;
	if (LocationIndex == 1)
	{
		LocationSources = &GStalkerEditorManager->SEFactoryManager->Location2_ForComboBox;
	}
	if (LocationIndex == 2)
	{
		LocationSources = &GStalkerEditorManager->SEFactoryManager->Location3_ForComboBox;
	}
	if (LocationIndex == 3)
	{
		LocationSources = &GStalkerEditorManager->SEFactoryManager->Location4_ForComboBox;
	}
	HeaderRow.NameContent()
	[
		PropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SAssignNew(StorysComboBox, SComboBox<TSharedPtr<FString>>)
			.OptionsSource(LocationSources)
			.OnGenerateWidget(this, &FStalkerSpawnData_LocationCustomization::MakeBasePoseComboWidget)
			.OnSelectionChanged(this, &FStalkerSpawnData_LocationCustomization::SelectionChanged)
			.ContentPadding(2)
			.Content()
			[
				SNew(STextBlock)
				.Text(this, &FStalkerSpawnData_LocationCustomization::GetStorysComboBoxContent)
			]
		]
	];
}

TSharedRef<SWidget> FStalkerSpawnData_LocationCustomization::MakeBasePoseComboWidget(TSharedPtr<FString> InItem)
{
	return 	SNew(STextBlock).Text(FText::FromString(*InItem));
}

void FStalkerSpawnData_LocationCustomization::SelectionChanged(TSharedPtr<FString> PoseName, ESelectInfo::Type SelectionType)
{
	ValueHandle->SetValue(*PoseName);
}

FText FStalkerSpawnData_LocationCustomization::GetStorysComboBoxContent() const
{
	FString	Result;
	ValueHandle->GetValue(Result);
	return FText::FromString(Result);
}



void FStalkerSpawnData_LightAnimCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	ValueHandle = PropertyHandle->GetChildHandle(TEXT("Value"));

	HeaderRow.NameContent()
	[
		PropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SAssignNew(StorysComboBox, SComboBox<TSharedPtr<FString>>)
			.OptionsSource(&GStalkerEditorManager->SEFactoryManager->LightAnims_ForComboBox)
			.OnGenerateWidget(this, &FStalkerSpawnData_LightAnimCustomization::MakeBasePoseComboWidget)
			.OnSelectionChanged(this, &FStalkerSpawnData_LightAnimCustomization::SelectionChanged)
			.ContentPadding(2)
			.Content()
			[
				SNew(STextBlock)
				.Text(this, &FStalkerSpawnData_LightAnimCustomization::GetStorysComboBoxContent)
			]
		]
	];
}

TSharedRef<SWidget> FStalkerSpawnData_LightAnimCustomization::MakeBasePoseComboWidget(TSharedPtr<FString> InItem)
{
	return 	SNew(STextBlock).Text(FText::FromString(*InItem));
}

void FStalkerSpawnData_LightAnimCustomization::SelectionChanged(TSharedPtr<FString> PoseName, ESelectInfo::Type SelectionType)
{
	ValueHandle->SetValue(*PoseName);
}

FText FStalkerSpawnData_LightAnimCustomization::GetStorysComboBoxContent() const
{
	FString	Result;
	ValueHandle->GetValue(Result);
	return FText::FromString(Result);
}

void FStalkerSpawnData_CharacterProfileCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	ValueHandle = PropertyHandle->GetChildHandle(TEXT("Value"));

	HeaderRow.NameContent()
	[
		PropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SAssignNew(StorysComboBox, SComboBox<TSharedPtr<FString>>)
			.OptionsSource(&GStalkerEditorManager->SEFactoryManager->CharacterProfiles_ForComboBox)
			.OnGenerateWidget(this, &FStalkerSpawnData_CharacterProfileCustomization::MakeBasePoseComboWidget)
			.OnSelectionChanged(this, &FStalkerSpawnData_CharacterProfileCustomization::SelectionChanged)
			.ContentPadding(2)
			.Content()
			[
				SNew(STextBlock)
				.Text(this, &FStalkerSpawnData_CharacterProfileCustomization::GetStorysComboBoxContent)
			]
		]
	];
}

TSharedRef<SWidget> FStalkerSpawnData_CharacterProfileCustomization::MakeBasePoseComboWidget(TSharedPtr<FString> InItem)
{
	return 	SNew(STextBlock).Text(FText::FromString(*InItem));
}

void FStalkerSpawnData_CharacterProfileCustomization::SelectionChanged(TSharedPtr<FString> PoseName, ESelectInfo::Type SelectionType)
{
	ValueHandle->SetValue(*PoseName);
}

FText FStalkerSpawnData_CharacterProfileCustomization::GetStorysComboBoxContent() const
{
	FString	Result;
	ValueHandle->GetValue(Result);
	return FText::FromString(Result);
}


void FStalkerSpawnData_SmartCoverDescriptionCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	ValueHandle = PropertyHandle->GetChildHandle(TEXT("Value"));

	HeaderRow.NameContent()
	[
		PropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SAssignNew(StorysComboBox, SComboBox<TSharedPtr<FString>>)
			.OptionsSource(&GStalkerEditorManager->SEFactoryManager->SmartCoverDescriptions_ForComboBox)
			.OnGenerateWidget(this, &FStalkerSpawnData_SmartCoverDescriptionCustomization::MakeBasePoseComboWidget)
			.OnSelectionChanged(this, &FStalkerSpawnData_SmartCoverDescriptionCustomization::SelectionChanged)
			.ContentPadding(2)
			.Content()
			[
				SNew(STextBlock)
				.Text(this, &FStalkerSpawnData_SmartCoverDescriptionCustomization::GetStorysComboBoxContent)
			]
		]
	];
}

TSharedRef<SWidget> FStalkerSpawnData_SmartCoverDescriptionCustomization::MakeBasePoseComboWidget(TSharedPtr<FString> InItem)
{
	return 	SNew(STextBlock).Text(FText::FromString(*InItem));
}

void FStalkerSpawnData_SmartCoverDescriptionCustomization::SelectionChanged(TSharedPtr<FString> PoseName, ESelectInfo::Type SelectionType)
{
	ValueHandle->SetValue(*PoseName);
}

FText FStalkerSpawnData_SmartCoverDescriptionCustomization::GetStorysComboBoxContent() const
{
	FString	Result;
	ValueHandle->GetValue(Result);
	return FText::FromString(Result);
}