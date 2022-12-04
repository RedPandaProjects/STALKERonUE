#include "StalkerKinematicsAnimData.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/motion.h"
THIRD_PARTY_INCLUDES_END

void FStalkerKinematicsAnimData::BuildFromLegacy(CMotionDef& Def, TArray<TSharedPtr<CBoneData>>& Bones, CPartition& Partition)
{
	Accrue = Def.Accrue();
	Power = Def.Power();
	Falloff = Def.Falloff();

	FXBoneOrPart = NAME_None;
	if (Def.flags&esmFX)
	{
		if (Def.bone_or_part != BI_NONE)
		{
			FXBoneOrPart =  Bones[Def.bone_or_part]->name.c_str();
		}
	}
	else if(Def.bone_or_part != BI_NONE)
	{
		FXBoneOrPart = Partition.part(Def.bone_or_part).Name.c_str();
	}

	Flags = 0;

	Flags |= Def.flags& esmFX?int32(EStalkerKinematicsAnimFlags::FX):0;
	Flags |= Def.flags & esmIdle ? int32(EStalkerKinematicsAnimFlags::Idle) : 0;
	Flags |= Def.flags & esmRootMover ? int32(EStalkerKinematicsAnimFlags::RootMover) : 0;
	Flags |= Def.flags & esmStopAtEnd ? int32(EStalkerKinematicsAnimFlags::StopAtEnd) : 0;
	Flags |= Def.flags & esmSyncPart ? int32(EStalkerKinematicsAnimFlags::SyncPart) : 0;
	Flags |= Def.flags & esmUseFootSteps ? int32(EStalkerKinematicsAnimFlags::UseFootSteps) : 0;

	for (auto& Mark : Def.marks)
	{
		Marks.AddDefaulted();
		for (auto& Intervals : Mark.intervals)
		{
			Marks.Last().Intervals. Add({Intervals.first, Intervals.second});
		}
	}
}
