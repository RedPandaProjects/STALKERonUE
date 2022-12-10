#include "StalkerBlueprintFunctionLibrary.h"

void UStalkerBlueprintFunctionLibrary::XRayConnect(const UObject* WorldContextObject,const FString& Server, const FString& Client)
{

	Engine->Event.Defer("KERNEL:start", u64(xr_strdup(TCHAR_TO_ANSI(*Server))), u64(xr_strdup(TCHAR_TO_ANSI(*Client))));
}
