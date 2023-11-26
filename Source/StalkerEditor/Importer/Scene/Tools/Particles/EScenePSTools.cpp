#include "EScenePSTools.h"
#include "../../Entitys/ParticlesObject/ParticlesObject.h"

EScenePSTools::~EScenePSTools()
{

}

CCustomObject* EScenePSTools::CreateObject(LPVOID data, LPCSTR name)
{
	CCustomObject* O	= new CParticlesObject(data,name);
    O->FParentTools		= this;
    return O;
}


