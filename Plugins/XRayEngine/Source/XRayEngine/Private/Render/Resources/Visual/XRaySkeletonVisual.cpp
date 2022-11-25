#include "XRaySkeletonVisual.h"
#include "../SkeletonMesh/XRaySkeletonMeshManager.h"

XRaySkeletonVisual::XRaySkeletonVisual():RefCount(0), Parent(nullptr)
{
	CountVertex = 0;
	CountIndex = 0;
	OffsetIndex = 0;
	OffsetVertex = 0;
}

XRaySkeletonVisual::~XRaySkeletonVisual()
{
	check(RefCount == 0);
	check(Children.size()==0);

}

void XRaySkeletonVisual::Load(const char* N, IReader* data, u32 dwFlags)
{
	Name = N;
	ogf_header	hdr;
	if (data->r_chunk_safe(OGF_HEADER, &hdr, sizeof(hdr)))
	{
		R_ASSERT2(hdr.format_version == xrOGF_FormatVersion, "Invalid visual version");
		Type = hdr.type;
		VisData.box.set(hdr.bb.min, hdr.bb.max);
		VisData.sphere.set(hdr.bs.c, hdr.bs.r);
	}
	else 
	{
		FATAL("Invalid visual");
	}

	if (data->find_chunk(OGF_CHILDREN))
	{
		// From stream
		IReader* OBJ = data->open_chunk(OGF_CHILDREN);
		if (OBJ) 
		{
			IReader* O = OBJ->open_chunk(0);
			for (int count = 1; O; count++) 
			{
				string_path			name_load, short_name, num;
				xr_strcpy(short_name, N);
				if (strext(short_name)) *strext(short_name) = 0;
				strconcat(sizeof(name_load), name_load, short_name, ":", itoa(count, num, 10));
				Children.push_back(GXRaySkeletonMeshManager->Get(name_load, O));
				O->close();
				O = OBJ->open_chunk(count);
			}
			OBJ->close();
		}
	}
}

void XRaySkeletonVisual::Copy(XRaySkeletonVisual* Right)
{
	check(Children.empty());
	Type = Right->Type;
	VisData = Right->VisData;
	Name = Right->Name;
	Parent = Right->Parent ? Right->Parent: Right;
	Parent->RefCount++;
	for (XRaySkeletonVisual* Child : Right->Children)
	{
		Children.push_back(GXRaySkeletonMeshManager->Dublicate(Child));
	}
}

void XRaySkeletonVisual::Spawn()
{
	for (XRaySkeletonVisual* Child : Children)
	{
		Child->Spawn();
	}
}

void XRaySkeletonVisual::Depart()
{
	if (Parent)
	{
		Parent->RefCount--;
		Parent = nullptr;
	}
	for (XRaySkeletonVisual* Child : Children)
	{
		GXRaySkeletonMeshManager->Destroy(Child);
	}
	Children.clear();
}

bool XRaySkeletonVisual::Release()
{
	if (RefCount)
	{
		return false;
	}
	for (XRaySkeletonVisual* Child : Children)
	{
		GXRaySkeletonMeshManager->Destroy(Child);
	}
	Children.clear();
	return true;
}

vis_data& _BCL XRaySkeletonVisual::getVisData()
{
	return VisData;
}

u32 XRaySkeletonVisual::getType()
{
	return Type;
}

shared_str _BCL XRaySkeletonVisual::getDebugName()
{
	return Name;
}
