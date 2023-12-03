#include "RBMKSOCMaterials.h"

void RBMKSOCMaterials::Load()
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


				string_path				T;
				float					s;

				int res = sscanf_s(*item.second, "%[^,],%f", T, uint32(sizeof(string_path)), &s);
				R_ASSERT(res == 2);
				Texture2Details.Add(item.first, TPair<shared_str, float>(T, s));
			}
		}//"association"

		if (ini.section_exist("specification"))
		{
			CInifile::Sect& sect = ini.r_section("specification");
			for (CInifile::SectCIt I2 = sect.Data.begin(); I2 != sect.Data.end(); ++I2)
			{
				const CInifile::Item& item = *I2;


				string_path				bmode;
				float					material;
				int res = sscanf_s(item.second.c_str(), "bump_mode[%[^]]], material[%f]", bmode,uint32( sizeof(string_path)), &material);

				R_ASSERT(res == 2);
				if ((bmode[0] == 'u') && (bmode[1] == 's') && (bmode[2] == 'e') && (bmode[3] == ':'))
				{
					Texture2Bumps.Add(item.first, TPair<shared_str, float>(bmode+4, material));
				}
				else
				{
					Texture2Bumps.Add(item.first, TPair<shared_str, float>("",material));
				}
			}
		}//"specification"
	}//file-exist*/
}

void RBMKSOCMaterials::UnLoad()
{
	Texture2Details.Empty();
	Texture2Bumps.Empty();
}
