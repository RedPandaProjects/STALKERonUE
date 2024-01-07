#include "StalkerEnvironment.h"


AStalkerEnvironment::AStalkerEnvironment()
{
}

bool AStalkerEnvironment::SetEffect(const char* Name)
{
	return K2_SetEffect(ANSI_TO_TCHAR(Name));
}
