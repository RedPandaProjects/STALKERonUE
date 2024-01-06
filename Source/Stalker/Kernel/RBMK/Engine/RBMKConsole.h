// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/XR_IOConsole.h"
#include "XrEngine/xr_ioc_cmd.h"
THIRD_PARTY_INCLUDES_END
/**
 * 
 */
class STALKER_API FRBMKConsole : public XRayConsoleInterface
{
private:
	static void									ExecuteCommandUE(IConsole_Command* cc, const TArray< FString >& Args);
public:
												FRBMKConsole	();
												~FRBMKConsole	() override;

	void										AddCommand		(IConsole_Command* cc) override;
	void										RemoveCommand	(IConsole_Command* cc) override;
private:
	TMap<IConsole_Command*, IConsoleCommand*>	UEConsoleCommands;
};
