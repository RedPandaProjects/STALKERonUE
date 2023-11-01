// Copyright(c) 2019 Vadim Slyusarev

#pragma once

#if WITH_EDITOR


#include "OptickStyle.h"

class FOptickCommands : public TCommands<FOptickCommands>
{
public:

	FOptickCommands()
		: TCommands<FOptickCommands>(TEXT("Optick"), NSLOCTEXT("Contexts", "Optick", "Optick Plugin"), NAME_None, FOptickStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};

#endif