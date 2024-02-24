#pragma once

class SStalkerStartupScreen: public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SStalkerStartupScreen) {}

	SLATE_END_ARGS()

	EVisibility ThrobberVsisble() const;
	void Construct(const FArguments& InArgs);
};
