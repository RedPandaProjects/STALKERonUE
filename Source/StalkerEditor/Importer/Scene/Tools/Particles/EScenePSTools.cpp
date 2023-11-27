#include "EScenePSTools.h"
#include "../../Entitys/ParticlesObject/ParticlesObject.h"

EScenePSTools::~EScenePSTools()
{

}

FXRayCustomObject* EScenePSTools::CreateObject(LPVOID data, LPCSTR name)
{
	FXRayCustomObject* O	= new CParticlesObject(data,name);
    O->FParentTools		= this;
    return O;
}


