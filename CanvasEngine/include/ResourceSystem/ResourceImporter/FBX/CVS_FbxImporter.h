#pragma once
#include "CVS_ResourceImporter.h"
#include "CVS_Mesh.h"

class CVS_Animation;
class CVS_AnimationLayer;
class CVS_AnimationNode;
class CVS_AnimationCurve;
class CVS_InternalResource;
struct CVS_FbxImporter : public CVS_ResourceImporter
{
	enum class EFbxImportMode
	{
		Skeleton,
		Mesh,
		Animation
	};

	struct FbxNodeOffsetStruct
	{
		cmat4 matGlobalTransform;
		
		cmat4 matLclTranslation;
		cmat4 matLclRotation;
		cmat4 matLclScaling;

		//cquat vecPreRotation; // In radians
		cvec3 vecLclTranslation;
		cquat vecLclRotation; // In radians
		cvec3 vecLclScaling;

		cvec3 vecGeoTranslation;
		cvec3 vecGeoRotation;
		cvec3 vecGeoScaling;
	};

	CVS_FbxImporter();

	~CVS_FbxImporter();

	virtual bool init();
	virtual bool import(const char* _filePath);

private:
	FbxScene* _initFBXScene(const char* _filepath);
	FbxNodeOffsetStruct getNodeOffset(FbxNode* _pNode);
	bool extractResource(FbxScene* _pFbxScene);
	bool hasAnyMesh(FbxScene* _pFbxScene);

	// Skeletons
	void ImportSkeletons(FbxScene* _pScene);
	bool isBone(FbxNode* _pNode);
	bool _initJointWithFbxNode(FbxNode* _pNode);
	bool InitBoneFromFbxNode(CVS_Bone* _pBone, FbxNode* _pNode);

	// Meshes
	void ImportMeshes(FbxScene* _pScene);
	bool isMesh(FbxNode* _pNode);
	bool InitMeshSkeletonWithCurrentFbxScene(CVS_Mesh* _pMesh, FbxNode* _pNode);
	bool InitMeshFromFbxNode(CVS_Mesh* _pMesh, FbxNode* _pNode);
	void _PrintClusterInfo(FbxCluster* _pCluster, int _index);

	// Animations
	void ImportAnimations(FbxScene* _pScene);
	void InitAnimsInFbxScene(FbxScene* _pScene);
	void InitAnimFromFbxAnimStack(FbxNode* _pNode, FbxAnimStack* _pFbxAnimStack, CVS_Animation* _pAnimation);
	void InitAnimNodeFromFbxAnimLayer(FbxNode* _pNode, FbxAnimLayer* _pFbxAnimLayer, CVS_AnimationLayer* _pAnimLayer);
	void InitAnimNodesFromFbxAnimLayerRecursive(FbxNode* _pNode, FbxAnimLayer* _pFbxAnimLayer, CVS_AnimationLayer* _pAnimLayer);
	bool InitAnimNodeFromFbxNode(CVS_AnimationNode* _pAnimNode, FbxNode* _pFbxNode, FbxAnimLayer* _pFbxAnimLayer, CVS_AnimationLayer* _pAnimLayer);
	void InitAnimCurveFromFbxProperty(FbxProperty* _pFbxProperty, FbxAnimLayer* _pFbxAnimLayer, CVS_AnimationCurve* _pAnimCurve);
	void _DisplayCurveKeys(FbxAnimCurve* pCurve);
	static double FrameRateToDouble(FbxTime::EMode fp, double customFPSVal = -1.0);
	static int InterpolationFlagToIndex(int flags);
	static int ConstantmodeFlagToIndex(int flags);
	static int TangentmodeFlagToIndex(int flags);
	static int TangentweightFlagToIndex(int flags);
	static int TangentVelocityFlagToIndex(int flags);

	// temp data
	std::string m_fileName;
	std::vector<CVS_Skeleton*> m_skeletons;
	std::vector<CVS_Mesh*> m_meshes;
	std::vector<CVS_Animation*> m_animations;

	float m_globalAnimationFrameRate;

	std::vector<FbxNode*> m_fbxPossibleSkeletons;
	std::map<FbxNode*, short> m_allFbxBoneIndices;
	std::map<FbxAnimLayer*, std::map<FbxNode*, CVS_InternalResource*>> m_layerNodeMaps;

	FbxManager* m_pFbxManager;
	FbxIOSettings* m_pIoSettings;
	FbxImporter* m_pImporter;
};
