#pragma once
#include "Resources/PhysicalMaterial/StalkerPhysicalMaterial.h"

struct FRBMKShaderDescription
{
public:
	uint64	ClassID = 0;
	FString	Name;
	uint16	Version = 0;
	inline void	Serialize(FArchive& Ar)
	{
		Ar<<ClassID;
		char InName[128];
		Ar.Serialize(InName,128);
		Name = ANSI_TO_TCHAR(InName);
		char InComputer[32];
		Ar.Serialize(InComputer,32);
		int32 Time;
		Ar<<Time;
		Ar<<Version;
		int16 Pad0 = 0;
		Ar<<Pad0;
	}
	friend FArchive& operator<<(FArchive& Ar, FRBMKShaderDescription& ShaderDescription)
	{
		ShaderDescription.Serialize(Ar);
		return Ar;
	}
};
enum class ERBMKShaderPropertiesType:uint32
{
	Marker = 0,
	Matrix = 1,
	Texture = 3,
	Integer = 4,
	Float = 5,
	Bool = 6,
};
class FRBMKShadersManager;
class FRBMKShaderBase
{
public:
									FRBMKShaderBase		(FRBMKShadersManager*Owner);
	virtual							~FRBMKShaderBase	();
	virtual void					Serialize			(FArchive& Ar);
	virtual UMaterialInterface*		ImportSurface		(const FString& Path,const FString& TextureName, const FString& GameMaterial,bool HudMode);
	static UStalkerPhysicalMaterial*GetPhysicalMaterial	(const FString&Name);
	FRBMKShaderDescription			Description;
protected:
	static void						SerializeMaker		(FArchive& Ar,ERBMKShaderPropertiesType PropertiesType);
	static void						SkipMatrix			(FArchive& Ar);
	static void						SerializeTexture	(FArchive& Ar,FString&Value);
	static void						SerializeInteger	(FArchive& Ar,int32&Value);
	static void						SerializeFloat		(FArchive& Ar,float&Value);
	static void						SerializeBool		(FArchive& Ar,bool&Value);
	int32							Priority;
	bool							StrictSorting;
	FString							MainTexture;
	FRBMKShadersManager*			Owner;
};