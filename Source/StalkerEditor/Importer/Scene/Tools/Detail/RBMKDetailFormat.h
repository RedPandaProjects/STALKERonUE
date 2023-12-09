#pragma once
#pragma pack(push,1)
struct FRBMKDetailHeader
{
	uint32		Version;
	uint32		ObjectCount;
	int32		OffsetX,OffsetZ;
	uint32		SizeX,	SizeZ;
};
struct FRBMKDetailPalette
{
	uint16		Alpha0:4;
	uint16		Alpha1:4;
	uint16		Alpha2:4;
	uint16		Alpha3:4;
};
struct FRBMKDetailSlot					// was(4+4+3*4+2 = 22b), now(8+2*4=16b)
{
	static constexpr uint32	IDEmpty	= 0x3f;

	inline float GetYBase	() const {	return static_cast<float>(YBase)*.2f - 200.f;}
	inline float GetYHeight	() const {	return static_cast<float>(YHeight)*.1f;}
	inline uint8 GetID		(int32 Index) const
	{	
        switch(Index)
		{
	        case 0: return static_cast<uint8>(ID0);
	        case 1: return static_cast<uint8>(ID1);
	        case 2: return static_cast<uint8>(ID2);
	        case 3: return static_cast<uint8>(ID3);
	        default: checkNoEntry();	
        }
		return 0;
    }

    inline void SetID (int32 Index, uint8 Value)
	{	
        switch(Index)
		{
	        case 0: ID0=Value; break;
	        case 1: ID1=Value; break;
	        case 2: ID2=Value; break;
	        case 3: ID3=Value; break;
	        default:  checkNoEntry();	
        }
    }

	uint32				YBase			:	12;
	uint32				YHeight			:	8;
	uint32				ID0				:   6;
	uint32				ID1				:	6;
	uint32				ID2				:	6;
	uint32				ID3				:	6;
	uint32				DirectionPower	:	4;
	uint32				HemiPower		:	4;
	uint32				ColorR			:	4;
	uint32				ColorG			:	4;
	uint32				ColorB			:	4;
	FRBMKDetailPalette	Palette[4];
};

#pragma pack(pop)
