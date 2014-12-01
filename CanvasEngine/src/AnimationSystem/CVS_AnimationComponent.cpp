#pragma once
#include "Canvas.h"
#include "CVS_AnimationComponent.h"

void CVS_AnimationComponent::Update()
{
// 	for (int i = 0, ei = m_pAnimation->m_animLayers.size(); i < ei; ++i)
// 	{
// 		auto pLayer = m_pAnimation->m_animLayers[i];
// 		for (int j = 0, ej = pLayer->m_animNodes.size(); j < ej; ++j)
// 		{
// 			auto pBone = &m_pRenderComp->m_pNode->mesh->m_pSkeleton->m_bones[j];
// 
// 			auto pNode = &pLayer->m_animNodes[j];
// 			auto tCurve = pNode->m_channels[static_cast<int>(CVS_AnimationNode::ECurveType::Translation)];
// 			auto rCurve = pNode->m_channels[static_cast<int>(CVS_AnimationNode::ECurveType::Rotation)];
// 			auto sCurve = pNode->m_channels[static_cast<int>(CVS_AnimationNode::ECurveType::Scaling)];
// 			cvec3 t = tCurve ? tCurve->m_keyFrames[0].m_keyValue : cvec3();
// 			cquat r = rCurve ? cquat(glm::radians(rCurve->m_keyFrames[0].m_keyValue)) : cquat();
// 			cvec3 s = sCurve ? sCurve->m_keyFrames[0].m_keyValue : cvec3(1.0f);
// 			//for (int k = 0, ek = static_cast<int>(CVS_AnimationNode::ECurveType::NumCurveTypes); k < ek; ++k)
// 
// 			m_pRenderComp->m_pNode->m_boneMats[j] =
// 				glm::translate(cmat4(), t) *
// 				glm::mat4_cast(r) *
// 				glm::scale(cmat4(), s);
// 		}
// 	}

}

CVS_AnimationComponent::CVS_AnimationComponent(CVS_GameObject* _pObject, CVS_RenderComponent* _pRenderComp) : CVS_GameComponent(_pObject), m_pRenderComp(_pRenderComp)
{

}

void CVS_AnimationComponent::PlayAnim(CVS_Animation* _pAnimation, bool isLooping, float _startTime)
{
	GLOBALSTATEMACHINE.m_AnimationSub.m_registeredClips[this].push_back(new CVS_AnimationClip(_pAnimation, _startTime, isLooping));
}

