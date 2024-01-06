#include "StalkerBlueprintFunctionLibrary.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/XR_IOConsole.h"
THIRD_PARTY_INCLUDES_END
void UStalkerBlueprintFunctionLibrary::StalkerConnect(const UObject* WorldContextObject,const FString& Server, const FString& Client)
{
	Engine->Event.Defer("KERNEL:start", u64(xr_strdup(TCHAR_TO_ANSI(*Server))), u64(xr_strdup(TCHAR_TO_ANSI(*Client))));
}

void UStalkerBlueprintFunctionLibrary::StalkerShowMenu(const UObject* WorldContextObject, bool Show)
{
	if (Show)
	{
		Console->Execute("main_menu on");
	}
	else
	{
		Console->Execute("main_menu off");
	}
}

void UStalkerBlueprintFunctionLibrary::StalkerActorTransferInfo(const FString& Name, bool Value /*= true*/)
{
	IRBMKLevelToBlueprint*LevelToBlueprint =   g_Engine->GetLevelScript();
	if (LevelToBlueprint)
	{
		LevelToBlueprint->ActorTransferInfo(TCHAR_TO_ANSI(*Name),Value);
	}
}


void UStalkerBlueprintFunctionLibrary::StalkerActorWaitInfoList(const TMap<FString, bool>& CheckValues,const UObject* WorldContextObject, FLatentActionInfo LatentInfo)
{
	class   FHasInfoAction : public FPendingLatentAction
	{
	public:
		TMap<shared_str, bool> CheckValues;
		FName ExecutionFunction;
		int32 OutputLink;
		FWeakObjectPtr CallbackTarget;

		FHasInfoAction(const TMap<FString, bool>& InCheckValues, const FLatentActionInfo& LatentInfo)
			: ExecutionFunction(LatentInfo.ExecutionFunction)
			, OutputLink(LatentInfo.Linkage)
			, CallbackTarget(LatentInfo.CallbackTarget)
		{
			for (auto& [Key, Value] : InCheckValues)
			{
				CheckValues.FindOrAdd(TCHAR_TO_ANSI(*Key),Value);
			}
		}

		virtual void UpdateOperation(FLatentResponse& Response) override
		{
			IRBMKLevelToBlueprint* LevelToBlueprint =  g_Engine->GetLevelScript();
			if (LevelToBlueprint)
			{
				if (!LevelToBlueprint->GetActor())
				{
					return;
				}
				for (auto& [Key, Value] : CheckValues)
				{
					if(LevelToBlueprint->ActorHasInfo(Key)!=Value)
					{
						return;
					}

				}
				Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
			}
		}

	#if WITH_EDITOR
		// Returns a human readable description of the latent operation's current state
		virtual FString GetDescription() const override
		{
			FString Result;
			Result += TEXT("Check Values(");
			IRBMKLevelToBlueprint* LevelToBlueprint =  g_Engine->GetLevelScript();
			if (LevelToBlueprint)
			{
				int32 Counter = 0;
				for (auto& [Key, Value] : CheckValues)
				{
					if (Counter)
					{
						Result+=TEXT(",");
					}
					Result += FString::Printf(TEXT("%S:{%s[%s]}"),Key.c_str(), LevelToBlueprint->ActorHasInfo(Key)?TEXT("true"):TEXT("false"),Value?TEXT("true"):TEXT("false"));
					Counter++;
				}
			}
			else
			{
				Result+=TEXT("LevelToBlueprint is null");
			}
			Result+=TEXT(")");
			return Result;
		}
	#endif
	};

	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
		if (LatentActionManager.FindExistingAction<FHasInfoAction>(LatentInfo.CallbackTarget, LatentInfo.UUID) == NULL)
		{
			LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, new FHasInfoAction(CheckValues, LatentInfo));
		}
	}
}

void UStalkerBlueprintFunctionLibrary::StalkerActorWaitInfo(const FString& Name, bool Value, const UObject* WorldContextObject, FLatentActionInfo LatentInfo)
{
	TMap<FString, bool> CheckValues;
	CheckValues.Add(Name,Value);
	StalkerActorWaitInfoList(CheckValues,WorldContextObject,LatentInfo);
}

bool UStalkerBlueprintFunctionLibrary::StalkerActorGetInfo(const FString& Name)
{
	IRBMKLevelToBlueprint* LevelToBlueprint = g_Engine->GetLevelScript();
	if (LevelToBlueprint)
	{
		return LevelToBlueprint->ActorHasInfo(TCHAR_TO_ANSI(*Name));
	}
	return false;
}

void UStalkerBlueprintFunctionLibrary::StalkerSpawnObject(const FString& SectionName, const FString& WayObjectName,int32 PointIndex,  float Angle)
{
	IRBMKLevelToBlueprint* LevelToBlueprint = g_Engine->GetLevelScript();
	if (LevelToBlueprint)
	{
		LevelToBlueprint->SpawnObject(TCHAR_TO_ANSI(*SectionName),TCHAR_TO_ANSI(*WayObjectName),PointIndex,Angle);
	}
}

float UStalkerBlueprintFunctionLibrary::StalkerGetGameTime()
{
	IRBMKLevelToBlueprint* LevelToBlueprint = g_Engine->GetLevelScript();
	if (LevelToBlueprint)
	{
		return LevelToBlueprint->GetGameTimeAsFloat();
	}
	return 0;
}
