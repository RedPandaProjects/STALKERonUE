#include "ESceneCustomOTools.h"

// chunks

static const u32 OTOOL_CHUNK_OBJECT_COUNT		= 0x0002;
static const u32 OTOOL_CHUNK_OBJECTS			= 0x0003;
static const u32 OTOOL_CHUNK_FLAGS			= 0x0004;


bool ESceneCustomOTool::OnLoadSelectionAppendObject(CCustomObject* obj)
{
    string256 				buf;
    Scene->GenObjectName	(obj->FClassID,buf,obj->GetName());
    obj->SetName(buf);
    Scene->AppendObject		(obj, false);
    return					true;
}


bool ESceneCustomOTool::OnLoadAppendObject(CCustomObject* O)
{
	Scene->AppendObject	(O,false);
    return true;
}


bool ESceneCustomOTool::LoadSelection(IReader& F)
{
    int count					= 0;
	F.r_chunk					(OTOOL_CHUNK_OBJECT_COUNT,&count);

    Scene->ReadObjectsStream	(F,OTOOL_CHUNK_OBJECTS, EScene::TAppendObject(this, &ESceneCustomOTool::OnLoadSelectionAppendObject));

    return true;
}



bool ESceneCustomOTool::LoadLTX(CInifile& ini)
{
	inherited::LoadLTX	(ini);

    u32 count			= ini.r_u32("main", "objects_count");


    u32 i				= 0;
    string128			buff;

      for(i=0; i<count; ++i)
      {
      	
        
          CCustomObject* obj	= NULL;
          FCStringAnsi::Sprintf				(buff, "object_%d", i);
          if( Scene->ReadObjectLTX(ini, buff, obj) )
          {
              if (!OnLoadAppendObject(obj))
                  delete obj;
          }
      }


    return true;
}

bool ESceneCustomOTool::LoadStream(IReader& F)
{
	inherited::LoadStream		(F);

    int count					= 0;
	F.r_chunk					(OTOOL_CHUNK_OBJECT_COUNT,&count);

    Scene->ReadObjectsStream	(F,OTOOL_CHUNK_OBJECTS, EScene::TAppendObject(this, &ESceneCustomOTool::OnLoadAppendObject));

    return true;
}

