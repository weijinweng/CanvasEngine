#include "CVS_Animation.h"
#include "CVS_AnimationSystem.h"
#include "Timer.h"
#include "CVS_Mesh.h"

bool CVS_AnimationSystem::End()
{
	return true;
}

bool CVS_AnimationSystem::Update()
{
	auto dt = (float)m_pTimer->split() / 1000;

	for (auto i : m_registeredClips)
	{
		auto pComponent = i.first;
		auto& localPose = pComponent->m_pRenderComp->m_pNode->m_pSkeletonPose->m_aLocalPose;
		auto& globalPose = pComponent->m_pRenderComp->m_pNode->m_pSkeletonPose->m_aGlobalPose;
		for (auto j : i.second)
		{
			// Update animation
			j->m_dt = dt;
			// Evaluate Animation
			auto pAnimationSample = evaluate(j->m_pAnimation, j->m_dt);
			localPose = pAnimationSample->m_aLocalPose;

			auto pSkeleton = pComponent->m_pRenderComp->m_pNode->mesh->m_pSkeleton;

			// First calculate absolute positions
			for (int i = 0, e = pSkeleton->m_bones.size(); i < e; ++i)
			{
				auto* pBone = &pSkeleton->m_bones[i];
				auto& parentTransform = (pBone->m_parentIndex == 0xFF) ? cmat4() : globalPose[pBone->m_parentIndex];// m_pSkeleton->m_bones[pBone->m_parentIndex].m_transform;
				// Before we send it to the render node, we must multiply the transform matrix by its local inverse bind pose matrix
				globalPose[i] = parentTransform * localPose[i].toMat4();
			}

			// Then apply inverse bind pose matrix
			// First calculate absolute positions
			for (int i = 0, e = pSkeleton->m_bones.size(); i < e; ++i)
			{
				auto* pBone = &pSkeleton->m_bones[i];
				// Before we send it to the render node, we must multiply the transform matrix by its local inverse bind pose matrix
				globalPose[i] *= pBone->m_bindPoseInv;
			}

			// must delete temp sample manually
			delete pAnimationSample;
		}

	}


	return true;
}

bool CVS_AnimationSystem::Initialize()
{
	m_pTimer = new Timer;
	return true;
}

CVS_AnimationSample* CVS_AnimationSystem::evaluate(CVS_Animation* _pAnimation, float _dt)
{
	auto frameRate = _pAnimation->m_frameRate;
	auto frameCount = _pAnimation->m_frameCount;
	float frameTime = _dt * frameRate;


	if (frameTime > frameCount)
	{
		if (_pAnimation->m_isLooping)
		{
			// no - 1 here because we wanna actually play the entirity of the last frame
			frameTime = fmod(frameTime, frameCount);
		}
		else
		{
			return getFrame(_pAnimation, frameCount);
		}
	}

	// get weight first to determine whether or not we need to blend
	float weight = fmod(frameTime, 1);
	auto pCurrentFrame = getFrame(_pAnimation, (int)frameTime);

	if (weight == 0.0f)
	{
		return pCurrentFrame;
	}

	auto pTargetFrame = getFrame(_pAnimation, (int)frameTime + 1);

	//printf("frametime : %d, +1: %d\n", (int)frameTime, (int)frameTime+1);

	int numNodes = 0;
	// get nodes
	numNodes = pCurrentFrame->m_aLocalPose.size();
	for (int i = 0, ei = numNodes; i < ei; ++i)
	{
		auto& currentBonePose = pCurrentFrame->m_aLocalPose[i];
		auto& targetBonePose = pTargetFrame->m_aLocalPose[i];

		currentBonePose.m_rotation = glm::slerp(currentBonePose.m_rotation, targetBonePose.m_rotation, weight);
		currentBonePose.m_translation = glm::mix(currentBonePose.m_translation, targetBonePose.m_translation, weight);
		currentBonePose.m_scaling = glm::mix(currentBonePose.m_scaling, targetBonePose.m_scaling, weight);
	}

	// Only keep the frame that stores the info we want
	delete pTargetFrame;

	return pCurrentFrame;
}

CVS_AnimationSample* CVS_AnimationSystem::getFrame(CVS_Animation* _pAnimation, int _frameIndex)
{
	std::vector<CVS_AnimationLayerSample*> sampleLayers;
	int numNodes = 0;
	// get nodes on all layers
	for (int i = 0, ei = _pAnimation->m_animLayers.size(); i < ei; ++i)
	{
		auto pLayer = _pAnimation->m_animLayers[i];
		CVS_AnimationLayerSample* pLayerSample = new CVS_AnimationLayerSample;
		// get weight
		pLayerSample->m_weight = pLayer->m_weight;
		// get nodes
		numNodes = pLayer->m_animNodes.size();
		pLayerSample->m_aLocalPose.resize(numNodes);
		for (int j = 0, ej = numNodes; j < ej; ++j)
		{
			auto& pNode = pLayer->m_animNodes[j];
			auto& tCurve = pNode.m_channels[static_cast<int>(CVS_AnimationNode::ECurveType::Translation)];
			auto& rCurve = pNode.m_channels[static_cast<int>(CVS_AnimationNode::ECurveType::Rotation)];
			auto& sCurve = pNode.m_channels[static_cast<int>(CVS_AnimationNode::ECurveType::Scaling)];
			cvec3 t = tCurve ? tCurve->m_keyFrames[_frameIndex].m_keyValue : cvec3();
			cquat r = rCurve ? cquat(glm::radians(rCurve->m_keyFrames[_frameIndex].m_keyValue)) : cquat();
			cvec3 s = sCurve ? sCurve->m_keyFrames[_frameIndex].m_keyValue : cvec3(1.0f);

			pLayerSample->m_aLocalPose[j] = CVS_BonePose(s, r, t);
		}
		sampleLayers.push_back(pLayerSample);
	}

	if (sampleLayers.size() == 1)
	{
		return static_cast<CVS_AnimationSample*>(sampleLayers.back());
	}

	// get weighted average
	CVS_AnimationSample* pFinalSample = sampleLayers[0];
	for (int i = 0, ei = numNodes; i < ei; ++i)
	{
		// TODO: it assumes there's at least 1 layer

		// First layer
		float& finalWeight = sampleLayers[0]->m_weight;
		cquat& finalR = sampleLayers[0]->m_aLocalPose[i].m_rotation;
		cvec3& finalT = sampleLayers[0]->m_aLocalPose[i].m_translation * finalWeight;
		cvec3& finalS = sampleLayers[0]->m_aLocalPose[i].m_scaling * finalWeight;

		// If more than 1 layer
		for (int j = 1, ej = sampleLayers.size(); j < ej; ++j)
		{
			auto pLayerSample = sampleLayers[j];

			// LERP T and S
			finalT += pLayerSample->m_aLocalPose[i].m_translation * pLayerSample->m_weight;
			finalS += pLayerSample->m_aLocalPose[i].m_scaling * pLayerSample->m_weight;

			// SLERP rotation
			finalWeight += pLayerSample->m_weight;
			glm::slerp(finalR, pLayerSample->m_aLocalPose[i].m_rotation, pLayerSample->m_weight / finalWeight);

		}

		pFinalSample->m_aLocalPose[i] = CVS_BonePose(finalS, finalR, finalT);
	}
	sampleLayers.erase(sampleLayers.begin());
	for (auto i : sampleLayers)
	{
		// Get rid of other temp layers
		delete i;
	}

	return pFinalSample;
}

