#include "Canvas.h"
#include "CVS_Bone.h"
#include "CVS_Skeleton.h"
#include "CVS_RenderComponents.h"

cvec3 CVS_Bone::getGlobalPosition(const cmat4* _parentGlobalPosition)
{
	cvec4 parentTransform;
	if (_parentGlobalPosition)
	{
		parentTransform = (*_parentGlobalPosition) * parentTransform;
	}

	return cvec3();
}

int CVS_Bone::initFromFbxNode(FbxNode* _pNode)
{
	// Check what kind of bone this is
	// We assume Empty nodes and nodes containing FbxNulls and FbxSkeletons
	// *Are all bones*

	m_name = _pNode->GetName();
	m_type = CVS_InternalResource::EType::Bone;


	auto attribute = _pNode->GetNodeAttribute();
	if (attribute)
	{

		auto attributeType = attribute->GetAttributeType();
		switch (attributeType)
		{
		case FbxNodeAttribute::eNull:
		{
		}
			break;
		case FbxNodeAttribute::eSkeleton:
		{

											static const char* skeletonTypes[] = { "Root", "Limb", "Limb Node", "Effector" };

											int childCount = _pNode->GetChildCount();

											FbxSkeleton* pSkeleton = (FbxSkeleton*)_pNode->GetNodeAttribute();

											printf("Bone: %s\n", _pNode->GetName());
											printf("    Type:  %s\n", skeletonTypes[pSkeleton->GetSkeletonType()]);

											if (pSkeleton->GetSkeletonType() == FbxSkeleton::eLimb)
											{
												// TODO: NOT SURE WHAT eLimb DOES
												printf("    Limb Length: %d\n", pSkeleton->LimbLength.Get());
											}
											else if (pSkeleton->GetSkeletonType() == FbxSkeleton::eLimbNode)
											{
												printf("    Limb Node Size: %d\n", pSkeleton->Size.Get());
											}
											else if (pSkeleton->GetSkeletonType() == FbxSkeleton::eRoot)
											{
												// TODO: NOT SURE WHAT eRoot DOES
												printf("    Limb Root Size: %d\n", pSkeleton->Size.Get());
											}

											auto boneColor = pSkeleton->GetLimbNodeColor();
											printf("    Color:  %f(red),   %f(green),   %f(blue)\n", boneColor.mRed, boneColor.mGreen, boneColor.mBlue);

											// Fill data
											m_color = cvec3(boneColor.mRed, boneColor.mGreen, boneColor.mBlue);
											m_size = pSkeleton->Size.Get();
											m_limbLength = pSkeleton->LimbLength.Get();
		}
			break;
		}
	}
	else
	{

	}
	return true;
}


CVS_Skeleton* CVS_Bone::getSkeleton()
{
	return static_cast<CVS_Skeleton*>(GLOBALSTATEMACHINE.m_ResourceSub.get(getRootBone()->m_name.c_str(), CVS_Resource::EType::Skeleton));
}

CVS_Bone* CVS_Bone::getRootBone()
{
// 	auto pParent = m_pParent;
// 	while (pParent->m_pParent)
// 	{
// 		pParent = pParent->m_pParent;
// 	}
	return nullptr;
}

bool CVS_Bone::hasBone(const char* _boneName)
{
	if (m_name == _boneName)
	{
		return true;
	}
	else
	{
		auto hr = false;
		for (auto i : m_children)
		{
			hr |= i->hasBone(_boneName);
		}
		return hr;
	}
}

CVS_Bone::CVS_Bone() : m_index(), m_bindPoseInv(), m_transform(), m_size(), m_limbLength(), m_rotation(), m_translation()
{

}