#pragma once

class FRBMKSceneParticleSystem: public FRBMKSceneObjectBase
{
public:
					        FRBMKSceneParticleSystem      (const FString&Name);
			                ~FRBMKSceneParticleSystem     () override;
  	bool 	                LoadStream			                (IReader&)override;
  	bool 	                LoadLTX				                (CInifile& INIFile, const FString&SectionName)override;
	void*                   QueryInterface                      (ERBMKSceneObjectType ObjectType) override;
    FString				    ReferenceName;
};