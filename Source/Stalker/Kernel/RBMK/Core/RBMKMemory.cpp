#include "RBMKMemory.h"

FRBMKMemory::FRBMKMemory()
{
}

u32 FRBMKMemory::mem_usage()
{
	return 0;
}

void* FRBMKMemory::mem_alloc(size_t size)
{
	return FMemory::Malloc(size);
}

void* FRBMKMemory::mem_realloc(void* p, size_t size)
{
	return FMemory::Realloc(p,size);
}

void FRBMKMemory::mem_free(void* p)
{
	return FMemory::Free(p);
}
