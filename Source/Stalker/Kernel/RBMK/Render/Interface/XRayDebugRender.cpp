#include "XRayDebugRender.h"
#include "../../../StalkerEngineManager.h"
#include "../../../Unreal/GameMode/StalkerGameMode.h"
#include "../../../Unreal/WorldSettings/StalkerWorldSettings.h"
#include "Entities/Debug/StalkerDebugRender.h"
XRayDebugRender::XRayDebugRender()
{
}

void XRayDebugRender::Render()
{
}

void XRayDebugRender::add_lines(Fvector const* vertices, u32 const& vertex_count, u16 const* pairs, u32 const& pair_count, u32 const& color)
{
	if (!IsValid(GWorld))
	{
		return;
	}
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>( GWorld->GetWorldSettings());
	if (!IsValid(StalkerWorldSettings))
	{
		return;
	}
	if (!IsValid(StalkerWorldSettings->DebugRender))
	{
		return;
	}
	for (u32 i = 0; i < pair_count; i++)
	{
		if (Device->vCameraPosition.distance_to(vertices[pairs[i * 2]]) > StalkerWorldSettings->DebugRender->RenderDistance)
		{
			continue;
		}
		if (Device->vCameraPosition.distance_to(vertices[pairs[i * 2+1]]) > StalkerWorldSettings->DebugRender->RenderDistance)
		{
			continue;
		}
		FVector Start = FVector(StalkerMath::RBMKLocationToUnreal(vertices[pairs[i * 2]]));
		FVector End = FVector(StalkerMath::RBMKLocationToUnreal(vertices[pairs[i * 2 + 1]]));
		StalkerWorldSettings->DebugRender->LineBatchComponent->DrawLine(Start, End, FColor(color), 0,3, 1.f);;
	}
}

void XRayDebugRender::NextSceneMode()
{
}

void XRayDebugRender::ZEnable(bool bEnable)
{
}

void XRayDebugRender::OnFrameEnd()
{
}

void XRayDebugRender::SetShader(const debug_shader& shader)
{
}

void XRayDebugRender::CacheSetXformWorld(const Fmatrix& M)
{
}

void XRayDebugRender::CacheSetCullMode(CullMode mode)
{
}

void XRayDebugRender::SetAmbient(u32 colour)
{
}

void XRayDebugRender::SetDebugShader(dbgShaderHandle shdHandle)
{
}

void XRayDebugRender::DestroyDebugShader(dbgShaderHandle shdHandle)
{
}

void XRayDebugRender::dbg_DrawTRI(Fmatrix& T, Fvector& Inp1, Fvector& Inp2, Fvector& Inp3, u32 C)
{
	if (!IsValid(GWorld))
	{
		return;
	}
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(GWorld->GetWorldSettings());
	if (!IsValid(StalkerWorldSettings))
	{
		return;
	}
	if (!IsValid(StalkerWorldSettings->DebugRender))
	{
		return;
	}
	Fvector p1;
	T.transform(p1, Inp1);
	if (Device->vCameraPosition.distance_to(p1) > StalkerWorldSettings->DebugRender->RenderDistance)
	{
		return;
	}
	Fvector p2;
	T.transform(p2, Inp2);
	if (Device->vCameraPosition.distance_to(p2) > StalkerWorldSettings->DebugRender->RenderDistance)
	{
		return;
	}
	Fvector p3;
	T.transform(p3, Inp3);
	if (Device->vCameraPosition.distance_to(p3) > StalkerWorldSettings->DebugRender->RenderDistance)
	{
		return;
	}

	StalkerWorldSettings->DebugRender->AddTriangle(FVector(StalkerMath::RBMKLocationToUnreal(p1)), FVector(StalkerMath::RBMKLocationToUnreal(p2)),FVector( StalkerMath::RBMKLocationToUnreal(p3)),FColor(C) );
}