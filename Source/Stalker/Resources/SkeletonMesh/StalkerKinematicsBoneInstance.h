#pragma once
class StalkerKinematicsBoneInstance :public IBoneInstance
{

public:
	StalkerKinematicsBoneInstance();
	const Fmatrix& GetTransform() const override;
	void SetTransform(const Fmatrix& val) override;
	Fmatrix Transform;
	bool bUpdated;
};