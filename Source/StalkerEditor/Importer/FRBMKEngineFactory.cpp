#include "FRBMKEngineFactory.h"

#include <Resources/StaticMesh/StalkerStaticMeshAssetUserData.h>

#include "AudioCompressionSettingsUtils.h"
#include "ObjectTools.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "Resources/SkeletonMesh/StalkerKinematicsBoneData.h"
#include "Resources/SkeletonMesh/StalkerKinematicsAssetUserData.h"
#include "Engine/EngineTypes.h"
#include "ImportUtils/StaticMeshImportUtils.h"
#include "../StalkerEditorManager.h"
#include "Engine/SkinnedAssetCommon.h"
#include "Kernel/StalkerEngineManager.h"
#include "Kernel/Unreal/GameSettings/StalkerGameSettings.h"
#include "Resources/PhysicalMaterial/StalkerPhysicalMaterialsManager.h"
#include "Resources/SkeletonMesh/StalkerKinematicsAnimAssetUserData.h"
#include "Resources/Sound/StalkerSoundWaveAssetUserData.h"
#include "Sound/SoundNodeLooping.h"
#include "Sound/SoundNodeWavePlayer.h"
THIRD_PARTY_INCLUDES_START
#include "Editors/XrECore/Engine/GameMtlLib.h"
THIRD_PARTY_INCLUDES_END
#include "Resources/PhysicalMaterial/StalkerPhysicalMaterial.h"
#include "Resources/PhysicalMaterial/StalkerPhysicalMaterialPairsData.h"

#include "VorbisAudioInfo.h"
#if WITH_OGGVORBIS
	#pragma pack(push, 8)
	#include "vorbis/vorbisenc.h"
	#include "vorbis/vorbisfile.h"
	#pragma pack(pop)
#endif

#define LOCTEXT_NAMESPACE "XRayImporterModule"


#pragma pack( push,2 )
struct  XRayVertBoned1W			// (3+3+3+3+2+1)*4 = 15*4 = 60 bytes
{
	Fvector	P;
	Fvector	N;
	Fvector	T;
	Fvector	B;
	float	u, v;
	u32		matrix;
};
struct  XRayVertBoned2W			// (1+3+3 + 1+3+3 + 2)*4 = 16*4 = 64 bytes
{
	u16		matrix0;
	u16		matrix1;
	Fvector	P;
	Fvector	N;
	Fvector	T;
	Fvector	B;
	float	w;
	float	u, v;
};
struct XRayVertBoned3W          // 70 bytes
{
	u16		m[3];
	Fvector	P;
	Fvector	N;
	Fvector	T;
	Fvector	B;
	float	w[2];
	float	u, v;
};
struct XRayVertBoned4W       //76 bytes
{
	u16		m[4];
	Fvector	P;
	Fvector	N;
	Fvector	T;
	Fvector	B;
	float	w[3];
	float	u, v;
};
#pragma pack(pop)

IC	void QR2Quat(const CKeyQR& K, Fquaternion& Q)
{
	Q.x = float(K.x) * KEY_QuantI;
	Q.y = float(K.y) * KEY_QuantI;
	Q.z = float(K.z) * KEY_QuantI;
	Q.w = float(K.w) * KEY_QuantI;
}

IC void QT8_2T(const CKeyQT8& K, const CMotion& M, Fvector& T)
{
	T.x = float(K.x1) * M._sizeT.x + M._initT.x;
	T.y = float(K.y1) * M._sizeT.y + M._initT.y;
	T.z = float(K.z1) * M._sizeT.z + M._initT.z;
}

IC void QT16_2T(const CKeyQT16& K, const CMotion& M, Fvector& T)
{
	T.x = float(K.x1) * M._sizeT.x + M._initT.x;
	T.y = float(K.y1) * M._sizeT.y + M._initT.y;
	T.z = float(K.z1) * M._sizeT.z + M._initT.z;
}

FRBMKEngineFactory::FRBMKEngineFactory(UObject* InParentPackage, EObjectFlags InFlags)
{
	ParentPackage = InParentPackage;
	ObjectFlags = InFlags;


	string_path fn;
	FS.update_path(fn, _game_data_, "shaders_xrlc.xr");
	if (ensure(FS.exist(fn)))
	{
		ShaderXRLC.Load(fn);
	}
}

FRBMKEngineFactory::~FRBMKEngineFactory()
{
	ShaderXRLC.Unload();
	for (CEditableObject* Object : Objects)
	{
		GXRayObjectLibrary->RemoveEditObject(Object);
	}
}
inline bool operator == (const Fmatrix33& Left, const Fmatrix33& Right)
{
	return Left.i == Right.i && Left.j == Right.j && Left.k == Right.k;

}

inline bool operator == (const Fobb& Left, const Fobb& Right)
{
	return Left.m_halfsize == Right.m_halfsize && Left.m_rotate == Right.m_rotate && Left.m_translate == Right.m_translate;

}
inline bool operator == (const Fsphere& Left, const Fsphere& Right)
{
	return Left.P == Right.P && FMath::IsNearlyEqual(Left.R, Right.R, 0.0000001);

}
inline bool operator == (const Fcylinder& Left, const Fcylinder& Right)
{
	return Left.m_center == Right.m_center && Left.m_direction == Right.m_direction && FMath::IsNearlyEqual(Left.m_radius, Right.m_radius, 0.0000001);

}
inline bool operator == (const SBoneShape& Left, const SBoneShape& Right)
{
	if (Left.type != Right.type || !Left.flags.equal(Right.flags))
	{
		return false;
	}
	switch (Right.type)
	{
	default:
		return true;
	case SBoneShape::stBox:
		return Left.box == Right.box;
	case SBoneShape::stSphere:
		return Left.sphere == Right.sphere;
	case SBoneShape::stCylinder:
		return Left.cylinder == Right.cylinder;
	}
	return false;
}
inline bool operator == (const SJointLimit& Left, const SJointLimit& Right)
{
	return Left.damping_factor == Right.damping_factor && Left.limit.x == Right.limit.x && Left.limit.y == Right.limit.y && Left.spring_factor == Right.spring_factor;

}
inline bool operator == (const SJointIKData& Left, const SJointIKData& Right)
{
	return Left.break_force == Right.break_force &&
		Left.break_torque == Right.break_torque &&
		Left.damping_factor == Right.damping_factor &&
		Left.friction == Right.friction &&
		Left.ik_flags == Right.ik_flags &&
		Left.limits[0] == Right.limits[0] &&
		Left.limits[1] == Right.limits[1] &&
		Left.limits[2] == Right.limits[2] &&
		Left.spring_factor == Right.spring_factor &&
		Left.type == Right.type;

}
USkeletalMesh* FRBMKEngineFactory::ImportOGF(const FString& FileName)
{
	USkeletalMesh* SkeletalMesh = nullptr;
	FString ObjectPath = UPackageTools::SanitizePackageName(ParentPackage->GetName() / FPaths::GetBaseFilename(FileName));
	if(!LoadOrCreateOrOverwriteAsset(ObjectPath,ObjectFlags,SkeletalMesh))
	{
		return SkeletalMesh;
	}

	IReader* FileData = FS.r_open(TCHAR_TO_ANSI(*FileName));
	if (!FileData)
	{
		UE_LOG(LogXRayImporter, Warning, TEXT("Can't load ogf %s"), *FileName);
		return nullptr;
	}


	ogf_header	Header;
	if (!FileData->r_chunk_safe(OGF_HEADER, &Header, sizeof(Header)))
	{
		UE_LOG(LogXRayImporter, Warning, TEXT("Can't load ogf %s no found OGF_HEADER"), *FileName);

		FS.r_close(FileData);
		return nullptr;
	}

	if (Header.type != MT_SKELETON_RIGID && Header.type != MT_SKELETON_ANIM)
	{
		UE_LOG(LogXRayImporter, Warning, TEXT("Can't load ogf %s unkown type 0x%x"), *FileName, Header.type);

		FS.r_close(FileData);
		return nullptr;
	}
	TArray<shared_str> BoneParents;
	TArray<TSharedPtr<CBoneData>> Bones;
	TMap<shared_str, CBoneData*> BonesMap;
	if (!FileData->find_chunk(OGF_S_BONE_NAMES))
	{
		UE_LOG(LogXRayImporter, Warning, TEXT("Can't load ogf %s no found OGF_S_BONE_NAMES"), *FileName);

		FS.r_close(FileData);
		return nullptr;
	}
	else
	{
		uint32 CountBones = FileData->r_u32();
		string256	BufferString;
		Bones.Empty(CountBones);
		for (uint32 ID = 0; ID < CountBones; ID++)
		{
			Bones.Add(MakeShared<CBoneData>(ID));
			FileData->r_stringZ(BufferString, sizeof(BufferString));	xr_strlwr(BufferString);
			Bones.Last()->name = shared_str(BufferString);
			FileData->r_stringZ(BufferString, sizeof(BufferString)); xr_strlwr(BufferString);
			BoneParents.Add(BufferString);
			FileData->r(&Bones.Last()->obb, sizeof(Fobb));
			BonesMap.FindOrAdd(Bones.Last()->name, Bones.Last().Get());
		}
		for (uint32 ID = 0; ID < CountBones; ID++)
		{
			if (!BoneParents[ID].size())
			{
				Bones[ID]->SetParentID(BI_NONE);
				continue;
			}
			Bones[ID]->SetParentID(BonesMap[BoneParents[ID]]->GetSelfID());
		}
	}
	BonesMap.Empty();
	BoneParents.Empty();

	IReader* IKD = FileData->open_chunk(OGF_S_IKDATA);
	if (!IKD)
	{
		UE_LOG(LogXRayImporter, Warning, TEXT("Can't load ogf %s no found OGF_S_IKDATA"), *FileName);

		FS.r_close(FileData);
		return nullptr;
	}

	for (int32 i = 0; i < Bones.Num(); i++)
	{
		CBoneData* B = Bones[i].Get();
		u16 vers = (u16)IKD->r_u32();
		IKD->r_stringZ(B->game_mtl_name);
		IKD->r(&B->shape, sizeof(SBoneShape));
		B->IK_data.Import(*IKD, vers);
		Fvector vXYZ, vT;
		IKD->r_fvector3(vXYZ);
		IKD->r_fvector3(vT);
		B->bind_transform.setXYZi(vXYZ);
		B->bind_transform.translate_over(vT);
		B->mass = IKD->r_float();
		IKD->r_fvector3(B->center_of_mass);
	}
	IKD->close();

	TArray<TPair<shared_str, shared_str>> MaterialsAndTextures;
	TArray<TArray<st_MeshVertex2>>  VerticesInstance;
	TArray<Fvector>  Vertices;
	if (!FileData->find_chunk(OGF_CHILDREN))
	{
		UE_LOG(LogXRayImporter, Warning, TEXT("Can't load ogf %s no found OGF_CHILDREN"), *FileName);

		FS.r_close(FileData);
		return nullptr;
	}
	else
	{
		// From stream
		IReader* OBJ = FileData->open_chunk(OGF_CHILDREN);
		if (OBJ)
		{
			IReader* O = OBJ->open_chunk(0);
			for (int32 CountElement = 1; O; CountElement++)
			{
				TArray<st_MeshVertex2> ElementVertices;
				TArray<u16> Indicies;
				check(O->find_chunk(OGF_INDICES));
				uint32 IndiciesCount = O->r_u32();
				for (uint32 i = 0; i < IndiciesCount; i++)
					Indicies.Add(O->r_u16());

				check(O->find_chunk(OGF_VERTICES));
				uint32 dwVertType = O->r_u32();
				uint32 dwVertCount = O->r_u32();
				uint32 MaxVertex = 0;
				switch (dwVertType)
				{
				case OGF_VERTEXFORMAT_FVF_1L: // 1-Link
				case 1:
				{
					XRayVertBoned1W* InVertices = (XRayVertBoned1W*)O->pointer();
					for (uint32 i = 0; i < IndiciesCount; i++)
					{
						XRayVertBoned1W InVertex = InVertices[Indicies[i]];
						st_MeshVertex2 OutVertex;

						for (uint32 a = 0; a < 4; a++)
							OutVertex.BoneID[a] = BI_NONE;

						OutVertex.Index = Indicies[i];
						MaxVertex = FMath::Max(MaxVertex,OutVertex.Index);
						OutVertex.Index +=Vertices.Num();
						OutVertex.Normal = InVertex.N;
						OutVertex.UV.set(InVertex.u, InVertex.v);
						OutVertex.BoneID[0] = InVertex.matrix;
						OutVertex.BoneWeight[0] = 1;
						ElementVertices.Add(OutVertex);
					}
						
						for (uint32 i = 0; i <= MaxVertex; i++)
						{
							Vertices.Add(InVertices[i].P);
						}
				}
				break;
				case OGF_VERTEXFORMAT_FVF_2L: // 2-Link
				case 2:
				{
					XRayVertBoned2W* InVertices = (XRayVertBoned2W*)O->pointer();
					for (uint32 i = 0; i < IndiciesCount; i++)
					{
						XRayVertBoned2W InVertex = InVertices[Indicies[i]];
						st_MeshVertex2 OutVertex;

						for (uint32 a = 0; a < 4; a++)
							OutVertex.BoneID[a] = BI_NONE;

						OutVertex.Index = Indicies[i];
						MaxVertex = FMath::Max(MaxVertex,OutVertex.Index);
						OutVertex.Index +=Vertices.Num();
						OutVertex.Normal = InVertex.N;
						OutVertex.UV.set(InVertex.u, InVertex.v);
						if (InVertex.matrix0 != InVertex.matrix1)
						{
							OutVertex.BoneID[0] = InVertex.matrix0;
							OutVertex.BoneID[1] = InVertex.matrix1;
							OutVertex.BoneWeight[0] = 1 - InVertex.w;
							OutVertex.BoneWeight[1] = InVertex.w;
						}
						else
						{
							OutVertex.BoneID[0] = InVertex.matrix0;
							OutVertex.BoneWeight[0] = 1.f;
						}
						ElementVertices.Add(OutVertex);
					}
						
						for (uint32 i = 0; i <= MaxVertex; i++)
						{
							Vertices.Add(InVertices[i].P);
						}
				}break;
				case OGF_VERTEXFORMAT_FVF_3L: // 3-Link
				case 3:
				{
					XRayVertBoned3W* InVertices = (XRayVertBoned3W*)O->pointer();
					for (uint32 i = 0; i < IndiciesCount; i++)
					{
						XRayVertBoned3W InVertex = InVertices[Indicies[i]];
						st_MeshVertex2 OutVertex;

						for (uint32 a = 0; a < 4; a++)
							OutVertex.BoneID[a] = BI_NONE;

						OutVertex.Index = Indicies[i];
						MaxVertex = FMath::Max(MaxVertex,OutVertex.Index);
						OutVertex.Index +=Vertices.Num();
						OutVertex.Normal = InVertex.N;
						OutVertex.UV.set(InVertex.u, InVertex.v);
						OutVertex.BoneID[0] = InVertex.m[0];
						OutVertex.BoneID[1] = InVertex.m[1];
						OutVertex.BoneID[2] = InVertex.m[2];
						OutVertex.BoneWeight[0] = InVertex.w[0];
						OutVertex.BoneWeight[1] = InVertex.w[1];
						OutVertex.BoneWeight[2] = 1 - InVertex.w[1] - InVertex.w[0];
						ElementVertices.Add(OutVertex);
					}
						
						for (uint32 i = 0; i <= MaxVertex; i++)
						{
							Vertices.Add(InVertices[i].P);
						}
				}break;
				case OGF_VERTEXFORMAT_FVF_4L: // 4-Link
				case 4:
				{
					XRayVertBoned4W* InVertices = (XRayVertBoned4W*)O->pointer();
					for (uint32 i = 0; i < IndiciesCount; i++)
					{
						XRayVertBoned4W InVertex = InVertices[Indicies[i]];
						st_MeshVertex2 OutVertex;

						for (uint32 a = 0; a < 4; a++)
							OutVertex.BoneID[a] = BI_NONE;

						OutVertex.Index = Indicies[i];
						MaxVertex = FMath::Max(MaxVertex,OutVertex.Index);
						OutVertex.Index +=Vertices.Num();
						OutVertex.Normal = InVertex.N;
						OutVertex.UV.set(InVertex.u, InVertex.v);
						OutVertex.BoneID[0] = InVertex.m[0];
						OutVertex.BoneID[1] = InVertex.m[1];
						OutVertex.BoneID[2] = InVertex.m[2];
						OutVertex.BoneID[3] = InVertex.m[3];
						OutVertex.BoneWeight[0] = InVertex.w[0];
						OutVertex.BoneWeight[1] = InVertex.w[1];
						OutVertex.BoneWeight[2] = InVertex.w[2];
						OutVertex.BoneWeight[3] = 1 - InVertex.w[2] - InVertex.w[1] - InVertex.w[0];
						ElementVertices.Add(OutVertex);
					}
					for (uint32 i = 0; i <= MaxVertex; i++)
					{
						Vertices.Add(InVertices[i].P);
					}
				}break;
				default:
					break;
				}
				
				IReader* LodData = O->open_chunk(OGF_SWIDATA);
				if (LodData)
				{
					LodData->r_u32();
					LodData->r_u32();
					LodData->r_u32();
					LodData->r_u32();

					TArray<FSlideWindow> SlideWindows;
					SlideWindows.AddDefaulted(LodData->r_u32());
					LodData->r(SlideWindows.GetData(), SlideWindows.Num() * sizeof(FSlideWindow));
					LodData->close();

					VerticesInstance.AddDefaulted();
					for (uint32 i = 0; i < static_cast<uint32>(SlideWindows[0].num_tris * 3); i++)
					{
						VerticesInstance.Last().Add(ElementVertices[i + SlideWindows[0].offset]);
					}
				}
				else
				{
					VerticesInstance.Add(ElementVertices);
				}
				if (O->find_chunk(OGF_TEXTURE))
				{
					string256		fnT, fnS;
					O->r_stringZ(fnT, sizeof(fnT));
					O->r_stringZ(fnS, sizeof(fnS));
					MaterialsAndTextures.Add(TPair<shared_str, shared_str>(fnS, fnT));
				}
				else
				{
					MaterialsAndTextures.Add(TPair<shared_str, shared_str>("default", "Unkown"));
				}



				O->close();
				O = OBJ->open_chunk(CountElement);
			}
		}

	}
	check(MaterialsAndTextures.Num() == VerticesInstance.Num());
	TArray<SkeletalMeshImportData::FBone> UBones;
	UBones.AddDefaulted(Bones.Num());
	for (int32 BoneIndex = 0; BoneIndex < Bones.Num(); BoneIndex++)
	{
		SkeletalMeshImportData::FBone& Bone = UBones[BoneIndex];
		Bone.Flags = 0;
		Bone.ParentIndex = INDEX_NONE;
		Bone.NumChildren = 0;
	}
	TArray<uint32> OutBones;
	for (TSharedPtr<CBoneData>& Bone : Bones)
	{
		FindBoneIDOrAdd(OutBones, Bones, Bone.Get());
	}

	check(OutBones.Num());
	check(Bones[OutBones[0]]->GetParentID() == BI_NONE);

	for (int32 BoneIndex = 0; BoneIndex < Bones.Num(); BoneIndex++)
	{
		SkeletalMeshImportData::FBone& Bone = UBones[BoneIndex];
		CBoneData& BoneData = *Bones[OutBones[BoneIndex]].Get();
		Bone.Flags = 0;
		Bone.ParentIndex = INDEX_NONE;
		Bone.NumChildren = 0;
		if (BoneIndex != 0)
		{
			Bone.ParentIndex = FindBoneIDOrAdd(OutBones, Bones, Bones[BoneData.GetParentID()].Get());
			UBones[Bone.ParentIndex].NumChildren++;
		}
		Bone.BonePos.Transform.SetRotation(StalkerMath::RBMKQuaternionToUnreal(BoneData.bind_transform));
		Bone.BonePos.Transform.SetLocation(StalkerMath::RBMKLocationToUnreal(BoneData.bind_transform.c));
		Bone.Name = BoneData.name.c_str();
	}

	TArray<st_MeshVertex2> Bone7;
	TArray<st_MeshVertex2> Bone3;

	for (size_t ElementID = 0; ElementID < MaterialsAndTextures.Num(); ElementID++)
	{
		TArray< st_MeshVertex2>& OutVertices = VerticesInstance[ElementID];
		if (OutVertices.Num() == 0)
		{
			continue;
		}
		for (size_t FaceID = 0; FaceID < OutVertices.Num(); FaceID++)
		{
			for (size_t InfluencesID = 0; InfluencesID < 4; InfluencesID++)
			{
				if (OutVertices[FaceID].BoneID[InfluencesID] == BI_NONE)
				{
					continue;
				}
				if (OutVertices[FaceID].BoneID[InfluencesID] == 3)
				{
					Bone3.Add(OutVertices[FaceID]);
				}
				if (OutVertices[FaceID].BoneID[InfluencesID] == 7)
				{
					Bone7.Add(OutVertices[FaceID]);
				}
			}
		}
	}
	TArray< FSkeletalMeshImportData> SkeletalMeshImportDataList;
	TArray<FSkeletalMaterial> InMaterials;
	SkeletalMeshImportDataList.AddDefaulted(1);
	int32 MaterialIndex = 0;

	for (int32 LodIndex = 0; LodIndex < 1; LodIndex++)
	{
		FSkeletalMeshImportData& InSkeletalMeshImportData = SkeletalMeshImportDataList[LodIndex];
		InSkeletalMeshImportData.NumTexCoords = 1;
		InSkeletalMeshImportData.bHasNormals = true;
		InSkeletalMeshImportData.RefBonesBinary = UBones;

		for (size_t ElementID = 0; ElementID < MaterialsAndTextures.Num(); ElementID++)
		{
			TArray< st_MeshVertex2>& OutVertices = VerticesInstance[ElementID];
			if (OutVertices.Num() == 0)
			{
				continue;
			}
			const FString SurfacePackageName = UPackageTools::SanitizePackageName(ObjectPath / TEXT("Materials") / TEXT("Mat_") + FString::FromInt(ElementID));

			SkeletalMeshImportData::FMaterial Material;
			Material.Material = ImportSurface(SurfacePackageName, MaterialsAndTextures[ElementID].Key, MaterialsAndTextures[ElementID].Value,"default",ObjectPath.EndsWith(TEXT("_hud")));
			Material.MaterialImportName = TEXT("Mat_") + FString::FromInt(ElementID);
			InMaterials.Add(FSkeletalMaterial(Material.Material.Get(), true, false, FName(Material.MaterialImportName), FName(Material.MaterialImportName)));
			int32 MaterialID = InSkeletalMeshImportData.Materials.Add(Material);

			TMap<int32,int32> ID2VertexIDs;
			for (int32 VertexID = 0; VertexID < Vertices.Num() ; VertexID++)
			{
				FVector3f VertexPositions;
				VertexPositions = StalkerMath::RBMKLocationToUnreal(Vertices[VertexID]);
				ID2VertexIDs.Add(VertexID, InSkeletalMeshImportData.Points.Add(VertexPositions));
			}

			
			for (size_t FaceID = 0; FaceID < OutVertices.Num() / 3; FaceID++)
			{
				SkeletalMeshImportData::FTriangle Triangle;
				Triangle.MatIndex = MaterialID;
				for (size_t VirtualVertexID = 0; VirtualVertexID < 3; VirtualVertexID++)
				{
					SkeletalMeshImportData::FVertex Wedge;
					static size_t VirtualVertices[3] = { 0,2,1 };
					size_t VertexID = VirtualVertices[VirtualVertexID] + FaceID * 3;

					int32 OutVertexID = ID2VertexIDs[OutVertices[VertexID].Index];

					for (size_t InfluencesID = 0; InfluencesID < 4; InfluencesID++)
					{
						if (OutVertices[VertexID].BoneID[InfluencesID] == BI_NONE)
						{
							continue;
						}
						SkeletalMeshImportData::FRawBoneInfluence BoneInfluence;
						BoneInfluence.BoneIndex = FindBoneIDOrAdd(OutBones, Bones, Bones[OutVertices[VertexID].BoneID[InfluencesID]].Get());
						BoneInfluence.Weight = OutVertices[VertexID].BoneWeight[InfluencesID];
						BoneInfluence.VertexIndex = OutVertexID;
						InSkeletalMeshImportData.Influences.Add(BoneInfluence);
					}

					Wedge.VertexIndex = OutVertexID;
					Wedge.MatIndex = MaterialID;
					Wedge.UVs[0] = FVector2f(OutVertices[VertexID].UV.x, OutVertices[VertexID].UV.y);
					Triangle.WedgeIndex[VirtualVertexID] = InSkeletalMeshImportData.Wedges.Add(Wedge);
					Triangle.TangentZ[VirtualVertexID].X = -OutVertices[VertexID].Normal.x;
					Triangle.TangentZ[VirtualVertexID].Y = OutVertices[VertexID].Normal.z;
					Triangle.TangentZ[VirtualVertexID].Z = OutVertices[VertexID].Normal.y;
				}

				InSkeletalMeshImportData.Faces.Add(Triangle);

			}

		}
		InSkeletalMeshImportData.bHasNormals = true;
		InSkeletalMeshImportData.MaxMaterialIndex = MaterialIndex;
	}
	//FSkeleto

	if (!CreateSkeletalMesh(SkeletalMesh, SkeletalMeshImportDataList, UBones))
	{
		SkeletalMesh->MarkAsGarbage();
		return nullptr;
	}
	FAssetRegistryModule::AssetCreated(SkeletalMesh);
	ObjectCreated.Add(SkeletalMesh);

	SkeletalMesh->SetMaterials(InMaterials);
	SkeletalMesh->PostEditChange();

	USkeleton* Skeleton = FindOrCreateSkeleton(ObjectPath + "_Skeleton", SkeletalMesh);

	Skeleton->SetPreviewMesh(SkeletalMesh);
	SkeletalMesh->SetSkeleton(Skeleton);

	SkeletalMesh->PostEditChange();
	SkeletalMesh->AddAssetUserData(NewObject<UStalkerKinematicsAssetUserData>(SkeletalMesh));
	UStalkerKinematicsAssetUserData* StalkerKinematicsData = SkeletalMesh->GetAssetUserDataChecked<UStalkerKinematicsAssetUserData>();

	IReader* UD = FileData->open_chunk(OGF_S_USERDATA);
	if (UD)
	{
		xr_string str;
		UD->r_stringZ(str);
		StalkerKinematicsData->UserData = str.c_str();
		UD->close();
	}
	StalkerKinematicsData->BuildFromLegacy(Bones);
	TArray<IBoneData*>  BonesPtr;
	for (TSharedPtr<CBoneData>& Bone : Bones)
	{
		BonesPtr.Add(Bone.Get());
	}
	CreatePhysicsAsset(ObjectPath + "_PhysicsAsset", SkeletalMesh, BonesPtr);
	if (Header.type == MT_SKELETON_ANIM)
	{
		CreateAnims(ObjectPath / TEXT("Anims"), SkeletalMesh, Bones, FileData);
	}
	g_pMotionsContainer->clean(false);
	FS.r_close(FileData);

	StalkerKinematicsData->Modify();
	SkeletalMesh->PostEditChange();
	return SkeletalMesh;
}

UObject* FRBMKEngineFactory::ImportObject(const FString& FileName, bool DivideSubObject)
{
	CEditableObject* Object = GXRayObjectLibrary->CreateEditObject(TCHAR_TO_ANSI(*FileName));
	if (Object)
	{
		Objects.Add(Object);
		if (Object->IsSkeleton())
		{
			return ImportObjectAsDynamicMesh(Object, false);
		}
		else
		{
			if(DivideSubObject)
			{
				TArray<UObject*> OutObjects;
				for (size_t MeshID = 0; MeshID < Object->MeshCount(); MeshID++)
				{
					const FString ObjectPath = ParentPackage->GetName() / FString(Object->Meshes()[MeshID]->Name().c_str());
					OutObjects.Add(ImportObjectAsStaticMesh(Object, ObjectPath, MeshID));
				}
				return OutObjects.Num() ? OutObjects[0] : nullptr;
			}
			else
			{
				return ImportObjectAsStaticMesh(Object, false);
			}
			
		}
	}
	return nullptr;
}

UStaticMesh* FRBMKEngineFactory::ImportObjectAsStaticMesh(CEditableObject* Object, bool UseOnlyFullPath)
{
	FString FileName = Object->GetName();
	FileName.ReplaceCharInline(TEXT('\\'), TEXT('/'));

	FString NewPath;
	if (UseOnlyFullPath)
	{
		NewPath = UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("Maps") / TEXT("Meshes") / FPaths::GetBaseFilename(FileName, false));
	}
	else
	{
		NewPath  = UPackageTools::SanitizePackageName(ParentPackage->GetName() / FPaths::GetBaseFilename(FileName));
	}

	return ImportObjectAsStaticMesh(Object, NewPath,-1);
}

TArray<UStaticMesh*> FRBMKEngineFactory::ImportObjectAsStaticMeshWithDivideSubObject(CEditableObject* Object)
{
	FString FileName = Object->GetName();
	FileName.ReplaceCharInline(TEXT('\\'), TEXT('/'));
	const FString Path = PackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("Maps") / TEXT("Meshes") / FPaths::GetBaseFilename(FileName, false));;
	TArray<UStaticMesh*> Result;
	for (size_t MeshID = 0; MeshID < Object->MeshCount(); MeshID++)
	{
		const FString ObjectPath = Path + TEXT("_") + ANSI_TO_TCHAR(Object->Meshes()[MeshID]->Name().c_str());
		Result.Add(ImportObjectAsStaticMesh(Object, ObjectPath, MeshID));
	}
	return Result;
}


UStaticMesh* FRBMKEngineFactory::ImportObjectAsStaticMesh(CEditableObject* Object, const FString& Path,int32 InMeshID)
{
	UStaticMesh* StaticMesh = nullptr;
	if(!LoadOrCreateOrOverwriteAsset(*Path,ObjectFlags,StaticMesh))
	{
		return StaticMesh;
	}

	UE_LOG(LogXRayImporter, Log, TEXT("Import object %S"), Object->GetName());

	FAssetRegistryModule::AssetCreated(StaticMesh);
	ObjectCreated.Add(StaticMesh);
	TArray<FStaticMaterial> Materials;

	bool NeedLodForCollision = false;
	bool NeedLodForRender = false;
	for (size_t ElementID = 0; ElementID < Object->SurfaceCount(); ElementID++)
	{
		if (ShaderXRLC.Get(Object->Surfaces()[ElementID]->_ShaderXRLCName()))
		{
			if (!ShaderXRLC.Get(Object->Surfaces()[ElementID]->_ShaderXRLCName())->flags.bCollision && !ShaderXRLC.Get(Object->Surfaces()[ElementID]->_ShaderXRLCName())->flags.bRendering)
			{
				continue;
			}
			
			xr_vector< Fvector> Vertices;
			xr_vector< st_MeshVertex2> VerticesInstances;
			for (size_t MeshID = 0; MeshID < Object->MeshCount(); MeshID++)
			{
				if(InMeshID	!= -1&&InMeshID != MeshID)
				{
					continue;
				}
				Object->Meshes()[MeshID]->GenerateVertices(Vertices,VerticesInstances, Object->Surfaces()[ElementID]);
			}

			if (VerticesInstances.size() == 0)
			{
				continue;
			}
			
			NeedLodForCollision |= ShaderXRLC.Get(Object->Surfaces()[ElementID]->_ShaderXRLCName())->flags.bCollision&&!ShaderXRLC.Get(Object->Surfaces()[ElementID]->_ShaderXRLCName())->flags.bRendering;
			NeedLodForRender |= ShaderXRLC.Get(Object->Surfaces()[ElementID]->_ShaderXRLCName())->flags.bRendering;;
		}
	}

	int32 MaterialIndex = 0;
	int32 LodIndex = 0;
	TMap<int32,TArray<int32>> MaterialsIdOfLod;
	TMap<int32,TArray<bool>> CollisionEnableIdOfLod;
	TMap<int32,TArray<bool>> RenderingEnableIdOfLod;
	TMap<int32,TArray<bool>> CastShadowEnableIdOfLod;
	TMap<int32,TArray<bool>> AffectDistanceFieldLightingEnableIdOfLod;
	auto CreateLod = [&](bool OnlyCollsion)
	{
		int32 VerticesCount = 0;
		MaterialsIdOfLod.Add(LodIndex);
		CollisionEnableIdOfLod.Add(LodIndex);
		RenderingEnableIdOfLod.Add(LodIndex);
		CastShadowEnableIdOfLod.Add(LodIndex);
		AffectDistanceFieldLightingEnableIdOfLod.Add(LodIndex);
		FStaticMeshSourceModel& SourceModel = StaticMesh->AddSourceModel();
		FMeshDescription* MeshDescription = StaticMesh->CreateMeshDescription(LodIndex);
		if (MeshDescription)
		{
			FStaticMeshAttributes StaticMeshAttributes(*MeshDescription);
			TVertexAttributesRef<FVector3f> VertexPositions = StaticMeshAttributes.GetVertexPositions();
			TEdgeAttributesRef<bool>  EdgeHardnesses = StaticMeshAttributes.GetEdgeHardnesses();
			TPolygonGroupAttributesRef<FName> PolygonGroupImportedMaterialSlotNames = StaticMeshAttributes.GetPolygonGroupMaterialSlotNames();
			TVertexInstanceAttributesRef<FVector3f> VertexInstanceNormals = StaticMeshAttributes.GetVertexInstanceNormals();
			TVertexInstanceAttributesRef<FVector2f> VertexInstanceUVs = StaticMeshAttributes.GetVertexInstanceUVs();
			
			for (size_t ElementID = 0; ElementID < Object->SurfaceCount(); ElementID++)
			{
				Shader_xrLC* ShaderLC = ShaderXRLC.Get(Object->Surfaces()[ElementID]->_ShaderXRLCName());
				if (ShaderLC)
				{
					if(OnlyCollsion)
					{
						if(!ShaderLC->flags.bCollision&&!ShaderLC->flags.bRendering)
						{
							continue;
						}
					}
					else
					{
						if ( !ShaderLC->flags.bRendering)
						{
							continue;
						}
					}
				}
			
				xr_vector< st_MeshVertex2> VerticesInstances;
				xr_vector<Fvector> Vertices;
				for (size_t MeshID = 0; MeshID < Object->MeshCount(); MeshID++)
				{
					if(InMeshID	!= -1&&InMeshID != MeshID)
					{
						continue;
					}
					Object->Meshes()[MeshID]->GenerateVertices(Vertices,VerticesInstances, Object->Surfaces()[ElementID]);
				}

				if (VerticesInstances.size() == 0)
				{
					continue;
				}
				VerticesCount += VerticesInstances.size();
				if(ShaderLC)
				{
					CollisionEnableIdOfLod[LodIndex].Add(!!ShaderLC->flags.bCollision);
					RenderingEnableIdOfLod[LodIndex].Add(!!ShaderLC->flags.bRendering);
					CastShadowEnableIdOfLod[LodIndex].Add(!!ShaderLC->flags.bLIGHT_CastShadow);
					if(GetDefault<UStalkerGameSettings>()->DisableDistanceFieldOfCompilerShaderWhenImport.Contains(ShaderLC->Name))
					{
						AffectDistanceFieldLightingEnableIdOfLod[LodIndex].Add(false);
					}
					else
					{
						AffectDistanceFieldLightingEnableIdOfLod[LodIndex].Add(true);
					}
				}
				else
				{
					CollisionEnableIdOfLod[LodIndex].Add(true);
					RenderingEnableIdOfLod[LodIndex].Add(true);
					CastShadowEnableIdOfLod[LodIndex].Add(true);
					AffectDistanceFieldLightingEnableIdOfLod[LodIndex].Add(true);
				}
				TMap<int32,FVertexID> ID2VertexIDs;
				for (int32 VertexID = 0; VertexID < Vertices.size() ; VertexID++)
				{
					FVertexID VertexIDs = MeshDescription->CreateVertex();
					VertexPositions[VertexIDs] = StalkerMath::RBMKLocationToUnreal(Vertices[VertexID]);
					ID2VertexIDs.Add(VertexID,VertexIDs);
				}
				
				VertexInstanceUVs.SetNumChannels(1);
				FPolygonGroupID CurrentPolygonGroupID = MeshDescription->CreatePolygonGroup();
				PolygonGroupImportedMaterialSlotNames[CurrentPolygonGroupID] = *FString::Printf(TEXT("mat_%d"), MaterialIndex);
				TArray<FVertexInstanceID> VertexInstanceIDs;
				VertexInstanceIDs.SetNum(3);
				for (size_t FaceID = 0; FaceID < VerticesInstances.size() / 3; FaceID++)
				{
					for (size_t VirtualVertexID = 0; VirtualVertexID < 3; VirtualVertexID++)
					{
						static size_t VirtualVertices[3] = { 0,2,1 };
						size_t VertexID = VirtualVertices[VirtualVertexID] + FaceID * 3;
						
						VertexInstanceIDs[VirtualVertexID] = MeshDescription->CreateVertexInstance(ID2VertexIDs[VerticesInstances[VertexID].Index] );
						FVector2f UV;
						UV.X = VerticesInstances[VertexID].UV.x;
						UV.Y = VerticesInstances[VertexID].UV.y;
						VertexInstanceUVs.Set(VertexInstanceIDs[VirtualVertexID], 0, UV);
						VertexInstanceNormals[VertexInstanceIDs[VirtualVertexID]].X = -VerticesInstances[VertexID].Normal.x;
						VertexInstanceNormals[VertexInstanceIDs[VirtualVertexID]].Y = VerticesInstances[VertexID].Normal.z;
						VertexInstanceNormals[VertexInstanceIDs[VirtualVertexID]].Z = VerticesInstances[VertexID].Normal.y;
					}
					TArray<FEdgeID> NewEdgeIDs;
					const FPolygonID NewPolygonID = MeshDescription->CreatePolygon(CurrentPolygonGroupID, VertexInstanceIDs,&NewEdgeIDs);
					for (const FEdgeID& NewEdgeID : NewEdgeIDs)
					{
						EdgeHardnesses[NewEdgeID] = false;
					}
				}
				MaterialsIdOfLod[LodIndex].Add(MaterialIndex++);
				const FString SurfacePackageName = UPackageTools::SanitizePackageName(Path / TEXT("Materials") / FPaths::GetBaseFilename(FString(Object->Surfaces()[ElementID]->_Name())));
				Materials.AddUnique(FStaticMaterial(ImportSurface(SurfacePackageName, Object->Surfaces()[ElementID]), *FString::Printf(TEXT("mat_%d"), MaterialIndex), *FString::Printf(TEXT("mat_%d"), MaterialIndex)));
			}
		}

		StaticMesh->CommitMeshDescription(LodIndex++);
		SourceModel.BuildSettings.bRecomputeNormals = false;
		SourceModel.BuildSettings.bGenerateLightmapUVs = true;
		SourceModel.BuildSettings.DstLightmapIndex = 1;
		SourceModel.BuildSettings.MinLightmapResolution = 128;
		return VerticesCount;
	};
	int32 VerticesCount = 0;
	if(NeedLodForCollision)
	{
		VerticesCount += CreateLod(true);
	}
	if(NeedLodForRender)
	{
		VerticesCount += CreateLod(false);
	}
	if(VerticesCount == 0)
	{
		ObjectTools::DeleteSingleObject(StaticMesh);
		return nullptr;
	}
	
	StaticMesh->SetStaticMaterials(Materials);
	StaticMesh->Build();
	for(auto&[Key,Value]:MaterialsIdOfLod)
	{
		for (size_t i = 0; i < Value.Num(); i++)
		{
			FMeshSectionInfo MeshSectionInfo = StaticMesh->GetSectionInfoMap().Get(Key, i);
			MeshSectionInfo.bCastShadow = true;
			MeshSectionInfo.bEnableCollision = CollisionEnableIdOfLod[Key][i];
			if(!RenderingEnableIdOfLod[Key][i])
			{
				MeshSectionInfo.bCastShadow = false;
				MeshSectionInfo.bAffectDistanceFieldLighting = false;
				MeshSectionInfo.bVisibleInRayTracing = false;
			}
			else
			{
				MeshSectionInfo.bCastShadow = CastShadowEnableIdOfLod[Key][i];
				MeshSectionInfo.bAffectDistanceFieldLighting = AffectDistanceFieldLightingEnableIdOfLod[Key][i];
			}
			MeshSectionInfo.MaterialIndex = Value[i];
			StaticMesh->GetSectionInfoMap().Set(Key, i, MeshSectionInfo);
		}
	}

	StaticMesh->GetBodySetup()->CollisionTraceFlag = ECollisionTraceFlag::CTF_UseComplexAsSimple;
	if(NeedLodForCollision&&NeedLodForRender)
	{
		StaticMesh->SetMinLOD(1);
	}
	StaticMesh->Modify();
	StaticMesh->PostEditChange();
	UStalkerStaticMeshAssetUserData * StalkerStaticMeshAssetUserData  = NewObject<UStalkerStaticMeshAssetUserData>(StaticMesh);
	StalkerStaticMeshAssetUserData->IsOnlyCollision = NeedLodForCollision&&!NeedLodForRender;
	StaticMesh->AddAssetUserData(StalkerStaticMeshAssetUserData);
	return StaticMesh;
}

USkeletalMesh* FRBMKEngineFactory::ImportObjectAsDynamicMesh(CEditableObject* Object, bool UseOnlyFullPath /*= false*/)
{
	FString FileName = Object->GetName();
	FileName.ReplaceCharInline(TEXT('\\'), TEXT('/'));

	FString NewPath;

	if (UseOnlyFullPath)
	{
		NewPath = UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("Meshes") / FPaths::GetBaseFilename(FileName, false));
	}
	else
	{
		NewPath = UPackageTools::SanitizePackageName(ParentPackage->GetName() / FPaths::GetBaseFilename(FileName));
	}

	USkeletalMesh* SkeletalMesh = nullptr;
	if(!LoadOrCreateOrOverwriteAsset<USkeletalMesh>(NewPath,ObjectFlags,SkeletalMesh))
	{
		return SkeletalMesh;
	}

	TArray<SkeletalMeshImportData::FBone> Bones;
	Bones.AddDefaulted(Object->Bones().size());
	for (int32 BoneIndex = 0; BoneIndex < Bones.Num(); BoneIndex++)
	{
		SkeletalMeshImportData::FBone& Bone = Bones[BoneIndex];
		Bone.Flags = 0;
		Bone.ParentIndex = INDEX_NONE;
		Bone.NumChildren = 0;
	}
	TArray<CBone*> InBones;
	for (CBone* Bone : Object->Bones())
	{
		FindBoneIDOrAdd(InBones, Bone);
	}
	check(InBones.Num());
	check(InBones[0]->IsRoot());

	;
	for (int32 BoneIndex = 0; BoneIndex < Bones.Num(); BoneIndex++)
	{
		SkeletalMeshImportData::FBone& Bone = Bones[BoneIndex];
		Bone.Flags = 0;
		Bone.ParentIndex = INDEX_NONE;
		Bone.NumChildren = 0;
		if (BoneIndex != 0)
		{
			Bone.ParentIndex = FindBoneIDOrAdd(InBones, InBones[BoneIndex]->Parent());
			Bones[Bone.ParentIndex].NumChildren++;
		}
		Fmatrix RotationMatrix;
		RotationMatrix.identity();
		RotationMatrix.setXYZi(InBones[BoneIndex]->_RestRotate());
		Fquaternion InQuat;
		InQuat.set(RotationMatrix);
		FQuat4f Quat(InQuat.x, -InQuat.z, -InQuat.y, InQuat.w);
		Fvector InLocation = InBones[BoneIndex]->_RestOffset();
		FVector3f Location(-InLocation.x * 100, InLocation.z * 100, InLocation.y * 100);
		Bone.BonePos.Transform.SetRotation(Quat);
		Bone.BonePos.Transform.SetLocation(Location);
		Bone.Name = InBones[BoneIndex]->Name().c_str();

	}

	TArray< FSkeletalMeshImportData> SkeletalMeshImportDataList;
	TArray<FSkeletalMaterial> InMaterials;
	SkeletalMeshImportDataList.AddDefaulted(1);
	int32 MaterialIndex = 0;
	for (int32 LodIndex = 0; LodIndex < 1; LodIndex++)
	{
		FSkeletalMeshImportData& InSkeletalMeshImportData = SkeletalMeshImportDataList[LodIndex];
		InSkeletalMeshImportData.NumTexCoords = 1;
		InSkeletalMeshImportData.bHasNormals = true;
		InSkeletalMeshImportData.RefBonesBinary = Bones;

		for (size_t ElementID = 0; ElementID < Object->SurfaceCount(); ElementID++)
		{
			xr_vector< Fvector> Vertices;
			xr_vector< st_MeshVertex2> VerticesInstances;
			for (size_t MeshID = 0; MeshID < Object->MeshCount(); MeshID++)
			{
				Object->Meshes()[MeshID]->GenerateVertices(Vertices,VerticesInstances, Object->Surfaces()[ElementID]);
			}
			if (Vertices.size() == 0)
			{
				continue;
			}
			const FString SurfacePackageName = UPackageTools::SanitizePackageName(NewPath / TEXT("Materials") / FPaths::GetBaseFilename(FString(Object->Surfaces()[ElementID]->_Name())));


			SkeletalMeshImportData::FMaterial Material;
			Material.Material = ImportSurface(SurfacePackageName, Object->Surfaces()[ElementID]);
			Material.MaterialImportName = Object->Surfaces()[ElementID]->_Name();
			InMaterials.Add(FSkeletalMaterial(Material.Material.Get(), true, false, FName(Material.MaterialImportName), FName(Material.MaterialImportName)));
			int32 MaterialID = InSkeletalMeshImportData.Materials.Add(Material);
			
			for (const Fvector& Vertex : Vertices)
			{
				InSkeletalMeshImportData.Points.Add(StalkerMath::RBMKLocationToUnreal(Vertex));
			}
			
			for (size_t FaceID = 0; FaceID < VerticesInstances.size() / 3; FaceID++)
			{
				SkeletalMeshImportData::FTriangle Triangle;
				Triangle.MatIndex = MaterialID;
				for (size_t VirtualVertexID = 0; VirtualVertexID < 3; VirtualVertexID++)
				{
					SkeletalMeshImportData::FVertex Wedge;
					static size_t VirtualVertices[3] = { 0,2,1 };
					size_t VertexID = VirtualVertices[VirtualVertexID] + FaceID * 3;
					int32 OutVertexID = VerticesInstances[VertexID].Index;

					for (size_t InfluencesID = 0; InfluencesID < 4; InfluencesID++)
					{
						if (VerticesInstances[VertexID].BoneID[InfluencesID] == 0xFFFF)
						{
							break;
						}
						SkeletalMeshImportData::FRawBoneInfluence BoneInfluence;
						BoneInfluence.BoneIndex = FindBoneIDOrAdd(InBones, Object->Bones()[VerticesInstances[VertexID].BoneID[InfluencesID]]);
						BoneInfluence.Weight = VerticesInstances[VertexID].BoneWeight[InfluencesID];
						BoneInfluence.VertexIndex = OutVertexID;
						InSkeletalMeshImportData.Influences.Add(BoneInfluence);
					}
					Wedge.VertexIndex = OutVertexID;
					Wedge.MatIndex = MaterialID;
					Wedge.UVs[0] = FVector2f(VerticesInstances[VertexID].UV.x, VerticesInstances[VertexID].UV.y);
					Triangle.WedgeIndex[VirtualVertexID] = InSkeletalMeshImportData.Wedges.Add(Wedge);
					Triangle.TangentZ[VirtualVertexID].X = -VerticesInstances[VertexID].Normal.x;
					Triangle.TangentZ[VirtualVertexID].Y = VerticesInstances[VertexID].Normal.z;
					Triangle.TangentZ[VirtualVertexID].Z = VerticesInstances[VertexID].Normal.y;
				}
				InSkeletalMeshImportData.Faces.Add(Triangle);;
			}

		}

		InSkeletalMeshImportData.MaxMaterialIndex = MaterialIndex;
	}
	//FSkeleto

	if (!CreateSkeletalMesh(SkeletalMesh, SkeletalMeshImportDataList, Bones))
	{
		SkeletalMesh->MarkAsGarbage();
		return nullptr;
	}
	FAssetRegistryModule::AssetCreated(SkeletalMesh);
	ObjectCreated.Add(SkeletalMesh);

	SkeletalMesh->SetMaterials(InMaterials);
	SkeletalMesh->PostEditChange();

	USkeleton* Skeleton = FindOrCreateSkeleton(NewPath + "_Skeleton", SkeletalMesh);

	Skeleton->SetPreviewMesh(SkeletalMesh);
	SkeletalMesh->SetSkeleton(Skeleton);

	SkeletalMesh->PostEditChange();
	TArray<IBoneData*>  BonesPtr;
	for (CBone* Bone : InBones)
	{
		BonesPtr.Add(Bone);
	}
	CreatePhysicsAsset(NewPath + "_PhysicsAsset", SkeletalMesh, BonesPtr);
	CreateAnims(NewPath / FPaths::GetBaseFilename(NewPath) / TEXT("Anims"), Skeleton, Object);
	return SkeletalMesh;
}

UMaterialInterface* FRBMKEngineFactory::ImportSurface(const FString& Path, CSurface* Surface)
{
	return ImportSurface(Path, Surface->_ShaderName(), Surface->_Texture(),Surface->_GameMtlName());
}

UMaterialInterface* FRBMKEngineFactory::ImportSurface(const FString& Path, shared_str ShaderName, shared_str TextureName, shared_str GameMaterial,bool HudMode )
{
	if(!ShadersManager)
	{
		ShadersManager = MakeUnique<FRBMKShadersManager>(this);
	}
	return ShadersManager->ImportSurface(Path,ShaderName.c_str(),TextureName.c_str(),GameMaterial.c_str(),HudMode);
}


UTexture2D* FRBMKEngineFactory::ImportTextureTHM(const FString& InFileName)
{
	string_path TexturesGamedataPath;
	FCStringAnsi::Strcpy(TexturesGamedataPath, FS.get_path(_game_textures_)->m_Path);
	FString FileName = InFileName;
	for (int32 i = 0; TexturesGamedataPath[i]; i++)
	{
		if (TexturesGamedataPath[i] == '\\')
		{
			TexturesGamedataPath[i] = '/';
		}
	}
	if (!FileName.StartsWith(TexturesGamedataPath, ESearchCase::IgnoreCase))
	{
		return nullptr;
	}
	FileName.RightChopInline(FCStringAnsi::Strlen(TexturesGamedataPath));
	while (FileName[0] && FileName[0] == TEXT('/'))
	{
		FileName.RemoveAt(0);
	}

	ETextureThumbnail THM(TCHAR_TO_ANSI(*FileName));
	if (THM.Load(TCHAR_TO_ANSI(*FileName)))
	{
		TObjectPtr<UTexture2D> BaseTexture = ImportTexture(FPaths::GetBaseFilename(FileName, false));
		if (THM._Format().bump_mode == STextureParams::tbmUse || THM._Format().bump_mode == STextureParams::tbmUseParallax)
		{
			TObjectPtr<UTexture2D> NormalMapTexture;
			TObjectPtr<UTexture2D> HeightGlossTexture;
			ImportBump2D(THM._Format().bump_name.c_str(), NormalMapTexture, HeightGlossTexture);
		}
		if (THM._Format().detail_name.size())
		{
			TObjectPtr<UTexture2D> DetailTexture = ImportTexture(THM._Format().detail_name.c_str());
			TObjectPtr<UTexture2D> NormalMapTextureDetail;
			TObjectPtr<UTexture2D> HeightGlossTextureDetail;

			ETextureThumbnail THMDetail(THM._Format().detail_name.c_str());
			if (THMDetail.Load(THM._Format().detail_name.c_str()))
			{

				if (THMDetail._Format().bump_mode == STextureParams::tbmUse || THMDetail._Format().bump_mode == STextureParams::tbmUseParallax)
				{

					ImportBump2D(THMDetail._Format().bump_name.c_str(), NormalMapTextureDetail,HeightGlossTextureDetail);
				}
			}
		}
		return BaseTexture;
	}
	return nullptr;
}

UObject* FRBMKEngineFactory::ImportPhysicsMaterials(const FString& FileName)
{
	CGameMtlLibrary Library;
	Library.Load(TCHAR_TO_ANSI(*FileName));

	UStalkerPhysicalMaterial* Result = nullptr;

	TMap<int32, UStalkerPhysicalMaterial*> ID2PhysicalMaterial;
	{
		u32 CountMaterials = Library.CountMaterial();
		for (u16 i = 0; i < CountMaterials; i++)
		{
			Result = ImportPhysicsMaterial(Library.GetMaterialByIdx(i));
			ID2PhysicalMaterial.Add(Library.GetMaterialByIdx(i)->GetID(), Result);
		}
	}
	{
		const FString Path = UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("PhysicalMaterials") /TEXT("MaterialPairs"));

		UStalkerPhysicalMaterialPairsData* PhysicalMaterialPairs = nullptr;
		if(LoadOrCreateOrOverwriteAsset(Path,ObjectFlags,PhysicalMaterialPairs))
		{
			FAssetRegistryModule::AssetCreated(PhysicalMaterialPairs);
			ObjectCreated.Add(PhysicalMaterialPairs);
		}


		PhysicalMaterialPairs->Pairs.Empty();

		for (auto i = Library.FirstMaterialPair(); i != Library.LastMaterialPair(); i++)
		{
			FStalkerPhysicalMaterialPair&Pair =  PhysicalMaterialPairs->Pairs.FindOrAdd(ID2PhysicalMaterial[(*i)->GetMtl0()]).Materials.Add(ID2PhysicalMaterial[(*i)->GetMtl1()]);
			Pair.BuildFromLegacy(*(*i));
		}
		PhysicalMaterialPairs->Modify();
		PhysicalMaterialPairs->PostEditChange();
	}

	Library.Unload();
	return ObjectCreated.Num()? ObjectCreated.Last(): nullptr;
}



UStalkerPhysicalMaterial* FRBMKEngineFactory::ImportPhysicsMaterial(class SGameMtl*Materials)
{
	FString Name =  Materials->m_Name.c_str();
	Name.ReplaceCharInline(TEXT('\\'), TEXT('/'));
	const FString Path = UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("PhysicalMaterials")/ TEXT("Materials") / Name);

	UStalkerPhysicalMaterial* PhysicalMaterial = nullptr;
	if(!LoadOrCreateOrOverwriteAsset(Path,ObjectFlags,PhysicalMaterial))
	{
		return PhysicalMaterial;
	}

	FAssetRegistryModule::AssetCreated(PhysicalMaterial);
	ObjectCreated.Add(PhysicalMaterial);

	PhysicalMaterial->BuildFromLegacy(*Materials);
	PhysicalMaterial->Modify();
	PhysicalMaterial->PostEditChange();
	return PhysicalMaterial;

}

UTexture2D* FRBMKEngineFactory::ImportTextureDDS(const FString& InFileName)
{
	const FString NewPath = UPackageTools::SanitizePackageName(ParentPackage->GetName() / FPaths::GetBaseFilename(InFileName));

	TObjectPtr<UTexture2D> BaseTexture = ImportTexture(InFileName, NewPath);

	return BaseTexture;
}

UTexture2D* FRBMKEngineFactory::ImportTexture(const FString& FileName,bool bIsUI)
{
	 
	FString PackageFileName = FPaths::ChangeExtension(FileName, TEXT(""));
	PackageFileName.ReplaceCharInline(TEXT('\\'), TEXT('/'));
	const FString Path = UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("Textures") / PackageFileName);
	
	UTexture2D*   Texture2D = nullptr;
	if(!LoadOrCreateOrOverwriteAsset(Path,ObjectFlags,Texture2D))
	{
		return Texture2D;
	}
	
	FAssetRegistryModule::AssetCreated(Texture2D);
	ObjectCreated.Add(Texture2D);

	RedImageTool::RedImage Image;
	string_path DDSFileName;
	FS.update_path(DDSFileName, _game_textures_, ChangeFileExt(TCHAR_TO_ANSI(*FileName), ".dds").c_str());
	if (!ensure(Image.LoadFromFile(DDSFileName)))
	{
		Image.Create(1,1);
		Image.Fill(RedImageTool::RedColor(0,0,0));
	}

	Image.Convert(RedImageTool::RedTexturePixelFormat::R8G8B8A8);
	const size_t CountPixel = Image.GetSizeInMemory()/4;
	bool UseAlpha = false;
	for (size_t x = 0; x < CountPixel; x++)
	{
		uint8*Pixel = static_cast<uint8*>(*Image) + x*4;
		Swap(Pixel[0], Pixel[2]);
		UseAlpha = UseAlpha || Pixel[3] <255;
	}
	constexpr ETextureSourceFormat SourceFormat = ETextureSourceFormat::TSF_BGRA8;
	Texture2D->Source.Init(Image.GetWidth(), Image.GetHeight(), 1, Image.GetMips(), SourceFormat, static_cast<uint8*>(*Image));
	Texture2D->LODGroup = TEXTUREGROUP_World;
	Texture2D->SRGB = false;
	if(bIsUI)
	{
		Texture2D->LODGroup = TEXTUREGROUP_UI;
		Texture2D->NeverStream = false;
		Texture2D->SRGB = true;
	}
	Texture2D->CompressionNoAlpha = !UseAlpha;
	Texture2D->Modify();
	Texture2D->PostEditChange();

	return Texture2D;
}

UTexture2D* FRBMKEngineFactory::ImportTexture(const FString& FileName, const FString& NewPath)
{
	UTexture2D* Texture2D = nullptr;
	if(!LoadOrCreateOrOverwriteAsset(NewPath,ObjectFlags,Texture2D))
	{
		return Texture2D;
	}

	RedImageTool::RedImage Image;
	if (!ensure(Image.LoadFromFile(TCHAR_TO_ANSI(*FileName))))
	{
		Image.Create(1,1);
		Image.Fill(RedImageTool::RedColor(0,0,0));
	}

	FAssetRegistryModule::AssetCreated(Texture2D);
	ObjectCreated.Add(Texture2D);
	Image.Convert(RedImageTool::RedTexturePixelFormat::R8G8B8A8);
	size_t CountPixel = Image.GetSizeInMemory() / 4;
	for (size_t x = 0; x < CountPixel; x++)
	{
		uint8* Pixel = ((uint8*)*Image) + x * 4;
		Swap(Pixel[0], Pixel[2]);

	}
	ETextureSourceFormat SourceFormat = ETextureSourceFormat::TSF_BGRA8;
	Texture2D->SRGB = false;
	Texture2D->Source.Init(Image.GetWidth(), Image.GetHeight(), 1, Image.GetMips(), SourceFormat, (uint8*)*Image);
	Texture2D->LODGroup = TEXTUREGROUP_World;
	Texture2D->Modify();
	Texture2D->PostEditChange();
	return Texture2D;
}

void FRBMKEngineFactory::ImportTextures(const TArray<FString>& Prefixes,const TArray<FString>& IgnorePrefixes)
{
	if(!ShadersManager)
	{
		ShadersManager = MakeUnique<FRBMKShadersManager>(this);
	}
	ShadersManager->ImportTextures(Prefixes,IgnorePrefixes);
}

void FRBMKEngineFactory::ImportBump2D(const FString& FileName, TObjectPtr<UTexture2D>& OutNormalMap, TObjectPtr<UTexture2D>& OutHeightGloss)
{
	FString NewFileName = FPaths::ChangeExtension(FileName, TEXT(""));
	NewFileName.ReplaceCharInline(TEXT('\\'), TEXT('/'));
	const FString NormalMapPath = UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath()/TEXT("Textures") / NewFileName);

	const FString HeightGlossPath = UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("Textures") / NewFileName+TEXT("_height_gloss"));

	bool NeedFillNormalMap = false;
	UTexture2D*NormalMap = nullptr;
	if(LoadOrCreateOrOverwriteAsset(NormalMapPath,ObjectFlags,NormalMap))
	{
		NeedFillNormalMap = true;
	}

	bool NeedFillHeightGloss = false;
	UTexture2D*HeightGloss = nullptr;
	if(LoadOrCreateOrOverwriteAsset(HeightGlossPath,ObjectFlags,HeightGloss))
	{
		NeedFillHeightGloss = true;
	}
	OutNormalMap = NormalMap;
	OutHeightGloss = HeightGloss;

	if(!NeedFillNormalMap&&!NeedFillHeightGloss)
	{
		return;
	}
	RedImageTool::RedImage ImageBump, ImageBumpError;
	string_path DDSFileName;
	FS.update_path(DDSFileName, _game_textures_, ChangeFileExt(TCHAR_TO_ANSI(*FileName), ".dds").c_str());
	if (!ensure(ImageBump.LoadFromFile(DDSFileName)))
	{
		ImageBump.Create(1,1);
		ImageBump.Fill(RedImageTool::RedColor(0,1,0,0));
		return;
	}

	FS.update_path(DDSFileName, _game_textures_, ChangeFileExt(TCHAR_TO_ANSI(*FileName), "#.dds").c_str());
	if (!ensure(ImageBumpError.LoadFromFile(DDSFileName)))
	{
		ImageBump.Create(1,1);
		ImageBump.Fill(RedImageTool::RedColor(0.5f,0.5f,0.5f,0.5f));
		return;
	}
	if (ImageBump.GetWidth() != ImageBumpError.GetWidth())return;
	if (ImageBump.GetHeight() != ImageBumpError.GetHeight())return;

	RedImageTool::RedImage NormalMapImage;
	RedImageTool::RedImage HeightGlossImage;

	ImageBump.ClearMipLevels();
	ImageBump.Convert(RedImageTool::RedTexturePixelFormat::R8G8B8A8);
	ImageBumpError.ClearMipLevels();
	ImageBumpError.Convert(RedImageTool::RedTexturePixelFormat::R8G8B8A8);

	NormalMapImage.Create(ImageBump.GetWidth(), ImageBump.GetHeight());
	HeightGlossImage.Create(ImageBump.GetWidth(), ImageBump.GetHeight(),1,1,RedImageTool::RedTexturePixelFormat::R8G8B8A8);

	for (size_t x = 0; x < ImageBump.GetWidth(); x++)
	{
		for (size_t y = 0; y < ImageBump.GetHeight(); y++)
		{
			RedImageTool::RedColor Pixel = ImageBump.GetPixel(x, y);
			RedImageTool::RedColor PixelError = ImageBumpError.GetPixel(x, y);
			RedImageTool::RedColor  NormalMapPixel, HeightGlossPixel;

			NormalMapPixel.R32F = Pixel.A32F + (PixelError.R32F - 0.5f);
			NormalMapPixel.G32F = Pixel.B32F + (PixelError.G32F - 0.5f);
			NormalMapPixel.B32F = Pixel.G32F + (PixelError.B32F - 0.5f);
			NormalMapPixel.A32F = 1.f;
			NormalMapPixel.SetAsFloat(NormalMapPixel.R32F, NormalMapPixel.G32F, NormalMapPixel.B32F, NormalMapPixel.A32F);
			HeightGlossPixel.SetAsFloat(PixelError.A32F, Pixel.R32F,0,0);
			NormalMapImage.SetPixel(NormalMapPixel, x, y);
			HeightGlossImage.SetPixel(HeightGlossPixel, x, y);
		}
	}
	float MaxHeight = 0;
	for (size_t x = 0; x < ImageBump.GetWidth(); x++)
	{
		for (size_t y = 0; y < ImageBump.GetHeight(); y++)
		{
			RedImageTool::RedColor Pixel = HeightGlossImage.GetPixel(x, y);
			MaxHeight = std::max(Pixel.R32F, MaxHeight);
		}
	}
	for (size_t x = 0; x < ImageBump.GetWidth(); x++)
	{
		for (size_t y = 0; y < ImageBump.GetHeight(); y++)
		{
			RedImageTool::RedColor Pixel = HeightGlossImage.GetPixel(x, y);
			Pixel.R32F = Pixel.R32F/ MaxHeight;
			Pixel.SetAsFloat(Pixel.R32F, Pixel.G32F, Pixel.B32F, Pixel.A32F);
			HeightGlossImage.SetPixel(Pixel,x,y);
		}
	}
	NormalMapImage.SwapRB();
	HeightGlossImage.SwapRB();
	//HeightImage.SwapRB();
	if (NeedFillNormalMap)
	{
		FAssetRegistryModule::AssetCreated(NormalMap);
		ObjectCreated.Add(NormalMap);
		ETextureSourceFormat SourceFormat = ETextureSourceFormat::TSF_BGRA8;
		NormalMapImage.GenerateMipmap();
		NormalMap->CompressionSettings = TextureCompressionSettings::TC_Normalmap;
		NormalMap->LODGroup = TEXTUREGROUP_WorldNormalMap;
		NormalMap->SRGB = false;
		NormalMap->Source.Init(NormalMapImage.GetWidth(), NormalMapImage.GetHeight(), 1, NormalMapImage.GetMips(), SourceFormat, (uint8*)*NormalMapImage);
		NormalMap->Modify();
		NormalMap->PostEditChange();
	}
	if (NeedFillHeightGloss)
	{
		FAssetRegistryModule::AssetCreated(HeightGloss);
		ObjectCreated.Add(HeightGloss);
		ETextureSourceFormat SourceFormat = ETextureSourceFormat::TSF_BGRA8;
		HeightGlossImage.GenerateMipmap();
		HeightGloss->CompressionSettings = TextureCompressionSettings::TC_Masks;
		HeightGloss->LODGroup = TEXTUREGROUP_WorldSpecular;
		HeightGloss->SRGB = false;
		HeightGloss->Source.Init(HeightGlossImage.GetWidth(), HeightGlossImage.GetHeight(), 1, HeightGlossImage.GetMips(), SourceFormat, (uint8*)*HeightGlossImage);
		HeightGloss->Modify();
		HeightGloss->PostEditChange();
	}
}


USoundWave* FRBMKEngineFactory::ImportSound(const FString& FileName)
{
	FString PackageFileName = FPaths::ChangeExtension(FileName, TEXT(""));
	PackageFileName.ReplaceCharInline(TEXT('\\'), TEXT('/'));
	const FString Path = UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("Sounds") / PackageFileName);


	USoundWave* SoundWave = nullptr;
	if(LoadOrCreateOrOverwriteAsset(Path,ObjectFlags,SoundWave))
	{
		string_path OGGFileName;
		FS.update_path(OGGFileName, _game_sounds_, ChangeFileExt(TCHAR_TO_ANSI(*FileName), ".ogg").c_str());

		FArrayReader OGGReader;
		if(!ensure(FFileHelper::LoadFileToArray(OGGReader,ANSI_TO_TCHAR(OGGFileName))))
		{
			return nullptr;
		}
		TArray<uint16> PCMData;
		int32 SampleRate,NumChannels;
		FRbmkOGGComment OGGComment;
		ReadOGGSound(OGGReader,PCMData,SampleRate,NumChannels,OGGComment);

		TArray<uint8> RawWaveData;
		SerializeWaveFile(RawWaveData, reinterpret_cast<uint8*>(PCMData.GetData()), PCMData.Num()*2, NumChannels, SampleRate);

		FSharedBuffer Buffer = FSharedBuffer::Clone(RawWaveData.GetData(), RawWaveData.Num());

		SoundWave->RawData.UpdatePayload(Buffer);

		SoundWave->RawPCMDataSize = PCMData.Num()*2;
		SoundWave->RawPCMData = static_cast<uint8*>(FMemory::Malloc(SoundWave->RawPCMDataSize));
		FMemory::Memcpy(SoundWave->RawPCMData, PCMData.GetData(), SoundWave->RawPCMDataSize);

		SoundWave->Duration = static_cast<float>(PCMData.Num())/static_cast<float>(NumChannels*SampleRate);
		SoundWave->SetSampleRate(SampleRate);
		SoundWave->NumChannels = NumChannels;
		SoundWave->Volume = OGGComment.BaseVolume;
		SoundWave->SetSoundAssetCompressionType(ESoundAssetCompressionType::BinkAudio);
		const bool bRebuildStreamingChunks = FPlatformCompressionUtilities::IsCurrentPlatformUsingStreamCaching();
		SoundWave->InvalidateCompressedData(true /* bFreeResources */, bRebuildStreamingChunks);
		
		const UStalkerGameSettings* SGSettings = GetDefault<UStalkerGameSettings>(); 
		for(auto&[Key,Value]:SGSettings->SoundClassByPrefixWhenImport)
		{
			FString CheckPath = Key.Replace(TEXT("\\"), TEXT("/"));
			if(PackageFileName.StartsWith(CheckPath))
			{
				SoundWave->SoundClassObject = Value.LoadSynchronous();
			}
		}
		if(!SoundWave->SoundClassObject)
		{
			SoundWave->SoundClassObject = SGSettings->DefaultSoundClass.LoadSynchronous();
		}
		// If stream caching is enabled, we need to make sure this asset is ready for playback.
		if (bRebuildStreamingChunks && SoundWave->IsStreaming(nullptr))
		{
			SoundWave->LoadZerothChunk();
		}

		SoundWave->PostImport();
		USoundAttenuation* SoundAttenuation = nullptr;
		if(USoundAttenuation** Item = SoundAttenuationsCached.Find(OGGComment))
		{
			SoundAttenuation = *Item;
		}
		else if(LoadOrCreateOrOverwriteAsset(Path + TEXT("_ATT"), ObjectFlags, SoundAttenuation))
		{
			SoundAttenuation->Attenuation.DistanceAlgorithm = EAttenuationDistanceModel::NaturalSound;
			SoundAttenuation->Attenuation.FalloffDistance = OGGComment.MaxDistance*100.f - OGGComment.MinDistance*100.f;
			SoundAttenuation->Attenuation.AttenuationShapeExtents.X = OGGComment.MinDistance*100.f;
			SoundAttenuation->Modify();
			SoundAttenuationsCached.Add(OGGComment,SoundAttenuation);
			ObjectCreated.Add(SoundAttenuation);
			FAssetRegistryModule::AssetCreated(SoundAttenuation);
			if(EnumHasAllFlags(static_cast<EStalkerSoundTypes>(OGGComment.Flags), EStalkerSoundTypes::Ambient|EStalkerSoundTypes::World))
			{
				SoundAttenuation->Attenuation.bEnableOcclusion = true;
				SoundAttenuation->Attenuation.OcclusionLowPassFilterFrequency = 1000;
				SoundAttenuation->Attenuation.OcclusionTraceChannel = ECC_WorldStatic;
				SoundAttenuation->Attenuation.OcclusionInterpolationTime = 3;
				SoundAttenuation->Attenuation.OcclusionVolumeAttenuation = 0.4f;
			}
		}
		SoundWave->AttenuationSettings = SoundAttenuation;
		UStalkerSoundWaveAssetUserData*UserData =	NewObject<UStalkerSoundWaveAssetUserData>(SoundWave);
		UserData->Flags = OGGComment.Flags;
		UserData->AIRatioToDistance = OGGComment.MaxAIDistance/OGGComment.MaxDistance;
		UserData->AIRatioToDistance = FMath::Clamp(UserData->AIRatioToDistance,0,1);
		if(IsValid(SoundWave))
		{
			UserData->LoopingSound = CreateSoundLoopCue(SoundWave,Path);
		}
		SoundWave->AddAssetUserData(UserData);
		SoundWave->Modify();

		ObjectCreated.Add(SoundWave);
		FAssetRegistryModule::AssetCreated(SoundWave);
	}
	return SoundWave;
}

USoundWave* FRBMKEngineFactory::ImportSoundWithCombineLR(const FString& FileName)
{
	FString PackageFileName = FPaths::ChangeExtension(FileName, TEXT(""));
	PackageFileName.ReplaceCharInline(TEXT('\\'), TEXT('/'));
	const FString Path = UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("Sounds") / PackageFileName);
	
	USoundWave* SoundWave = nullptr;
	if(LoadOrCreateOrOverwriteAsset(Path,ObjectFlags,SoundWave))
	{
		TArray<uint16> PCMData;
		int32 SampleRate,NumChannels = 2;
		FRbmkOGGComment OGGComment;
		{
			TArray<uint16> InPCMData;
			string_path OGGFileName;
			FS.update_path(OGGFileName, _game_sounds_, ChangeFileExt(TCHAR_TO_ANSI(*FileName), "_l.ogg").c_str());

			FArrayReader OGGReader;
			if(!ensure(FFileHelper::LoadFileToArray(OGGReader,ANSI_TO_TCHAR(OGGFileName))))
			{
				return nullptr;
			}
			int32 InNumChannels;
			ReadOGGSound(OGGReader,InPCMData,SampleRate,InNumChannels,OGGComment);
			if(!ensure(InNumChannels==1))
			{
				return nullptr;
			}
			PCMData.AddUninitialized(InPCMData.Num()*2);
			for(int32 i = 0; i<InPCMData.Num();i++)
			{
				PCMData[i*2] = InPCMData[i];
			}
		}
		{
			TArray<uint16> InPCMData;
			string_path OGGFileName;
			FS.update_path(OGGFileName, _game_sounds_, ChangeFileExt(TCHAR_TO_ANSI(*FileName), "_r.ogg").c_str());

			FArrayReader OGGReader;
			if(!ensure(FFileHelper::LoadFileToArray(OGGReader,ANSI_TO_TCHAR(OGGFileName))))
			{
				return nullptr;
			}
			int32 InNumChannels;
			int32 InSampleRate;
			ReadOGGSound(OGGReader,InPCMData,InSampleRate,InNumChannels,OGGComment);
			if(!ensure(InNumChannels==1))
			{
				return nullptr;
			}
			if(!ensure(SampleRate==InSampleRate))
			{
				return nullptr;
			}
			for(int32 i = 0; i<InPCMData.Num();i++)
			{
				PCMData[i*2 + 1] = InPCMData[i];
			}
		}

		TArray<uint8> RawWaveData;
		SerializeWaveFile(RawWaveData, reinterpret_cast<uint8*>(PCMData.GetData()), PCMData.Num()*2, NumChannels, SampleRate);

		FSharedBuffer Buffer = FSharedBuffer::Clone(RawWaveData.GetData(), RawWaveData.Num());

		SoundWave->RawData.UpdatePayload(Buffer);

		SoundWave->RawPCMDataSize = PCMData.Num()*2;
		SoundWave->RawPCMData = static_cast<uint8*>(FMemory::Malloc(SoundWave->RawPCMDataSize));
		FMemory::Memcpy(SoundWave->RawPCMData, PCMData.GetData(), SoundWave->RawPCMDataSize);

		SoundWave->Duration = static_cast<float>(PCMData.Num())/static_cast<float>(NumChannels*SampleRate);
		SoundWave->SetSampleRate(SampleRate);
		SoundWave->NumChannels = NumChannels;
		SoundWave->Volume = OGGComment.BaseVolume;
		SoundWave->SetSoundAssetCompressionType(ESoundAssetCompressionType::BinkAudio);
		const bool bRebuildStreamingChunks = FPlatformCompressionUtilities::IsCurrentPlatformUsingStreamCaching();
		SoundWave->InvalidateCompressedData(true /* bFreeResources */, bRebuildStreamingChunks);

		const UStalkerGameSettings* SGSettings = GetDefault<UStalkerGameSettings>(); 
		for(auto&[Key,Value]:SGSettings->SoundClassByPrefixWhenImport)
		{
			FString CheckPath = Key.Replace(TEXT("\\"), TEXT("/"));
			if(PackageFileName.StartsWith(CheckPath))
			{
				SoundWave->SoundClassObject = Value.LoadSynchronous();
			}
		}
		if(!SoundWave->SoundClassObject)
		{
			SoundWave->SoundClassObject = SGSettings->DefaultSoundClass.LoadSynchronous();
		}
		// If stream caching is enabled, we need to make sure this asset is ready for playback.
		if (bRebuildStreamingChunks && SoundWave->IsStreaming(nullptr))
		{
			SoundWave->LoadZerothChunk();
		}

		SoundWave->PostImport();
		USoundAttenuation* SoundAttenuation = nullptr;
		if(USoundAttenuation** Item = SoundAttenuationsCached.Find(OGGComment))
		{
			SoundAttenuation = *Item;
		}
		else if(LoadOrCreateOrOverwriteAsset(Path + TEXT("_ATT"), ObjectFlags, SoundAttenuation))
		{
			SoundAttenuation->Attenuation.DistanceAlgorithm = EAttenuationDistanceModel::NaturalSound;
			SoundAttenuation->Attenuation.FalloffDistance = OGGComment.MaxDistance*100.f - OGGComment.MinDistance*100.f;
			SoundAttenuation->Attenuation.AttenuationShapeExtents.X = OGGComment.MinDistance*100.f;
			SoundAttenuation->Modify();
			ObjectCreated.Add(SoundAttenuation);
			FAssetRegistryModule::AssetCreated(SoundAttenuation);
			SoundAttenuationsCached.Add(OGGComment,SoundAttenuation);
			if(EnumHasAllFlags(static_cast<EStalkerSoundTypes>(OGGComment.Flags), EStalkerSoundTypes::Ambient|EStalkerSoundTypes::World))
			{
				SoundAttenuation->Attenuation.bEnableOcclusion = true;
				SoundAttenuation->Attenuation.OcclusionLowPassFilterFrequency = 1000;
				SoundAttenuation->Attenuation.OcclusionTraceChannel = ECC_WorldStatic;
				SoundAttenuation->Attenuation.OcclusionInterpolationTime = 3;
				SoundAttenuation->Attenuation.OcclusionVolumeAttenuation = 0.4f;
			}
		}
		SoundWave->AttenuationSettings = SoundAttenuation;
		UStalkerSoundWaveAssetUserData*UserData =	NewObject<UStalkerSoundWaveAssetUserData>(SoundWave);
		UserData->Flags = OGGComment.Flags;
		UserData->AIRatioToDistance = OGGComment.MaxAIDistance/OGGComment.MaxDistance;
		UserData->AIRatioToDistance = FMath::Clamp(UserData->AIRatioToDistance,0,1);
		if(IsValid(SoundWave))
		{
			UserData->LoopingSound = CreateSoundLoopCue(SoundWave,Path);
		}
		SoundWave->AddAssetUserData(UserData);
		SoundWave->Modify();

		ObjectCreated.Add(SoundWave);
		FAssetRegistryModule::AssetCreated(SoundWave);
	}
	
	return SoundWave;
}


bool FRBMKEngineFactory::LoadOrCreateOrOverwriteAsset(UClass* InClass, const FString& Path, EObjectFlags InFlags,UObject*& OutNewObject)
{

	UObject* ExistingAsset = nullptr;
	const UStalkerGameSettings* SGSettings = GetDefault<UStalkerGameSettings>();

	if(!SGSettings->AlwaysOverwriteAssetsWhenImport)
	{
		const FString ObjectPath = Path + TEXT(".") + FPaths::GetBaseFilename(Path);
		ExistingAsset = StaticLoadObject(InClass,nullptr, *ObjectPath, nullptr, LOAD_NoWarn);
		if(ExistingAsset)
		{
			OutNewObject = ExistingAsset;
			return false;
		}
	}
	
	UPackage* InParent = CreatePackage(*Path);
	FName InName = *FPaths::GetBaseFilename(Path);
	// create an asset if it doesn't exist
	ExistingAsset = StaticFindObject(nullptr, InParent, *InName.ToString());
	
	if (!ExistingAsset)
	{
		OutNewObject = NewObject<UObject>(InParent, InClass, InName, InFlags, nullptr);
		return true;
	}

	// overwrite existing asset, if possible
	if (ExistingAsset->GetClass()->IsChildOf(InClass))
	{
		OutNewObject = NewObject<UObject>(InParent, InClass, InName, InFlags, nullptr);
		return true;
	}
	
	// otherwise delete and replace
	if (!ObjectTools::DeleteSingleObject(ExistingAsset))
	{
		UE_LOG(LogXRayImporter, Warning, TEXT("Could not delete existing asset %s"), *ExistingAsset->GetFullName());
		OutNewObject = nullptr;
		return false;
	}

	// keep InPackage alive through the GC, in case ExistingAsset was the only reason it was around
	const bool bRootedPackage = InParent->IsRooted();

	if (!bRootedPackage)
	{
		InParent->AddToRoot();
	}

	// force GC so we can cleanly create a new asset (and not do an 'in place' replacement)
	CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);

	if (!bRootedPackage)
	{
		InParent->RemoveFromRoot();
	}

	// try to find the existing asset again now that the GC has occurred
	ExistingAsset = StaticFindObject(nullptr, InParent, *InName.ToString());

	// if the object is still around after GC, fail this operation
	if (ExistingAsset)
	{

		OutNewObject = nullptr;
		return false;
	}

	// create the asset in the package
	OutNewObject = NewObject<UObject>(InParent, InClass, InName, InFlags, nullptr);
	return true;

}

int32 FRBMKEngineFactory::FindBoneIDOrAdd(TArray<CBone*>& InBones, CBone* InBone)
{
	if (InBones.Find(InBone)!=INDEX_NONE)
	{
		return InBones.Find(InBone);
	}
	if (InBone->Parent())
	{
		FindBoneIDOrAdd(InBones, InBone->Parent());
		InBones.Add(InBone);
	}
	else
	{
		check(InBones.Num()==0);
		InBones.Add(InBone);
	}
	return InBones.Num()-1;
}

int32 FRBMKEngineFactory::FindBoneIDOrAdd(TArray<uint32>& OutBones, TArray<TSharedPtr<CBoneData>>& InBones, CBoneData* InBone)
{
	if (OutBones.Find(InBone->GetSelfID()) != INDEX_NONE)
	{
		return OutBones.Find(InBone->GetSelfID());
	}

	if (InBone->GetParentID() != BI_NONE && OutBones.Find(InBone->GetParentID()) == INDEX_NONE)
	{
		FindBoneIDOrAdd(OutBones,InBones, InBones[InBone->GetParentID()].Get());
	}

	return OutBones.Add(InBone->GetSelfID());
}

bool FRBMKEngineFactory::CreateSkeletalMesh(USkeletalMesh* SkeletalMesh, TArray<FSkeletalMeshImportData>& LODIndexToSkeletalMeshImportData, const TArray<SkeletalMeshImportData::FBone>& InSkeletonBones)
{
	if (LODIndexToSkeletalMeshImportData.Num() == 0 || InSkeletonBones.Num() == 0)
	{
		return false;
	}

	// A SkeletalMesh could be retrieved for re-use and updated for animations
	// For now, create a new USkeletalMesh
	// Note: Remember to initialize UsedMorphTargetNames with existing morph targets, whenever the SkeletalMesh is reused


	// Process reference skeleton from import data
	int32 SkeletalDepth = 0;
	FSkeletalMeshImportData DummyData;
	DummyData.RefBonesBinary = InSkeletonBones;

	if (!SkeletalMeshImportUtils::ProcessImportMeshSkeleton(SkeletalMesh->GetSkeleton(), SkeletalMesh->GetRefSkeleton(), SkeletalDepth, DummyData))
	{
		return false;
	}

	if (SkeletalMesh->GetRefSkeleton().GetRawBoneNum() == 0)
	{
		SkeletalMesh->MarkAsGarbage();
	}

	// This prevents PostEditChange calls when it is alive, also ensuring it is called once when we return from this function.
	// This is required because we must ensure the morphtargets are in the SkeletalMesh before the first call to PostEditChange(),
	// or else they will be effectively discarded
	FScopedSkeletalMeshPostEditChange ScopedPostEditChange(SkeletalMesh);
	SkeletalMesh->PreEditChange(nullptr);

	// Create initial bounding box based on expanded version of reference pose for meshes without physics assets
	const FSkeletalMeshImportData& LowestLOD = LODIndexToSkeletalMeshImportData[0];
	FBox3f BoundingBox(LowestLOD.Points.GetData(), LowestLOD.Points.Num());
	FBox3f Temp = BoundingBox;
	FVector3f MidMesh = 0.5f * (Temp.Min + Temp.Max);
	BoundingBox.Min = Temp.Min + 1.0f * (Temp.Min - MidMesh);
	BoundingBox.Max = Temp.Max + 1.0f * (Temp.Max - MidMesh);
	BoundingBox.Min[2] = Temp.Min[2] + 0.1f * (Temp.Min[2] - MidMesh[2]);
	const FVector3f BoundingBoxSize = BoundingBox.GetSize();
	if (LowestLOD.Points.Num() > 2 && BoundingBoxSize.X < THRESH_POINTS_ARE_SAME && BoundingBoxSize.Y < THRESH_POINTS_ARE_SAME && BoundingBoxSize.Z < THRESH_POINTS_ARE_SAME)
	{
		return false;
	}

#if WITH_EDITOR
	IMeshUtilities& MeshUtilities = FModuleManager::Get().LoadModuleChecked<IMeshUtilities>("MeshUtilities");
#endif // WITH_EDITOR

	FSkeletalMeshModel* ImportedResource = SkeletalMesh->GetImportedModel();
	ImportedResource->LODModels.Empty();
	SkeletalMesh->ResetLODInfo();
	bool bHasVertexColors = false;


	for (int32 LODIndex = 0; LODIndex < LODIndexToSkeletalMeshImportData.Num(); ++LODIndex)
	{
		FSkeletalMeshImportData& LODImportData = LODIndexToSkeletalMeshImportData[LODIndex];
		ImportedResource->LODModels.Add(new FSkeletalMeshLODModel());
		FSkeletalMeshLODModel& LODModel = ImportedResource->LODModels.Last();

		// Process bones influence (normalization and optimization) (optional)
		SkeletalMeshImportUtils::ProcessImportMeshInfluences(LODImportData, SkeletalMesh->GetPathName());

		FSkeletalMeshLODInfo& NewLODInfo = SkeletalMesh->AddLODInfo();
		NewLODInfo.ReductionSettings.NumOfTrianglesPercentage = 1.0f;
		NewLODInfo.ReductionSettings.NumOfVertPercentage = 1.0f;
		NewLODInfo.ReductionSettings.MaxDeviationPercentage = 0.0f;
		NewLODInfo.BuildSettings.bRecomputeNormals = false;
		NewLODInfo.LODHysteresis = 0.02f;

		bHasVertexColors |= LODImportData.bHasVertexColors;

		LODModel.NumTexCoords = FMath::Max<uint32>(1, LODImportData.NumTexCoords);

		// Data needed by BuildSkeletalMesh
		LODImportData.PointToRawMap.AddUninitialized(LODImportData.Points.Num());
		for (int32 PointIndex = 0; PointIndex < LODImportData.Points.Num(); ++PointIndex)
		{
			LODImportData.PointToRawMap[PointIndex] = PointIndex;
		}

		TArray<FVector3f> LODPoints;
		TArray<SkeletalMeshImportData::FMeshWedge> LODWedges;
		TArray<SkeletalMeshImportData::FMeshFace> LODFaces;
		TArray<SkeletalMeshImportData::FVertInfluence> LODInfluences;
		TArray<int32> LODPointToRawMap;
		LODImportData.CopyLODImportData(LODPoints, LODWedges, LODFaces, LODInfluences, LODPointToRawMap);

#if WITH_EDITOR
		IMeshUtilities::MeshBuildOptions BuildOptions;
		BuildOptions.TargetPlatform = GetTargetPlatformManagerRef().GetRunningTargetPlatform();
		// #ueent_todo: Normals and tangents shouldn't need to be recomputed when they are retrieved from USD
		BuildOptions.bComputeNormals = !LODImportData.bHasNormals;
		BuildOptions.bComputeTangents = !LODImportData.bHasTangents;
		BuildOptions.bUseMikkTSpace = true;
		TArray<FText> WarningMessages;
		TArray<FName> WarningNames;

		bool bBuildSuccess = MeshUtilities.BuildSkeletalMesh(LODModel, SkeletalMesh->GetPathName(), SkeletalMesh->GetRefSkeleton(), LODInfluences, LODWedges, LODFaces, LODPoints, LODPointToRawMap, BuildOptions, &WarningMessages, &WarningNames);

		for (int32 WarningIndex = 0; WarningIndex < FMath::Max(WarningMessages.Num(), WarningNames.Num()); ++WarningIndex)
		{
			const FText& Text = WarningMessages.IsValidIndex(WarningIndex) ? WarningMessages[WarningIndex] : FText::GetEmpty();
			const FName& Name = WarningNames.IsValidIndex(WarningIndex) ? WarningNames[WarningIndex] : NAME_None;

			if (bBuildSuccess)
			{
				UE_LOG(LogXRayImporter, Warning, TEXT("Warning when trying to build skeletal mesh from USD: '%s': '%s'"), *Name.ToString(), *Text.ToString());
			}
			else
			{
				UE_LOG(LogXRayImporter, Error, TEXT("Error when trying to build skeletal mesh from USD: '%s': '%s'"), *Name.ToString(), *Text.ToString());
			}
		}

		if (!bBuildSuccess)
		{
			SkeletalMesh->MarkAsGarbage();
			return false;
		}

		// This is important because it will fill in the LODModel's RawSkeletalMeshBulkDataID,
		// which is the part of the skeletal mesh's DDC key that is affected by the actual mesh data
		SkeletalMesh->SaveLODImportedData(LODIndex, LODImportData);
#endif // WITH_EDITOR
	}

	SkeletalMesh->SetImportedBounds(FBoxSphereBounds((FBox)BoundingBox));
	SkeletalMesh->SetHasVertexColors(bHasVertexColors);
	SkeletalMesh->SetVertexColorGuid(SkeletalMesh->GetHasVertexColors() ? FGuid::NewGuid() : FGuid());
	SkeletalMesh->CalculateInvRefMatrices();

	// Generate a Skeleton and associate it to the SkeletalMesh
	//FString SkeletonName = SkeletalMeshName + "_Skeleton";


	/*if (!FindOrAddSkeleton(SkeletalMesh, SkeletonName, ParentPackage, Flags))
	{
		return false;
	}
	CreateMorphTargets(SkeletalMeshImport, LODIndexToSkeletalMeshImportData, SkeletalMesh, ParentPackage, Flags, SkeletalMeshName);*/
	return true;
}

USkeleton* FRBMKEngineFactory::FindOrCreateSkeleton(const FString& Path, USkeletalMesh* InMesh)
{
	for (USkeleton* Skeleton : GStalkerEditorManager->Skeletons)
	{
		const FReferenceSkeleton& SkeletonRefSkel = Skeleton->GetReferenceSkeleton();
		const FReferenceSkeleton& MeshRefSkel = InMesh->GetRefSkeleton();

		if (SkeletonRefSkel.GetNum() != MeshRefSkel.GetNum())
		{
			continue;
		}
		if (SkeletonRefSkel.GetBoneName(0) != MeshRefSkel.GetBoneName(0))
		{
			continue;
		}
		TMap<FName,FName> Bones;
		for (int32 i=1;i< SkeletonRefSkel.GetNum();i++)
		{
			Bones.Add(SkeletonRefSkel.GetBoneName(i), SkeletonRefSkel.GetBoneName(SkeletonRefSkel.GetParentIndex(i)));
		}
		bool bResult = true;
		for (int32 i = 1; i < MeshRefSkel.GetNum(); i++)
		{
			if (!Bones.Contains(MeshRefSkel.GetBoneName(i))||Bones[MeshRefSkel.GetBoneName(i)] != MeshRefSkel.GetBoneName(MeshRefSkel.GetParentIndex(i)))
			{
				bResult = false;
			}
		}
		if (bResult)
		{
			return Skeleton;
		}
	}

	USkeleton* Skeleton = nullptr;
	if(!LoadOrCreateOrOverwriteAsset(Path,ObjectFlags,Skeleton))
	{
		return Skeleton;
	}

	Skeleton->MergeAllBonesToBoneTree(InMesh);
	ObjectCreated.Add(Skeleton);
	FAssetRegistryModule::AssetCreated(Skeleton);
	GStalkerEditorManager->Skeletons.Add(Skeleton);
	return Skeleton;
}

void FRBMKEngineFactory::CreateAnims(const FString& FullName, USkeleton* InMesh, CEditableObject* Object)
{
	for (CSMotion* Motion : Object->SMotions())
	{
		CreateAnims(FullName,InMesh, Motion);
	}
}

void FRBMKEngineFactory::CreateAnims(const FString& FullName, USkeleton* Skeleton, CSMotion* InMotion)
{
	const FString MotionPath = UPackageTools::SanitizePackageName(FullName / FString( InMotion->name.c_str()));

	UAnimSequence* AnimSequence = nullptr;
	if(!LoadOrCreateOrOverwriteAsset(MotionPath,ObjectFlags,AnimSequence))
	{
		return;
	}

	AnimSequence->SetSkeleton(Skeleton);
	AnimSequence->SetPreviewMesh(Skeleton->GetPreviewMesh());


	// In a regular import workflow this NameMapping will exist and be populated with the blend shape names we imported, if any
	const FSmartNameMapping* NameMapping = Skeleton->GetSmartNameContainer(USkeleton::AnimCurveMappingName);
	if (!NameMapping)
	{
		return;
	}
	IAnimationDataController& Controller = AnimSequence->GetController();

	// If we should transact, we'll already have a transaction from somewhere else. We should suppress this because
	// it will also create a transaction when importing into UE assets, and the level sequence assets can emit some warnings about it
	const bool bShouldTransact = false;
	Controller.OpenBracket(LOCTEXT("XRayEngineFactoryImporterAnimData_Bracket", "FRBMKEngineFactory Animation Data"), bShouldTransact);
	Controller.ResetModel(bShouldTransact);
	Controller.InitializeModel();
	// Bake the animation for each frame.
	// An alternative route would be to convert the time samples into TransformCurves, add them to UAnimSequence::RawCurveData,
	// and then call UAnimSequence::BakeTrackCurvesToRawAnimation. Doing it this way provides a few benefits though: The main one is that the way with which
	// UAnimSequence bakes can lead to artifacts on problematic joints (e.g. 90 degree rotation joints children of -1 scale joints, etc.) as it compounds the
	// transformation with the rest pose. Another benefit is that that doing it this way lets us offload the interpolation to USD, so that it can do it
	// however it likes, and we can just sample the joints at the target framerate

	for (uint32 BoneIndex = 0; BoneIndex < InMotion->BoneMotions().size(); ++BoneIndex)
	{
		shared_str InBoneName = InMotion->BoneMotions()[BoneIndex].name;
		FName BoneName = FName(InBoneName.c_str());
		TArray<FVector> PosKeys;
		TArray<FQuat>   RotKeys;
		TArray<FVector> ScaleKeys;
		for (int32 FrameIndex = 0; FrameIndex < InMotion->Length(); ++FrameIndex)
		{
			FVector3f PosKey;
			FQuat4f   RotKey;
			FVector3f ScaleKey(1,1,1);
			Fvector3 InLocation;
			Fvector3 InRotation;

			InMotion->_Evaluate(BoneIndex, static_cast<float>(FrameIndex + InMotion->FrameStart()) / InMotion->FPS(), InLocation, InRotation);
			Fmatrix InMatrixRotation;
			InMatrixRotation.setXYZi(InRotation.x, InRotation.y, InRotation.z);
			Fquaternion InQuat; InQuat.set(InMatrixRotation);
			PosKey.Set(-InLocation.x * 100, InLocation.z * 100, InLocation.y * 100);
			if(!ensure(!PosKey.ContainsNaN()))
			{
				PosKey = FVector3f(0,0,0);
			}
			RotKey = FQuat4f(InQuat.x, -InQuat.z, -InQuat.y, InQuat.w);
			PosKeys.Add(FVector(PosKey.X, PosKey.Y, PosKey.Z));
			RotKeys.Add(FQuat(RotKey.X, RotKey.Y, RotKey.Z, RotKey.W));
			RotKeys.Last().Normalize();
			ScaleKeys.Add(FVector(ScaleKey.X, ScaleKey.Y, ScaleKey.Z));
		}

		Controller.AddBoneTrack(BoneName, bShouldTransact);
		Controller.SetBoneTrackKeys(BoneName, PosKeys, RotKeys, ScaleKeys, bShouldTransact);
	}

	AnimSequence->Interpolation = EAnimInterpolationType::Linear;
	AnimSequence->RateScale = InMotion->fSpeed;



	Controller.SetPlayLength(static_cast<float>(InMotion->Length() - 1) / InMotion->FPS(), bShouldTransact);

	FFrameRate FrameRate;
	FrameRate.Denominator = 1;
	for (; !FMath::IsNearlyZero(FMath::Frac(InMotion->FPS() * FrameRate.Denominator)); FrameRate.Denominator *= 10) {}
	FrameRate.Numerator = FMath::FloorToInt32(InMotion->FPS() * FrameRate.Denominator);

	Controller.SetFrameRate(FrameRate, bShouldTransact);
	Controller.NotifyPopulated(); // This call is important to get the controller to not use the sampling frequency as framerate
	Controller.CloseBracket(bShouldTransact);

	AnimSequence->Modify();
	AnimSequence->PostEditChange();
	ObjectCreated.Add(AnimSequence);
	FAssetRegistryModule::AssetCreated(AnimSequence);
}

void FRBMKEngineFactory::CreateAnims(const FString& Name, USkeletalMesh* InMesh, TArray<TSharedPtr<CBoneData>>&BonesData, IReader* InMotion)
{
	shared_motions Motions;
	if (InMotion->find_chunk(OGF_S_MOTION_REFS))
	{
		string_path		items_nm;
		InMotion->r_stringZ(items_nm, sizeof(items_nm));
		size_t set_cnt = _GetItemCount(items_nm);
		R_ASSERT(set_cnt < MAX_ANIM_SLOT);
		string_path		nm;
		for (size_t k = 0; k < set_cnt; ++k)
		{
			_GetItem(items_nm, k, nm);
			FString NameOmf = nm;
			xr_strcat(nm, ".omf");

			string_path	fn;
			if (!FS.exist(fn, "$level$", nm))
			{
				if (!FS.exist(fn, "$game_meshes$", nm))
				{
#ifdef _EDITOR
					Msg("!Can't find motion file '%s'.", nm);
					return;
#else
					Debug.fatal(DEBUG_INFO, "Can't find motion file '%s'.", nm);
#endif
				}
			}

			IReader* MS = FS.r_open(fn);
			NameOmf.ReplaceInline(TEXT("\\"), TEXT("/"));
			const FString PackageNameAnims = UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("Meshes") / NameOmf);
			CreateAnims(PackageNameAnims, InMesh, BonesData, MS);
			FS.r_close(MS);
		}
	}
	else  if (InMotion->find_chunk(OGF_S_MOTION_REFS2))
	{
		u32 set_cnt = InMotion->r_u32();
		string_path		nm;
		for (u32 k = 0; k < set_cnt; ++k)
		{
			InMotion->r_stringZ(nm, sizeof(nm));
			FString NameOmf = nm;
			xr_strcat(nm, ".omf");
			// Check compatibility
			string_path	fn;
			if (!FS.exist(fn, "$level$", nm))
			{
				if (!FS.exist(fn, "$game_meshes$", nm))
				{
#ifdef _EDITOR
					Msg("!Can't find motion file '%s'.", nm);
					return;
#else
					Debug.fatal(DEBUG_INFO, "Can't find motion file '%s'.", nm);
#endif
				}
			}
			IReader* MS = FS.r_open(fn);
			NameOmf.ReplaceInline(TEXT("\\"), TEXT("/"));
			const FString PackageNameAnims = UPackageTools::SanitizePackageName(GStalkerEditorManager->GetGamePath() / TEXT("Meshes") / NameOmf);
			CreateAnims(PackageNameAnims, InMesh, BonesData, MS);
			FS.r_close(MS);
		}
	}
	else
	{
		UStalkerKinematicsAssetUserData*StalkerKinematicsAssetUserData = InMesh->GetAssetUserDataChecked<UStalkerKinematicsAssetUserData>();
		const FString& Path = UPackageTools::SanitizePackageName(Name);
		UStalkerKinematicsAnimsData* StalkerKinematicsAnimsData = nullptr;
		if(!LoadOrCreateOrOverwriteAsset(Path,ObjectFlags,StalkerKinematicsAnimsData))
		{
			StalkerKinematicsAssetUserData->Anims.Add(StalkerKinematicsAnimsData);
			return;
		}
		FAssetRegistryModule::AssetCreated(StalkerKinematicsAnimsData);
		ObjectCreated.Add(StalkerKinematicsAnimsData);

		string_path	nm;
		strconcat(sizeof(nm), nm, TCHAR_TO_ANSI(*Path), ".ogf");
		xr_vector<CBoneData*> bones;
		for (size_t i = 0; i < BonesData.Num(); i++)
		{
			bones.push_back(BonesData[i].Get());
		}
		Motions.create(nm, InMotion,& bones);
		
		for(auto&[Key,Value]:*Motions.motion_map())
		{
			const FString MotionPath = Name / FString(Key.c_str());
			UAnimSequence* OutAnim = CreateAnim(MotionPath,  InMesh->GetSkeleton(), BonesData, Motions, Value);
			if (!OutAnim)
			{
				continue;
			}
			OutAnim->AddAssetUserData(NewObject<UStalkerKinematicsAnimAssetUserData>(OutAnim));
			UStalkerKinematicsAnimAssetUserData* StalkerKinematicsAnimAssetUserData =  OutAnim->GetAssetUserDataChecked<UStalkerKinematicsAnimAssetUserData>();
			StalkerKinematicsAnimAssetUserData->BuildFromLegacy(*Motions.motion_def(Value), BonesData, *Motions.partition());
			StalkerKinematicsAnimsData->Anims.Add(FName(Key.c_str()), OutAnim);
		}
		for (u8 i = 0; i < Motions.partition()->count()&&i<4; i++)
		{
			const CPartDef& Part =  Motions.partition()->part(i);
			FStalkerKinematicsAnimsBonesPart&BonesPart =  StalkerKinematicsAnimsData->BonesParts.Add(Part.Name.c_str());
			for (u32 ID : Part.bones)
			{
				BonesPart.Bones.Add(FName(BonesData[ID]->name.c_str()));
			}
		}

		StalkerKinematicsAssetUserData->Anims.Add(StalkerKinematicsAnimsData);

		StalkerKinematicsAnimsData->Modify();
		StalkerKinematicsAnimsData->PostEditChange();
	}
}

UAnimSequence* FRBMKEngineFactory::CreateAnim(const FString& Name, USkeleton* InMesh, TArray<TSharedPtr<CBoneData>>&BoneData, shared_motions&InMotion, u16 ID)
{

	const FString& Path = UPackageTools::SanitizePackageName(Name);

	UAnimSequence* AnimSequence = nullptr;
	if(!LoadOrCreateOrOverwriteAsset(Path,ObjectFlags,AnimSequence))
	{
		return AnimSequence;
	}

	AnimSequence->SetSkeleton(InMesh);
	AnimSequence->SetPreviewMesh(InMesh->GetPreviewMesh());

	IAnimationDataController& Controller = AnimSequence->GetController();

	// If we should transact, we'll already have a transaction from somewhere else. We should suppress this because
	// it will also create a transaction when importing into UE assets, and the level sequence assets can emit some warnings about it
	const bool bShouldTransact = false;
	Controller.OpenBracket(LOCTEXT("XRayEngineFactoryImporterAnimData_Bracket", "FRBMKEngineFactory Animation Data"), bShouldTransact);
	Controller.ResetModel(bShouldTransact);
	Controller.InitializeModel();

	// Bake the animation for each frame.
	// An alternative route would be to convert the time samples into TransformCurves, add them to UAnimSequence::RawCurveData,
	// and then call UAnimSequence::BakeTrackCurvesToRawAnimation. Doing it this way provides a few benefits though: The main one is that the way with which
	// UAnimSequence bakes can lead to artifacts on problematic joints (e.g. 90 degree rotation joints children of -1 scale joints, etc.) as it compounds the
	// transformation with the rest pose. Another benefit is that that doing it this way lets us offload the interpolation to USD, so that it can do it
	// however it likes, and we can just sample the joints at the target framerate
	u32 Count = u32(-1);
	for (int32 BoneIndex = 0; BoneIndex < BoneData.Num(); ++BoneIndex)
	{
		shared_str InBoneName = BoneData[BoneIndex]->name;
		FName BoneName = FName(InBoneName.c_str());
		TArray<FVector> PosKeys;
		TArray<FQuat>   RotKeys;
		TArray<FVector> ScaleKeys;

		MotionVec* BoneMotion = InMotion.bone_motions(InBoneName);
		if(!BoneMotion)continue;
		CMotion&Motion = BoneMotion->at(ID);
		if (Count == u32(-1))
		{
			Count = Motion.get_count();
		}
		check(Count == Motion.get_count());
		for (uint32 FrameIndex = 0; FrameIndex < Count; ++FrameIndex)
		{
			Fvector3 InLocation;
			Fquaternion InQuat;
			if (!Motion.test_flag(flRKeyAbsent))
			{
				QR2Quat(Motion._keysR[FrameIndex], InQuat);

			}
			else
			{
				QR2Quat(Motion._keysR[0], InQuat);
			}
			if (Motion.test_flag(flTKeyPresent))
			{
				if (Motion.test_flag(flTKey16IsBit))
					QT16_2T(Motion._keysT16[FrameIndex], Motion, InLocation);
				else
					QT8_2T(Motion._keysT8[FrameIndex], Motion, InLocation);
			}
			else
				InLocation.set(Motion._initT);

			FVector3f PosKey = StalkerMath::RBMKLocationToUnreal(InLocation);
			if(!ensure(!PosKey.ContainsNaN()))
			{
				PosKey = FVector3f(0,0,0);
			}
			FQuat4f   RotKey = StalkerMath::RBMKQuaternionToUnreal(InQuat);
			FVector3f ScaleKey(1, 1, 1);
			PosKeys.Add(FVector(PosKey.X, PosKey.Y, PosKey.Z));
			RotKeys.Add(FQuat(RotKey.X, RotKey.Y, RotKey.Z, RotKey.W));
			RotKeys.Last().Normalize();
			ScaleKeys.Add(FVector(ScaleKey.X, ScaleKey.Y, ScaleKey.Z));
		}

		Controller.AddBoneTrack(BoneName, bShouldTransact);
		Controller.SetBoneTrackKeys(BoneName, PosKeys, RotKeys, ScaleKeys, bShouldTransact);
	}

	AnimSequence->Interpolation = EAnimInterpolationType::Linear;
	AnimSequence->RateScale = InMotion.motion_def(ID)->Speed();



	Controller.SetPlayLength(static_cast<float>(Count - 1)*SAMPLE_SPF, bShouldTransact);

	FFrameRate FrameRate;
	FrameRate.Denominator = 1;
	for (; !FMath::IsNearlyZero(FMath::Frac(SAMPLE_FPS * FrameRate.Denominator)); FrameRate.Denominator *= 10) {}
	FrameRate.Numerator = FMath::FloorToInt32(SAMPLE_FPS * FrameRate.Denominator);

	Controller.SetFrameRate(FrameRate, bShouldTransact);
	Controller.NotifyPopulated(); // This call is important to get the controller to not use the sampling frequency as framerate
	Controller.CloseBracket(bShouldTransact);

	AnimSequence->Modify();
	AnimSequence->PostEditChange();
	ObjectCreated.Add(AnimSequence);
	FAssetRegistryModule::AssetCreated(AnimSequence);
	return AnimSequence;
}	

void FRBMKEngineFactory::CreatePhysicsAsset(const FString& FullName, USkeletalMesh* InMesh,const TArray<IBoneData*>&Bones)
{
	UPhysicsAsset* PhysicsAsset = nullptr;
	if(!LoadOrCreateOrOverwriteAsset(FullName,ObjectFlags,PhysicsAsset))
	{
		return;
	}
	for (IBoneData* Bone:Bones)
	{

		ECollisionEnabled::Type CollisionEnabled = ECollisionEnabled::NoCollision;
		TObjectPtr<USkeletalBodySetup> NewBodySetup = NewObject<USkeletalBodySetup>(PhysicsAsset);
		NewBodySetup->BodySetupGuid = FGuid::NewGuid();
		NewBodySetup->BoneName = Bone->GetName().c_str();
		if (Bone->get_shape().flags.is(SBoneShape::sfNoPhysics | SBoneShape::sfNoPickable))
		{
			CollisionEnabled = ECollisionEnabled::NoCollision;
		}
		else if(Bone->get_shape().flags.test(SBoneShape::sfNoPhysics))
		{
			CollisionEnabled = ECollisionEnabled::QueryOnly;
		}
		else if (Bone->get_shape().flags.test(SBoneShape::sfNoPickable))
		{
			CollisionEnabled = ECollisionEnabled::PhysicsOnly;
		}
		else 
		{
			CollisionEnabled = ECollisionEnabled::QueryAndPhysics;
		}

		switch (Bone->get_shape().type)
		{
		case SBoneShape::EShapeType::stBox:
		{
			FKBoxElem Box;
			Box.Center = FVector(-Bone->get_shape().box.m_translate.x * 100, Bone->get_shape().box.m_translate.z * 100, Bone->get_shape().box.m_translate.y * 100);
			Box.X = Bone->get_shape().box.m_halfsize.x * 200;
			Box.Y = Bone->get_shape().box.m_halfsize.z * 200;
			Box.Z = Bone->get_shape().box.m_halfsize.y * 200;
			Fmatrix InRotation;
			Bone->get_shape().box.xform_get(InRotation);
			Fquaternion InQuat; InQuat.set(InRotation);
			Box.Rotation = FRotator(FQuat(InQuat.x, -InQuat.z, -InQuat.y, InQuat.w));
			Box.SetCollisionEnabled(CollisionEnabled);
			NewBodySetup->AggGeom.BoxElems.Add(Box);
			break;
		}

		case SBoneShape::EShapeType::stCylinder:
		{
			FKSphylElem Sphyl;
			Sphyl.Center = FVector(-Bone->get_shape().cylinder.m_center.x * 100, Bone->get_shape().cylinder.m_center.z * 100, Bone->get_shape().cylinder.m_center.y * 100);
			Sphyl.Radius = Bone->get_shape().cylinder.m_radius * 100;
			Sphyl.Length = Bone->get_shape().cylinder.m_height * 100;
			Fmatrix InRotation;
			InRotation.k.set(Bone->get_shape().cylinder.m_direction);
			Fvector::generate_orthonormal_basis(InRotation.k, InRotation.j, InRotation.i);
			Fquaternion InQuat; InQuat.set(InRotation);
			FRotator RotationToUnreal(0, 0, -90);
			FRotator OutRotation(FQuat(InQuat.x, -InQuat.z, -InQuat.y, InQuat.w) * FQuat(RotationToUnreal));
			Sphyl.Rotation = OutRotation;
			Sphyl.SetCollisionEnabled(CollisionEnabled);
			NewBodySetup->AggGeom.SphylElems.Add(Sphyl);
		break;
		}
		case SBoneShape::EShapeType::stSphere:
		{
			FKSphereElem Sphere;
			Sphere.Center = FVector( -Bone->get_shape().sphere.P.x * 100, Bone->get_shape().sphere.P.z * 100, Bone->get_shape().sphere.P.y * 100);
			Sphere.Radius = Bone->get_shape().sphere.R * 100;
			Sphere.SetCollisionEnabled(CollisionEnabled);
			NewBodySetup->AggGeom.SphereElems.Add(Sphere);
			break;
		}
		break;
		default:
		{
			NewBodySetup->MarkAsGarbage();
			continue;
			break;
		}
		}

		NewBodySetup->DefaultInstance.SetMassOverride(Bone->get_mass());
		PhysicsAsset->SkeletalBodySetups.Add(NewBodySetup);
		NewBodySetup->PostEditChange();
	}

	PhysicsAsset->UpdateBodySetupIndexMap();
	PhysicsAsset->UpdateBoundsBodiesArray();
	PhysicsAsset->SetPreviewMesh(InMesh);
	InMesh->SetPhysicsAsset(PhysicsAsset);
	PhysicsAsset->PostEditChange();
	ObjectCreated.Add(PhysicsAsset);
	FAssetRegistryModule::AssetCreated(PhysicsAsset);
}


static int OGGSeekFunction(void* DataSource, ogg_int64_t Offset, int Type)
{
	switch (Type)
	{
	case SEEK_SET:
		static_cast<FArchive*>(DataSource)->Seek((int64)Offset);
		break;
	case SEEK_CUR:
		static_cast<FArchive*>(DataSource)->Seek((int64)Offset+ static_cast<FArchive*>(DataSource)->Tell());
		break;
	case SEEK_END:
		static_cast<FArchive*>(DataSource)->Seek((int64)Offset + ((FArchive*)DataSource)->TotalSize());
		break;
	}
	return 0;
}
static size_t OGGReadFunction(void* Pointer, size_t Size, size_t NumBlocks, void* DataSource)
{
	FArchive* F = static_cast<FArchive*>(DataSource);
	const size_t Elapsed = F->TotalSize()-F->Tell();
	const size_t ExistBlock = FMath::Max(static_cast<int64>(0), static_cast<int64>(Elapsed / Size));
	const size_t ReadBlock =  FMath::Min(ExistBlock, NumBlocks);
	F->Serialize(Pointer, static_cast<size_t>(ReadBlock * Size));
	return ReadBlock;
}

static int OGGCloseFunction(void* DataSource)
{
	return 0;
}

static long OGGTellFunction(void* DataSource)
{
	return static_cast<FArchive*>(DataSource)->Tell();
}

void FRBMKEngineFactory::ReadOGGSound(FArchive& Ar, TArray<uint16>& OutData, int32& OutSampleRate, int32& OutNumChannels,FRbmkOGGComment& OutOGGComment)
{
	OggVorbis_File OggVorbisFileData = {};
	ov_callbacks OGGCallbacks = { OGGReadFunction, OGGSeekFunction, OGGCloseFunction, OGGTellFunction };
	ov_open_callbacks(&Ar, &OggVorbisFileData, nullptr, 0, OGGCallbacks);
	vorbis_info* ovi = ov_info(&OggVorbisFileData, -1);
	OutSampleRate = ovi->rate;
	OutNumChannels = ovi->channels;
	ogg_int64_t PCMTotal = ov_pcm_total(&OggVorbisFileData, -1)*2* ovi->channels;

	vorbis_comment*	ovm		= ov_comment(&OggVorbisFileData,-1);
	if (ovm->comments)
	{
		FArrayReader InComment;
		InComment.AddUninitialized(ovm->comment_lengths[0]);
		FMemory::Memcpy(InComment.GetData(),ovm->user_comments[0],ovm->comment_lengths[0]);
		int32 Version;
		InComment<<Version;
		switch (Version)
		{
		case 1:
			InComment<<OutOGGComment.MinDistance;
			InComment<<OutOGGComment.MaxDistance;
			InComment<<OutOGGComment.Flags;
			OutOGGComment.MaxAIDistance = OutOGGComment.MaxDistance;
			break;
		case 2:
			InComment<<OutOGGComment.MinDistance;
			InComment<<OutOGGComment.MaxDistance;
			InComment<<OutOGGComment.BaseVolume;
			InComment<<OutOGGComment.Flags;
			OutOGGComment.MaxAIDistance = OutOGGComment.MaxDistance;
			break;
		case 3:
			InComment<<OutOGGComment.MinDistance;
			InComment<<OutOGGComment.MaxDistance;
			InComment<<OutOGGComment.BaseVolume;
			InComment<<OutOGGComment.Flags;
			InComment<<OutOGGComment.MaxAIDistance;
			break;
		default: ;
		}
		OutOGGComment.MaxAIDistance = FMath::Min(OutOGGComment.MaxAIDistance,OutOGGComment.MaxDistance);

	}

	OutData.AddUninitialized(PCMTotal/2);

	int64 CurrentOffset = 0;
	int32 CurrentSection = 0;
	
	//m_Duration = static_cast<float>(PCMTotal) / static_cast<float>(OutSampleRate* OutNumChannels*2);
	while (CurrentOffset <PCMTotal)
	{
		
		int32 Result = ov_read(&OggVorbisFileData, reinterpret_cast<char*>(OutData.GetData()) + CurrentOffset, PCMTotal - CurrentOffset, 0, 2, 1, &CurrentSection);

		if (Result == 0)
		{
			break;
		}
		else if (!ensure(Result >= 0)) 
		{
			
		}
		else
		{
			CurrentOffset += Result;
		}
}

	ov_clear(&OggVorbisFileData);
}

USoundCue* FRBMKEngineFactory::CreateSoundLoopCue(USoundWave* InSound, const FString& Path)
{
	auto InsertSoundNodeLambda = [](USoundCue* SoundCue, UClass* NodeClass, int32 NodeIndex)
	{
		USoundNode* SoundNode = SoundCue->ConstructSoundNode<USoundNode>(NodeClass);

		// If this node allows >0 children but by default has zero - create a connector for starters
		if (SoundNode->GetMaxChildNodes() > 0 && SoundNode->ChildNodes.Num() == 0)
		{
			SoundNode->CreateStartingConnectors();
		}

		SoundNode->GraphNode->NodePosX = -150 * NodeIndex - 100;
		SoundNode->GraphNode->NodePosY = -35;

		// Link the node to the cue.
		SoundNode->ChildNodes[0] = SoundCue->FirstNode;

		// Link the attenuation node to root.
		SoundCue->FirstNode = SoundNode;

		SoundCue->LinkGraphNodesFromSoundNodes();
		return SoundCue;
	};

	USoundCue* SoundCue = nullptr;
	if(LoadOrCreateOrOverwriteAsset(Path + TEXT("_LoopCue"), ObjectFlags, SoundCue))
	{
		USoundNodeWavePlayer* WavePlayer = SoundCue->ConstructSoundNode<USoundNodeWavePlayer>();
		int32 NodeIndex = 0;
		WavePlayer->GraphNode->NodePosX = -150 * (NodeIndex++) - 100;
		WavePlayer->GraphNode->NodePosY = -35;
		WavePlayer->bLooping = true;
		WavePlayer->SetSoundWave(InSound);
		SoundCue->SoundClassObject = InSound->SoundClassObject;
		SoundCue->VolumeMultiplier = 1;
		SoundCue->FirstNode = WavePlayer;
		SoundCue->LinkGraphNodesFromSoundNodes();
		SoundCue->AttenuationSettings = InSound->AttenuationSettings;
		//InsertSoundNode(SoundCue,USoundNodeLooping::StaticClass(),NodeIndex);
		FAssetRegistryModule::AssetCreated(SoundCue);
	}

	return SoundCue;
}
#undef LOCTEXT_NAMESPACE 
