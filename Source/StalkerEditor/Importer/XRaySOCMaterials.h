#pragma once
class XRaySOCMaterials
{
public:
	void Load();
	void UnLoad();
	const TMap<shared_str, TPair<shared_str, float>>&GetTexture2Details() const { return Texture2Details; }
	const TMap<shared_str, TPair<shared_str, float>>&GetTexture2Bumps() const { return Texture2Bumps; }
private:
	TMap<shared_str, TPair<shared_str, float>> Texture2Details;
	TMap<shared_str, TPair<shared_str, float>> Texture2Bumps;

};