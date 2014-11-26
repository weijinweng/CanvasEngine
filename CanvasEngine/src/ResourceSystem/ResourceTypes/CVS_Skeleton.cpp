#include "CVS_Bone.h"
#include "CVS_Skeleton.h"

void CVS_Skeleton::AssignBoneIndex(CVS_Bone* _pParent)
{
	int index = 0;
	// Breadth first inline recursion
	std::queue<CVS_Bone*> bones;
	while (_pParent != NULL)
	{
		_pParent->m_index = index++;
		for (int i = 0, e = _pParent->m_children.size(); i < e; ++i)
		{
			bones.push(_pParent->m_children[i]);
		}

		if (bones.size() > 0)
		{
			_pParent = bones.front();
			bones.pop();
		}
		else
		{
			_pParent = NULL;
		}
	}
}

void CVS_Skeleton::Init()
{
	AssignBoneIndex(m_pRootBone);
}

bool CVS_Skeleton::hasBone(const char* _boneName)
{
	return m_pRootBone->hasBone(_boneName);
}

void CVS_Skeleton::SetRootBone(CVS_Bone* _pBone)
{
	m_pRootBone = _pBone;
	this->m_name = m_pRootBone->m_name;
}

CVS_Skeleton::CVS_Skeleton() : CVS_Resource("DefaultSkeletonName", eType::Skeleton)
{

}
