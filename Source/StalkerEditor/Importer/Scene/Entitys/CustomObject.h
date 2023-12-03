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
class FRBMKSceneObjectsToolBase;

class  FXRayCustomObject
{
public:
	shared_str		FName;
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

    FXRayCustomObject*	m_pOwnerObject;
public:
	LPCSTR			GetName			() const {return *FName; }
	void			SetName			(LPCSTR N){string256 tmp;FCStringAnsi::Strcpy(tmp,N); strlwr(tmp); FName=tmp;}

   const Fvector& GetPosition	()	const { return FPosition; 	}
   const Fvector& GetRotation	()	const { return FRotation;	}
   const Fvector& GetScale		()	const { return FScale; 		}

   void 	SetPosition		(const Fvector& pos)	{ FPosition.set(pos);	UpdateTransform();}
	void 	SetRotation		(const Fvector& rot)	{ FRotation.set(rot);  VERIFY(_valid(FRotation)); UpdateTransform();}
   void 	SetScale		(const Fvector& scale)	{ FScale.set(scale);	UpdateTransform();}

public:
					FXRayCustomObject	(LPVOID data, LPCSTR name);
	virtual 		~FXRayCustomObject	();


    void			ResetTransform	(){
    					FScale.set				(1,1,1);
    					FRotation.set			(0,0,0);
    					FPosition.set			(0,0,0);
						FTransform.identity		();
	    				FTransformRP.identity	();
                        FITransform.identity	();
	    				FITransformRP.identity	();
					}
    virtual void 	UpdateTransform	();

    // animation methods
    
    // grouping methods
    FXRayCustomObject* 	GetOwner		(){return m_pOwnerObject;}
	virtual bool 	LoadStream		(IReader&);
	virtual bool 	LoadLTX			(CInifile& ini, LPCSTR sect_name);
    virtual LPCSTR	RefName			(){return 0;}


    virtual void* QueryInterface(ERBMKSceneObjectType InClassID) { if(InClassID == ERBMKSceneObjectType::AllTypes) return this; return nullptr;;}

    ERBMKSceneObjectType		FClassID;
    FRBMKSceneObjectsToolBase*  FParentTools;
private:
    int m_ButtonId;
    float m_FromTime;
    float m_ToTime;
    float m_ScaleFactor;
    float m_Speed;
};