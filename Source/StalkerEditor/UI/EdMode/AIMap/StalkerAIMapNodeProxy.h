#pragma once
#include "Resources/AIMap/StalkerAIMapNode.h"
struct HStalkerAIMapNodeProxy : public HHitProxy
{
	DECLARE_HIT_PROXY();

	HStalkerAIMapNodeProxy(FStalkerAIMapNode* InNode)
		: HHitProxy(HPP_World),Node(InNode)
	{}
	FStalkerAIMapNode*Node;
};
