#pragma once
#include "CVS_Resource.h"

class CVS_Skeleton;

struct CVS_AnimationKeyFrame
{
	enum EInterpolationType
	{
		eInterpolationConstant = 0x00000002,	//!< Constant value until next key.
		eInterpolationLinear = 0x00000004,		//!< Linear progression to next key.
		eInterpolationCubic = 0x00000008		//!< Cubic progression to next key.
	};

	EInterpolationType m_interpolationType;
	cvec3 m_keyValue;
};

struct CVS_AnimationCurve : public CVS_InternalResource
{
	CVS_AnimationCurve() : CVS_InternalResource("AnimationCurve", EType::AnimationCurve)
	{

	}

	std::vector<CVS_AnimationKeyFrame> m_keyFrames;
};

struct CVS_AnimationNode : public CVS_InternalResource
{
	CVS_AnimationNode() : CVS_InternalResource("AnimationNode", EType::AnimationNode), m_pParent()
	{
		m_channels.resize(static_cast<int>(ECurveType::NumCurveTypes));
	}
	
	enum ECurveType
	{
		Translation,
		Rotation,
		Scaling,
		NumCurveTypes
	};

	CVS_AnimationNode* m_pParent;
	std::vector<CVS_AnimationNode*> m_children;
	std::vector<CVS_AnimationCurve*> m_channels;

	int initFromFbxNode(FbxNode* _pNode);
};

struct CVS_AnimationLayer : CVS_InternalResource
{
	CVS_AnimationLayer() : CVS_InternalResource("AnimationLayer", EType::AnimationLayer), m_pRootAnimNode()
	{

	}
	CVS_AnimationNode* m_pRootAnimNode;
	std::vector<CVS_AnimationNode> m_animNodes;
	float m_weight;
};


struct CVS_Animation : public CVS_Resource
{
	CVS_Animation() : CVS_Resource("Animation", EType::Animation), m_frameRate(0), m_frameCount(0), m_isLooping(true)
	{

	}

	CVS_Skeleton* m_pSkeleton;
	float m_frameRate;
	int m_frameCount;
	bool m_isLooping;
	std::vector<CVS_AnimationLayer*> m_animLayers;
};
