#pragma once

#define SPAWNPOINT_VERSION   			0x0017

// refs
class ISE_Visual;
class ISE_Motion;
class CObjectAnimator;

class CLE_Visual 
{
public:
    ISE_Visual* source;
public:
    CLE_Visual(ISE_Visual* src);
    virtual			~CLE_Visual();
};
class CSpawnPoint : public FXRayCustomObject
{
	typedef FXRayCustomObject inherited;

    friend class    SceneBuilder;
public:                           
   
    class CLE_Motion
    {
    public:
        ISE_Motion*		source;
        CObjectAnimator*animator;
        void 			OnChangeMotion	();
        void 			PlayMotion		();
    public:
						CLE_Motion		(ISE_Motion* src);
        virtual			~CLE_Motion		();
    };
	struct SSpawnData: public ISE_AbstractLEOwner
	{
		CLASS_ID		m_ClassID;
        shared_str 		m_Profile;
		ISE_Abstract*	m_Data;
        Flags8			m_flags;

        CSpawnPoint*	m_owner;
        enum{ eSDTypeRespawn = (1<<0),};
        SSpawnData	(CSpawnPoint* o)
        {
			m_ClassID	= 0;
			m_Data		= 0;
            m_owner		= o;
            m_flags.zero();
        }
        ~SSpawnData	()
        {
        	Destroy	();
        }
        void			Create			(LPCSTR entity_ref);
        void			Destroy			();
        bool			Valid			()const{return m_Data != nullptr;}

        bool		    LoadStream		(IReader&);
        bool		    LoadLTX			(CInifile& ini, LPCSTR sect_name);

        
		virtual void get_bone_xform				(LPCSTR name, Fmatrix& xform);
        IC ISE_Abstract* GetEntity()const { return m_Data; }
	};

	SSpawnData    	m_SpawnData;
	TSharedPtr<FXRayCustomObject>	m_AttachedObject;

    shared_str		m_rpProfile;
    union{
    	struct{
		    u8					m_RP_TeamID;
		    u8					m_RP_Type;
			GameTypeChooser     m_GameType;
        };
        struct{
        	float	m_EM_Radius;
            float 	m_EM_Power;
            float	m_EM_ViewDist;
            u32		m_EM_FogColor;
            float	m_EM_FogDensity;
            u32		m_EM_AmbientColor;
            u32		m_EM_SkyColor;
            u32		m_EM_HemiColor;
            Flags16	m_EM_Flags;
        };
    };

public:
	                CSpawnPoint    	(LPVOID data, LPCSTR name);
    void            Construct   	(LPVOID data);
    virtual         ~CSpawnPoint   	();
    virtual bool	CanAttach		() {return true;}

    bool			CreateSpawnData	(LPCSTR entity_ref);

	virtual bool 	LoadStream		(IReader&);
	virtual bool 	LoadLTX			(CInifile& ini, LPCSTR sect_name);




	void* QueryInterface(ERBMKSceneObjectType InClassID) override;
};
