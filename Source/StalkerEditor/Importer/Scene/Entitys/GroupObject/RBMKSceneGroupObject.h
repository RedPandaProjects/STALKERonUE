#pragma once

class FRBMKSceneGroupObject: public FRBMKSceneObjectBase
{
public:
					        FRBMKSceneGroupObject	    (const FString&Name);
	virtual 		        ~FRBMKSceneGroupObject	    ();
  	virtual bool 	        LoadStream			(IReader&);
  	virtual bool 	        LoadLTX				(CInifile& INIFile, const FString& SectionName);
	void*                   QueryInterface      (ERBMKSceneObjectType InClassID) override;
private:
    TArray<TSharedPtr<FRBMKSceneObjectBase>>    ObjectsInGroup;
};