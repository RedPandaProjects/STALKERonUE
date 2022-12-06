// Fill out your copyright notice in the Description page of Project Settings.

#include "XRayConsole.h"
#include "Framework/Commands/Commands.h"

void XRayConsole::ExecuteCommandUE(IConsole_Command* cc, const TArray<FString>& Args)
{
	if (cc && cc->bEnabled)
	{
		if (Args.IsEmpty())
		{
			if (cc->bEmptyArgsHandled)
			{
				cc->Execute(nullptr);
			}
			else
			{
				IConsole_Command::TStatus stat;
				cc->Status(stat);
				Msg("- %s %s", cc->Name(), stat);
			}
		}
		else
		{
			FString all_args;
			for (size_t i = 0; i < Args.Num(); i++)
			{
				if (Args[i] == "=") continue; //ignore unreal like "key = value"

				all_args += Args[i];
				if (i + 1 < Args.Num())
					all_args += " ";
			}
			if (cc->bLowerCaseArgs)
				all_args.ToLowerInline();
			cc->Execute(TCHAR_TO_ANSI(*all_args));
		}
	}
	else
	{
		Log("! Command disabled.");
	}
}


XRayConsole::XRayConsole()
{

}

XRayConsole::~XRayConsole()
{
	checkSlow(m_game_to_ue.Num()  == 0);
}

void XRayConsole::AddCommand(IConsole_Command* cc)
{
	FString name = "xray." + FString(cc->Name());
	auto pUECommand = IConsoleManager::Get().RegisterConsoleCommand(*name, *name, FConsoleCommandWithArgsDelegate::CreateLambda([cc](const TArray<FString>& Args) {XRayConsole::ExecuteCommandUE(cc, Args); }));
	m_game_to_ue.Add(cc, pUECommand);

	XRayConsoleInterface::AddCommand(cc);
}

void XRayConsole::RemoveCommand(IConsole_Command* cc)
{
	auto pUECommand = m_game_to_ue.FindAndRemoveChecked(cc);
	IConsoleManager::Get().UnregisterConsoleObject(pUECommand);
	delete pUECommand; //?

	XRayConsoleInterface::RemoveCommand(cc);
}