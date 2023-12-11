#include "RBMKDebug.h"

FRBMKDebug::FRBMKDebug()
{

}

void FRBMKDebug::CriticalError(const char* InText)
{
	FString Text = InText;
	ensure(*Text);
}
