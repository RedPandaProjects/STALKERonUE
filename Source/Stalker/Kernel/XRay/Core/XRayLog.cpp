#include "XRayLog.h"

XRayLog::XRayLog()
{

}

void XRayLog::Log(const char* Text)
{
	UE_LOG(LogStalker, Log, TEXT("%S"), Text);
}
