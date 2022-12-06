#pragma once
class XRayBoneInstanceLegacy :public IBoneInstance
{

public:
	const Fmatrix& GetTransform() const override;
	void SetTransform(const Fmatrix& val) override;
	Fmatrix mTransform;
	Fmatrix mRenderTransform;
	void construct();
};