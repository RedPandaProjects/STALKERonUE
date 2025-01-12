#include "RBMKEngine.h"

#include "MoviePlayer.h"
#include "MoviePlayerProxy.h"
#include "RBMKDevice.h"
#include "../Render/Interface/XRayRenderInterface.h"
#include "../Render/Interface/XRayRenderFactory.h"
#include "../Render/Interface/XRayDUInterface.h"
#include "../Render/Interface/XRayDebugRender.h"
#include "../Render/Interface/UI/XRayUIRender.h"
#include "RBMKInput.h"
#include "../../StalkerEngineManager.h"
#include "RBMKConsole.h"
#include "Entities/Levels/Proxy/StalkerProxy.h"
#include "Resources/StalkerResourcesManager.h"
#include "../../Unreal/WorldSettings/StalkerWorldSettings.h"
#include "Entities/Levels/Environment/StalkerEnvironment.h"
#include "Resources/CFrom/StalkerCForm.h"
#include "Resources/PhysicalMaterial/StalkerPhysicalMaterialsManager.h"
#include "Resources/PhysicalMaterial/StalkerPhysicalMaterial.h"
#include "Resources/AIMap/StalkerAIMap.h"
#include "Resources/Spawn/StalkerGameSpawn.h"
#include "Entities/Player/Character/StalkerPlayerCharacter.h"
#include "Resources/Sound/StalkerSoundManager.h"
#include "Sound/SoundClass.h"
#include "DynamicRHI.h"
#include "Kernel/Unreal/GameSettings/StalkerGameSettings.h"
THIRD_PARTY_INCLUDES_START
#include "XrCDB/xr_area.h"
THIRD_PARTY_INCLUDES_END
XRayRenderFactory GRenderFactory;
XRayDUInterface  GDUInterface;
XRayDebugRender GDebugRender;
FRBMKEngine::FRBMKEngine()
{

}

void FRBMKEngine::Initialize()
{
	Device = new FRBMKDevice;
	Console = new  FRBMKConsole;

	FRBMKInput* MyInput = new FRBMKInput;
	GStalkerEngineManager->SetInput(MyInput);
	GXRayInput = MyInput;
	::Render = &GRenderInterface;
	::RenderFactory = &GRenderFactory;
	::DU = &GDUInterface;
	UIRender = &GXRayUIRender;
	DRender = &GDebugRender;
	IRBMKEngine::Initialize();
}

void FRBMKEngine::Destroy()
{
	GStalkerEngineManager->SetInput(nullptr);
	IRBMKEngine::Destroy();
	delete GXRayInput;
	delete Console;
	delete Device;
	Console	= nullptr;
	Device = nullptr;
	GXRayInput = nullptr;
}

void FRBMKEngine::OnFrame()
{
	IRBMKEngine::OnFrame();
}

void FRBMKEngine::Destroy(class IRBMKUnrealProxy*InProxy)
{
	if (!InProxy)
	{
		return;
	}
	AStalkerProxy * StalkerProxy =  reinterpret_cast<	AStalkerProxy *>(InProxy->CastUnrealObject(ERBMKUnrealObjectType::StalkerProxy));
	StalkerProxy->Destroy();
}

class ILevelGraph* FRBMKEngine::GetLevelGraph(const char* Name)
{
	return GStalkerEngineManager->GetLevelGraph(Name);
}

class IGameGraph* FRBMKEngine::GetGameGraph()
{
	UStalkerGameSpawn*GameSpawn =  GStalkerEngineManager->GetResourcesManager()->GetGameSpawn();
	if (IsValid(GameSpawn))
	{
		GameSpawn->GameGraph.ResetEnabled();
		return &GameSpawn->GameGraph;
	}
	return nullptr;
}

IReader FRBMKEngine::GetGameSpawn()
{
	UStalkerGameSpawn* GameSpawn = GStalkerEngineManager->GetResourcesManager()->GetGameSpawn();
	if (IsValid(GameSpawn))
	{
		return  { GameSpawn->SpawnData.GetData(),GameSpawn->SpawnData.Num() };;
	}
	return {nullptr,0};
}

bool FRBMKEngine::LoadWorld(const char* Name)
{
	return GStalkerEngineManager->LoadWorld(Name);
}

class IRBMKUnrealProxy* FRBMKEngine::CreateUnrealProxy(const char*UnrealProxyClass)
{
#if WITH_EDITORONLY_DATA
	check(GWorld&&GWorld->IsGameWorld());
#endif
	UClass* ProxyClass = nullptr;
	if (UnrealProxyClass)
	{
		ProxyClass = LoadObject<UClass>(nullptr,ANSI_TO_TCHAR(UnrealProxyClass));
	}
	if (!ProxyClass)
	{
		ProxyClass = GetDefault<UStalkerGameSettings>()->ProxyClass.LoadSynchronous();
	}
	if (!ProxyClass)
	{
		ProxyClass = AStalkerProxy::StaticClass();
	}
	FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
	SpawnParameters.ObjectFlags = EObjectFlags::RF_Transient;
	AStalkerProxy* Result = GWorld->SpawnActor< AStalkerProxy>(ProxyClass,SpawnParameters);
	return Result;
}
void FRBMKEngine::LoadCFormFormCurrentWorld(class CObjectSpace& ObjectSpace, CDB::build_callback build_callback)
{
#if WITH_EDITORONLY_DATA
	check(GWorld&&GWorld->IsGameWorld());
#endif
	UWorld*World = GWorld;
	check(World);
	AStalkerWorldSettings* StalkerWorldSettings = Cast<AStalkerWorldSettings>(World->GetWorldSettings());
	check(StalkerWorldSettings);
	UStalkerCForm* CForm = StalkerWorldSettings->GetCForm();
	check(IsValid(CForm)); 
	check (IsValid(GStalkerEngineManager->GetPhysicalMaterialsManager()->DefaultPhysicalMaterial));
	static_assert(sizeof(FVector3f)==sizeof(Fvector));

	TMap<uint32,uint32>		CFormMaterialID2GlobalID;
	for (uint32 Index=0; Index <static_cast<uint32>(CForm->Name2ID.Num()); Index++)
	{
		FString& Name = CForm->Name2ID[Index];
		shared_str LegacyName = TCHAR_TO_ANSI(*Name);
		int32 GlobalIndex =  GStalkerEngineManager->GetPhysicalMaterialsManager()->Names.IndexOfByKey(LegacyName);
		if (GlobalIndex == INDEX_NONE)
		{
			UE_LOG(LogStalker,Error,TEXT("Can't found material %s in time build cform"),*Name);
			GlobalIndex = GStalkerEngineManager->GetPhysicalMaterialsManager()->PhysicalMaterials.IndexOfByKey(GStalkerEngineManager->GetPhysicalMaterialsManager()->DefaultPhysicalMaterial);
			check(GlobalIndex!= INDEX_NONE);
		}
		if (GStalkerEngineManager->GetPhysicalMaterialsManager()->GetMaterialByID(GlobalIndex)->Flags.is(SGameMtl::flDynamic))
		{
			GlobalIndex = GStalkerEngineManager->GetPhysicalMaterialsManager()->PhysicalMaterials.IndexOfByKey(GStalkerEngineManager->GetPhysicalMaterialsManager()->DefaultPhysicalMaterial);
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
		LegacyCFormVertices.Add(StalkerMath::UnrealLocationToRBMK(Vertex));
	}
	hdrCFORM LegacyCFormHeader={};
	LegacyCFormHeader.aabb.invalidate();
	LegacyCFormHeader.aabb.modify(StalkerMath::UnrealLocationToRBMK(CForm->AABB.Max));
	LegacyCFormHeader.aabb.modify(StalkerMath::UnrealLocationToRBMK(CForm->AABB.Min));
	LegacyCFormHeader.vertcount = LegacyCFormVertices.Num();
	LegacyCFormHeader.facecount = LegacyCFormTriangles.Num();
	LegacyCFormHeader.version = CFORM_CURRENT_VERSION;

	ObjectSpace.Create(LegacyCFormVertices.GetData(), LegacyCFormTriangles.GetData(), LegacyCFormHeader, build_callback);
}

ERBMKWorldStatus FRBMKEngine::GetWorldStatus()
{
	switch (GStalkerEngineManager->GetWorldStatus())
	{
	default:
		return ERBMKWorldStatus::Failure;
		break;
	case EStalkerWorldStatus::None:
		return ERBMKWorldStatus::None;
		break;
	case EStalkerWorldStatus::Ready:
		return ERBMKWorldStatus::Ready;
		break;
	case EStalkerWorldStatus::Loading:
		return ERBMKWorldStatus::Loading;
		break;
	case EStalkerWorldStatus::Failure:
		return ERBMKWorldStatus::Failure;
		break;
	} 
}

class IRBMKUnrealProxy* FRBMKEngine::GetUnrealPlayerCharacter()
{
	if (IsValid(GWorld))
	{
#if WITH_EDITORONLY_DATA
		check(GWorld->IsGameWorld());
#endif
		if (APlayerController* PC = GWorld->GetFirstPlayerController())
		{
			return Cast<AStalkerPlayerCharacter>(PC->GetPawn());
		}
	}

	return nullptr;
}

shared_str FRBMKEngine::GetUnrealVersion()
{
	return TCHAR_TO_ANSI(FApp::GetBuildVersion()); 
}

IRBMKSoundManager* FRBMKEngine::GetSoundManager()
{
	return GStalkerEngineManager->GetResourcesManager()->GetSoundManager();
}

IRBMKEnvironment* FRBMKEngine::GetEnvironment()
{
#if WITH_EDITORONLY_DATA
	check(GWorld&&GWorld->IsGameWorld());
#endif
	if(AStalkerWorldSettings*WorldSettings = Cast<AStalkerWorldSettings>(GWorld->GetWorldSettings()))
	{
		if(!WorldSettings->Environment)
		{
			UE_DEBUG_BREAK();
		}
		return WorldSettings->Environment;
	}
	return nullptr;
}

void FRBMKEngine::OnRunGame(const char* ServerParams, const char* ClientParams)
{
	GStalkerEngineManager->GetResourcesManager()->LoadingScreenManager.Play(FCStringAnsi::Strstr(ServerParams,"/load") == nullptr,9);

	IRBMKEngine::OnRunGame(ServerParams, ClientParams);
	if(!GIsEditor)
	{
		while (g_loading_events->size())
		{
			if (g_loading_events->front()())
			{
				g_loading_events->pop_front();
			}
			else
			{
				GEngine->Tick(FApp::GetDeltaTime(), false);
			}
		}
		GEngine->Tick(FApp::GetDeltaTime(), false);
	}
	GStalkerEngineManager->GetResourcesManager()->LoadingScreenManager.Wait();
	
}

IRBMKLoadingScreenManager* FRBMKEngine::GetLoadingScreen()
{
	return &GStalkerEngineManager->GetResourcesManager()->LoadingScreenManager;
}

void FRBMKEngine::LoadDefaultWorld()
{	
	GStalkerEngineManager->LoadDefaultWorld();
}

void FRBMKEngine::Exit()
{	
	GEngine->Exec(nullptr, TEXT( "Exit" ) );
}


void FRBMKEngine::ExecUeCmd(const char* cmd)
{
	GEngine->Exec(NULL, ANSI_TO_TCHAR(cmd));
}

void FRBMKEngine::ChangeUeSettingsInt(const std::pair<int, int>& settinglist)
{

		UE_LOG(LogStalker, Display, TEXT("ChangeUeSettings: %d, %d"), settinglist.first, settinglist.second);

		UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();

		switch (settinglist.first)
		{
		case ShadowQuality:
		{
			UserSettings->SetShadowQuality(settinglist.second);
		}break;
		case ViewDistanceQuality:
		{
			UserSettings->SetViewDistanceQuality(settinglist.second);
		}break;
		case GlobalIlluminationQuality:
		{
			UserSettings->SetGlobalIlluminationQuality(settinglist.second);
		}break;
		case AntiAliasingQuality:
		{
			UserSettings->SetAntiAliasingQuality(settinglist.second);
		}break;
		case ReflectionQuality:
		{
			UserSettings->SetReflectionQuality(settinglist.second);
		}break;
		case PostProcessingQuality:
		{
			UserSettings->SetPostProcessingQuality(settinglist.second);
		}break;
		case TextureQuality:
		{
			UserSettings->SetTextureQuality(settinglist.second);
		}break;
		case VisualEffectsQuality:
		{
			UserSettings->SetVisualEffectQuality(settinglist.second);
		}break;
		case FoliageQuality:
		{
			UserSettings->SetFoliageQuality(settinglist.second);
		}break;
		case ShadingQuality:
		{
			UserSettings->SetShadingQuality(settinglist.second);
		}break;
		case FrameLimit:
		{
			UserSettings->SetFrameRateLimit(settinglist.second);
		}break;
		default:
			UE_LOG(LogStalker, Error, TEXT("unknown setting"));
			return;
		}

		UserSettings->ApplySettings(true);
}

int FRBMKEngine::GetSettingInt(int setting, int& min, int& max)
{
	UE_LOG(LogStalker, Display, TEXT("GetSetting: %d"), setting);

	UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();

	switch (setting)
	{
	case ShadowQuality:
	{
		min = 0;
		max = 4;
		return UserSettings->GetShadowQuality();
	}
	case ViewDistanceQuality:
	{
		min = 0;
		max = 4;
		return UserSettings->GetViewDistanceQuality();
	}
	case GlobalIlluminationQuality:
	{
		min = 0;
		max = 4;
		return UserSettings->GetGlobalIlluminationQuality();
	}
	case AntiAliasingQuality:
	{
		min = 0;
		max = 4;
		return UserSettings->GetAntiAliasingQuality();
	}
	case ReflectionQuality:
	{
		min = 0;
		max = 4;
		return UserSettings->GetReflectionQuality();
	}
	case PostProcessingQuality:
	{
		min = 0;
		max = 4;
		return UserSettings->GetPostProcessingQuality();
	}
	case TextureQuality:
	{
		min = 0;
		max = 4;
		return UserSettings->GetTextureQuality();
	}
	case VisualEffectsQuality:
	{
		min = 0;
		max = 4;
		return UserSettings->GetVisualEffectQuality();
	}
	case FoliageQuality:
	{
		min = 0;
		max = 4;
		return UserSettings->GetFoliageQuality();
	}
	case ShadingQuality:
	{
		min = 0;
		max = 4;
		return UserSettings->GetShadingQuality();
	}
	case FrameLimit:
	{
		min = 0;
		max = 300;
		return UserSettings->GetFrameRateLimit();
	}
	default:
		min = 0;
		max = 1;
		UE_LOG(LogStalker, Error, TEXT("unknown setting"));
		return 0;
	}
}

void FRBMKEngine::ChangeUeSettingsFloat(const std::pair<int, float>& settinglist)
{
	USoundClass* Sound;

		UE_LOG(LogStalker, Display, TEXT("ChangeUeSettingsFloat: %d, %f"), settinglist.first, settinglist.second);
		switch (settinglist.first)
		{
		case EffectsVolume:
		{
			Sound = LoadObject<USoundClass>(nullptr, TEXT("/Game/Base/Sounds/EffectClass.EffectClass"));
			if (Sound)
			{
				Sound->Properties.Volume = settinglist.second;
				Sound->TryUpdateDefaultConfigFile();
			}
		}break;
		case MusicVolume:
		{
			Sound = LoadObject<USoundClass>(nullptr, TEXT("/Game/Base/Sounds/MusicClass.MusicClass"));
			if (Sound)
			{
				Sound->Properties.Volume = settinglist.second;
				Sound->TryUpdateDefaultConfigFile();
			}
		}break;
		case MasterVolume:
		{
			Sound = LoadObject<USoundClass>(nullptr, TEXT("/Game/Base/Sounds/MasterClass.MasterClass"));
			if (Sound)
			{
				Sound->Properties.Volume = settinglist.second;
				Sound->TryUpdateDefaultConfigFile();
			}
		}break;
		default:
			UE_LOG(LogStalker, Error, TEXT("unknown setting"));
			break;
		}
}

float FRBMKEngine::GetSettingFloat(int setting, float& min, float& max)
{
	USoundClass* Sound;
	switch (setting)
	{
	case EffectsVolume:
	{
		Sound = LoadObject<USoundClass>(nullptr, TEXT("/Game/Base/Sounds/EffectClass.EffectClass"));
		if (Sound)
		{
			min = 0.f;
			max = 1.f;
			return Sound->Properties.Volume;
		}
		else
			return 0.f;
	}break;
	case MusicVolume:
	{
		Sound = LoadObject<USoundClass>(nullptr, TEXT("/Game/Base/Sounds/MusicClass.MusicClass"));
		if (Sound)
		{
			min = 0.f;
			max = 1.f;
			return Sound->Properties.Volume;
		}
		else
			return 0.f;
	}break;
	case MasterVolume:
	{
		Sound = LoadObject<USoundClass>(nullptr, TEXT("/Game/Base/Sounds/MasterClass.MasterClass"));
		if (Sound)
		{
			min = 0.f; 
			max = 1.f;
			return Sound->Properties.Volume;
		}
		else
			return 0.f;
	}break;
	default:
		return 0.0f;
		break;
	}
}

void FRBMKEngine::GetCurrentResolution(u32& w, u32& h)
{
	FIntPoint res = GEngine->GetGameUserSettings()->GetScreenResolution();
	w = res.X;
	h = res.Y;
}

void	FRBMKEngine::GetResolutions(std::vector<xr_string>& ResVec)
{
	FScreenResolutionArray ResList;
	RHIGetAvailableResolutions(ResList, true);


	for (FScreenResolutionRHI element : ResList)
	{
		u32 w = element.Width;
		u32 h = element.Height;

		xr_string Comb;
		Comb.Printf("%dx%d", w, h);
		ResVec.push_back(Comb);
	}
}

void FRBMKEngine::SetResolution(u32 w, u32 h)
{
	FIntPoint res;
	res.X = w;
	res.Y = h;
	GEngine->GetGameUserSettings()->SetScreenResolution(res);
	GEngine->GetGameUserSettings()->ApplySettings(true);
}