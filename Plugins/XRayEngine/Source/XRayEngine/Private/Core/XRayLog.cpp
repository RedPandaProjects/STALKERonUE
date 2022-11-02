#include "XRayLog.h"

XRayLog::XRayLog()
{

}

void XRayLog::Log(const char* Text)
{
	UE_LOG(LogXRayEngine, Log, TEXT("%S"), Text);
}
