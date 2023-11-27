#include "CustomObject.h"

void FXRayCustomObject::AnimationCreateKey(float t)
{
    Fvector R;		R.set(-GetRotation().x,-GetRotation().y,-GetRotation().z);
    m_Motion->CreateKey(t,GetPosition(),R);
}

void FXRayCustomObject::AnimationDeleteKey(float t)
{
    m_Motion->DeleteKey(t);
}

//float speed = 0.f;
void FXRayCustomObject::AnimationUpdate(float t)
{
    Fvector R,P,r;   
    m_Motion->_Evaluate		(t,P,r);
    R.set					(-r.x,-r.y,-r.z);

//    speed					= speed*0.9f+(P.distance_to(GetPosition())/EDevice->fTimeDelta)*0.1f;
//    Log("speed: ",speed);
    SetPosition(P);
    SetRotation(R);
    BOOL bAK				= m_CO_Flags.is(flAutoKey);
    m_CO_Flags.set			(flAutoKey,FALSE);
    UpdateTransform			(true);
    m_CO_Flags.set			(flAutoKey,bAK);
}

void FXRayCustomObject::AnimationOnFrame()
{
	VERIFY (m_Motion);

    if (Selected()&&m_MotionParams->bPlay)
    {
    	AnimationUpdate			(m_MotionParams->Frame());
        m_MotionParams->Update	(Device->fTimeDelta,1.f,true);
    }
}
