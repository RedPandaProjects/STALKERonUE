#include "EShape.h"

#define SHAPE_COLOR_TRANSP		0x3C808080
#define SHAPE_COLOR_EDGE		0xFF202020



#define SHAPE_CURRENT_VERSION 	0x0002

#define SHAPE_CHUNK_VERSION 	0x0000
#define SHAPE_CHUNK_SHAPES 		0x0001
#define SHAPE_CHUNK_DATA 		0x0002


xr_token shape_type_tok[]=
{
	{ "common",	eShapeCommon		},
	{ "level bound",	eShapeLevelBound},
	{ 0,				0	}
};

CEditShape::CEditShape(LPVOID data, LPCSTR name):CCustomObject(data,name)
{
	Construct(data);
}

CEditShape::~CEditShape()
{
}

void CEditShape::Construct(LPVOID data)
{
	FClassID				= OBJCLASS_SHAPE;
    m_DrawTranspColor	= SHAPE_COLOR_TRANSP;
    m_DrawEdgeColor		= SHAPE_COLOR_EDGE;
    m_shape_type		= eShapeCommon;
}

void CEditShape::OnUpdateTransform()
{
	inherited::OnUpdateTransform();
}

void CEditShape::SetScale(const Fvector& val)
{
	if (shapes.size()==1){
		switch (shapes[0].type){
		case cfSphere:{
        	FScale.set(val.x,val.x,val.x);
        }break;
		case cfBox:		FScale.set(val.x,val.y,val.z);	break;
        default: THROW;
		}
    }else{
		FScale.set(val.x,val.x,val.x);

    }
    UpdateTransform	();
}

void CEditShape::ApplyScale()
{
	for (ShapeIt it=shapes.begin(); it!=shapes.end(); it++){
		switch (it->type){
		case cfSphere:{
            Fsphere&	T	= it->data.sphere;
            FTransformS.transform_tiny(T.P);
            T.R				*= FScale.x;
		}break;
		case cfBox:{
            Fmatrix& B		= it->data.box;
            B.mulA_43		(FTransformS);
		}break;
        }
    }
    FScale.set		(1.f,1.f,1.f);
    UpdateTransform	(true);

}

void CEditShape::add_sphere(const Fsphere& S)
{
	shapes.push_back(shape_def());
	shapes.back().type	= cfSphere;
	shapes.back().data.sphere.set(S);

}

void CEditShape::add_box(const Fmatrix& B)
{
	shapes.push_back(shape_def());
	shapes.back().type	= cfBox;
	shapes.back().data.box.set(B);

}

void CEditShape::Attach(CEditShape* from)
{
	ApplyScale				();
	// transfer data
    from->ApplyScale		();
	Fmatrix M 				= from->_Transform();
    M.mulA_43				(_ITransform());
	for (ShapeIt it=from->shapes.begin(); it!=from->shapes.end(); it++){
		switch (it->type){
		case cfSphere:{
            Fsphere& T		= it->data.sphere;
            M.transform_tiny(T.P);
            add_sphere		(T);
		}break;
		case cfBox:{
            Fmatrix B		= it->data.box;
            B.mulA_43		(M);
            add_box			(B);
		}break;
        default: THROW;
		}
    }
    // common
    Scene->RemoveObject		(from,true,true);
    delete from;

}

void CEditShape::Detach()
{
	if (shapes.size()>1){
    	
        ApplyScale		();
        // create scene shapes
        const Fmatrix& M = _Transform();
        ShapeIt it=shapes.begin(); it++;
        for (; it!=shapes.end(); it++){
            string256 namebuffer;
            Scene->GenObjectName	(OBJCLASS_SHAPE, namebuffer, GetName());
            CEditShape* shape 	= (CEditShape*)Scene->GetOTool(FClassID)->CreateObject(0, namebuffer);
            switch (it->type){
            case cfSphere:{
                Fsphere	T		= it->data.sphere;
                M.transform_tiny(T.P);
                shape->SetPosition( T.P);
                T.P.set			(0,0,0);
                shape->add_sphere(T);
            }break;
            case cfBox:{
                Fmatrix B		= it->data.box;
                B.mulA_43		(M);
                shape->SetPosition(B.c);
                B.c.set			(0,0,0);
                shape->add_box	(B);
            }break;
            default: THROW;
            }
            Scene->AppendObject	(shape,false);
        }
        // erase shapes in base object
        it=shapes.begin(); it++;
        shapes.erase(it,shapes.end());

    }
}

void CEditShape::OnDetach()
{
	inherited::OnDetach	();

    m_DrawTranspColor	= SHAPE_COLOR_TRANSP;
    m_DrawEdgeColor		= SHAPE_COLOR_EDGE;
}


void* CEditShape::QueryInterface(ObjClassID InClassID)
{
	if (InClassID == OBJCLASS_SHAPE)
		return this;
	return inherited::QueryInterface(InClassID);
}

bool CEditShape::LoadLTX(CInifile& ini, LPCSTR sect_name)
{
    u32 vers		= ini.r_u32(sect_name, "version");

 	inherited::LoadLTX	(ini, sect_name);

    u32 count 			= ini.r_u32			(sect_name, "shapes_count");
    if(vers>0x0001)
    	m_shape_type	= ini.r_u8			(sect_name, "shape_type");
        
    string128			buff;
    shapes.resize		(count);
    for(u32 i=0; i<count; ++i)
    {
       sprintf			(buff,"shape_type_%d", i);
       shapes[i].type	= ini.r_u8(sect_name, buff);
       if(shapes[i].type==CShapeData::cfSphere)
       {
       	sprintf			(buff,"shape_center_%d", i);
		shapes[i].data.sphere.P = ini.r_fvector3	(sect_name, buff);

       	sprintf			(buff,"shape_radius_%d", i);
		shapes[i].data.sphere.R = ini.r_float		(sect_name, buff);
       }else
       {
       	 R_ASSERT		(shapes[i].type==CShapeData::cfBox);
         sprintf			(buff,"shape_matrix_i_%d", i);
         shapes[i].data.box.i = ini.r_fvector3	(sect_name, buff);

         sprintf			(buff,"shape_matrix_j_%d", i);
         shapes[i].data.box.j = ini.r_fvector3	(sect_name, buff);

         sprintf			(buff,"shape_matrix_k_%d", i);
         shapes[i].data.box.k = ini.r_fvector3	(sect_name, buff);

         sprintf			(buff,"shape_matrix_c_%d", i);
         shapes[i].data.box.c = ini.r_fvector3	(sect_name, buff);
       }
    }


	return true;
}

bool CEditShape::LoadStream(IReader& F)
{
	R_ASSERT(F.find_chunk(SHAPE_CHUNK_VERSION));
    u16 vers		= F.r_u16();

	inherited::LoadStream	(F);

	R_ASSERT(F.find_chunk(SHAPE_CHUNK_SHAPES));
    shapes.resize	(F.r_u32());
    F.r				(shapes.data(),shapes.size()*sizeof(shape_def));

    if(F.find_chunk(SHAPE_CHUNK_DATA))
    	m_shape_type	= F.r_u8();
    
	return true;
}

