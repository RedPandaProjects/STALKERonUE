#pragma once
class StalkerKinematicsBoneInstance :public IBoneInstance
{

public:
	const Fmatrix& GetTransform() const override;
	void SetTransform(const Fmatrix& val) override;
	Fmatrix Transform;
};