#pragma once
#include "CVS_ResourceImporter.h"
#include "CVS_Skeleton.h"
#include "CVS_Mesh.h"

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

	// Skeletons
	void ImportSkeletons(FbxNode* _pNode);
	bool _processSkeletonNodeRecursive(FbxNode* _pNode);
	bool InitBoneFromFbxNode(CVS_Bone* _pBone, FbxNode* _pNode);

	// Meshes
	void ImportMeshes(FbxNode* _pNode);
	bool _processMeshNodeRecursive(FbxNode* _pNode);
	bool InitMeshFromFbxNode(CVS_Mesh* _pMesh, FbxNode* _pNode);
	void _PrintClusterInfo(FbxCluster* _pCluster, int _index);

	// Animations
	void ImportAnimations(FbxNode* _pNode, EFbxImportMode _parseMode);
	void _DisplayCurveKeys(FbxAnimCurve* pCurve);
	static double FrameRateToDouble(FbxTime::EMode fp, double customFPSVal = -1.0);
	static int InterpolationFlagToIndex(int flags);
	static int ConstantmodeFlagToIndex(int flags);
	static int TangentmodeFlagToIndex(int flags);
	static int TangentweightFlagToIndex(int flags);
	static int TangentVelocityFlagToIndex(int flags);

	// temp data
	std::vector<CVS_Skeleton*> m_skeletons;
	std::vector<CVS_Mesh*> m_meshes;
	std::map<FbxNode*, CVS_InternalResource*> m_skeletonRootBones;
	std::map<FbxNode*, CVS_InternalResource*> m_allBones;
	std::map<FbxNode*, CVS_InternalResource*> m_animations;

	FbxManager* m_pFbxManager;
	FbxIOSettings* m_pIoSettings;
	FbxImporter* m_pImporter;
};
