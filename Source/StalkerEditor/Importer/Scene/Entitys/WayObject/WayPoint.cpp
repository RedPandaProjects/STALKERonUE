#include "WayPoint.h"

#define WAYPOINT_SIZE 	1.5f
#define WAYPOINT_RADIUS WAYPOINT_SIZE*.5f

// BASE offset
#define WAY_BASE					0x1000
#define POINT_BASE					0x2000

// POINT chunks
#define RPOINT_CHUNK				POINT_BASE+ptRPoint

// WAY chunks
#define WAY_PATROLPATH_CHUNK		WAY_BASE+wtPatrolPath
//----------------------------------------------------

#define WAYOBJECT_VERSION			0x0013
//----------------------------------------------------
#define WAYOBJECT_CHUNK_VERSION		0x0001
#define WAYOBJECT_CHUNK_POINTS		0x0002
#define WAYOBJECT_CHUNK_LINKS		0x0003
#define WAYOBJECT_CHUNK_TYPE		0x0004
#define WAYOBJECT_CHUNK_NAME		0x0005


// Way Point
CWayPoint::CWayPoint(const char* name)
{
	m_Name			= name;
	m_vPosition.set	(0,0,0);
	m_Flags.zero	();
    m_bSelected		= false;
}

CWayPoint::~CWayPoint()
{
	for (WPLIt it=m_Links.begin(); it!=m_Links.end(); it++)
    	delete *it;
    m_Links.clear	();
}

void CWayPoint::CreateLink(CWayPoint* P, float pb)
{
	if (P!=this) m_Links.push_back(new SWPLink(P,pb));
}
bool CWayPoint::AppendLink(CWayPoint* P, float pb)
{
	if (FindLink(P)==m_Links.end()){
    	CreateLink(P,pb);
        return true;
    }
    return false;
}
bool CWayPoint::DeleteLink(CWayPoint* P)
{
	WPLIt it = FindLink(P);
	if (it!=m_Links.end()){
    	delete		*it;
		m_Links.erase	(it);
    	return true;
    }
    return false;
}
// Way Object
CWayObject::CWayObject(LPVOID data, const char* name):FXRayCustomObject(data,name)
{
	Construct(data);
}

void CWayObject::Construct(LPVOID data)
{
	FClassID   	= OBJCLASS_WAY;
}

CWayObject::~CWayObject()
{
	Clear();
}

void CWayObject::Clear()
{
	for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++)
    	delete *it;
    m_WayPoints.clear();
}


bool CWayObject::LoadLTX(CInifile& ini, const char* sect_name)
{
	Clear();

	u32 version 	= ini.r_u32(sect_name, "version");

    if(version!=WAYOBJECT_VERSION)
    {
        ELog.DlgMsg	( mtError, "CWayPoint: Unsupported version.");
        return 		false;
    }

	FXRayCustomObject::LoadLTX(ini, sect_name);

    if(!GetName())
	{
		ELog.DlgMsg(mtError, "Corrupted scene file.[%s] sect[%s] has empty name", ini.fname(), sect_name);
		return false;
    }

    u32 cnt = ini.r_u32(sect_name, "wp_count");
    m_WayPoints.resize(cnt);

    string128 		buff;
    u32				wp_idx	= 0;
	for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); ++it,++wp_idx)
    {
    	CWayPoint* W 	= new CWayPoint("");
        *it 			= W;

        sprintf				(buff,"wp_%d_pos",wp_idx);
        W->m_vPosition		= ini.r_fvector3(sect_name, buff);

        sprintf				(buff,"wp_%d_flags",wp_idx);
    	W->m_Flags.assign	(ini.r_u32(sect_name, buff));

        sprintf				(buff,"wp_%d_selected",wp_idx);
        W->m_bSelected		= ini.r_bool(sect_name, buff);

        sprintf				(buff,"wp_%d_name",wp_idx);
        W->m_Name			= ini.r_string(sect_name, buff);
    }

    CInifile::Sect& S 		= ini.r_section(sect_name);
    CInifile::SectCIt cit 	= S.Data.begin();
    CInifile::SectCIt cit_e 	= S.Data.end();
    for( ;cit!=cit_e; ++cit)
    {
    	if( cit->first.c_str() == strstr(cit->first.c_str(),"link_wp_") )
        {
        	wp_idx 		= u32(-1);
        	u32 wp_link_idx = u32(-1);

        	int res = sscanf(cit->first.c_str(),"link_wp_%4d_%4d",&wp_idx, &wp_link_idx);
            check(res==2);

            Fvector2 val 		= ini.r_fvector2(sect_name,cit->first.c_str());
            u32 wp_to_idx 		= iFloor(val.x);
        	m_WayPoints[wp_idx]->CreateLink(m_WayPoints[wp_to_idx], val.y);
        }
    }

    return true;
}

void* CWayObject::QueryInterface(EXRayObjectClassID InClassID)
{
	if (InClassID == OBJCLASS_WAY)
		return this;
	return inherited::QueryInterface(InClassID);
}

bool CWayObject::LoadStream(IReader& F)
{
	Clear();

	u16 version = 0;
    shared_str buf;

    if (!F.find_chunk(WAYOBJECT_CHUNK_VERSION)) return false;
    check(F.r_chunk(WAYOBJECT_CHUNK_VERSION,&version));
    if(version!=WAYOBJECT_VERSION){
        ELog.DlgMsg( mtError, "CWayPoint: Unsupported version.");
        return false;
    }

	FXRayCustomObject::LoadStream(F);

	check(F.find_chunk(WAYOBJECT_CHUNK_POINTS));
    m_WayPoints.resize(F.r_u16());
	for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++){
    	CWayPoint* W 	= new CWayPoint(""); *it = W;
    	F.r_fvector3	(W->m_vPosition);
    	W->m_Flags.assign(F.r_u32());
        W->m_bSelected	= F.r_u16();
        F.r_stringZ		(buf);
        W->m_Name		= buf.c_str();
    }

	check(F.find_chunk(WAYOBJECT_CHUNK_LINKS));
    int l_cnt = F.r_u16();
    for (int k=0; k<l_cnt; k++){
    	int idx0 	= F.r_u16();
    	int idx1 	= F.r_u16();
        float pb	= F.r_float();
        m_WayPoints[idx0]->CreateLink(m_WayPoints[idx1],pb);
    }


    return true;
}
WPLIt CWayPoint::FindLink(CWayPoint* P)
{
	for (WPLIt it = m_Links.begin(); it != m_Links.end(); it++)
		if ((*it)->way_point == P) return it;
	return m_Links.end();
}