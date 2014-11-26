#pragma once
#include "CVS_Resource.h"
class CVS_Skeleton;

struct CVS_Bone : public CVS_InternalResource
{
	CVS_Bone* m_pParent;
	std::vector<CVS_Bone*> m_children;

	uint8 m_index;
	cvec3 m_color;

	cmat4 m_offset;
	cmat4 m_transform;

	// Current SRT
	cquat m_rotation;
	cvec3 m_translation;
	cvec3 m_scaling;

	// Geometry Offset SRT
	cvec3 m_geoRotation;
	cvec3 m_geoTranslation;
	cvec3 m_geoScaling;

	// Not sure what they do yet..
	double m_size;
	double m_limbLength;

	CVS_Bone();
	int initFromFbxNode(FbxNode* _pNode);
	cvec3 getGlobalPosition(const cmat4* _parentGlobalPosition);
	bool hasBone(const char* _boneName);

	CVS_Bone* getRootBone();
	CVS_Skeleton* getSkeleton();
};
