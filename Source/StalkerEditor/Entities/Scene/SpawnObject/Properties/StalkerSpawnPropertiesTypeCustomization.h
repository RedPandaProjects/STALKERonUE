#pragma once
class FStalkerSpawnData_StorysCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShared<FStalkerSpawnData_StorysCustomization>();
	}

	virtual void										CustomizeHeader				(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils);

	virtual void										CustomizeChildren			(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
	{}
private:
	TSharedRef<SWidget>									MakeBasePoseComboWidget		(TSharedPtr<FString> InItem);
	void												SelectionChanged			(TSharedPtr<FString> PoseName, ESelectInfo::Type SelectionType);
	FText												GetStorysComboBoxContent	() const;
	TSharedPtr<IPropertyHandle>							ValueHandle;
	TSharedPtr<class SComboBox< TSharedPtr<FString> > > StorysComboBox;
};

class FStalkerSpawnData_SpawnStorysCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShared<FStalkerSpawnData_SpawnStorysCustomization>();
	}

	virtual void										CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils);

	virtual void										CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
	{}
private:
	TSharedRef<SWidget>									MakeBasePoseComboWidget(TSharedPtr<FString> InItem);
	void												SelectionChanged(TSharedPtr<FString> PoseName, ESelectInfo::Type SelectionType);
	FText												GetStorysComboBoxContent() const;
	TSharedPtr<IPropertyHandle>							ValueHandle;
	TSharedPtr<class SComboBox< TSharedPtr<FString> > > StorysComboBox;
};


class FStalkerSpawnData_LocationCustomization : public IPropertyTypeCustomization
{
public:
	FStalkerSpawnData_LocationCustomization(int32 Index);

	static TSharedRef<IPropertyTypeCustomization> MakeInstance(int32 Location )
	{
		return MakeShared<FStalkerSpawnData_LocationCustomization>(Location);
	}

	virtual void										CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils);

	virtual void										CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
	{}
private:
	TSharedRef<SWidget>									MakeBasePoseComboWidget(TSharedPtr<FString> InItem);
	void												SelectionChanged(TSharedPtr<FString> PoseName, ESelectInfo::Type SelectionType);
	FText												GetStorysComboBoxContent() const;
	TSharedPtr<IPropertyHandle>							ValueHandle;
	TSharedPtr<class SComboBox< TSharedPtr<FString> > > StorysComboBox;
	int32												LocationIndex;
};

class FStalkerSpawnData_LightAnimCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShared<FStalkerSpawnData_LightAnimCustomization>();
	}

	virtual void										CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils);

	virtual void										CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
	{}
private:
	TSharedRef<SWidget>									MakeBasePoseComboWidget(TSharedPtr<FString> InItem);
	void												SelectionChanged(TSharedPtr<FString> PoseName, ESelectInfo::Type SelectionType);
	FText												GetStorysComboBoxContent() const;
	TSharedPtr<IPropertyHandle>							ValueHandle;
	TSharedPtr<class SComboBox< TSharedPtr<FString> > > StorysComboBox;
};

class FStalkerSpawnData_CharacterProfileCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShared<FStalkerSpawnData_CharacterProfileCustomization>();
	}

	virtual void										CustomizeHeader			(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils);

	virtual void										CustomizeChildren		(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
	{}
private:
	TSharedRef<SWidget>									MakeBasePoseComboWidget	(TSharedPtr<FString> InItem);
	void												SelectionChanged		(TSharedPtr<FString> PoseName, ESelectInfo::Type SelectionType);
	FText												GetStorysComboBoxContent() const;
	TSharedPtr<IPropertyHandle>							ValueHandle;
	TSharedPtr<class SComboBox< TSharedPtr<FString> > > StorysComboBox;
};

class FStalkerSpawnData_SmartCoverDescriptionCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShared<FStalkerSpawnData_SmartCoverDescriptionCustomization>();
	}

	virtual void										CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils);

	virtual void										CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
	{}
private:
	TSharedRef<SWidget>									MakeBasePoseComboWidget(TSharedPtr<FString> InItem);
	void												SelectionChanged(TSharedPtr<FString> PoseName, ESelectInfo::Type SelectionType);
	FText												GetStorysComboBoxContent() const;
	TSharedPtr<IPropertyHandle>							ValueHandle;
	TSharedPtr<class SComboBox< TSharedPtr<FString> > > StorysComboBox;
};
