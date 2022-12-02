#pragma once
#define CHUNK_OBJECT_BODY   0x7777

struct SRayPickInfo;
struct FSChunkDef;
class CFrustum;
class IReader;
class IWriter;
class COMotion;
class CCustomMotion;
class SSceneSummary;
class ESceneCustomOTool;

struct SExportStreamItem
{
	int					chunk;
	CMemoryWriter		stream;
    SExportStreamItem	():chunk(0){;}
};

struct SExportStreams
{
	SExportStreamItem	spawn;
	SExportStreamItem	spawn_rs;
	SExportStreamItem	patrolpath;
	SExportStreamItem	rpoint;
	SExportStreamItem	sound_static;
	SExportStreamItem	sound_env_geom;
	SExportStreamItem	pe_static;
	SExportStreamItem	envmodif;
	SExportStreamItem	fog_vol;
};

class  CCustomObject
{
protected:
    shared_str		EName;
    // orientation
	Fvector 		EPosition;
	Fvector 		EPositionSaved;
	Fvector 		EScale;
	Fvector 		EScaleSaved;
	Fvector 		ERotation;
	Fvector 		ERotateSaved;
	SAnimParams*	m_MotionParams;
    COMotion*		m_Motion;

    // private animation methods
    void 			AnimationOnFrame	();
	void 			AnimationDrawPath	();
    void			AnimationCreateKey	(float t);
    void			AnimationDeleteKey	(float t);
    void			AnimationUpdate		(float t);
public:
	enum{
    	flSelected_notused			= (1<<0),
    	flVisible_notused			= (1<<1),
    	flLocked_notused			= (1<<2),
    	flMotion					= (1<<3),
    	flRenderAnyWayIfSelected	= (1<<4),
        flObjectInGroup				= (1<<5),
        flObjectInGroupUnique		= (1<<6),
        
    	flAutoKey					= (1<<30),
    	flCameraView				= (1<<31),
    };
    Flags32			m_CO_Flags;

	enum{
        flRT_Valid			= (1<<0),
        flRT_UpdateTransform= (1<<1),
        flRT_NeedSelfDelete	= (1<<2),
    	flRT_Selected		= (1<<3),
    	flRT_Visible		= (1<<4),
    	flRT_SelectedLast	= (1<<5),
        
    };
    Flags32			m_RT_Flags;
public:
	shared_str		FName;
	int 			save_id;
    // orientation
    Fvector 		FPosition;
    Fvector 		FScale;
    Fvector 		FRotation;
    Fmatrix			FTransformP;
    Fmatrix			FTransformR;
    Fmatrix			FTransformS;
    Fmatrix			FTransformRP;
	Fmatrix 		FTransform;
    Fmatrix			FITransformRP;
	Fmatrix 		FITransform;

    CCustomObject*	m_pOwnerObject;
public:
	LPCSTR			GetName			() const {return *FName; }
	void			SetName			(LPCSTR N){string256 tmp;FCStringAnsi::Strcpy(tmp,N); strlwr(tmp); FName=tmp;}

    virtual const Fvector& GetPosition	()	const { return FPosition; 	}
    virtual const Fvector& GetRotation	()	const { return FRotation;	}
    virtual const Fvector& GetScale		()	const { return FScale; 		}

    virtual void 	SetPosition		(const Fvector& pos)	{ FPosition.set(pos);	UpdateTransform();}
	virtual void 	SetRotation		(const Fvector& rot)	{ FRotation.set(rot);  VERIFY(_valid(FRotation)); UpdateTransform();}
    virtual void 	SetScale		(const Fvector& scale)	{ FScale.set(scale);	UpdateTransform();}


    virtual void	DeleteThis		(){m_RT_Flags.set(flRT_NeedSelfDelete,TRUE);}
public:
					CCustomObject	(LPVOID data, LPCSTR name);
	virtual 		~CCustomObject	();

    BOOL 			Editable		() const ;
    
	IC BOOL 		Motionable		()const {return m_CO_Flags.is(flMotion); 	}
	IC BOOL 		Visible			()const {return m_RT_Flags.is(flRT_Visible);	}
	IC BOOL 		Selected		()const {return m_RT_Flags.is(flRT_Selected);}
    IC BOOL			Valid			()const {return m_RT_Flags.is(flRT_Valid);}
    IC BOOL			IsDeleted		()const {return m_RT_Flags.is(flRT_NeedSelfDelete);}


    virtual void 	OnUpdateTransform();

    void			ResetTransform	(){
    					FScale.set				(1,1,1);
    					FRotation.set			(0,0,0);
    					FPosition.set			(0,0,0);
						FTransform.identity		();
	    				FTransformRP.identity	();
                        FITransform.identity	();
	    				FITransformRP.identity	();
					}
    virtual void 	ResetAnimation	(bool upd_t=true){;}
    virtual void 	UpdateTransform	(bool bForced=false){m_RT_Flags.set(flRT_UpdateTransform,TRUE);if(bForced)OnUpdateTransform();}

    // animation methods
    
    // grouping methods
    virtual void	OnDetach		();
    virtual void	OnAttach		(CCustomObject* owner);
    CCustomObject* 	GetOwner		(){return m_pOwnerObject;}
    virtual bool	CanAttach		()=0;

    virtual bool	OnChooseQuery	(LPCSTR specific){return true;}
    
    // change position/orientation methods

	virtual bool 	LoadStream		(IReader&);
	virtual bool 	LoadLTX			(CInifile& ini, LPCSTR sect_name);


    virtual LPCSTR	RefName			(){return 0;}

    IC const Fmatrix& _ITransform			(){return FITransform;}
    IC const Fmatrix& _Transform			(){return FTransform;}
    IC const Fvector& _Position				(){return FPosition;}
    IC const Fvector& _Rotation				(){return FRotation;}
    IC const Fvector& _Scale				(){return FScale;}
	IC void ScaleSave                       (){EScaleSaved = FScale;}
	IC const Fvector& GetSaveScale          (){return EScaleSaved;}
	IC void RotateSave                      (){ERotateSaved = FRotation;}
	IC const Fvector& GetSaveRotate         (){return ERotateSaved;}
    virtual void           PositionSave          () { EPositionSaved = FPosition; }
	IC const Fvector& GetSavePosition       () { return EPositionSaved; }

    virtual void* QueryInterface(ObjClassID InClassID) { if(InClassID == OBJCLASS_DUMMY) return this; return nullptr;;}

    ObjClassID		FClassID;
    ESceneCustomOTool* FParentTools;
private:
    int m_ButtonId;
    float m_FromTime;
    float m_ToTime;
    float m_ScaleFactor;
    float m_Speed;
};