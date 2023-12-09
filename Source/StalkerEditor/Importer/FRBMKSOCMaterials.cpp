#include "FRBMKSOCMaterials.h"

void FRBMKSOCMaterials::Load()
{
	string_path INIFileName;
	if (FS.exist(INIFileName, "$game_textures$", "textures.ltx"))
	{
		CInifile			ini(INIFileName);
		if (ini.section_exist("association"))
		{
			CInifile::Sect& data = ini.r_section("association");
			CInifile::SectCIt I = data.Data.begin();
			CInifile::SectCIt E = data.Data.end();
			for (; I != E; ++I)
			{
				const CInifile::Item& item = *I;

				string_path				TextureName;
				float					DetailScale;

				const int32 Result = sscanf_s(*item.second, "%[^,],%f", TextureName, static_cast<uint32>(sizeof(string_path)), &DetailScale);
				check(Result == 2);
				Texture2Details.Add(item.first, TPair<shared_str, float>(TextureName, DetailScale));
			}
		}

		if (ini.section_exist("specification"))
		{
			CInifile::Sect& sect = ini.r_section("specification");
			for (CInifile::SectCIt I2 = sect.Data.begin(); I2 != sect.Data.end(); ++I2)
			{
				const CInifile::Item& item = *I2;


				string_path				BumpMode;
				float					Material;
				const int32 Result = sscanf_s(item.second.c_str(), "bump_mode[%[^]]], material[%f]", BumpMode,static_cast<uint32>(sizeof(string_path)), &Material);
				check(Result == 2);
				if ((BumpMode[0] == 'u') && (BumpMode[1] == 's') && (BumpMode[2] == 'e') && (BumpMode[3] == ':'))
				{
					Texture2Bumps.Add(item.first, TPair<shared_str, float>(BumpMode+4, Material));
				}
				else
				{
					Texture2Bumps.Add(item.first, TPair<shared_str, float>("",Material));
				}
			}
		}
	}
}

void FRBMKSOCMaterials::UnLoad()
{
	Texture2Details.Empty();
	Texture2Bumps.Empty();
}
