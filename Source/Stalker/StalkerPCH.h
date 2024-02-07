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
#include "Animation/AnimInstanceProxy.h"
#include "Animation/AnimSequence.h"
#include "Components/BrushComponent.h"
#include "GameFramework/Volume.h"
#include "Engine/GameInstance.h"
#include "PhysicsEngine/BoxElem.h"
#include "PhysicsEngine/SphereElem.h"
#include "PhysicsEngine/SphylElem.h"
THIRD_PARTY_INCLUDES_START
#include "XrAPI/xrGameManager.h"
#include "XrCore/stdafx.h"
#include "xrEngine/stdafx.h"
#include "xrEngine/Render.h"
#include "xrEngine/FMesh.h"
#include "xrEngine/bone.h"
#include "xrCDB/xrCDB.h"
#include "XrEngine/Render/RenderVisual.h"
#include "XrEngine/Render/KinematicsAnimated.h"
#include "XrEngine/Render/Kinematics.h"
THIRD_PARTY_INCLUDES_END
#include "Kernel/StalkerMath.h"
#include "Kernel/StalkerGlobal.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

DECLARE_STATS_GROUP(TEXT("XRayEngine"), STATGROUP_XRayEngine, STATCAT_Advanced);
DECLARE_LOG_CATEGORY_EXTERN(LogStalker, Log, All);

#define ECC_Kinematics  ECC_GameTraceChannel1