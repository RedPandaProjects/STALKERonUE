#include "XRayEngineFactory.h"


XRayEngineFactory::XRayEngineFactory(UObject* InParentPackage, EObjectFlags InFlags)
{
	ParentPackage = InParentPackage;
	ObjectFlags = InFlags;
}

XRayEngineFactory::~XRayEngineFactory()
{
	for (CEditableObject* Object :Objects)
	{
		Lib.RemoveEditObject(Object);
	}
}

UObject* XRayEngineFactory::ImportObject(const FString& FileName, bool UseOnlyFullPath)
{
	CEditableObject* Object =  Lib.CreateEditObject(TCHAR_TO_ANSI(*FileName));
	if (Object)
	{
		Objects.Add(Object); 
		return ImportObjectAsStaticMesh(Object, UseOnlyFullPath); 
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
		PackageName = UPackageTools::SanitizePackageName(TEXT("/Game/Meshes") / FPaths::GetBaseFilename(FileName,false));
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
				xr_vector< st_StaticMeshVertex> Vertices;
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
		const FString ParentPackageName = UPackageTools::SanitizePackageName(TEXT("/Game/Materials")/ ParentName);
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
		const FString ParentPackageName = UPackageTools::SanitizePackageName(TEXT("/Game/Materials") / ParentName);
		
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
			NewStaticParameterSet.StaticSwitchParameters.Add(SwitchParameter);
		}
		break;
		case STextureParams::tmMetal_OrenNayar:
		{
			FStaticSwitchParameter SwitchParameter;
			SwitchParameter.ParameterInfo.Name = TEXT("IsMetalOrenNayar");
			SwitchParameter.Value = true;
			SwitchParameter.bOverride = true;
			NewStaticParameterSet.StaticSwitchParameters.Add(SwitchParameter);
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
			NewStaticParameterSet.StaticSwitchParameters.Add(SwitchParameter);

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
				NewStaticParameterSet.StaticSwitchParameters.Add(SwitchParameter);
				if (DetailTexture)
				{
					NewMaterial->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(TEXT("DetailDefault")), DetailTexture);
				}
				if (THM._Format().bump_mode == STextureParams::tbmUse)
				{
					SwitchParameter.ParameterInfo.Name = TEXT("UseDetailBump");
					SwitchParameter.Value = true;
					SwitchParameter.bOverride = true;
					NewStaticParameterSet.StaticSwitchParameters.Add(SwitchParameter);
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

UTexture2D* XRayEngineFactory::ImportTexture(const FString& FileName)
{
	UTexture2D*   Texture2D = nullptr;
	 
	FString PackageFileName = FPaths::ChangeExtension(FileName, TEXT(""));
	PackageFileName.ReplaceCharInline(TEXT('\\'), TEXT('/'));
	const FString PackageName = UPackageTools::SanitizePackageName(TEXT("/Game/Textures") / PackageFileName);
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
	const FString PackageName = UPackageTools::SanitizePackageName(TEXT("/Game/Textures") / PackageFileName);
	const FString NewObjectPath = PackageName + TEXT(".") + FPaths::GetBaseFilename(PackageName);


	FString PackageFileNameSpecular = FPaths::ChangeExtension(FileName, TEXT(""))+TEXT("_specular");
	PackageFileNameSpecular.ReplaceCharInline(TEXT('\\'), TEXT('/'));
	const FString PackageNameSpecular = UPackageTools::SanitizePackageName(TEXT("/Game/Textures") / PackageFileNameSpecular);
	const FString NewObjectPathSpecular = PackageNameSpecular + TEXT(".") + FPaths::GetBaseFilename(PackageNameSpecular);

	FString PackageFileNameHeight = FPaths::ChangeExtension(FileName, TEXT("")) + TEXT("_height");
	PackageFileNameHeight.ReplaceCharInline(TEXT('\\'), TEXT('/'));
	const FString PackageNameHeight = UPackageTools::SanitizePackageName(TEXT("/Game/Textures") / PackageFileNameHeight);
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
