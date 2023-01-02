#include "XRayEngine.h"
#include "XRayDevice.h"
#include "../Render/Interface/XRayRenderInterface.h"
#include "../Render/Interface/XRayRenderFactory.h"
#include "../Render/Interface/XRayDUInterface.h"
#include "../Render/Interface/XRayDebugRender.h"
#include "../Render/Interface/UI/XRayUIRender.h"
#include "XRayInput.h"
#include "../../StalkerEngineManager.h"
#include "XRayConsole.h"
#include "Entities/Levels/Proxy/StalkerProxy.h"
#include "Resources/StalkerResourcesManager.h"
#include "../../Unreal/WorldSettings/StalkerWorldSettings.h"
#include "Resources/CFrom/StalkerCForm.h"
#include "Resources/PhysicalMaterial/StalkerPhysicalMaterialsManager.h"
#include "Resources/PhysicalMaterial/StalkerPhysicalMaterial.h"
THIRD_PARTY_INCLUDES_START
#include "XrCDB/xr_area.h"
THIRD_PARTY_INCLUDES_END
XRayRenderFactory GRenderFactory;
XRayDUInterface  GDUInterface;
XRayDebugRender GDebugRender;
XRayEngine::XRayEngine()
{

}

void XRayEngine::Initialize()
{
	Device = new XRayDevice;
	Console = new  XRayConsole;

	XRayInput* MyInput = new XRayInput;
	GXRayEngineManager->SetInput(MyInput);
	GXRayInput = MyInput;
	::Render = &GRenderInterface;
	::RenderFactory = &GRenderFactory;
	::DU = &GDUInterface;
	UIRender = &GXRayUIRender;
	DRender = &GDebugRender;
	XRayEngineInterface::Initialize();
}

void XRayEngine::Destroy()
{
	GXRayEngineManager->SetInput(nullptr);
	XRayEngineInterface::Destroy();
	delete GXRayInput;
	delete Console;
	delete Device;
	Console	= nullptr;
	Device = nullptr;
	GXRayInput = nullptr;
}

void XRayEngine::Destroy(class XRayUnrealProxyInterface*InProxy)
{
	if (!InProxy)
	{
		return;
	}
	AStalkerProxy * StalkerProxy = InProxy->CastToStalkerProxy();
	GXRayEngineManager->GetResourcesManager()->Destroy(StalkerProxy);
	
}

class XRayUnrealProxyInterface* XRayEngine::CreateUnrealProxy(class CObject*InObject)
{

	return GXRayEngineManager->GetResourcesManager()->CreateProxy(InObject);
}

void XRayEngine::Level_Scan()
{
	XRayEngineInterface::Level_Scan();
}

int XRayEngine::Level_ID(LPCSTR name, LPCSTR ver, bool bSet)
{
	return XRayEngineInterface::Level_ID(name,ver,bSet);
}

void XRayEngine::Level_Set(u32 ID)
{
	XRayEngineInterface::Level_Set(ID);
	string_path Name;
	FCStringAnsi::Strcpy(Name, Levels[ID].folder);
	if (FCStringAnsi::Strlen(Name) && Name[FCStringAnsi::Strlen(Name) - 1] == '\\')
	{
		Name[FCStringAnsi::Strlen(Name) - 1]=0;
	}
	CurrentLevelName = Name;
	
}

void XRayEngine::LoadAllArchives()
{
}

void XRayEngine::LoadCFormFormCurrentWorld(class CObjectSpace& ObjectSpace, CDB::build_callback build_callback)
{
	UWorld*World = GXRayEngineManager->GetGameWorld();
	check(World);
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	check(StalkerWorldSettings);
	UStalkerCForm* CForm = StalkerWorldSettings->GetCForm();
	check(IsValid(CForm)); 
	check (IsValid(GXRayEngineManager->GetPhysicalMaterialsManager()->DefaultPhysicalMaterial));
	static_assert(sizeof(FVector3f)==sizeof(Fvector));

	TMap<uint32,uint32>		CFormMaterialID2GlobalID;
	for (uint32 Index=0; Index <static_cast<uint32>(CForm->Name2ID.Num()); Index++)
	{
		FString& Name = CForm->Name2ID[Index];
		shared_str LegacyName = TCHAR_TO_ANSI(*Name);
		int32 GlobalIndex =  GXRayEngineManager->GetPhysicalMaterialsManager()->Names.IndexOfByKey(LegacyName);
		if (GlobalIndex == INDEX_NONE)
		{
			UE_LOG(LogStalker,Error,TEXT("Can't found material %s in time build cform"),*Name);
			GlobalIndex = GXRayEngineManager->GetPhysicalMaterialsManager()->PhysicalMaterials.IndexOfByKey(GXRayEngineManager->GetPhysicalMaterialsManager()->DefaultPhysicalMaterial);
			check(GlobalIndex!= INDEX_NONE);
		}
		CFormMaterialID2GlobalID.Add(Index,static_cast<uint32>(GlobalIndex));
	}

	TArray<CDB::TRI>		LegacyCFormTriangles;
	LegacyCFormTriangles.Empty(CForm->Triangles.Num());
	for (FStalkerCFormTriangle& Triangle : CForm->Triangles)
	{
		LegacyCFormTriangles.AddDefaulted();
		CDB::TRI&LegacyTriangle = LegacyCFormTriangles.Last();
		LegacyTriangle.material= CFormMaterialID2GlobalID[Triangle.MaterialIndex];
		LegacyTriangle.verts[0] = Triangle.VertexIndex0;
		LegacyTriangle.verts[1] = Triangle.VertexIndex1;
		LegacyTriangle.verts[2] = Triangle.VertexIndex2;
	}

	TArray<Fvector>		LegacyCFormVertices;
	LegacyCFormVertices.Empty(CForm->Vertices.Num());
	for (FVector3f& Vertex : CForm->Vertices)
	{
		LegacyCFormVertices.Add(StalkerMath::UnrealLocationToXRay(Vertex));
	}
	hdrCFORM LegacyCFormHeader={};
	LegacyCFormHeader.aabb.invalidate();
	LegacyCFormHeader.aabb.modify(StalkerMath::UnrealLocationToXRay(CForm->AABB.Max));
	LegacyCFormHeader.aabb.modify(StalkerMath::UnrealLocationToXRay(CForm->AABB.Min));
	LegacyCFormHeader.vertcount = LegacyCFormVertices.Num();
	LegacyCFormHeader.facecount = LegacyCFormTriangles.Num();
	LegacyCFormHeader.version = CFORM_CURRENT_VERSION;

	ObjectSpace.Create(LegacyCFormVertices.GetData(), LegacyCFormTriangles.GetData(), LegacyCFormHeader, build_callback);
}
