// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
class FXRayImporterModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
