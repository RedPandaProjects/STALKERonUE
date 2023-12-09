#pragma once
class  FRBMKSceneObjectBase
{
public:
    					    FRBMKSceneObjectBase    (const FString&InName);
	virtual 		        ~FRBMKSceneObjectBase   ();
	virtual bool 	        LoadStream              (IReader&);
	virtual bool 	        LoadLTX                 (CInifile& INIFile, const FString&SectionName);
    virtual const TCHAR*    ReferenceName	        (){return TEXT("");}
    virtual void*           QueryInterface          (ERBMKSceneObjectType InClassID);
    FRBMKSceneObjectBase* 	GetOwner                (){return Owner;}
    ERBMKSceneObjectType    GetObjectType           () const {return ObjectType;}
    const FString&          GetName                 () const {return Name;}
    const FTransform&       GetTransform            () const {return Transform;}
protected:
    ERBMKSceneObjectType    ObjectType;
    FRBMKSceneObjectBase*   Owner;
    FString                 Name;
    FTransform              Transform;
};