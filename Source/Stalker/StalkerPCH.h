#pragma once
#include "Core.h"
#include "Engine.h"
#include "Modules/ModuleManager.h"
#include "Async/AsyncWork.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "Slate/SlateBrushAsset.h"
#include "SlateMaterialBrush.h"
#include "Misc/Paths.h"
#include "Engine/GameViewportClient.h"
#include "Engine/DataAsset.h"
#include "Animation/AnimNodeBase.h"
#include "AnimGraph/Public/AnimGraphNode_Base.h"
THIRD_PARTY_INCLUDES_START
#include "XrAPI/xrGameManager.h"
#include "XrCore/stdafx.h"
#include "xrEngine/stdafx.h"
#include "xrEngine/Render.h"
#include "xrEngine/FMesh.h"
#include "xrEngine/bone.h"
#include "xrCDB/xrCDB.h"
#include "XrRender/Public/RenderVisual.h"
#include "XrRender/Public/KinematicsAnimated.h"
#include "XrRender/Public/Kinematics.h"
THIRD_PARTY_INCLUDES_END
#include "Kernel/StalkerMath.h"

DECLARE_STATS_GROUP(TEXT("XRayEngine"), STATGROUP_XRayEngine, STATCAT_XRayEngine);
DECLARE_LOG_CATEGORY_EXTERN(LogStalker, Log, All);