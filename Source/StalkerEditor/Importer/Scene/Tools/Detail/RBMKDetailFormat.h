#pragma once
#pragma pack(push,1)
struct FDetailHeader
{
	uint32		Version;
	uint32		ObjectCount;
	int32		OffsetX,OffsetZ;
	uint32		SizeX,	SizeZ;
};
struct FDetailPalette
{
	uint16		Alpha0:4;
	uint16		Alpha1:4;
	uint16		Alpha2:4;
	uint16		Alpha3:4;
};
struct FDetailSlot					// was(4+4+3*4+2 = 22b), now(8+2*4=16b)
{
	static constexpr uint32	IDEmpty	= 0x3f;

	inline float GetYBase	()	{	return float(YBase)*.2f - 200.f;}
	inline float GetYHeight	()	{	return float(YHeight)*.1f;}
	inline uint8 GetID		(int32 Index)
	{	
        switch(Index)
		{
	        case 0: return (uint8)id0;
	        case 1: return (uint8)id1;
	        case 2: return (uint8)id2;
	        case 3: return (uint8)id3;
	        default: checkNoEntry();	
        }
		return 0;
    }

    inline void SetID (int32 Index, uint8 Value)
	{	
        switch(Index)
		{
	        case 0: id0=Value; break;
	        case 1: id1=Value; break;
	        case 2: id2=Value; break;
	        case 3: id3=Value; break;
	        default:  checkNoEntry();	
        }
    }

	uint32				YBase			:	12;	// 11	// 1 unit = 20 cm, low = -200m, high = 4096*20cm - 200 = 619.2m
	uint32				YHeight			:	8;	// 20	// 1 unit = 10 cm, low = 0,     high = 256*10 ~= 25.6m
	uint32				id0				:   6;	// 26	// 0x3F(63) = empty
	uint32				id1				:	6;	// 32	// 0x3F(63) = empty
	uint32				id2				:	6;	// 38	// 0x3F(63) = empty
	uint32				id3				:	6;	// 42	// 0x3F(63) = empty
	uint32				DirectionPower	:	4;	// 48	// 0..1 q
	uint32				HemiPower		:	4;	// 52	// 0..1 q
	uint32				ColorR			:	4;	// 56	// rgb = 4.4.4
	uint32				ColorG			:	4;	// 60	// rgb = 4.4.4
	uint32				ColorB			:	4;	// 64	// rgb = 4.4.4
	FDetailPalette		Palette [4];
};

#pragma pack(pop)
