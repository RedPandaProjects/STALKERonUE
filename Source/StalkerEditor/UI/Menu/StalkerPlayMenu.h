#pragma once
class StalkerPlayMenu
{
public:
	void Initialize();
	void Destroy();
private:
	void CreateAutoBuildSubMenu(UToolMenu*InToolMenu);
};