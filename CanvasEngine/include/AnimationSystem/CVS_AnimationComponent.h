#pragma once
#include "CVS_World.h"

struct CVS_AnimationComponent : public CVS_GameComponent
{
	CVS_AnimationComponent(CVS_GameObject* _pObject, CVS_RenderComponent* _pRenderComp);

	float m_dt;
	CVS_Animation* m_pAnimation;
	CVS_RenderComponent* m_pRenderComp;
	void Update();
	void PlayAnim(CVS_Animation* _pAnimation, bool isLooping, float _startTime = .0f);
};