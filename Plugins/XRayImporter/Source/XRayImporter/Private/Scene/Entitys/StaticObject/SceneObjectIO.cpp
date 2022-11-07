#include "SceneObject.h"

#define SCENEOBJ_CURRENT_VERSION		0x0012

#define SCENEOBJ_CHUNK_VERSION		  	0x0900
#define SCENEOBJ_CHUNK_REFERENCE     	0x0902
#define SCENEOBJ_CHUNK_PLACEMENT     	0x0904
#define SCENEOBJ_CHUNK_FLAGS			0x0905
#define SCENEOBJ_CHUNK_SURFACE			0x0906
bool CSceneObject::LoadLTX(CInifile& ini, LPCSTR sect_name)
{
    bool bRes = true;
	do
    {
        u32 version = ini.r_u32(sect_name, "version");

		CCustomObject::LoadLTX						(ini, sect_name);

        xr_string ref_name  = ini.r_string			(sect_name, "reference_name");

        if (!SetReference(ref_name.c_str()))
        {
            ELog.Msg            ( mtError, "CSceneObject: '%s' not found in library", ref_name.c_str() );
            bRes                = false;
            int mr              = mrNone;

            xr_string       _new_name;
            bool b_found    = Scene->GetSubstObjectName(ref_name.c_str(), _new_name);
            if(b_found)
            {
                xr_string _message;
                _message = "Object ["+ref_name+"] not found. Relace it with ["+_new_name+"] or select other from library?";
                mr = ELog.DlgMsg(mtConfirmation,mbYes |mbNo, _message.c_str());
                if(mrYes==mr)
                {
                    bRes = SetReference(_new_name.c_str())!=nullptr;
                }
            }

        }

      	m_Flags.assign(ini.r_u32(sect_name, "flags"));
        if (m_Flags.test(flUseSurface))
        {
            SIniFileStream 			ini_stream;
            ini_stream.ini = &ini;
            ini_stream.sect = sect_name;
            ini_stream.move_begin();
            if (ini.line_exist(sect_name, ini_stream.gen_name()))
            {
                ini_stream.move_begin();
                u32 Size;
                ini_stream.r_u32(Size);
                for (u32 i = 0; i < Size; i++)
                {
                    xr_string Name;
                    ini_stream.r_string(Name);
                    CSurface* Surf = nullptr;
                    for (SurfaceIt sf_it = m_Surfaces.begin(); sf_it != m_Surfaces.end(); ++sf_it)
                    {
                        if ((*sf_it)->m_Name == Name.c_str())
                        {
                            Surf = *sf_it;
                            break;
                        }
                    }

                    if (Surf)
                    {
                        if (!Surf->IsVoid())
                            Surf->OnDeviceDestroy();
                    }
                    {
                        ini_stream.r_string(Name);
                        if (Surf)Surf->SetShader(Name.c_str());
                        ini_stream.r_string(Name);
                        if (Surf)Surf->SetShaderXRLC(Name.c_str());
                        ini_stream.r_string(Name);
                        if (Surf)Surf->SetGameMtl(Name.c_str());
                        ini_stream.r_string(Name);
                        if (Surf)Surf->SetTexture(Name.c_str());
                        ini_stream.r_string(Name);
                        if (Surf)Surf->SetVMap(Name.c_str());
                    }

                    if (Surf) Surf->OnDeviceCreate();
                }
            }
        }


        if (!bRes) break;
    }while(0);

    return bRes;
}


bool CSceneObject::LoadStream(IReader& F)
{
    bool bRes = true;
	do{
        u16 version = 0;
        string1024 buf;
        R_ASSERT(F.r_chunk(SCENEOBJ_CHUNK_VERSION,&version));

        if (version==0x0010)
        {
	        R_ASSERT(F.find_chunk(SCENEOBJ_CHUNK_PLACEMENT));
    	    F.r_fvector3(FPosition);
	        F.r_fvector3(FRotation);
    	    F.r_fvector3(FScale);
        }

		CCustomObject::LoadStream(F);

        R_ASSERT(F.find_chunk(SCENEOBJ_CHUNK_REFERENCE));
        if(version<=0x0011)
        {
                F.r_u32();
                F.r_u32();
        }
        F.r_stringZ	(buf,sizeof(buf));

        if (!SetReference(buf))
        {
            ELog.Msg            ( mtError, "CSceneObject: '%s' not found in library", buf );
            bRes                = false;
            int mr              = mrNone;

            xr_string       _new_name;
            bool b_found    = Scene->GetSubstObjectName(buf, _new_name);
            if(b_found)
            {
                xr_string _message;
                _message = "Object ["+xr_string(buf)+"] not found. Relace it with ["+_new_name+"] or select other from library?";
                mr = ELog.DlgMsg(mtConfirmation,mbYes |mbNo, _message.c_str());
                if(mrYes==mr)
                {
                    bRes = SetReference(_new_name.c_str())!=nullptr;
                }
            }
        }
       
        


//        if(!CheckVersion()){
//            ELog.Msg( mtError, "CSceneObject: '%s' different file version!", buf );
//            }

        // flags
        if (F.find_chunk(SCENEOBJ_CHUNK_FLAGS)){
        	m_Flags.assign(F.r_u32());
        }
        if (m_Flags.test(flUseSurface))
        {
            if (F.find_chunk(SCENEOBJ_CHUNK_SURFACE))
            {

                u32 Size = F.r_u32();
                for (u32 i = 0; i < Size; i++)
                {
                    xr_string Name;
                    F.r_stringZ(Name);
                    CSurface* Surf = nullptr;
                    for (SurfaceIt sf_it = m_Surfaces.begin(); sf_it != m_Surfaces.end(); ++sf_it)
                    {
                        if ((*sf_it)->m_Name == Name.c_str())
                        {
                            Surf = *sf_it;
                            break;
                        }
                    }
                    if (Surf) Surf->OnDeviceDestroy();
                    {
                        F.r_stringZ(Name);
                        if (Surf)Surf->SetShader(Name.c_str());
                        F.r_stringZ(Name);
                        if (Surf)Surf->SetShaderXRLC(Name.c_str());
                        F.r_stringZ(Name);
                        if (Surf)Surf->SetGameMtl(Name.c_str());
                        F.r_stringZ(Name);
                        if (Surf)Surf->SetTexture(Name.c_str());
                        F.r_stringZ(Name);
                        if (Surf)Surf->SetVMap(Name.c_str());
                    }
                    if (Surf) Surf->OnDeviceCreate();
                }
            }
        }
        if (!bRes) break;
    }while(0);

    return bRes;
}


