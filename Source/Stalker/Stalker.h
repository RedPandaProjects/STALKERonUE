#pragma once

class FStalkerModule : public FDefaultGameModuleImpl
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void OnPreExit();
};
