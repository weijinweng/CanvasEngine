#pragma once
#include "CVS_Bone.h"

class CVS_Resource;

struct CVS_SkeletonPose
{
	CVS_Skeleton* m_pSkeleton;
	std::vector<CVS_BonePose> m_aLocalPose;
	std::vector<cmat4> m_aGlobalPose;

	CVS_SkeletonPose(CVS_Skeleton* _pSkeleton);
};

struct CVS_Skeleton : public CVS_Resource
{
	//CVS_Bone* m_pRootBone;
	std::vector<CVS_Bone> m_bones;

	CVS_Skeleton();
	void SetRootBone(CVS_Bone* _pBone);
	bool hasBone(const char* _boneName);

	void Init();

	void AssignBoneIndex(CVS_Bone* _pParent);
};