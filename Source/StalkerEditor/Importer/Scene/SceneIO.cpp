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


TSharedPtr<FXRayCustomObject> EScene::ReadObjectStream(IReader& F)
{
    ERBMKSceneObjectType clsid		= ERBMKSceneObjectType::Unkown;
    R_ASSERT				(F.find_chunk(CHUNK_OBJECT_CLASS));
    clsid 					= ERBMKSceneObjectType(F.r_u32());

    if (GetTool(clsid) == nullptr)
    {
        return nullptr;
    }

	TSharedPtr<FXRayCustomObject> O = GetOTool(clsid)->CreateObject(0,0);

    IReader* S 				= F.open_chunk(CHUNK_OBJECT_BODY);
    R_ASSERT				(S);
    bool bRes 				= O->LoadStream(*S);
    S->close				();
	if (!bRes)
    {
        return nullptr;
    }
	return O;
}

TSharedPtr<FXRayCustomObject> EScene::ReadObjectLTX(CInifile& ini, LPCSTR sect_name)
{
    ERBMKSceneObjectType clsid = ERBMKSceneObjectType::Unkown;
    clsid 					= ERBMKSceneObjectType(ini.r_u32(sect_name,"clsid"));

    if (GetTool(clsid) == nullptr)
    {
        return nullptr;
    }

	TSharedPtr<FXRayCustomObject> O = GetOTool(clsid)->CreateObject(0,0);

    bool bRes 				= O->LoadLTX(ini, sect_name);

	if (!bRes)
	{
		return nullptr;
	}
	return O;
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




        m_GUID.LoadLTX			(ini,"guid","guid");


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

	    if (F->find_chunk(CHUNK_TOOLS_GUID))
        {
		    F->r			(&m_GUID,sizeof(m_GUID));
        }

        DWORD obj_cnt 		= 0;

        if (F->find_chunk(CHUNK_OBJECT_COUNT))
        	obj_cnt 		= F->r_u32();

      	FRMBKSceneAppendObjectDelegate AppendObjectDelegate;
		AppendObjectDelegate.BindLambda([this](TSharedPtr<FXRayCustomObject> Object)
		{
			AppendObject	(Object);
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
		AppendObjectDelegate.BindLambda([this](TSharedPtr<FXRayCustomObject> Object)
		{
			string256 				buf;
		    GenObjectName			(Object->FClassID,buf,Object->GetName());
		    Object->SetName(buf);
		    AppendObject			(Object);
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
        if (TSharedPtr<FXRayCustomObject> Object = ReadObjectLTX(ini, buff))
        {
            LPCSTR obj_name = Object->GetName();
            TSharedPtr<FXRayCustomObject> existing = FindObjectByName(obj_name, Object->FClassID);
            if (existing)
            {
                string256 				buf;
                GenObjectName(Object->FClassID, buf, Object->GetName());
                Object->SetName(buf);
            }
			on_append.Execute(Object);

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
            if (TSharedPtr<FXRayCustomObject> Object = ReadObjectStream(*O))
            {
                LPCSTR obj_name = Object->GetName();
                TSharedPtr<FXRayCustomObject> existing = FindObjectByName(obj_name, Object->FClassID);
                if (existing)
                {
                    string256 				buf;
                    GenObjectName(Object->FClassID, buf, Object->GetName());
                    Object->SetName(buf);
                }
                on_append.Execute(Object);
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