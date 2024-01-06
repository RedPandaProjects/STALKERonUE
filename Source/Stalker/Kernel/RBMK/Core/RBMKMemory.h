#pragma once
THIRD_PARTY_INCLUDES_START
#include "XrCore/stdafx.h"
THIRD_PARTY_INCLUDES_END
class FRBMKMemory:public XRayMemoryInterface
{
public:
	FRBMKMemory();
	u32 mem_usage() override;
	void* mem_alloc(size_t size) override;
	void* mem_realloc(void* p, size_t size) override;
	void mem_free(void* p) override;
};