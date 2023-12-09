#pragma once
struct FRMBKSceneSpawnData
{
							FRMBKSceneSpawnData		(class FRBMKSceneSpawnPoint* InOwner);
							~FRMBKSceneSpawnData	();
	void					Create					(const char* EntityRerefence);
    void					Destroy					();
    bool					Valid					()const;

    bool					LoadStream				(IReader&);
    bool					LoadLTX					(CInifile& INIFile, const FString& SectionName);
	ISE_Abstract*			GetEntity				() const;
private:
	CLASS_ID				ClassID;
    shared_str 				Profile;
	ISE_Abstract*			ServerAbstract;
    Flags8					Flags;
    FRBMKSceneSpawnPoint*	Owner;
};

class FRBMKSceneSpawnPoint : public FRBMKSceneObjectBase
{
public:            
										FRBMKSceneSpawnPoint    	(const FString&InName);
										~FRBMKSceneSpawnPoint   	() override; 
	bool 								LoadStream		(IReader&) override;
	bool 								LoadLTX			(CInifile& INIFile, const FString&SectionName) override;
	void*								QueryInterface	(ERBMKSceneObjectType InClassID) override;

	FRMBKSceneSpawnData    				SpawnData;
	TSharedPtr<FRBMKSceneObjectBase>	AttachedObject;
};
