#include "XRayDebug.h"

XRayDebug::XRayDebug()
{

}

void XRayDebug::CriticalError(const char* InText)
{
	FString Text = InText;
	ensure(*Text);
}
