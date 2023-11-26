#include "StalkerKinematicsAnimAssetUserData.h"
THIRD_PARTY_INCLUDES_START
#include "XrEngine/motion.h"
THIRD_PARTY_INCLUDES_END

void UStalkerKinematicsAnimAssetUserData::BuildFromLegacy(CMotionDef& Def, TArray<TSharedPtr<CBoneData>>& Bones, CPartition& Partition)
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

void UStalkerKinematicsAnimAssetUserData::BuildToLegacy(CMotionDef& Def, TMap<shared_str, u16>& BonesName2ID, TMap<shared_str, u32>& BonesPartsName2ID)
{
	Def.motion = BI_NONE;
	Def.flags = 0;
	Def.flags |= Flags & int32(EStalkerKinematicsAnimFlags::FX) ? esmFX : 0;
	Def.flags |= Flags & int32(EStalkerKinematicsAnimFlags::Idle) ? esmIdle : 0;
	Def.flags |= Flags & int32(EStalkerKinematicsAnimFlags::RootMover) ? esmRootMover : 0;
	Def.flags |= Flags & int32(EStalkerKinematicsAnimFlags::StopAtEnd) ? esmStopAtEnd : 0;
	Def.flags |= Flags & int32(EStalkerKinematicsAnimFlags::SyncPart) ? esmSyncPart : 0;
	Def.flags |= Flags & int32(EStalkerKinematicsAnimFlags::UseFootSteps) ? esmUseFootSteps : 0;

	Def.accrue = Def.Quantize(Accrue / fQuantizerRangeExt);
	Def.falloff = Def.Quantize(Falloff / fQuantizerRangeExt);
	Def.power = Def.Quantize(Power);
	Def.speed = 0;
	Def.bone_or_part = BI_NONE;
	if (!FXBoneOrPart.IsNone())
	{
		shared_str Name = TCHAR_TO_ANSI(*FXBoneOrPart.ToString().ToLower());
		if (Def.flags & esmFX)
		{

			Def.bone_or_part = BonesName2ID[Name];

		}
		else
		{
			Def.bone_or_part = BonesPartsName2ID[Name];
		}
	}
	Def.marks.clear();
	for (auto& Mark : Marks)
	{
		Def.marks.push_back(motion_marks());
		for (auto& Interval : Mark.Intervals)
		{
			motion_marks::interval interval;
			interval.first = Interval.Start;
			interval.second = Interval.End;
			Def.marks.back().intervals.push_back(interval);
		}
	}
}
