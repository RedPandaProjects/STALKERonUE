#include "RBMKSceneShape.h"

FRBMKSceneShape::FRBMKSceneShape(const FString&Name):FRBMKSceneObjectBase(Name)
{
	ObjectType = ERBMKSceneObjectType::Shape;
	ShapeType = ERBMKSceneShapeUsage::Common;
}

FRBMKSceneShape::~FRBMKSceneShape()
{
}


void* FRBMKSceneShape::QueryInterface(ERBMKSceneObjectType InClassID)
{
	if (InClassID == ERBMKSceneObjectType::Shape)
		return this;
	return FRBMKSceneObjectBase::QueryInterface(InClassID);
}

bool FRBMKSceneShape::LoadLTX(CInifile& INIFile, const FString&SectionName)
{
	const uint32 Version = INIFile.r_u32(TCHAR_TO_ANSI(*SectionName), "version");

 	FRBMKSceneObjectBase::LoadLTX	(INIFile, SectionName);

	const uint32 Count = INIFile.r_u32(TCHAR_TO_ANSI(*SectionName), "shapes_count");

    if(Version>0x0001)
    {
    	ShapeType = static_cast<ERBMKSceneShapeUsage>(INIFile.r_u8(TCHAR_TO_ANSI(*SectionName), "shape_type"));
    }
	shapes.resize(Count);
    for(u32 i=0; i<Count; ++i)
    {
	    string128 BufferStr;
	    FCStringAnsi::Sprintf(BufferStr,"shape_type_%d", i);
		shapes[i].type	= INIFile.r_u8(TCHAR_TO_ANSI(*SectionName),BufferStr);
		if(shapes[i].type == CShapeData::cfSphere)
		{
		    FCStringAnsi::Sprintf			(BufferStr,"shape_center_%d", i);
		    shapes[i].data.sphere.P = INIFile.r_fvector3	(TCHAR_TO_ANSI(*SectionName), BufferStr);
		    
		    FCStringAnsi::Sprintf			(BufferStr,"shape_radius_%d", i);
		    shapes[i].data.sphere.R = INIFile.r_float		(TCHAR_TO_ANSI(*SectionName), BufferStr);
		}
		else
		{
		    check		(shapes[i].type==CShapeData::cfBox);
		    FCStringAnsi::Sprintf			(BufferStr,"shape_matrix_i_%d", i);
		    shapes[i].data.box.i = INIFile.r_fvector3	(TCHAR_TO_ANSI(*SectionName), BufferStr);
		    
		    FCStringAnsi::Sprintf			(BufferStr,"shape_matrix_j_%d", i);
		    shapes[i].data.box.j = INIFile.r_fvector3	(TCHAR_TO_ANSI(*SectionName), BufferStr);
		    
		    FCStringAnsi::Sprintf			(BufferStr,"shape_matrix_k_%d", i);
		    shapes[i].data.box.k = INIFile.r_fvector3	(TCHAR_TO_ANSI(*SectionName), BufferStr);
		    
		    FCStringAnsi::Sprintf			(BufferStr,"shape_matrix_c_%d", i);
		    shapes[i].data.box.c =INIFile.r_fvector3	(TCHAR_TO_ANSI(*SectionName), BufferStr);
		}
    }
	return true;
}

bool FRBMKSceneShape::LoadStream(IReader& F)
{
    enum
    {
		SHAPE_CURRENT_VERSION   = 0x0002,
		SHAPE_CHUNK_VERSION     = 0x0000,
		SHAPE_CHUNK_SHAPES  	= 0x0001,
		SHAPE_CHUNK_DATA 	    = 0x0002,
    };

	if(!ensure(F.find_chunk(SHAPE_CHUNK_VERSION)))
	{
		return false;
	}
    uint16 Version		= F.r_u16();

	FRBMKSceneObjectBase::LoadStream	(F);

	if(!ensure(F.find_chunk(SHAPE_CHUNK_SHAPES)))
    {
	    return false;
    }

    shapes.resize(F.r_u32());
	F.r(shapes.data(),shapes.size()*sizeof(shape_def));
  
    if(F.find_chunk(SHAPE_CHUNK_DATA))
    {
    	ShapeType = static_cast<ERBMKSceneShapeUsage>(F.r_u8());
    }
	return true;
}

