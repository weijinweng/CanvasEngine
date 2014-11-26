#pragma once

class CVS_Resource;
class CVS_Bone;

struct CVS_Skeleton : public CVS_Resource
{
	CVS_Bone* m_pRootBone;

	CVS_Skeleton();
	void SetRootBone(CVS_Bone* _pBone);
	bool hasBone(const char* _boneName);

	void Init();

	void AssignBoneIndex(CVS_Bone* _pParent);
};