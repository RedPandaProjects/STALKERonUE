#pragma once
#include "XrCore/stdafx.h"
class XRayMemory:public XRayMemoryInterface
{
public:
	XRayMemory();

	u32 mem_usage() override;
	void* mem_alloc(size_t size) override;
	void* mem_realloc(void* p, size_t size) override;
	void mem_free(void* p) override;

};