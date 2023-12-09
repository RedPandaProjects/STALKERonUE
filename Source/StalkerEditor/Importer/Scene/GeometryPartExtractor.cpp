#include "GeometryPartExtractor.h"

THIRD_PARTY_INCLUDES_START
void ECORE_API ComputeOBB_RAPID	(Fobb &B, FvectorVec& V, u32 t_cnt);
void ECORE_API ComputeOBB_WML	(Fobb &B, FvectorVec& V);
THIRD_PARTY_INCLUDES_END

void SBPart::append_face			(SBFace* F)
{
    m_Faces.push_back				(F);
}

void SBPart::use_face				(SBFace* F, u32& cnt, u32 bone_id, float& area)
{
    VERIFY							(F->bone_id==-1);
    F->marked						= true;
    F->bone_id						= bone_id;
    area							+= F->CalcArea();
    cnt++;
}

void SBPart::recurse_fragment		(SBFace* F, u32& cnt, u32 bone_id, u32 max_faces, float& area)
{
    if (F){
        if (!F->marked)	use_face	(F,cnt,bone_id,area);
        // fill nearest
        SBFaceVec r_vec;
        for (SBFaceVecIt n_it=F->adjs.begin(); n_it!=F->adjs.end(); n_it++){
            if (cnt>=max_faces)		break;
            if ((*n_it)->marked)	continue;
            use_face				(*n_it,cnt,bone_id,area);
            r_vec.push_back			(*n_it);
        }     
        // recurse adjs   	
        for (SBFaceVecIt a_it=r_vec.begin(); a_it!=r_vec.end(); a_it++){
            if (cnt>=max_faces)					break;
            if ((*a_it)->bone_id!=(int)bone_id)	continue;
            recurse_fragment					(*a_it,cnt,bone_id,max_faces,area);
        } 
    }
}


bool SBPart::prepare				(SBAdjVec& adjs, u32 bone_face_min)
{
    m_bValid	= true;
    // compute OBB
    FvectorVec pts; pts.reserve		(m_Faces.size()*3);
    for (SBFaceVecIt f_it=m_Faces.begin(); f_it!=m_Faces.end(); f_it++){
        (*f_it)->marked				= false;
        for (int k=0; k<3; k++)		pts.push_back((*f_it)->o[k]);
    }
    ComputeOBB_RAPID				(m_OBB,pts,pts.size()/3);
    // fill adjacent
    for (SBFaceVecIt a_it=m_Faces.begin(); a_it!=m_Faces.end(); a_it++){
        SBFace* A					= *a_it;
        for (int k=0; k<3; k++){
            SBFaceVec& b_vec		= adjs[A->vert_id[k]];
            for (SBFaceVecIt b_it=b_vec.begin(); b_it!=b_vec.end(); b_it++){
                SBFace* B			= *b_it;
                if (A!=B){
                    int cnt			= 0;
                    for (int a=0; a<3; a++) for (int b=0; b<3; b++) if (A->vert_id[a]==B->vert_id[b]) cnt++;
                    if (cnt>=2){
                        if (std::find(A->adjs.begin(),A->adjs.end(),B)==A->adjs.end()) A->adjs.push_back(B);
                        if (std::find(B->adjs.begin(),B->adjs.end(),A)==B->adjs.end()) B->adjs.push_back(A);
                    }
                }
            }        
        }
    }
    // prepare transform matrix
    m_BBox.invalidate				();
    Fmatrix M; M.set				(m_OBB.m_rotate.i,m_OBB.m_rotate.j,m_OBB.m_rotate.k,m_OBB.m_translate);
    m_RefOffset.set					(m_OBB.m_translate);
    M.getXYZ						(m_RefRotate); // не i потому что в движке так
    M.invert						();

    // transform vertices & calculate bounding box
    for (auto f_it=m_Faces.begin(); f_it!=m_Faces.end(); f_it++)
    {
        SBFace* F					= (*f_it);
        if (F->adjs.empty())
        {	
            m_bValid				= false;
        }
        for (int k=0; k<3; k++){ 
            M.transform_dir			(F->n[k]);
            M.transform_tiny		(F->o[k]);
            m_BBox.modify			(F->o[k]);
        }
    }   
    if (m_bValid){
        // calculate bone params
        int bone_cnt_calc				= iFloor(float(m_Faces.size())/bone_face_min);
        int bone_cnt_max				= (bone_cnt_calc<62)?(bone_cnt_calc<=0?1:bone_cnt_calc):62;
        int bone_face_max				= iFloor(float(m_Faces.size())/bone_cnt_max+0.5f); bone_face_max *= 4.f;
        int bone_idx					= 0;
        // create big fragment
        u32 face_accum_total			= 0;
        xr_string parent_bone			= "";
        do{
            SBFace* F					= 0;
            // find unused face
            for (SBFaceVecIt f_it=m_Faces.begin(); f_it!=m_Faces.end(); f_it++){
                if (!(*f_it)->marked){
                    F					= *f_it;
                    int cnt 			= 0;
                    for (SBFaceVecIt a_it=F->adjs.begin(); a_it!=F->adjs.end(); a_it++) cnt+=(*a_it)->marked?0:1;
                    if ((cnt==0)||(cnt>=2))	break;
                }
            }
            if (!F)						break;
            float area					= 0;
            u32 face_accum				= 0;
            u32 face_max_count 			= Random.randI(bone_face_min,bone_face_max+1);
            // fill faces
            recurse_fragment			(F,face_accum,bone_idx,face_max_count,area);
            if (face_accum==1){
//            	F->marked				= false;
                F->bone_id				= -1;
            }else{
                m_Bones.push_back		(SBBone(face_accum,area));
                bone_idx				++;
                face_accum_total		+= face_accum;
            }
            // create bone
        }while(bone_idx<bone_cnt_max);
        
        // attach small single face to big fragment
        u32 face_accum_total_saved 		= face_accum_total;
        while (face_accum_total<m_Faces.size()){
        	if (face_accum_total_saved!=face_accum_total){
				face_accum_total_saved 	= face_accum_total;
            }else{
            	// если проход оказался безрезультатным (т.е. не смогли добавить хоть один фейс) - добавляем 
                //  все неприаттаченные фейсы к 0-кости (иначе зацикливается (mike - L03_agroprom))
                for (SBFaceVecIt f_it=m_Faces.begin(); f_it!=m_Faces.end(); f_it++){
                    SBFace* F				= *f_it;
                    if (-1==F->bone_id){
                        F->marked		= true;
                        F->bone_id		= 0;
                        face_accum_total++;
                    }
                } 
            }
            for (SBFaceVecIt f_it=m_Faces.begin(); f_it!=m_Faces.end(); f_it++){
                SBFace* F				= *f_it;
                if (-1==F->bone_id){
                    SBFace* P			= 0;
                    if (F->adjs.empty()){
                        F->marked		= true;
                        F->bone_id		= 0;
                        face_accum_total++;
                    }else{
                        for (SBFaceVecIt a_it=F->adjs.begin(); a_it!=F->adjs.end(); a_it++){ 
                            if (-1!=(*a_it)->bone_id){	
	                            P		= *a_it;
                            	break;
                            }
                        }
                    }
                    if (P){
                    	VERIFY(-1!=P->bone_id);
                        F->marked		= true;
                        F->bone_id		= P->bone_id;
                        face_accum_total++;
                    }
                }
            } 
        }
        
        // calculate bone offset
        for (auto f_it=m_Faces.begin(); f_it!=m_Faces.end(); f_it++){
            SBFace* F					= *f_it;
        	VERIFY						(F->bone_id!=-1);
            SBBone& B					= m_Bones[F->bone_id];
            for (int k=0; k<3; k++)		B.offset.add(F->o[k]);
        }
        for (SBBoneVecIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++){
            SBBone& B					= *b_it;
            VERIFY						(0!=B.f_cnt);
            B.offset.div				(B.f_cnt*3);
        }
        Fvector& offs					= m_Bones.front().offset;
        for (SBBoneVecIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++)
            b_it->offset.sub			(offs);
    }
    return m_bValid;
}

IC void recurse_tri(SBPart* P, SBFaceVec& faces, SBAdjVec& adjs, SBFace* F)
{
	if (F->marked) 		return;

	P->append_face		(F);
    F->marked			= true;

    // recurse
    for (int k=0; k<3; k++){
	    SBFaceVec& PL 	= adjs[F->vert_id[k]];
        for (SBFaceVecIt pl_it=PL.begin(); pl_it!=PL.end(); pl_it++)
            recurse_tri	(P,faces,adjs,*pl_it);
    }
}

void CGeomPartExtractor::AppendFace(const Fvector* v, const Fvector* n, const Fvector2* uvs[3])
{
	SBFace* F			= new SBFace(uvs);
    // insert verts
    for (int k=0; k<3; k++){
        F->vert_id[k] 	= m_Verts->add_vert(v[k]);
	    F->o[k].set		(v[k]);
	    F->n[k].set		(n[k]);
    }
    m_Faces.push_back	(F);
}
CGeomPartExtractor::CGeomPartExtractor	()
{
	m_Verts				= 0;
}
void CGeomPartExtractor::Initialize	(const Fbox& bb, float eps, u32 per_bone_face_count_min)
{
	VERIFY				(0==m_Verts);
	m_Verts 			= new VCPacked(bb,eps);
    m_PerBoneFaceCountMin=per_bone_face_count_min; VERIFY(m_PerBoneFaceCountMin>0);
}
void CGeomPartExtractor::Clear		()
{
    delete		m_Verts;
    for (SBFaceVecIt f_it=m_Faces.begin(); f_it!=m_Faces.end(); f_it++) 
    	delete	*f_it;
    m_Faces.clear	();
    for (SBPartVecIt p_it=m_Parts.begin(); p_it!=m_Parts.end(); p_it++) 
    	delete	*p_it;
    m_Parts.clear	();
    m_Adjs.clear	();
}
BOOL CGeomPartExtractor::Process()
{
    // make adjacement
    {
        m_Adjs.resize	(m_Verts->getVS());
        for (SBFaceVecIt f_it=m_Faces.begin(); f_it!=m_Faces.end(); f_it++)
            for (int k=0; k<3; k++) m_Adjs[(*f_it)->vert_id[k]].push_back(*f_it);
    }
    // extract parts
    {
        for (SBFaceVecIt f_it=m_Faces.begin(); f_it!=m_Faces.end(); f_it++)
        {
            SBFace* F	= *f_it;
            if (!F->marked)
            {
                SBPart* P 		= new SBPart();
                recurse_tri		(P,m_Faces,m_Adjs,*f_it);
                m_Parts.push_back	(P);
            }
        }
    }
    // simplify parts
    {
        for (SBPartVecIt p_it=m_Parts.begin(); p_it!=m_Parts.end(); p_it++)
        {	
        	(*p_it)->prepare	(m_Adjs,m_PerBoneFaceCountMin);
        }
    }
    return TRUE;
}

