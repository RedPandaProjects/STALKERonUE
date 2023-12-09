#pragma once

enum class ERBMKSceneShapeUsage:uint32 
{
	Common = 0,
	LevelBound = 1
};

class FRBMKSceneShape: public FRBMKSceneObjectBase,protected CShapeData
{
public:
							FRBMKSceneShape 		(const FString&Name);
							~FRBMKSceneShape		() override;
  	bool 					LoadStream				(IReader&) override;
  	bool 					LoadLTX					(CInifile& INIFile, const FString&SectionName) override;
    ShapeVec&				GetShapes				(){return shapes;}
	void*					QueryInterface			(ERBMKSceneObjectType InClassID) override;
	
	ERBMKSceneShapeUsage	ShapeType;
};