#pragma once
class XRayEngineFactory
{
public:
	XRayEngineFactory(UObject* InParentPackage, EObjectFlags InFlags);
	~XRayEngineFactory();
	UObject*		ImportObject(const FString& FileName, bool UseOnlyFullPath = false);
	UStaticMesh*	ImportObjectAsStaticMesh(CEditableObject*Object,bool UseOnlyFullPath = false);
	UMaterialInterface* ImportSurface(const FString& Path, CSurface* Surface);
	UTexture2D* ImportTexture (const FString& FileName);
	void		ImportBump2D  (const FString& FileName, TObjectPtr<UTexture2D>& NormalMap, TObjectPtr<UTexture2D>& Specular, TObjectPtr<UTexture2D>& Height);
private:
	UObject* ParentPackage;
	EObjectFlags ObjectFlags;
	TArray< CEditableObject*> Objects;
};