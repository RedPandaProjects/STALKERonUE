#pragma once

class CFrustum;
class CWayPoint;

struct SWPLink
{          
	CWayPoint*	way_point;
    float		probability;
//			    SWPLink():way_point(0),probability(0){}
			    SWPLink(CWayPoint* wp, float pb):way_point(wp),probability(pb){}
};

DEFINE_VECTOR(SWPLink*, WPLVec, WPLIt);

class CWayPoint
{

	friend class Gizmo;
    friend class CPatrolPoint;
    friend class CPatrolPath;
    friend class CWayObject;
    friend class TfrmPropertiesWayPoint;
    void		CreateLink		(CWayPoint* P, float pb);
    bool		AppendLink		(CWayPoint* P, float pb);
    bool		DeleteLink		(CWayPoint* P);
public:

	shared_str		m_Name;
	Fvector		m_vPosition;
	Fvector		m_vSavePosition;
	Flags32		m_Flags;
	BOOL		m_bSelected;
	WPLVec		m_Links;
	WPLIt		FindLink        (CWayPoint* P);
	            CWayPoint       (LPCSTR name);
	            ~CWayPoint      ();
};

DEFINE_VECTOR(CWayPoint*,WPVec,WPIt);

class CWayObject: public CCustomObject
{
protected:
    friend class TfrmPropertiesWayPoint;
    friend class CPatrolPath;
	friend class CPatrolPoint;
	friend class Gizmo;
    typedef CCustomObject inherited;
public:
					CWayObject		(LPVOID data, const char* name);
	virtual			~CWayObject		();
    void            Construct   	(LPVOID data);
    void			Clear			();
    // change position/orientation methods
	virtual void 	RotateParent	(Fvector& axis, float angle ){;}
	virtual void 	RotateLocal		(Fvector& axis, float angle ){;}
	virtual void 	RotatePivot		(const Fmatrix& prev_inv, const Fmatrix& current){;}
	virtual void 	Scale			(Fvector& amount){;}
	virtual void 	ScalePivot		(const Fmatrix& prev_inv, const Fmatrix& current, Fvector& amount){;}

  	virtual bool 	LoadStream		(IReader&);
  	virtual bool 	LoadLTX			(CInifile& ini, const char* sect_name);
	virtual bool	CanAttach() { return true; }
	
	void*			QueryInterface(ObjClassID InClassID) override;
	WPVec			m_WayPoints;
};