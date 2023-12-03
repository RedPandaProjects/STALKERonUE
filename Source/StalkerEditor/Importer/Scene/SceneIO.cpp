#include "scene.h"

// file: SceneChunks.h
#define CURRENT_FILE_VERSION    	0x00000005

#define CURRENT_LEVELOP_VERSION 	0x0000000C
//0x00000008

#define CURRENT_LEVELOP_BP_VERSION 	0x00000009
#define CURRENT_ENV_VERSION	 		0x00000007

#define CHUNK_VERSION       	0x9df3
#define CHUNK_OBJECT_CLASS  	0x7703
#define CHUNK_OBJECT_LIST		0x7708
#define CHUNK_CAMERA        	0x7709
#define CHUNK_SNAPOBJECTS   	0x7710
#define CHUNK_LEVELOP       	0x7711
#define CHUNK_OBJECT_COUNT  	0x7712
#define CHUNK_LEVEL_TAG			0x7777

#define CHUNK_TOOLS_GUID		0x7000
#define CHUNK_TOOLS_DATA		0x8000

// level options
#define CHUNK_LO_VERSION		0x7801
#define CHUNK_LO_NAMES 			0x7802
#define CHUNK_LO_BOP		 	0x7803
#define CHUNK_LO_PREFIX 		0x7804
#define CHUNK_LO_BP_VERSION		0x7849
#define CHUNK_BUILD_PARAMS		0x7850
#define CHUNK_LIGHT_QUALITY		0x7851
#define CHUNK_MAP_USAGE			0x7852
#define CHUNK_LO_MAP_VER	 	0x7853

// Level Options

void st_LevelOptions::ReadLTX(CInifile& ini)
{
	LPCSTR section 	= "level_options";

    u32 vers_op 		= ini.r_u32(section, "version");
    if( vers_op < 0x00000008 )
    {
        ELog.DlgMsg( mtError, "Skipping bad version of level options." );
        return;
    }

    m_FNLevelPath		= ini.r_string 		(section, "level_path");
    m_LevelPrefix		= ini.r_string 		(section, "level_prefix");
    m_BOPText			= ini.r_string_wb	(section, "bop");

    if(vers_op > 0x0000000B)
    	m_map_version		= ini.r_string		(section, "map_version");


    m_mapUsage.SetDefaults			();
    if(vers_op > 0x0000000A)
	{
     m_mapUsage.LoadLTX				(ini,section,false);
    }else
    {

    m_mapUsage.m_GameType.set		(eGameIDDeathmatch ,	ini.r_s32(section, "usage_deathmatch"));
    m_mapUsage.m_GameType.set		(eGameIDTeamDeathmatch, ini.r_s32(section, "usage_teamdeathmatch"));
    m_mapUsage.m_GameType.set		(eGameIDArtefactHunt,	ini.r_s32(section, "usage_artefacthunt"));


	if(vers_op > 0x00000008)
    {
        m_mapUsage.m_GameType.set	(eGameIDCaptureTheArtefact,	ini.r_s32(section, "usage_captretheartefact"));

        m_mapUsage.m_GameType.set	(eGameIDTeamDominationZone,	ini.r_s32(section, "usage_team_domination_zone"));
        if(vers_op==0x00000009)
        	m_mapUsage.m_GameType.set(eGameIDDominationZone,		ini.r_s32(section, "domination_zone"));
        else
        	m_mapUsage.m_GameType.set(eGameIDDominationZone,		ini.r_s32(section, "usage_domination_zone"));
     }
    }
}

#pragma pack(push,4)
struct b_params
{
	// Normals & optimization
	float		m_sm_angle;				// normal smooth angle		- 89.0
	float		m_weld_distance;		// by default 0.005f		- 5mm

	// Light maps
	float		m_lm_pixels_per_meter;	// LM - by default: 4 ppm
	u32			m_lm_jitter_samples;	// 1/4/9 - by default		- 4
	u32			m_lm_rms_zero;			// RMS - after what the lightmap will be shrinked to ZERO pixels
	u32			m_lm_rms;				// RMS - shrink and recalc

	// build quality
	u16			m_quality;
	u16			u_reserved;

	// Progressive
	float		f_reserved[6];

	void SaveLTX(CInifile& ini)
	{
		LPCSTR section = "build_params";
		ini.w_float(section, "smooth_angle", m_sm_angle);
		ini.w_float(section, "weld_distance", m_weld_distance);
		ini.w_float(section, "light_pixel_per_meter", m_lm_pixels_per_meter);
		ini.w_u32(section, "light_jitter_samples", m_lm_jitter_samples);
		ini.w_u32(section, "light_rms_zero", m_lm_rms_zero);
		ini.w_u32(section, "light_rms", m_lm_rms);
		ini.w_u16(section, "light_quality", m_quality);
		ini.w_u16(section, "light_quality_reserved", u_reserved);
		for (u32 i = 0; i < 6; ++i)
		{
			string128	buff;
			xr_sprintf(buff, sizeof(buff), "reserved_%d", i);
			ini.w_float(section, buff, f_reserved[i]);
		}
	}
	void LoadLTX(CInifile& ini)
	{
		LPCSTR section = "build_params";
		m_sm_angle = ini.r_float(section, "smooth_angle");
		m_weld_distance = ini.r_float(section, "weld_distance");
		m_lm_pixels_per_meter = ini.r_float(section, "light_pixel_per_meter");
		m_lm_jitter_samples = ini.r_u32(section, "light_jitter_samples");
		m_lm_rms_zero = ini.r_u32(section, "light_rms_zero");
		m_lm_rms = ini.r_u32(section, "light_rms");
		m_quality = ini.r_u16(section, "light_quality");
		u_reserved = ini.r_u16(section, "light_quality_reserved");
		for (u32 i = 0; i < 6; ++i)
		{
			string128		buff;
			xr_sprintf(buff, sizeof(buff), "reserved_%d", i);
			f_reserved[i] = ini.r_float(section, buff);
		}
	}

	void        Init()
	{
		// Normals & optimization
		m_sm_angle = 75.f;
		m_weld_distance = 0.005f;

		// Light maps
		m_lm_rms_zero = 4;
		m_lm_rms = 4;

		setHighQuality();
	}
	void		setDraftQuality()
	{
		m_quality = ebqDraft;
		m_lm_pixels_per_meter = 0.1f;
		m_lm_jitter_samples = 1;
	}
	void		setHighQuality()
	{
		m_quality = ebqHigh;
		m_lm_pixels_per_meter = 10;
		m_lm_jitter_samples = 9;
	}
};
#pragma pack(pop)
void st_LevelOptions::Read(IReader& F)
{
	R_ASSERT(F.find_chunk(CHUNK_LO_VERSION));
    DWORD vers = F.r_u32( );
    if( vers < 0x00000008 )
    {
        ELog.DlgMsg( mtError, "Skipping bad version of level options." );
        return;
    }

    R_ASSERT(F.find_chunk(CHUNK_LO_NAMES));
    F.r_stringZ 	(m_FNLevelPath);

    if (F.find_chunk(CHUNK_LO_PREFIX)) F.r_stringZ 	(m_LevelPrefix);

    R_ASSERT(F.find_chunk(CHUNK_LO_BOP));
    F.r_stringZ 	(m_BOPText); 

    if (F.find_chunk(CHUNK_LO_MAP_VER))
	    F.r_stringZ	(m_map_version);

    vers = 0;
    if (F.find_chunk(CHUNK_LO_BP_VERSION))
	    vers = F.r_u32( );
    b_params m_BuildParams;
	if (CURRENT_LEVELOP_BP_VERSION == vers) {
		if (F.find_chunk(CHUNK_BUILD_PARAMS))
			F.r(&m_BuildParams, sizeof(m_BuildParams));
	}
	else {
		ELog.DlgMsg(mtError, "Skipping bad version of build params.");
		m_BuildParams.Init();
	}
    UE_LOG(LogXRayImporter,Log,TEXT("Level Smoth angle %f"), m_BuildParams.m_sm_angle);
    if (F.find_chunk(CHUNK_MAP_USAGE))
    {
    	if(vers > 0x00000008)
        {
          m_mapUsage.m_GameType.assign	(F.r_u16());
        }else
        {
            m_mapUsage.m_GameType.zero					();
            m_mapUsage.m_GameType.set					(eGameIDDeathmatch ,	F.r_s32());
            m_mapUsage.m_GameType.set					(eGameIDTeamDeathmatch, F.r_s32());
            m_mapUsage.m_GameType.set					(eGameIDArtefactHunt,	F.r_s32());
        }
    }
}


// Scene

BOOL EScene::LoadLevelPartLTX(FRBMKSceneToolBase* M, LPCSTR mn)
{
	string_path map_name;
    FCStringAnsi::Strcpy(map_name, mn);
    
	if(!M->CanUseInifile())
    	return LoadLevelPartStream(M, map_name);

    int fnidx=0;
    while(FS.exist(map_name))
    {
        IReader* R		= FS.r_open	(map_name);
        VERIFY			(R);
        char 			ch;
        R->r			(&ch,sizeof(ch));
        bool b_is_inifile = (ch=='[');
        FS.r_close		(R);

        if(!b_is_inifile)
            return LoadLevelPartStream(M, map_name);


        CInifile			ini(map_name);

        // check level part GUID
        xrGUID				guid;
        guid.LoadLTX		(ini, "guid", "guid");

        if (guid!=m_GUID)
        {
            ELog.DlgMsg		(mtError,"Skipping invalid version of level part: '%s\\%s.part'",EFS.ExtractFileName(map_name).c_str(),M->ClassName());
            return 			FALSE;
        }
        // read data
        M->LoadLTX			(ini);

		++fnidx;
       FCStringAnsi::Sprintf(map_name, "%s%d", mn, fnidx);
    }

    return 					TRUE;
}

BOOL EScene::LoadLevelPart(FRBMKSceneToolBase* M, LPCSTR map_name)
{
	if(M->CanUseInifile())
	    return LoadLevelPartLTX(M, map_name);
    return LoadLevelPartStream(M, map_name);
	
}

BOOL EScene::LoadLevelPartStream(FRBMKSceneToolBase* M, LPCSTR map_name)
{
    if (FS.exist(map_name))
    {
        // check locking

        IReader* R = FS.r_open(map_name);
        VERIFY(R);
        // check level part GUID
        R_ASSERT(R->find_chunk(CHUNK_TOOLS_GUID));
        xrGUID			guid;
        R->r(&guid, sizeof(guid));

    /*    if (guid != m_GUID)
        {
            ELog.DlgMsg(mtError, "Skipping invalid version of level part: '%s\\%s.part'", EFS.ExtractFileName(map_name).c_str(), M->ClassName());
            FS.r_close(R);
            return 			FALSE;
        }*/
        // read data
        IReader* chunk = R->open_chunk(CHUNK_TOOLS_DATA + static_cast<int32>(M->ObjectType));
        if (chunk != NULL)
        {
            M->LoadStream(*chunk);
            chunk->close();
        }
        else
        {
            ELog.DlgMsg(mtError, "Skipping corrupted version of level part: '%s\\%s.part'", EFS.ExtractFileName(map_name).c_str(), M->ClassName());
            FS.r_close(R);
            return 			FALSE;
        }
        //success
        FS.r_close(R);
        return 				TRUE;
    }
    return 					TRUE;
}

BOOL EScene::LoadLevelPart(LPCSTR map_name, ERBMKSceneObjectType cls)
{
	xr_string pn	= LevelPartName(map_name,cls);
    if (LoadLevelPart(GetTool(cls),pn.c_str()))
    	return 		TRUE;
    else
	    return 			FALSE;
}

BOOL EScene::UnloadLevelPart(FRBMKSceneToolBase* M)
{
	M->Clear		();
    return 			TRUE;
}

BOOL EScene::UnloadLevelPart(LPCSTR map_name, ERBMKSceneObjectType cls)
{
	xr_string pn	= LevelPartName(map_name,cls);
    if (UnloadLevelPart(GetTool(cls)))
    	return 		TRUE;
    else
    	return			FALSE;
}

xr_string EScene::LevelPartPath(LPCSTR full_name)
{
    return 			EFS.ExtractFilePath(full_name)+EFS.ExtractFileName(full_name)+"\\";
}

xr_string EScene::LevelPartName(LPCSTR map_name, ERBMKSceneObjectType cls)
{
    return 			LevelPartPath(map_name)+GetTool(cls)->ClassName() + ".part";
}

bool EScene::LoadToolLTX(ERBMKSceneObjectType clsid, LPCSTR fn)
{
    FRBMKSceneToolBase* tool 	= GetTool(clsid);
    tool->Clear				();
	bool res 				= LoadLevelPartLTX(tool, fn);
	return 					res;
}


bool EScene::ReadObjectStream(IReader& F, FXRayCustomObject*& O)
{
    ERBMKSceneObjectType clsid		= ERBMKSceneObjectType::Unkown;
    R_ASSERT				(F.find_chunk(CHUNK_OBJECT_CLASS));
    clsid 					= ERBMKSceneObjectType(F.r_u32());

    if (GetTool(clsid) == nullptr)
    {
        return false;
    }

	O 						= GetOTool(clsid)->CreateObject(0,0);

    IReader* S 				= F.open_chunk(CHUNK_OBJECT_BODY);
    R_ASSERT				(S);
    bool bRes 				= O->LoadStream(*S);
    S->close				();

	if (!bRes)
    {
        delete			O;
        O = nullptr;
    }

	return bRes;
}

bool EScene::ReadObjectLTX(CInifile& ini, LPCSTR sect_name, FXRayCustomObject*& O)
{
    ERBMKSceneObjectType clsid = ERBMKSceneObjectType::Unkown;
    clsid 					= ERBMKSceneObjectType(ini.r_u32(sect_name,"clsid"));

    if (GetTool(clsid) == nullptr)
    {
        return false;
    }

	O 						= GetOTool(clsid)->CreateObject(0,0);

    bool bRes 				= O->LoadLTX(ini, sect_name);

	if (!bRes)
	{
		delete			O;
		O = nullptr;
	}

	return bRes;
}

bool EScene::OnLoadAppendObject(FXRayCustomObject* O)
{
	AppendObject	(O,false);
    return true;
}


bool EScene::LoadLTX(LPCSTR map_name, bool bUndo)
{
    DWORD version = 0;
	if (!map_name||(0==map_name[0])) return false;

    xr_string 		full_name;
    full_name 		= map_name;

	ELog.Msg( mtInformation, "EScene: loading '%s'", map_name);
    if (FS.exist(full_name.c_str()))
    {
        CTimer T; T.Start();

        // lock main level
		CInifile	ini(full_name.c_str());
        version 	= ini.r_u32("version","value");

        if (version!=CURRENT_FILE_VERSION)
        {
            ELog.DlgMsg( mtError, "EScene: unsupported file version. Can't load Level.");
            return false;
        }

        m_LevelOp.ReadLTX	(ini);


        m_GUID.LoadLTX			(ini,"guid","guid");

		m_OwnerName				= ini.r_string("level_tag","owner");
        m_CreateTime			= ini.r_u32("level_tag","create_time");


    	for(auto&[Key,Value]:SceneTools)
        {
            if (Value)
            {
                {
                    if (!bUndo && (Key!=ERBMKSceneObjectType::AllTypes))
                    {
                        xr_string fn 		 = LevelPartName(map_name,Key).c_str();
                        LoadLevelPartLTX	(Value.Get(), fn.c_str());
                    }
                }
            }
		}

        Msg("EScene: %d objects loaded, %3.2f sec", ObjCount(), T.GetElapsed_sec() );
        
		return true;
    }
    else
    {
    	ELog.Msg(mtError,"Can't find file: '%s'",map_name);
    }
	return false;
}

bool EScene::Load(LPCSTR map_name, bool bUndo)
{
    u32 version = 0;

	if (!map_name||(0==map_name[0])) return false;

    xr_string 		full_name;
    full_name 		= map_name;

	ELog.Msg( mtInformation, "EScene: loading '%s'", map_name);
    if (FS.exist(full_name.c_str()))
    {
        CTimer T; T.Start();
            
        // read main level
        IReader* F 	= FS.r_open(full_name.c_str()); VERIFY(F);
        // Version
        R_ASSERT	(F->r_chunk(CHUNK_VERSION, &version));
        if (version!=CURRENT_FILE_VERSION)
        {
            ELog.DlgMsg( mtError, "EScene: unsupported file version. Can't load Level.");
            FS.r_close(F);
            return false;
        }

        // Lev. ops.
        IReader* LOP = F->open_chunk(CHUNK_LEVELOP);
        if (LOP)
        {
	        m_LevelOp.Read	(*LOP);
        	LOP->close		();
        }else
        {
			ELog.DlgMsg		(mtError, "Skipping old version of level options.\nCheck level options after loading.");
	    }


	    if (F->find_chunk(CHUNK_TOOLS_GUID))
        {
		    F->r			(&m_GUID,sizeof(m_GUID));
        }

        if (F->find_chunk(CHUNK_LEVEL_TAG))
        {
            F->r_stringZ	(m_OwnerName);
            F->r			(&m_CreateTime,sizeof(m_CreateTime));
        }else
        {
            m_OwnerName		= "";
            m_CreateTime	= 0;
        }

        DWORD obj_cnt 		= 0;

        if (F->find_chunk(CHUNK_OBJECT_COUNT))
        	obj_cnt 		= F->r_u32();

      	FRMBKSceneAppendObjectDelegate AppendObjectDelegate;
		AppendObjectDelegate.BindLambda([this](FXRayCustomObject* Object)
		{
			AppendObject	(Object,false);
		});
        ReadObjectsStream	(*F,CHUNK_OBJECT_LIST, AppendObjectDelegate);

        for(auto&[Key,Value]:SceneTools)
        {
            if (Value)
            {
			    IReader* chunk 		= F->open_chunk(CHUNK_TOOLS_DATA+static_cast<int32>(Key));
            	if (chunk)
                {
	                Value->LoadStream(*chunk);
    	            chunk->close	();
                }
            	else
                {
                    if (!bUndo  && (Key!=ERBMKSceneObjectType::AllTypes))
                    {
                        LoadLevelPart	(Value.Get(),LevelPartName(map_name,Key).c_str());
                    }
                }
            }
		}
        
        // snap list
       

        Msg("EScene: %d objects loaded, %3.2f sec", ObjCount(), T.GetElapsed_sec() );

		FS.r_close(F);

		return true;
    }
	else
    {
    	ELog.Msg(mtError,"Can't find file: '%s'",map_name);
    }
	return false;
}

bool EScene::OnLoadSelectionAppendObject(FXRayCustomObject* obj)
{
    string256 				buf;
    GenObjectName			(obj->FClassID,buf,obj->GetName());
    obj->SetName(buf);
    AppendObject			(obj, false);
    return 					true;
}


bool EScene::LoadSelection( LPCSTR fname )
{
    u32 version = 0;

	VERIFY( fname );

    xr_string 		full_name;
    full_name 		= fname;

	ELog.Msg( mtInformation, "EScene: loading part %s...", fname );

    bool res = true;

    if (FS.exist(full_name.c_str()))
    {

        IReader* F = FS.r_open(full_name.c_str());

        // Version
        R_ASSERT(F->r_chunk(CHUNK_VERSION, &version));
        if (version!=CURRENT_FILE_VERSION)
        {
            ELog.DlgMsg( mtError, "EScene: unsupported file version. Can't load Level.");
            FS.r_close(F);
            return false;
        }
    	FRMBKSceneAppendObjectDelegate AppendObjectDelegate;
		AppendObjectDelegate.BindLambda([this](FXRayCustomObject* Object)
		{
			string256 				buf;
		    GenObjectName			(Object->FClassID,buf,Object->GetName());
		    Object->SetName(buf);
		    AppendObject			(Object, false);
		});
        // Objects
        if (!ReadObjectsStream(*F,CHUNK_OBJECT_LIST, AppendObjectDelegate))
        {
            ELog.DlgMsg(mtError,"EScene. Failed to load selection.");
            res = false;
        }

         for(auto&[Key,Value]:SceneTools)
         {
            if (Value){
			    IReader* chunk 		= F->open_chunk(CHUNK_TOOLS_DATA+static_cast<int32>(Key));
            	if (chunk){
	                Value->LoadSelection(*chunk);
    	            chunk->close	();
                }
            }
         }
        // Synchronize
		FS.r_close(F);
    }
	return res;
}


bool EScene::ReadObjectsLTX(CInifile& ini, LPCSTR sect_name_parent, LPCSTR sect_name_prefix, FRMBKSceneAppendObjectDelegate on_append)
{
    string128			buff;

    FCStringAnsi::Sprintf(buff, "%s_count", sect_name_prefix);
    u32 count = ini.r_u32(sect_name_parent, buff);
    bool bRes = true;

    for (u32 i = 0; i < count; ++i)
    {
        FCStringAnsi::Sprintf(buff, "%s_%s_%d", sect_name_parent, sect_name_prefix, i);
        FXRayCustomObject* obj = NULL;

        if (ReadObjectLTX(ini, buff, obj))
        {
            LPCSTR obj_name = obj->GetName();
            FXRayCustomObject* existing = FindObjectByName(obj_name, obj->FClassID);
            if (existing)
            {
                string256 				buf;
                GenObjectName(obj->FClassID, buf, obj->GetName());
                obj->SetName(buf);
            }
			on_append.Execute(obj);

        }

        else
            bRes = false;

    }
    return bRes;
}

bool EScene::ReadObjectsStream(IReader& F, u32 chunk_id, FRMBKSceneAppendObjectDelegate on_append)
{
    bool bRes = true;
    IReader* OBJ = F.open_chunk(chunk_id);
    if (OBJ)
    {
        IReader* O = OBJ->open_chunk(0);
        for (int count = 1; O; ++count)
        {
            FXRayCustomObject* obj = NULL;
            if (ReadObjectStream(*O, obj))
            {
                LPCSTR obj_name = obj->GetName();
                FXRayCustomObject* existing = FindObjectByName(obj_name, obj->FClassID);
                if (existing)
                {
                    /*if(g_frmConflictLoadObject->m_result!=2 && g_frmConflictLoadObject->m_result!=4 && g_frmConflictLoadObject->m_result!=6)
                    {
                        g_frmConflictLoadObject->m_existing_object 	= existing;
                        g_frmConflictLoadObject->m_new_object 		= obj;
                        g_frmConflictLoadObject->Prepare			();
                        g_frmConflictLoadObject->ShowModal			();
                    }
                    switch(g_frmConflictLoadObject->m_result)
                    {
                        case 1: //Overwrite
                        case 2: //Overwrite All
                        {
                           bool res = RemoveObject		(existing, true, true);
                            if(!res)
                                Msg("! RemoveObject [%s] failed", existing->Name);
                             else
                                xr_delete(existing);
                        }break;
                        case 3: //Insert new
                        case 4: //Insert new All*/

                    string256 				buf;
                    GenObjectName(obj->FClassID, buf, obj->GetName());
                    obj->SetName(buf);
                    /*}break;
                    case 0: //Cancel
                    case 5: //Skip
                    case 6: //Skip All
                    {
                        xr_delete(obj);
                    }break;
                }*/
                }
                on_append.Execute(obj);
            }
            else
                bRes = false;

            O->close();
            O = OBJ->open_chunk(count);

        }
        OBJ->close();
    }
    return bRes;
}