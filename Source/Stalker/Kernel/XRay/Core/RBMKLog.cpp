#include "RBMKLog.h"

FRBMKLog::FRBMKLog()
{

}

void FRBMKLog::Log(const char* Text)
{
	UE_LOG(LogStalker, Log, TEXT("%S"), Text);
}
