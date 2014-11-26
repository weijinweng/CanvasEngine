#pragma once
#include "CVS_Resource.h"

static const char animChannels[] = { FBXSDK_CURVENODE_TRANSLATION, FBXSDK_CURVENODE_ROTATION, FBXSDK_CURVENODE_SCALING };
static const char animComponents[] = { FBXSDK_CURVENODE_COMPONENT_X, FBXSDK_CURVENODE_COMPONENT_Y, FBXSDK_CURVENODE_COMPONENT_Z };

struct CVS_AnimationNode : public CVS_InternalResource
{

	CVS_AnimationNode* m_pParent;
	std::vector<CVS_AnimationNode*> m_children;

	std::vector<CVS_AnimationCurve*> m_channels;

	uint8 m_index;
	cvec3 m_color;

	cmat4 m_offset;
	cmat4 m_transform;

	// Current SRT
	cquat m_rotation;
	cvec3 m_translation;
	cvec3 m_scaling;

	CVS_AnimationNode();
	int initFromFbxNode(FbxNode* _pNode);
};

struct CVS_AnimationCurve
{
	uint8 m_boneIndex;

};