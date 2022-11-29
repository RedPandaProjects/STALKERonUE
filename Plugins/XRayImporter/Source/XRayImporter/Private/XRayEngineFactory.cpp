#include "XRayEngineFactory.h"
#include "PhysicsEngine/PhysicsAsset.h"
#define LOCTEXT_NAMESPACE "XRayImporterModule"

XRayEngineFactory::XRayEngineFactory(UObject* InParentPackage, EObjectFlags InFlags)
{
	ParentPackage = InParentPackage;
	ObjectFlags = InFlags;
}

XRayEngineFactory::~XRayEngineFactory()
{
	for (CEditableObject* Object :Objects)
	{
		GRayObjectLibrary->RemoveEditObject(Object);
	}
}

UObject* XRayEngineFactory::ImportObject(const FString& FileName, bool UseOnlyFullPath)
{
	CEditableObject* Object = GRayObjectLibrary->CreateEditObject(TCHAR_TO_ANSI(*FileName));
	if (Object)
	{
		Objects.Add(Object); 
		if (Object->IsSkeleton())
		{
			return ImportObjectAsDynamicMesh(Object, UseOnlyFullPath);
		}
		else
		{
			return ImportObjectAsStaticMesh(Object, UseOnlyFullPath);
		} 
	}
	return nullptr;
}

UStaticMesh* XRayEngineFactory::ImportObjectAsStaticMesh(CEditableObject* Object, bool UseOnlyFullPath)
{
	UStaticMesh* StaticMesh = nullptr;

	FString FileName = Object->GetName();
	FString PackageName;
	if (UseOnlyFullPath)
	{
		FileName.ReplaceCharInline(TEXT('\\'),TEXT('/'));
		PackageName = UPackageTools::SanitizePackageName(GetGamePath() / TEXT("Meshes") / FPaths::GetBaseFilename(FileName,false));
	}
	else
	{
		PackageName = UPackageTools::SanitizePackageName(ParentPackage->GetName() / FPaths::GetBaseFilename(FileName));
	}
	const FString NewObjectPath = PackageName + TEXT(".") + FPaths::GetBaseFilename(PackageName);
	StaticMesh = LoadObject<UStaticMesh>(nullptr, *NewObjectPath);
	if(StaticMesh)
		return StaticMesh;

	UPackage* AssetPackage = CreatePackage(*PackageName);
	StaticMesh = NewObject<UStaticMesh>(AssetPackage, *FPaths::GetBaseFilename(PackageName), ObjectFlags);
	FAssetRegistryModule::AssetCreated(StaticMesh);
	ObjectCreated.Add(StaticMesh);
	TArray<FStaticMaterial> Materials;
	FStaticMeshSourceModel& SourceModel = StaticMesh->AddSourceModel();
	int32 MaterialIndex = 0;
	for (int32 LodIndex = 0; LodIndex < 1; LodIndex++)
	{
		FMeshDescription* MeshDescription = StaticMesh->CreateMeshDescription(LodIndex);
		if (MeshDescription)
		{
			FStaticMeshAttributes StaticMeshAttributes(*MeshDescription);
			TVertexAttributesRef<FVector3f> VertexPositions = StaticMeshAttributes.GetVertexPositions();
			TEdgeAttributesRef<bool>  EdgeHardnesses = StaticMeshAttributes.GetEdgeHardnesses();
			TPolygonGroupAttributesRef<FName> PolygonGroupImportedMaterialSlotNames = StaticMeshAttributes.GetPolygonGroupMaterialSlotNames();
			TVertexInstanceAttributesRef<FVector3f> VertexInstanceNormals = StaticMeshAttributes.GetVertexInstanceNormals();
			TVertexInstanceAttributesRef<FVector3f> VertexInstanceTangents = StaticMeshAttributes.GetVertexInstanceTangents();
			TVertexInstanceAttributesRef<float> VertexInstanceBinormalSigns = StaticMeshAttributes.GetVertexInstanceBinormalSigns();
			TVertexInstanceAttributesRef<FVector2f> VertexInstanceUVs = StaticMeshAttributes.GetVertexInstanceUVs();
			TVertexInstanceAttributesRef<FVector4f> VertexInstanceColors = StaticMeshAttributes.GetVertexInstanceColors();

			for (size_t ElementID = 0; ElementID < Object->SurfaceCount(); ElementID++)
			{
				xr_vector< st_MeshVertex> Vertices;
				for (size_t MeshID = 0; MeshID < Object->MeshCount(); MeshID++)
				{
					Object->Meshes()[MeshID]->GenerateVertices(Vertices, Object->Surfaces()[ElementID]);
				}
				if (Vertices.size() == 0)
				{
					continue;
				}
				VertexInstanceUVs.SetNumChannels(1);
				FPolygonGroupID CurrentPolygonGroupID = MeshDescription->CreatePolygonGroup();
				PolygonGroupImportedMaterialSlotNames[CurrentPolygonGroupID] = *FString::Printf(TEXT("mat_%d"), MaterialIndex);
				TArray<FVertexInstanceID> VertexInstanceIDs;
				VertexInstanceIDs.SetNum(3);
				for (size_t FaceID = 0; FaceID < Vertices.size()/3; FaceID++)
				{
					for (size_t VirtualVertexID = 0; VirtualVertexID <3; VirtualVertexID++)
					{
						static size_t VirtualVertices[3] ={0,2,1};
						size_t VertexID = VirtualVertices[ VirtualVertexID]+ FaceID*3;
						FVertexID VertexIDs = MeshDescription->CreateVertex();
						VertexPositions[VertexIDs].X = -Vertices[VertexID].Position.x * 100;
						VertexPositions[VertexIDs].Y = Vertices[VertexID].Position.z * 100;
						VertexPositions[VertexIDs].Z = Vertices[VertexID].Position.y * 100;
						VertexInstanceIDs[VirtualVertexID] = MeshDescription->CreateVertexInstance(VertexIDs);
						FVector2f UV;
						UV.X = Vertices[VertexID].UV.x;
						UV.Y = Vertices[VertexID].UV.y;
						VertexInstanceUVs.Set(VertexInstanceIDs[VirtualVertexID], 0, UV);
						VertexInstanceNormals[VertexInstanceIDs[VirtualVertexID]].X = -Vertices[VertexID].Normal.x;
						VertexInstanceNormals[VertexInstanceIDs[VirtualVertexID]].Y = Vertices[VertexID].Normal.z;
						VertexInstanceNormals[VertexInstanceIDs[VirtualVertexID]].Z = Vertices[VertexID].Normal.y;
					}
					const FPolygonID NewPolygonID = MeshDescription->CreatePolygon(CurrentPolygonGroupID, VertexInstanceIDs);
				}
				const FString SurfacePackageName = UPackageTools::SanitizePackageName(PackageName / TEXT("Materials") / FPaths::GetBaseFilename(FString(Object->Surfaces()[ElementID]->_Name())));
			;
				Materials.AddUnique(FStaticMaterial(ImportSurface(SurfacePackageName, Object->Surfaces()[ElementID]), *FString::Printf(TEXT("mat_%d"), MaterialIndex), *FString::Printf(TEXT("mat_%d"), MaterialIndex)));
			}
		}

		StaticMesh->CommitMeshDescription(LodIndex);
	}
	SourceModel.BuildSettings.bRecomputeNormals = false;
	SourceModel.BuildSettings.bGenerateLightmapUVs = true;
	SourceModel.BuildSettings.DstLightmapIndex = 1;
	SourceModel.BuildSettings.MinLightmapResolution = 128;
	StaticMesh->SetStaticMaterials(Materials);
	StaticMesh->Build();
	StaticMesh->MarkPackageDirty();
	StaticMesh->PostEditChange();
	return StaticMesh;
}


USkeletalMesh* XRayEngineFactory::ImportObjectAsDynamicMesh(CEditableObject* Object, bool UseOnlyFullPath /*= false*/)
{
	USkeletalMesh* SkeletalMesh = nullptr;

	FString FileName = Object->GetName();
	FString PackageName;
	if (UseOnlyFullPath)
	{
		FileName.ReplaceCharInline(TEXT('\\'), TEXT('/'));
		PackageName = UPackageTools::SanitizePackageName(GetGamePath() / TEXT("Meshes") / FPaths::GetBaseFilename(FileName, false));
	}
	else
	{
		PackageName = UPackageTools::SanitizePackageName(ParentPackage->GetName() / FPaths::GetBaseFilename(FileName));
	}
	const FString NewObjectPath = PackageName + TEXT(".") + FPaths::GetBaseFilename(PackageName);
	SkeletalMesh = LoadObject<USkeletalMesh>(nullptr, *NewObjectPath);
	if (SkeletalMesh)
		return SkeletalMesh;

	UPackage* AssetPackage = CreatePackage(*PackageName);
	SkeletalMesh = NewObject<USkeletalMesh>(AssetPackage, *FPaths::GetBaseFilename(PackageName), ObjectFlags);

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
	for (CBone* Bone: Object->Bones())
	{
		FindBoneIDOrAdd(InBones,Bone);
	}
	check(InBones.Num());
	check(InBones[0]->IsRoot());

	;
	for (int32 BoneIndex = 0; BoneIndex<Bones.Num(); BoneIndex++)
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
			xr_vector< st_MeshVertex> Vertices;
			for (size_t MeshID = 0; MeshID < Object->MeshCount(); MeshID++)
			{
				Object->Meshes()[MeshID]->GenerateVertices(Vertices, Object->Surfaces()[ElementID]);
			}
			if (Vertices.size() == 0)
			{
				continue;
			}
			const FString SurfacePackageName = UPackageTools::SanitizePackageName(PackageName / TEXT("Materials") / FPaths::GetBaseFilename(FString(Object->Surfaces()[ElementID]->_Name())));

		
			SkeletalMeshImportData::FMaterial Material;
			Material.Material = ImportSurface(SurfacePackageName, Object->Surfaces()[ElementID]);
			Material.MaterialImportName = Object->Surfaces()[ElementID]->_Name();
			InMaterials.Add(FSkeletalMaterial(Material.Material.Get(), true, false, FName(Material.MaterialImportName), FName(Material.MaterialImportName)));
			int32 MaterialID = InSkeletalMeshImportData.Materials.Add(Material);

			for (size_t FaceID = 0; FaceID < Vertices.size() / 3; FaceID++)
			{
				SkeletalMeshImportData::FTriangle Triangle;
				Triangle.MatIndex = MaterialID;
				for (size_t VirtualVertexID = 0; VirtualVertexID < 3; VirtualVertexID++)
				{
					SkeletalMeshImportData::FVertex Wedge;
					static size_t VirtualVertices[3] = { 0,2,1 };
					size_t VertexID = VirtualVertices[VirtualVertexID] + FaceID * 3;
					
					FVector3f VertexPositions;
					VertexPositions.X = -Vertices[VertexID].Position.x * 100;
					VertexPositions.Y = Vertices[VertexID].Position.z * 100;
					VertexPositions.Z = Vertices[VertexID].Position.y * 100;
					int32 OutVertexID = InSkeletalMeshImportData.Points.Add(VertexPositions);

					for (size_t InfluencesID = 0; InfluencesID < 4; InfluencesID++)
					{
						if (Vertices[VertexID].BoneID[InfluencesID] == 0xFFFF)
						{
							break;
						}
						SkeletalMeshImportData::FRawBoneInfluence BoneInfluence;
						BoneInfluence.BoneIndex = FindBoneIDOrAdd(InBones, Object->Bones()[Vertices[VertexID].BoneID[InfluencesID]]);
						BoneInfluence.Weight = Vertices[VertexID].BoneWeight[InfluencesID];
						BoneInfluence.VertexIndex = OutVertexID;
						InSkeletalMeshImportData.Influences.Add(BoneInfluence);
					}
					Wedge.VertexIndex = OutVertexID;
					Wedge.MatIndex = MaterialID;
					Wedge.UVs[0] = FVector2f(Vertices[VertexID].UV.x, Vertices[VertexID].UV.y);
					Triangle.WedgeIndex[VirtualVertexID] = InSkeletalMeshImportData.Wedges.Add(Wedge);
					Triangle.TangentZ[VirtualVertexID].X = -Vertices[VertexID].Normal.x;
					Triangle.TangentZ[VirtualVertexID].Y = Vertices[VertexID].Normal.z;
					Triangle.TangentZ[VirtualVertexID].Z = Vertices[VertexID].Normal.y;
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

	USkeleton * Skeleton = CreateSkeleton(PackageName+"_Skeleton", SkeletalMesh);

	Skeleton->SetPreviewMesh(SkeletalMesh);
	SkeletalMesh->SetSkeleton(Skeleton);

	SkeletalMesh->PostEditChange();
	CreatePhysicsAsset(PackageName + "_PhysicsAsset", SkeletalMesh, Object);
	CreateAnims(PackageName/FPaths::GetBaseFilename(PackageName) / TEXT("Anims"), Skeleton, Object);
	return SkeletalMesh;
}

UMaterialInterface* XRayEngineFactory::ImportSurface(const FString& Path, CSurface* Surface)
{
	if(!Surface->_Texture()||!Surface->_Texture()[0])
		return nullptr;
	ETextureThumbnail THM(Surface->_Texture());

	const FString NewObjectPath = Path + TEXT(".") + FPaths::GetBaseFilename(Path);
	UMaterialInterface*  Material = LoadObject<UMaterialInterface>(nullptr, *NewObjectPath);
	if (Material)
		return Material;
	FString ParentName = FString(Surface->_ShaderName());
	ParentName.ReplaceCharInline(TEXT('\\'), TEXT('/'));
	UMaterialInterface* ParentMaterial = nullptr;
	{
		const FString ParentPackageName = UPackageTools::SanitizePackageName(GetGamePath() / TEXT("Materials")/ ParentName);
		const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
		ParentMaterial = LoadObject<UMaterialInterface>(nullptr, *ParentObjectPath);
	}
	if(!IsValid(ParentMaterial))
	{
		const FString ParentPackageName = UPackageTools::SanitizePackageName(TEXT("/XRayEngine/Materials") / ParentName);
		const FString ParentObjectPath = ParentPackageName + TEXT(".") + FPaths::GetBaseFilename(ParentPackageName);
		ParentMaterial = LoadObject<UMaterialInterface>(nullptr, *ParentObjectPath);
	}
	if (!IsValid(ParentMaterial))
	{
		UMaterialInterface* UnkownMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/XRayEngine/Materials/Unkown.Unkown"));
		check (IsValid(UnkownMaterial));
		const FString ParentPackageName = UPackageTools::SanitizePackageName(GetGamePath() / TEXT("Materials") / ParentName);
		
		UPackage* AssetPackage = CreatePackage(*ParentPackageName);
		UMaterialInstanceConstant* NewParentMaterial = NewObject<UMaterialInstanceConstant>(AssetPackage, *FPaths::GetBaseFilename(ParentPackageName), ObjectFlags);
		NewParentMaterial->Parent = UnkownMaterial;
		FAssetRegistryModule::AssetCreated(NewParentMaterial);
		ObjectCreated.Add(NewParentMaterial);
		NewParentMaterial->PostEditChange();
		NewParentMaterial->MarkPackageDirty();
		ParentMaterial = NewParentMaterial;
	}
	UPackage* AssetPackage = CreatePackage(*Path);
	UMaterialInstanceConstant* NewMaterial = NewObject<UMaterialInstanceConstant>(AssetPackage, *FPaths::GetBaseFilename(Path), ObjectFlags);
	FAssetRegistryModule::AssetCreated(NewMaterial);
	ObjectCreated.Add(NewMaterial);
	NewMaterial->Parent = ParentMaterial;

	FStaticParameterSet NewStaticParameterSet;

	TObjectPtr<UTexture2D> BaseTexture = ImportTexture(Surface->_Texture());
	if (BaseTexture)
	{
		NewMaterial->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(TEXT("Default")), BaseTexture);
	}
	if (THM.Load(Surface->_Texture()))
	{
		TObjectPtr<UTexture2D> NormalMapTexture;
		TObjectPtr<UTexture2D> SpecularTexture;
		TObjectPtr<UTexture2D> HeightTexture;
		switch (THM._Format().material)
		{
		case STextureParams::tmPhong_Metal:
		{
			FStaticSwitchParameter SwitchParameter;
			SwitchParameter.ParameterInfo.Name = TEXT("IsPhongMetal");
			SwitchParameter.Value = true;
			SwitchParameter.bOverride = true;
			NewStaticParameterSet.EditorOnly.StaticSwitchParameters.Add(SwitchParameter);
		}
		break;
		case STextureParams::tmMetal_OrenNayar:
		{
			FStaticSwitchParameter SwitchParameter;
			SwitchParameter.ParameterInfo.Name = TEXT("IsMetalOrenNayar");
			SwitchParameter.Value = true;
			SwitchParameter.bOverride = true;
			NewStaticParameterSet.EditorOnly.StaticSwitchParameters.Add(SwitchParameter);
		}
		break;
		default:
			break;
		}
		if (THM._Format().bump_mode == STextureParams::tbmUse|| THM._Format().bump_mode == STextureParams::tbmUseParallax)
		{
			FStaticSwitchParameter SwitchParameter;
			SwitchParameter.ParameterInfo.Name = TEXT("UseBump");
			SwitchParameter.Value = true;
			SwitchParameter.bOverride = true;
			NewStaticParameterSet.EditorOnly.StaticSwitchParameters.Add(SwitchParameter);

			ImportBump2D(THM._Format().bump_name.c_str(), NormalMapTexture, SpecularTexture, HeightTexture);
			if (NormalMapTexture)
			{
				NewMaterial->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(TEXT("NormalMap")), NormalMapTexture);
			}
			if (SpecularTexture)
			{
				NewMaterial->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(TEXT("Specular")), SpecularTexture);
			}
			if (HeightTexture)
			{
				NewMaterial->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(TEXT("Height")), HeightTexture);
			}
			NewMaterial->SetScalarParameterValueEditorOnly(FMaterialParameterInfo(TEXT("ParallaxHeight")), THM._Format().bump_virtual_height/5.f);
		}
		if (THM._Format().detail_name.size())
		{
			TObjectPtr<UTexture2D> DetailTexture = ImportTexture(THM._Format().detail_name.c_str());
			TObjectPtr<UTexture2D> NormalMapTextureDetail;
			TObjectPtr<UTexture2D> SpecularTextureDetail;
			TObjectPtr<UTexture2D> HeightTextureDetail;

			ETextureThumbnail THMDetail(THM._Format().detail_name.c_str());
			if (THMDetail.Load(THM._Format().detail_name.c_str()))
			{
				FStaticSwitchParameter SwitchParameter;
				SwitchParameter.ParameterInfo.Name = TEXT("UseDetail");
				SwitchParameter.Value = true;
				SwitchParameter.bOverride = true;
				NewStaticParameterSet.EditorOnly.StaticSwitchParameters.Add(SwitchParameter);
				if (DetailTexture)
				{
					NewMaterial->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(TEXT("DetailDefault")), DetailTexture);
				}
				if (THMDetail._Format().bump_mode == STextureParams::tbmUse|| THMDetail._Format().bump_mode == STextureParams::tbmUse)
				{
					SwitchParameter.ParameterInfo.Name = TEXT("UseDetailBump");
					SwitchParameter.Value = true;
					SwitchParameter.bOverride = true;
					NewStaticParameterSet.EditorOnly.StaticSwitchParameters.Add(SwitchParameter);
					ImportBump2D(THMDetail._Format().bump_name.c_str(), NormalMapTextureDetail, SpecularTextureDetail, HeightTextureDetail);

					if (NormalMapTextureDetail)
					{
						NewMaterial->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(TEXT("DetailNormalMap")), NormalMapTextureDetail);
					}

					if (SpecularTextureDetail)
					{
						NewMaterial->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(TEXT("SpecularDetail")), SpecularTextureDetail);
					}

					NewMaterial->SetScalarParameterValueEditorOnly(FMaterialParameterInfo(TEXT("DetailScale")), THM._Format().detail_scale);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogXRayImporter, Warning, TEXT("Can't found thm %S"), Surface->_Texture());
	}
	NewMaterial->UpdateStaticPermutation(NewStaticParameterSet);
	NewMaterial->InitStaticPermutation();
	NewMaterial->PostEditChange();
	NewMaterial->MarkPackageDirty();
	return NewMaterial;
}

UTexture2D* XRayEngineFactory::ImportTextureTHM(const FString& InFileName)
{

	string_path TexturesGamedataPath;
	FCStringAnsi::Strcpy(TexturesGamedataPath,  FS.get_path(_game_textures_)->m_Path);
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
	while (FileName[0] &&  FileName[0] == TEXT('/'))
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
			TObjectPtr<UTexture2D> SpecularTexture;
			TObjectPtr<UTexture2D> HeightTexture;
			ImportBump2D(THM._Format().bump_name.c_str(), NormalMapTexture, SpecularTexture, HeightTexture);
		}
		if (THM._Format().detail_name.size())
		{
			TObjectPtr<UTexture2D> DetailTexture = ImportTexture(THM._Format().detail_name.c_str());
			TObjectPtr<UTexture2D> NormalMapTextureDetail;
			TObjectPtr<UTexture2D> SpecularTextureDetail;
			TObjectPtr<UTexture2D> HeightTextureDetail;

			ETextureThumbnail THMDetail(THM._Format().detail_name.c_str());
			if (THMDetail.Load(THM._Format().detail_name.c_str()))
			{
				
				if (THMDetail._Format().bump_mode == STextureParams::tbmUse || THMDetail._Format().bump_mode == STextureParams::tbmUseParallax)
				{
				
					ImportBump2D(THMDetail._Format().bump_name.c_str(), NormalMapTextureDetail, SpecularTextureDetail, HeightTextureDetail);
				}
			}
		}
		return BaseTexture;
	}
	return nullptr;
}

UTexture2D* XRayEngineFactory::ImportTexture(const FString& FileName)
{
	UTexture2D*   Texture2D = nullptr;
	 
	FString PackageFileName = FPaths::ChangeExtension(FileName, TEXT(""));
	PackageFileName.ReplaceCharInline(TEXT('\\'), TEXT('/'));
	const FString PackageName = UPackageTools::SanitizePackageName(GetGamePath() / TEXT("Textures") / PackageFileName);
	const FString NewObjectPath = PackageName + TEXT(".") + FPaths::GetBaseFilename(PackageName);

	Texture2D = LoadObject<UTexture2D>(nullptr, *NewObjectPath);
	if (Texture2D)
		return Texture2D;

	RedImageTool::RedImage Image;
	string_path DDSFileName;
	FS.update_path(DDSFileName, _game_textures_, ChangeFileExt(TCHAR_TO_ANSI(*FileName), ".dds").c_str());
	if (!Image.LoadFromFile(DDSFileName))
	{
		return nullptr;
	}

	UPackage* AssetPackage = CreatePackage(*PackageName);
	Texture2D = NewObject<UTexture2D>(AssetPackage, *FPaths::GetBaseFilename(PackageName), ObjectFlags);
	FAssetRegistryModule::AssetCreated(Texture2D);
	ObjectCreated.Add(Texture2D);
	Image.Convert(RedImageTool::RedTexturePixelFormat::R8G8B8A8);
	size_t CountPixel = Image.GetSizeInMemory()/4;
	for (size_t x = 0; x < CountPixel; x++)
	{
		uint8*Pixel = ((uint8*)*Image) + x*4;
		Swap(Pixel[0], Pixel[2]);

	}
	ETextureSourceFormat SourceFormat = ETextureSourceFormat::TSF_BGRA8;
	Texture2D->Source.Init(Image.GetWidth(), Image.GetHeight(), 1, Image.GetMips(), SourceFormat, (uint8*)*Image);
	Texture2D->MarkPackageDirty();
	Texture2D->PostEditChange();
	return Texture2D;
}

void XRayEngineFactory::ImportBump2D(const FString& FileName, TObjectPtr<UTexture2D>& NormalMap, TObjectPtr<UTexture2D>& Specular, TObjectPtr<UTexture2D>& Height)
{
	FString PackageFileName = FPaths::ChangeExtension(FileName, TEXT(""));
	PackageFileName.ReplaceCharInline(TEXT('\\'), TEXT('/'));
	const FString PackageName = UPackageTools::SanitizePackageName(GetGamePath()/TEXT("Textures") / PackageFileName);
	const FString NewObjectPath = PackageName + TEXT(".") + FPaths::GetBaseFilename(PackageName);


	FString PackageFileNameSpecular = FPaths::ChangeExtension(FileName, TEXT(""))+TEXT("_specular");
	PackageFileNameSpecular.ReplaceCharInline(TEXT('\\'), TEXT('/'));
	const FString PackageNameSpecular = UPackageTools::SanitizePackageName(GetGamePath() / TEXT("Textures") / PackageFileNameSpecular);
	const FString NewObjectPathSpecular = PackageNameSpecular + TEXT(".") + FPaths::GetBaseFilename(PackageNameSpecular);

	FString PackageFileNameHeight = FPaths::ChangeExtension(FileName, TEXT("")) + TEXT("_height");
	PackageFileNameHeight.ReplaceCharInline(TEXT('\\'), TEXT('/'));
	const FString PackageNameHeight = UPackageTools::SanitizePackageName(GetGamePath() / TEXT("Textures") / PackageFileNameHeight);
	const FString NewObjectPathHeight = PackageNameHeight + TEXT(".") + FPaths::GetBaseFilename(PackageNameHeight);

	NormalMap = LoadObject<UTexture2D>(nullptr, *NewObjectPath);
	Specular = LoadObject<UTexture2D>(nullptr, *NewObjectPathSpecular);
	Height = LoadObject<UTexture2D>(nullptr, *NewObjectPathHeight);
	if (NormalMap&& Specular&& Height)
		return;

	RedImageTool::RedImage ImageBump, ImageBumpError;
	string_path DDSFileName;
	FS.update_path(DDSFileName, _game_textures_, ChangeFileExt(TCHAR_TO_ANSI(*FileName), ".dds").c_str());
	if (!ImageBump.LoadFromFile(DDSFileName))
	{
		return;
	}

	FS.update_path(DDSFileName, _game_textures_, ChangeFileExt(TCHAR_TO_ANSI(*FileName), "#.dds").c_str());
	if (!ImageBumpError.LoadFromFile(DDSFileName))
	{
		return;
	}
	if (ImageBump.GetWidth() != ImageBumpError.GetWidth())return;
	if (ImageBump.GetHeight() != ImageBumpError.GetHeight())return;

	RedImageTool::RedImage NormalMapImage;
	RedImageTool::RedImage SpecularImage;
	RedImageTool::RedImage HeightImage;

	ImageBump.ClearMipLevels();
	ImageBump.Convert(RedImageTool::RedTexturePixelFormat::R8G8B8A8);
	ImageBumpError.ClearMipLevels();
	ImageBumpError.Convert(RedImageTool::RedTexturePixelFormat::R8G8B8A8);

	NormalMapImage.Create(ImageBump.GetWidth(), ImageBump.GetHeight());
	SpecularImage.Create(ImageBump.GetWidth(), ImageBump.GetHeight(),1,1,RedImageTool::RedTexturePixelFormat::R8);
	HeightImage.Create(ImageBump.GetWidth(), ImageBump.GetHeight(), 1,1,RedImageTool::RedTexturePixelFormat::R8);

	for (size_t x = 0; x < ImageBump.GetWidth(); x++)
	{
		for (size_t y = 0; y < ImageBump.GetHeight(); y++)
		{
			RedImageTool::RedColor Pixel = ImageBump.GetPixel(x, y);
			RedImageTool::RedColor PixelError = ImageBumpError.GetPixel(x, y);
			RedImageTool::RedColor  NormalMapPixel, SpecularPixel, HeightPixel;

			NormalMapPixel.R32F = Pixel.A32F + (PixelError.R32F - 0.5f);
			NormalMapPixel.G32F = Pixel.B32F + (PixelError.G32F - 0.5f);
			NormalMapPixel.B32F = Pixel.G32F + (PixelError.B32F - 0.5f);
			NormalMapPixel.A32F = 1.f;
			NormalMapPixel.SetAsFloat(NormalMapPixel.R32F, NormalMapPixel.G32F, NormalMapPixel.B32F, NormalMapPixel.A32F);
			SpecularPixel.SetAsFloat(Pixel.R32F, Pixel.R32F, Pixel.R32F, Pixel.R32F);
			HeightPixel.SetAsFloat(PixelError.A32F, PixelError.A32F, PixelError.A32F, PixelError.A32F);
			NormalMapImage.SetPixel(NormalMapPixel, x, y);
			SpecularImage.SetPixel(SpecularPixel, x, y);
			HeightImage.SetPixel(HeightPixel, x, y);
		}
	}
	float MaxHeight = 0;
	for (size_t x = 0; x < ImageBump.GetWidth(); x++)
	{
		for (size_t y = 0; y < ImageBump.GetHeight(); y++)
		{
			RedImageTool::RedColor Pixel = HeightImage.GetPixel(x, y);
			MaxHeight = std::max(Pixel.R32F, MaxHeight);
		}
	}
	for (size_t x = 0; x < ImageBump.GetWidth(); x++)
	{
		for (size_t y = 0; y < ImageBump.GetHeight(); y++)
		{
			RedImageTool::RedColor Pixel = HeightImage.GetPixel(x, y);
			Pixel.R32F = Pixel.R32F/ MaxHeight;
			Pixel.SetAsFloat(Pixel.R32F, Pixel.R32F, Pixel.R32F, Pixel.R32F);
			HeightImage.SetPixel(Pixel,x,y);
		}
	}
	NormalMapImage.SwapRB();
	//SpecularImage.SwapRB();
	//HeightImage.SwapRB();
	if (!NormalMap)
	{
		UPackage* AssetPackage = CreatePackage(*PackageName);
		NormalMap = NewObject<UTexture2D>(AssetPackage, *FPaths::GetBaseFilename(PackageName), ObjectFlags);
		FAssetRegistryModule::AssetCreated(NormalMap);
		ObjectCreated.Add(NormalMap);
		ETextureSourceFormat SourceFormat = ETextureSourceFormat::TSF_BGRA8;
		NormalMapImage.GenerateMipmap();
		NormalMap->CompressionSettings = TextureCompressionSettings::TC_Normalmap;
		NormalMap->SRGB = false;
		NormalMap->Source.Init(NormalMapImage.GetWidth(), NormalMapImage.GetHeight(), 1, NormalMapImage.GetMips(), SourceFormat, (uint8*)*NormalMapImage);
		NormalMap->MarkPackageDirty();
		NormalMap->PostEditChange();
	}
	if (!Specular)
	{
		UPackage* AssetPackage = CreatePackage(*PackageNameSpecular);
		Specular = NewObject<UTexture2D>(AssetPackage, *FPaths::GetBaseFilename(PackageNameSpecular), ObjectFlags);
		FAssetRegistryModule::AssetCreated(Specular);
		ObjectCreated.Add(Specular);
		ETextureSourceFormat SourceFormat = ETextureSourceFormat::TSF_G8;
		SpecularImage.GenerateMipmap();
		Specular->CompressionSettings = TextureCompressionSettings::TC_Alpha;
		Specular->SRGB = false;
		Specular->Source.Init(SpecularImage.GetWidth(), SpecularImage.GetHeight(), 1, SpecularImage.GetMips(), SourceFormat, (uint8*)*SpecularImage);
		Specular->MarkPackageDirty();
		Specular->PostEditChange();
	}
	if (!Height)
	{
		
		UPackage* AssetPackage = CreatePackage(*PackageNameHeight);
		Height = NewObject<UTexture2D>(AssetPackage, *FPaths::GetBaseFilename(PackageNameHeight), ObjectFlags);
		FAssetRegistryModule::AssetCreated(Height);
		ObjectCreated.Add(Height);
		ETextureSourceFormat SourceFormat = ETextureSourceFormat::TSF_G8;
		HeightImage.GenerateMipmap();
		Height->CompressionSettings = TextureCompressionSettings::TC_Alpha;
		Height->SRGB = false;
		Height->Source.Init(HeightImage.GetWidth(), HeightImage.GetHeight(), 1, HeightImage.GetMips(), SourceFormat, (uint8*)*HeightImage);
		Height->MarkPackageDirty();
		Height->PostEditChange();
	}
}

int32 XRayEngineFactory::FindBoneIDOrAdd(TArray<CBone*>& InBones, CBone* InBone)
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

bool XRayEngineFactory::CreateSkeletalMesh(USkeletalMesh* SkeletalMesh, TArray<FSkeletalMeshImportData>& LODIndexToSkeletalMeshImportData, const TArray<SkeletalMeshImportData::FBone>& InSkeletonBones)
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

USkeleton* XRayEngineFactory::CreateSkeleton(const FString& FullName, USkeletalMesh* InMesh)
{
	const FString NewObjectPath = FullName + TEXT(".") + FPaths::GetBaseFilename(FullName);
	USkeleton* Skeleton = LoadObject<USkeleton>(nullptr, *NewObjectPath);
	if (Skeleton)
	{
		return Skeleton;
	}
	UPackage* AssetPackage = CreatePackage(*FullName);
	Skeleton = NewObject<USkeleton>(AssetPackage, *FPaths::GetBaseFilename(FullName), ObjectFlags);
	Skeleton->MergeAllBonesToBoneTree(InMesh);
	ObjectCreated.Add(Skeleton);
	FAssetRegistryModule::AssetCreated(Skeleton);
	return Skeleton;
}

void XRayEngineFactory::CreateAnims(const FString& FullName, USkeleton* InMesh, CEditableObject* Object)
{
	for (CSMotion* Motion : Object->SMotions())
	{
		CreateAnims(FullName,InMesh, Motion);
	}
}

void XRayEngineFactory::CreateAnims(const FString& FullName, USkeleton* Skeleton, CSMotion* InMotion)
{
	const FString MotionPath = FullName / FString( InMotion->name.c_str());
	const FString MotionFullPath = MotionPath + TEXT(".") + FPaths::GetBaseFilename(MotionPath);
	UAnimSequence* Anim = LoadObject<UAnimSequence>(nullptr, *MotionFullPath);
	if (IsValid(Anim))
	{
		return;
	}
	UPackage* AssetPackage = CreatePackage(*MotionPath);
	UAnimSequence* AnimSequence = NewObject<UAnimSequence>(AssetPackage, *FPaths::GetBaseFilename(MotionPath), ObjectFlags);

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
	Controller.OpenBracket(LOCTEXT("XRayEngineFactoryImporterAnimData_Bracket", "XRayEngineFactory Animation Data"), bShouldTransact);
	Controller.ResetModel(bShouldTransact);

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
			RotKey = FQuat4f(InQuat.x, -InQuat.z, -InQuat.y, InQuat.w);
			PosKeys.Add(FVector(PosKey.X, PosKey.Y, PosKey.Z));
			RotKeys.Add(FQuat(RotKey.X, RotKey.Y, RotKey.Z, RotKey.W));
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

	AnimSequence->PostEditChange();
	AnimSequence->MarkPackageDirty();
	ObjectCreated.Add(AnimSequence);
	FAssetRegistryModule::AssetCreated(AnimSequence);
}

void XRayEngineFactory::CreatePhysicsAsset(const FString& FullName, USkeletalMesh* InMesh, CEditableObject* Object)
{
	const FString NewObjectPath = FullName + TEXT(".") + FPaths::GetBaseFilename(FullName);
	UPhysicsAsset* PhysicsAsset = LoadObject<UPhysicsAsset>(nullptr, *NewObjectPath);
	if (PhysicsAsset)
	{
		return;
	}
	UPackage* AssetPackage = CreatePackage(*FullName);
	PhysicsAsset = NewObject<UPhysicsAsset>(AssetPackage, *FPaths::GetBaseFilename(FullName), ObjectFlags);

	for (CBone* Bone : Object->Bones())
	{
		
		TObjectPtr<USkeletalBodySetup> NewBodySetup = NewObject<USkeletalBodySetup>(PhysicsAsset);
		NewBodySetup->BodySetupGuid = FGuid::NewGuid();
		NewBodySetup->BoneName = Bone->Name().c_str();
		switch (Bone->shape.type)
		{
		case SBoneShape::EShapeType::stBox:
		{
			FKBoxElem Box;
			Box.Center = FVector(-Bone->shape.box.m_translate.x * 100, Bone->shape.box.m_translate.z * 100, Bone->shape.box.m_translate.y * 100);
			Box.X = Bone->shape.box.m_halfsize.x * 200;
			Box.Y = Bone->shape.box.m_halfsize.z * 200;
			Box.Z = Bone->shape.box.m_halfsize.y * 200;
			Fmatrix InRotation;
			Bone->shape.box.xform_get(InRotation);
			Fquaternion InQuat; InQuat.set(InRotation);
			Box.Rotation = FRotator(FQuat(InQuat.x, -InQuat.z, -InQuat.y, InQuat.w));
			NewBodySetup->AggGeom.BoxElems.Add(Box);
			NewBodySetup->CalculateMass()
			break;
		}

		case SBoneShape::EShapeType::stCylinder:
		{
			FKSphylElem Sphyl;
			Sphyl.Center = FVector(-Bone->shape.cylinder.m_center.x * 100, Bone->shape.cylinder.m_center.z * 100, Bone->shape.cylinder.m_center.y * 100);
			Sphyl.Radius = Bone->shape.cylinder.m_radius * 100;
			Sphyl.Length = Bone->shape.cylinder.m_height * 100;
			Fmatrix InRotation;
			InRotation.k.set(Bone->shape.cylinder.m_direction);
			Fvector::generate_orthonormal_basis(InRotation.k, InRotation.j, InRotation.i);
			Fquaternion InQuat; InQuat.set(InRotation);
			FRotator RotationToUnreal(0, 0, -90);
			FRotator OutRotation(FQuat(InQuat.x, -InQuat.z, -InQuat.y, InQuat.w) * FQuat(RotationToUnreal));
			Sphyl.Rotation = OutRotation;
			NewBodySetup->AggGeom.SphylElems.Add(Sphyl);
		break;
		}
		case SBoneShape::EShapeType::stSphere:
		{
			FKSphereElem Sphere;
			Sphere.Center = FVector( -Bone->shape.sphere.P.x * 100, Bone->shape.sphere.P.z * 100, Bone->shape.sphere.P.y * 100);
			Sphere.Radius = Bone->shape.sphere.R*100;
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

		NewBodySetup->DefaultInstance.SetMassOverride(Bone->mass);
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

FString XRayEngineFactory::GetGamePath()
{
	switch (xrGameManager::GetGame())
	{
	case EGame::CS:
		return TEXT("/Game/CS");
	case EGame::SHOC:
		return TEXT("/Game/SHOC");
	default:
		return TEXT("/Game/COP");
	}
}
#undef LOCTEXT_NAMESPACE 