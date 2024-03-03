#include "StalkerEnvironment.h"


AStalkerEnvironment::AStalkerEnvironment()
{
}

bool AStalkerEnvironment::SetEffect(const char* Name,float StartTime)
{
	return K2_SetEffect(ANSI_TO_TCHAR(Name),StartTime);
}
