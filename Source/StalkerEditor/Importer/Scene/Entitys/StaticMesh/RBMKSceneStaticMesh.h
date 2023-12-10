#pragma once
class FRBMKSceneStaticMesh : public FRBMKSceneObjectBase
{
public:
						FRBMKSceneStaticMesh	(const FString&InName);
						~FRBMKSceneStaticMesh	();	
	
	bool 				LoadStream				(IReader&) override;	
	bool 				LoadLTX					(CInifile& INIFile, const FString&SectionName) override;	

	void*				QueryInterface			(ERBMKSceneObjectType InClassID) override;

	bool 				IsMUStatic     			() const;
	bool 				IsStatic     			() const;
	CEditableObject*	GetReferenceObject		() const;
	FString				ReferenceName;
private:
	void				UpdateReference			();
	CEditableObject*	ReferenceObject;
};

