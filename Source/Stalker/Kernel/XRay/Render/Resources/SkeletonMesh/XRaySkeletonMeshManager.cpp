#include "XRaySkeletonMeshManager.h"
#include "../Visual/XRaySkeletonVisual.h"
#include "../Visual/XRayFSkinned.h"
#include "../Visual/XRayKinematicsLegacy.h"
#include "../Visual/XRayKinematicsAnimatedLegacy.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/IGame_Persistent.h"
THIRD_PARTY_INCLUDES_END
XRaySkeletonMeshManager* GXRaySkeletonMeshManager = nullptr;
XRaySkeletonMeshManager::XRaySkeletonMeshManager()
{
	bIsDiscardDestroy = true;

	g_pMotionsContainer = new motions_container();
}

XRaySkeletonMeshManager::~XRaySkeletonMeshManager()
{
	Flush();
	check(Meshes.Num() == 0);
	delete g_pMotionsContainer;
}

class XRaySkeletonVisual* XRaySkeletonMeshManager::Get(shared_str Name)
{
	if (Meshes.Contains(Name))
	{
		return Dublicate(Meshes[Name]);
	}
	return Dublicate(Load(Name));
}

class XRaySkeletonVisual* XRaySkeletonMeshManager::Get(shared_str InName, IReader* FileData)
{
	if (Meshes.Contains(InName))
	{
		return Dublicate(Meshes[InName]);
	}
	XRaySkeletonVisual* Result = nullptr;
	ogf_header			Header;

	FileData->r_chunk_safe(OGF_HEADER, &Header, sizeof(Header));

	Result = Create(Header.type);
	Result->Load(InName.c_str(), FileData, 0);

	g_pGamePersistent->RegisterModel(Result);

	Meshes.Add(InName, Result);
	return Dublicate(Result);
}

class XRaySkeletonVisual* XRaySkeletonMeshManager::Dublicate(class XRaySkeletonVisual* In)
{
	XRaySkeletonVisual* Result = Create(In->getType());
	Result->Copy(In);
	Result->Spawn();
	return Result;
}

void XRaySkeletonMeshManager::Destroy(class XRaySkeletonVisual* In)
{
	if (bIsDiscardDestroy)
	{

		Pool.Add(In);
	}
	else
	{
		XRaySkeletonVisual* Parent = In->Parent;
		In->Depart();
		if (Parent && Parent->Release())
		{
			Meshes.Remove(Parent->Name);
			delete Parent;
		}
		delete In;
	}
}

void XRaySkeletonMeshManager::Flush()
{
	bIsDiscardDestroy  = false;
	for (XRaySkeletonVisual* Visual : Pool)
	{
		Destroy(Visual);
	}
	Pool.Empty();
	bIsDiscardDestroy = true;
}

XRaySkeletonVisual* XRaySkeletonMeshManager::Load(shared_str InName)
{
	XRaySkeletonVisual* Result = nullptr;
	string_path			Name;
	ogf_header			Header;
	string_path			FileName;

	if (!strext(InName.c_str()))
	{
		xr_strcpy(Name, InName.c_str());
		xr_strcat(Name, ".ogf");
	}
	else
	{
		FCStringAnsi::Strcpy(Name, InName.c_str());
	}

	if (!FS.exist(InName.c_str()))
	{
		if (!FS.exist(FileName, "$level$", Name) && !FS.exist(FileName, "$game_meshes$", Name))
		{
			check(false);
			return nullptr;
		}
	}

	else  	FCStringAnsi::Strcpy(FileName, InName.c_str()); 

	IReader* FileData = FS.r_open(FileName);

	FileData->r_chunk_safe(OGF_HEADER, &Header, sizeof(Header));

	Result = Create(Header.type);
	Result->Load(InName.c_str(), FileData, 0);
	FS.r_close(FileData);

	g_pGamePersistent->RegisterModel(Result);

	Meshes.Add(InName,Result);
	return Result;
}

XRaySkeletonVisual* XRaySkeletonMeshManager::Create(u32 Type)
{
	switch (Type)
	{
	case MT_SKELETON_ANIM:			return new XRayKinematicsAnimatedLegacy();
	case MT_SKELETON_RIGID:			return new XRayKinematicsLegacy();
	case MT_SKELETON_GEOMDEF_PM:	
	case MT_SKELETON_GEOMDEF_ST:	return new XRaySkeletonMesh();//  xr_new < CSkeletonX_ST>(); break;
	default:
		check(false);
		return nullptr;
	}
}
