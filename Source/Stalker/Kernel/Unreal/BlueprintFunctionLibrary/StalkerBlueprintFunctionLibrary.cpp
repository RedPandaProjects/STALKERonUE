#include "StalkerBlueprintFunctionLibrary.h"
THIRD_PARTY_INCLUDES_START
#include "../Source/XrEngine/XR_IOConsole.h"
THIRD_PARTY_INCLUDES_END
void UStalkerBlueprintFunctionLibrary::XRayConnect(const UObject* WorldContextObject,const FString& Server, const FString& Client)
{
	Engine->Event.Defer("KERNEL:start", u64(xr_strdup(TCHAR_TO_ANSI(*Server))), u64(xr_strdup(TCHAR_TO_ANSI(*Client))));
}

void UStalkerBlueprintFunctionLibrary::XRayShowMenu(const UObject* WorldContextObject, bool Show)
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
