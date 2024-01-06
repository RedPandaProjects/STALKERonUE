#include "XRayFontRender.h"
THIRD_PARTY_INCLUDES_START
#include "xrEngine/GameFont.h"
THIRD_PARTY_INCLUDES_END
#include "Resources/StalkerResourcesManager.h"
#include "Kernel/StalkerEngineManager.h"
ENGINE_API extern  Fvector2 g_current_font_scale;

XRayFontRender::XRayFontRender() /*: m_index_count(0), m_vertex_count(0)*/
{
	Font = nullptr;
	FontSize = 1.f;
}

XRayFontRender::~XRayFontRender()
{
}


void XRayFontRender::Initialize(LPCSTR font, float size)
{
	Font = GStalkerEngineManager->GetResourcesManager()->GetFont(font);
	FontSize = size*0.7f;
}

float XRayFontRender::GetTextSize(LPCSTR s)
{
	{
		TextChace.Empty();
		size_t Size = MultiByteToWideChar(CP_ACP, 0, s, -1, NULL, 0);
		TextChace.AddZeroed(Size + 1);
		MultiByteToWideChar(CP_ACP, 0, s, -1, TextChace.GetData(), Size);
	}
	return GetTextSize(WCHAR_TO_TCHAR(TextChace.GetData()));;
}

float XRayFontRender::GetTextSize(const wide_char* wsStr)
{
	return GetTextSize((const TCHAR *)wsStr);
}

float XRayFontRender::GetTextSize(const char cChar)
{
	char Text[2] = {cChar,0};
	return GetTextSize(Text);
}

float XRayFontRender::GetTextSize(const TCHAR* Text)
{
	if (!IsValid(Font))
	{
		return 0;
	}
	float Scale = FontSize / Font->LegacyFontSize;
	return Font->GetStringSize(Text)* Scale;
}

void XRayFontRender::OnRender(CGameFont& owner)
{
	if (!IsValid(Font))
	{
		return;
	}
	for (u32 i = 0; i < owner.strings.size();i++ )
	{
		CGameFont::String& PS = owner.strings[i];
		FString Text;
		{
			TextChace.Empty();
			size_t Size = MultiByteToWideChar(CP_ACP, 0, PS.string, -1, NULL, 0);
			TextChace.AddZeroed(Size + 1);
			MultiByteToWideChar(CP_ACP, 0, PS.string, -1, TextChace.GetData(), Size);
			Text = FString(WCHAR_TO_TCHAR(TextChace.GetData()));
		}

		float X = PS.x;
		float fSize = 0;
		if (PS.align)
		{
			fSize = GetTextSize(*Text)*owner.fCurrentHeight;
		}

		switch (PS.align)
		{
		case CGameFont::alCenter:
			X -= (iFloor(fSize * 0.5f));

			break;
		case CGameFont::alRight:
			X -= iFloor(fSize);
			break;
		}
		
		GXRayUIRender.PushText(X,PS.y, PS.height, PS.c, Font, FontSize*owner.fCurrentHeight, *Text);
	} 
//	if (!FontShader.Brush)
//	{
//		return;
//	}
//	if (!(owner.uFlags & CGameFont::fsValid)) 
//	{
//		FVector2D ImageSize = FontShaderBrush->Brush.GetImageSize();
//		owner.vTS.set((int)ImageSize.X, (int)ImageSize.Y);
//		owner.fTCHeight = owner.fHeight / float(owner.vTS.y);
//		owner.uFlags |= CGameFont::fsValid;
//	}
//
//	for (u32 i = 0; i < owner.strings.size(); )
//	{
//		// calculate first-fit
//		int		count = 1;
//
//		size_t length = owner.smart_strlen(owner.strings[i].string);
//
//		while ((i + count) < owner.strings.size()) {
//			int L = owner.smart_strlen(owner.strings[i + count].string);
//
//			if ((L + length) < MAX_MB_CHARS) {
//				count++;
//				length += L;
//			}
//			else		break;
//		}
//		GXRayUIRender.SetShader(FontShader);
//		GXRayUIRender.StartPrimitive(length * 6,IUIRender::ptTriList,IUIRender::pttTL);
//		// fill vertices
//		u32 last = i + count;
//		for (; i < last; i++) 
//		{
//			CGameFont::String& PS = owner.strings[i];
//			wide_char wsStr[MAX_MB_CHARS];
//
//			int	len = owner.IsMultibyte() ?
//				mbhMulti2Wide(wsStr, NULL, MAX_MB_CHARS, PS.string) :
//				xr_strlen(PS.string);
//
//			if (len) 
//			{
//				float	X = float(iFloor(PS.x));
//				float	Y = float(iFloor(PS.y));
//				float	S = PS.height * g_current_font_scale.y;
//				float	Y2 = Y + S;
//				float fSize = 0;
//
//				if (PS.align)
//					fSize = owner.IsMultibyte() ? owner.SizeOf_(wsStr) : owner.SizeOf_(PS.string);
//
//				switch (PS.align)
//				{
//				case CGameFont::alCenter:
//					X -= (iFloor(fSize * 0.5f)) * g_current_font_scale.x;
//					break;
//				case CGameFont::alRight:
//					X -= iFloor(fSize);
//					break;
//				}
//
//				u32	clr, clr2;
//				clr2 = clr = PS.c;
//				if (owner.uFlags & CGameFont::fsGradient) {
//					u32	_R = color_get_R(clr) / 2;
//					u32	_G = color_get_G(clr) / 2;
//					u32	_B = color_get_B(clr) / 2;
//					u32	_A = color_get_A(clr);
//					clr2 = color_rgba(_R, _G, _B, _A);
//				}
//
//				X -= 0.5f;
//				Y -= 0.5f;
//				Y2 -= 0.5f;
//
//				float	tu, tv;
//				for (int j = 0; j < len; j++)
//				{
//					Fvector l;
//
//					l = owner.IsMultibyte() ? owner.GetCharTC(wsStr[1 + j]) : owner.GetCharTC((u16)(u8)PS.string[j]);
//
//					float scw = l.z * g_current_font_scale.x;
//
//					float fTCWidth = l.z / owner.vTS.x;
//
//					if (!fis_zero(l.z))
//					{
//						//						tu			= ( l.x / owner.vTS.x ) + ( 0.5f / owner.vTS.x );
//						//						tv			= ( l.y / owner.vTS.y ) + ( 0.5f / owner.vTS.y );
//						tu = (l.x / owner.vTS.x);
//						tv = (l.y / owner.vTS.y);
//#ifndef	USE_DX10
//						//	Make half pixel offset for 1 to 1 mapping
//						tu += (0.5f / owner.vTS.x);
//						tv += (0.5f / owner.vTS.y);
//#endif	//	USE_DX10
//
//						GXRayUIRender.PushPoint(X, Y2,0, clr2, tu, tv + owner.fTCHeight);					
//						GXRayUIRender.PushPoint(X, Y, clr, 0, tu, tv);									
//						GXRayUIRender.PushPoint(X + scw, Y2, 0, clr2, tu + fTCWidth, tv + owner.fTCHeight);	
//						GXRayUIRender.PushPoint(X, Y, 0, clr, tu, tv);									
//						GXRayUIRender.PushPoint(X + scw, Y, 0, clr, tu + fTCWidth, tv);					
//						GXRayUIRender.PushPoint(X + scw, Y2, 0, clr2, tu + fTCWidth, tv + owner.fTCHeight);		
//					}
//					X += scw * owner.vInterval.x;
//					if (owner.IsMultibyte()) {
//						X -= 2;
//						if (IsNeedSpaceCharacter(wsStr[1 + j]))
//							X += owner.fXStep;
//					}
//				}
//			}
//		}
//
//		GXRayUIRender.FlushPrimitive();
//
//	}
}
