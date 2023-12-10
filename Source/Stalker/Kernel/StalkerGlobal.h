#pragma once
namespace FStalkerFileSystem
{
	inline FArrayReader ReadChunk(FArchive&Ar,uint32 ChunkID)
	{
		Ar.Seek(0);
		while (!Ar.AtEnd())
		{
			uint32 InSize,InChunkID;
			Ar<<InChunkID<<InSize;
			if (InChunkID == ChunkID)
			{
				FArrayReader ArrayReader;
				ArrayReader.AddUninitialized(InSize);
				Ar.Serialize(ArrayReader.GetData(),InSize);
				return MoveTemp(ArrayReader);
			}
			else
			{
				Ar.Seek(Ar.Tell() + InSize);
			}
		}
		checkNoEntry();
		return {};
	}
	inline bool FindChunk(FArchive&Ar,uint32 ChunkID)
	{
		Ar.Seek(0);
		while (!Ar.AtEnd())
		{
			uint32 InSize,InChunkID;
			Ar<<InChunkID<<InSize;
			if (InChunkID == ChunkID)
			{
				return true;
			}
			else
			{
				Ar.Seek(Ar.Tell() + InSize);
			}
		}
		return false;
	}
	inline void SkipStringZ(FArchive&Ar)
	{
		while(!Ar.AtEnd())
		{
			int8 Ch = 0;
			Ar<<Ch;
			if (Ch == 0)
			{
				return;
			}
		}
	}
}

namespace FStalkerTools
{
	FORCEINLINE constexpr uint64 MakeClassID(char Symbol0,char Symbol1,char Symbol2,char Symbol3,char Symbol4,char Symbol5,char Symbol6,char Symbol7)
	{
		return (uint64(Symbol0)<<uint64(56))|(uint64(Symbol1)<<uint64(48))|(uint64(Symbol2)<<uint64(40))|(uint64(Symbol3)<<uint64(32))|(uint64(Symbol4)<<uint64(24))|(uint64(Symbol5)<<uint64(16))|(uint64(Symbol6)<<uint64(8))|(uint64(Symbol7));
	}
}
