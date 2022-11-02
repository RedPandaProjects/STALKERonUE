#include "XRayMemory.h"

XRayMemory::XRayMemory()
{
}

u32 XRayMemory::mem_usage()
{
	return 0;
}

void* XRayMemory::mem_alloc(size_t size)
{
	return FMemory::Malloc(size);
}

void* XRayMemory::mem_realloc(void* p, size_t size)
{
	return FMemory::Realloc(p,size);
}

void XRayMemory::mem_free(void* p)
{
	return FMemory::Free(p);
}
