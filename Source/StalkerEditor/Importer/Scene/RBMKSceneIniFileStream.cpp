#include "RBMKSceneIniFileStream.h"

const char* FRBMKSceneIniFileStream::gen_name()
{
	++Counter;
	FCStringAnsi::Sprintf(BufferStr,"%06d",Counter);
	return BufferStr;
}

void FRBMKSceneIniFileStream::w_float( float a)
{
	INIFile->w_float(SectionName.c_str(),gen_name(),a);
}

void FRBMKSceneIniFileStream::w_vec3( const Fvector& a)
{
	INIFile->w_fvector3(SectionName.c_str(),gen_name(),a);
}

void FRBMKSceneIniFileStream::w_vec4( const Fvector4& a)
{
	INIFile->w_fvector4(SectionName.c_str(),gen_name(),a);
}
void FRBMKSceneIniFileStream::w_u64( u64 a)
{
	INIFile->w_u64(SectionName.c_str(),gen_name(),a);
}
void FRBMKSceneIniFileStream::w_s64( s64 a)
{
	INIFile->w_s64(SectionName.c_str(),gen_name(),a);
}
void FRBMKSceneIniFileStream::w_u32( u32 a)
{
	INIFile->w_u32(SectionName.c_str(),gen_name(),a);
}
void FRBMKSceneIniFileStream::w_s32( s32 a)
{
	INIFile->w_s32(SectionName.c_str(),gen_name(),a);
}
void FRBMKSceneIniFileStream::w_u16( u16 a)
{
	INIFile->w_u16(SectionName.c_str(),gen_name(),a);
}
void FRBMKSceneIniFileStream::w_s16( s16 a)
{
	INIFile->w_s16(SectionName.c_str(),gen_name(),a);
}
void FRBMKSceneIniFileStream::w_u8( u8 a)
{
	INIFile->w_u8(SectionName.c_str(),gen_name(),a);
}
void FRBMKSceneIniFileStream::w_s8( s8 a)
{
	INIFile->w_s8(SectionName.c_str(),gen_name(),a);
}
void FRBMKSceneIniFileStream::w_stringZ( LPCSTR S)
{
	string4096 Buffer;
	FCStringAnsi::Sprintf(Buffer, "\"%s\"",(S)?S:"");
	INIFile->w_string(SectionName.c_str(),gen_name(),Buffer);
}

void FRBMKSceneIniFileStream::r_vec3(Fvector& A)
{
	A = INIFile->r_fvector3(SectionName.c_str(),gen_name());
}
void FRBMKSceneIniFileStream::r_vec4(Fvector4& A)
{
	A = INIFile->r_fvector4(SectionName.c_str(),gen_name());
}
void FRBMKSceneIniFileStream::r_float(float& A)
{
	A = INIFile->r_float(SectionName.c_str(),gen_name());
}
void FRBMKSceneIniFileStream::r_u8(u8& A)
{
	A = INIFile->r_u8(SectionName.c_str(),gen_name());
}
void FRBMKSceneIniFileStream::r_u16(u16& A)
{
	A = INIFile->r_u16(SectionName.c_str(),gen_name());
}
void FRBMKSceneIniFileStream::r_u32(u32& A)
{
	A = INIFile->r_u32(SectionName.c_str(),gen_name());
}
void FRBMKSceneIniFileStream::r_u64(u64& A)
{
	A = INIFile->r_u64(SectionName.c_str(),gen_name());
}

void FRBMKSceneIniFileStream::r_s8(s8& A)
{
	A = INIFile->r_s8(SectionName.c_str(),gen_name());
}
void FRBMKSceneIniFileStream::r_s16(s16& A)
{
	A = INIFile->r_s16(SectionName.c_str(),gen_name());
}
void FRBMKSceneIniFileStream::r_s32(s32& A)
{
	A = INIFile->r_s32(SectionName.c_str(),gen_name());
}
void FRBMKSceneIniFileStream::r_s64(s64& A)
{
	A = INIFile->r_s64(SectionName.c_str(),gen_name());
}

void FRBMKSceneIniFileStream::r_string(LPSTR dest, u32 dest_size)
{
    shared_str S;
	S = INIFile->r_string_wb(SectionName.c_str(),gen_name());
    check(dest_size>=S.size());
    xr_strcpy(dest, dest_size, S.c_str());
}

void FRBMKSceneIniFileStream::r_string(xr_string& name)
{
	name = INIFile->r_string_wb(SectionName.c_str(), gen_name()).c_str();
}

void FRBMKSceneIniFileStream::skip_stringZ()
{
	gen_name();
}

