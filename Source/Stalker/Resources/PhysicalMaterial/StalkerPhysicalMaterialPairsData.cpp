#include "StalkerPhysicalMaterialPairsData.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/XrGameMaterialLibraryInterface.h"
THIRD_PARTY_INCLUDES_END
void FStalkerPhysicalMaterialPair::BuildFromLegacy(const SGameMtlPair& Legacy)
{
	BreakingSounds.Empty();
	for (const shared_str& name : Legacy.BreakingSoundsNames)
	{
		BreakingSounds.Add(name.c_str());
	}
	CollideSounds.Empty();
	for (const shared_str& name : Legacy.CollideSoundsNames)
	{
		CollideSounds.Add(name.c_str());
	}
	StepSounds.Empty();
	for (const shared_str& name : Legacy.StepSoundsNames)
	{
		StepSounds.Add(name.c_str());
	}
	CollideParticles.Empty();
	for (const shared_str& name : Legacy.CollideParticles)
	{
		CollideParticles.Add(name.c_str());
	}

	CollideMarks.Empty();
	for (const shared_str& name : Legacy.CollideMarksLegacy)
	{
		CollideMarks.Add(name.c_str());
	}
}

void FStalkerPhysicalMaterialPair::BuildToLegacy(SGameMtlPair& Legacy)
{
	checkSlow(Legacy.BreakingSounds.empty());
	Legacy.BreakingSounds.resize(BreakingSounds.Num());
	for (int32 i = 0; i < BreakingSounds.Num(); i++)
	{
		Legacy.BreakingSounds[i].create(TCHAR_TO_ANSI(*BreakingSounds[i]), st_Effect, sg_SourceType);
	}
	checkSlow(Legacy.CollideSounds.empty());
	Legacy.CollideSounds.resize(CollideSounds.Num());
	for (int32 i = 0; i < CollideSounds.Num(); i++)
	{
		Legacy.CollideSounds[i].create(TCHAR_TO_ANSI(*CollideSounds[i]), st_Effect, sg_SourceType);
	}
	checkSlow(Legacy.StepSounds.empty());
	Legacy.StepSounds.resize(StepSounds.Num());
	for (int32 i = 0; i < StepSounds.Num(); i++)
	{
		Legacy.StepSounds[i].create(TCHAR_TO_ANSI(*StepSounds[i]), st_Effect, sg_SourceType);
	}
	checkSlow(Legacy.CollideParticles.empty());
	Legacy.CollideParticles.clear();
	for (const FString& name : CollideParticles)
	{
		Legacy.CollideParticles.push_back(TCHAR_TO_ANSI(*name));
	}
	checkSlow(Legacy.CollideMarks->IsEmpty());
	Legacy.CollideMarks->Clear();
	for (const FString& name : CollideMarks)
	{
		Legacy.CollideMarks->AppendMark(TCHAR_TO_ANSI(*name));
	}
}
