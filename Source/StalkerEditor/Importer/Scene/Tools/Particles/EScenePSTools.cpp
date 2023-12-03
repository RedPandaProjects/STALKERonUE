#include "EScenePSTools.h"
#include "../../Entitys/ParticlesObject/ParticlesObject.h"

EScenePSTools::~EScenePSTools()
{

}

TSharedPtr<FXRayCustomObject> EScenePSTools::CreateObject(LPVOID data, LPCSTR name)
{
	return MakeShared<CParticlesObject>(data,name);
}
