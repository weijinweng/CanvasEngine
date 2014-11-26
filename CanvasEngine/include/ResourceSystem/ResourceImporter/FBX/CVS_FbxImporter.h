#pragma once
#include "CVS_ResourceImporter.h"

class CVS_InternalResource;
struct CVS_FbxImporter : public CVS_ResourceImporter
{
	enum class EFbxImportMode
	{
		Skeleton,
		Mesh,
		Animation
	};

	CVS_FbxImporter();

	~CVS_FbxImporter();

	virtual bool init();

	virtual bool import(const char* _filePath);

private:
	FbxScene* _initFBXScene(const char* _filepath);
	void _PrintClusterInfo(FbxCluster* _pCluster, int _index);
	bool _processFbxNodeRecursive(FbxNode* _pNode, EFbxImportMode _parseMode);
	void ImportAllNodes(FbxNode* _pNode, EFbxImportMode _parseMode);
	bool extractResource(FbxScene* _pFbxScene);

	// skeletons must be parsed as bones first and reconstructed
	// after the first complete traversal of the scene

	std::map<FbxNode*, CVS_InternalResource*> m_skeletonRootBones;
	std::map<FbxNode*, CVS_InternalResource*> m_allBones;

	FbxManager* m_pFbxManager;
	FbxIOSettings* m_pIoSettings;
	FbxImporter* m_pImporter;
};
