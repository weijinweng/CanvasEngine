#include "CVS_FbxImporter.h"
#include "CVS_Mesh.h"
#include "CVS_Bone.h"

//Standard curve node names
#define FBXSDK_CURVENODE_TRANSLATION	'T'
#define FBXSDK_CURVENODE_ROTATION		'R'
#define FBXSDK_CURVENODE_SCALING		'S'
#define FBXSDK_CURVENODE_COMPONENT_X	'X'
#define FBXSDK_CURVENODE_COMPONENT_Y	'Y'
#define FBXSDK_CURVENODE_COMPONENT_Z	'Z'
#define FBXSDK_CURVENODE_COLOR			'C'
#define FBXSDK_CURVENODE_COLOR_RED		FBXSDK_CURVENODE_COMPONENT_X
#define FBXSDK_CURVENODE_COLOR_GREEN	FBXSDK_CURVENODE_COMPONENT_Y
#define FBXSDK_CURVENODE_COLOR_BLUE		FBXSDK_CURVENODE_COMPONENT_Z

static const char AnimChannels[] = { FBXSDK_CURVENODE_TRANSLATION, FBXSDK_CURVENODE_ROTATION, FBXSDK_CURVENODE_SCALING };
static const char AnimComponents[] = { FBXSDK_CURVENODE_COMPONENT_X, FBXSDK_CURVENODE_COMPONENT_Y, FBXSDK_CURVENODE_COMPONENT_Z };

CVS_FbxImporter::~CVS_FbxImporter()
{
	m_pImporter->Destroy();
	m_pIoSettings->Destroy();
	m_pFbxManager->Destroy();
}

CVS_FbxImporter::CVS_FbxImporter()
{
	this->AddFormatSupport(".fbx");
}

bool CVS_FbxImporter::init()
{
	m_pFbxManager = FbxManager::Create();
	m_pIoSettings = FbxIOSettings::Create(m_pFbxManager, IOSROOT);
	m_pFbxManager->SetIOSettings(m_pIoSettings);
	m_pImporter = FbxImporter::Create(m_pFbxManager, "");

	return true;
}

bool CVS_FbxImporter::import(const char* _filePath)
{
	auto pFbxScene = _initFBXScene(_filePath);
	if (!pFbxScene)
	{
		return false;
	}
	return extractResource(pFbxScene);
}

FbxScene* CVS_FbxImporter::_initFBXScene(const char* _filepath)
{
	// Create the importer.
	int fileFormat = -1;
	if (!m_pFbxManager->GetIOPluginRegistry()->DetectReaderFileFormat(_filepath, fileFormat))
	{
		// Unrecognizable file format. Try to fall back to FbxImporter::eFBX_BINARY
		fileFormat = m_pFbxManager->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");;
	}

	if (!m_pImporter->Initialize(_filepath, -1, m_pFbxManager->GetIOSettings()))
	{
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", m_pImporter->GetStatus().GetErrorString());
		exit(-1);
	}

	auto pScene = FbxScene::Create(m_pFbxManager, "My scene");
	if (!pScene)
	{
		FBXSDK_printf("Error: Unable to create FBX scene!\n");
		exit(1);
	}

	if (m_pImporter->Import(pScene) == true)
	{
		auto pGlobalSettings = &pScene->GetGlobalSettings();

		// Convert Axis System to what is used in this example, if needed
		FbxAxisSystem SceneAxisSystem = pScene->GetGlobalSettings().GetAxisSystem();
		FbxAxisSystem OurAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
		if (SceneAxisSystem != OurAxisSystem)
		{
			OurAxisSystem.ConvertScene(pScene);
		}

		// Requires the complete fbxsdk library
		// Convert Unit System to what is used in this example, if needed
		FbxSystemUnit SceneSystemUnit = pScene->GetGlobalSettings().GetSystemUnit();
		if (SceneSystemUnit.GetScaleFactor() != 1.0)
		{
			//The unit in this example is centimeter.
			FbxSystemUnit::cm.ConvertScene(pScene);
		}

		printf("_loadFBXScene: Animation Time Mode : %s\n", FbxGetTimeModeName(pGlobalSettings->GetTimeMode()));

		// Animation Stack
		for (int i = 0; i < pScene->GetSrcObjectCount<FbxAnimStack>(); i++)
		{
			FbxAnimStack* pAnimStack = pScene->GetSrcObject<FbxAnimStack>(i);
			int numAnimLayers = pAnimStack->GetMemberCount<FbxAnimLayer>();

			printf("Animation Stack Name: %s\n\n", pAnimStack->GetName());
			printf("Animation stack contains %d Animation Layer(s)\n", numAnimLayers);

			// Animation Layer
			for (int j = 0; j < numAnimLayers; j++)
			{
				FbxAnimLayer* pAnimLayer = pAnimStack->GetMember<FbxAnimLayer>(j);
				printf("AnimLayer %d\n", j);
				printf("     Node Name: %s\n\n", pScene->GetRootNode()->GetName());
			}
		}


		FbxArray<FbxString*> mAnimStackNameArray;
		// Get the list of all the animation stack.
		pScene->FillAnimStackNameArray(mAnimStackNameArray);

		// Convert mesh, NURBS and patch into triangle mesh
		FbxGeometryConverter lGeomConverter(m_pFbxManager);
		lGeomConverter.Triangulate(pScene, true);

		// Split meshes per material, so that we only have one material per mesh (for VBO support)
		lGeomConverter.SplitMeshesPerMaterial(pScene, true);
	}
	else
	{
		printf("Unable to import file %s.\n", _filepath);
		printf("Error reported: ", m_pImporter->GetStatus().GetErrorString());
	}

	return pScene;
}

// Main
CVS_FbxImporter::FbxNodeOffsetStruct CVS_FbxImporter::getNodeOffset(FbxNode* _pNode)
{
	FbxNodeOffsetStruct o;
	FbxAMatrixToMat4(&(_pNode->GetAnimationEvaluator()->GetNodeGlobalTransform(_pNode)), o.matGlobalTransform);
	//o.vecPreRotation = cquat(cvec3(glm::radians(glm::make_vec3(_pNode->PreRotation.Get().mData))));
	o.vecLclTranslation = cvec3(glm::make_vec3(_pNode->LclTranslation.Get().mData));
	o.vecLclScaling = cvec3(glm::make_vec3(_pNode->LclScaling.Get().mData));
	o.vecLclRotation = cquat(cvec3(glm::radians(glm::make_vec3(_pNode->LclRotation.Get().mData)))) *
		cquat(cvec3(glm::radians(glm::make_vec3(_pNode->PreRotation.Get().mData))));

	o.matGlobalTransform = o.matGlobalTransform;
	o.matLclTranslation = glm::translate(cmat4(), o.vecLclTranslation);
	o.matLclScaling = glm::scale(cmat4(), o.vecLclScaling);
	o.matLclRotation = glm::mat4_cast(o.vecLclRotation);

	o.vecGeoTranslation = cvec3(glm::make_vec3(_pNode->GetGeometricTranslation(FbxNode::eSourcePivot).mData));
	o.vecGeoRotation = cvec3(glm::make_vec3(_pNode->GetGeometricRotation(FbxNode::eSourcePivot).mData));
	o.vecGeoScaling = cvec3(glm::make_vec3(_pNode->GetGeometricScaling(FbxNode::eSourcePivot).mData));

	return o;
}

bool CVS_FbxImporter::extractResource(FbxScene* _pFbxScene)
{
	// No constructor nor copy constructor, virtually a singleton 
	FbxGlobalSettings& gSettings = _pFbxScene->GetGlobalSettings();

	std::vector<CVS_Mesh*> meshes;

	auto pRootNode = _pFbxScene->GetRootNode();
	// import skeleton
	ImportSkeletons(pRootNode);
	//ImportAllNodes(pRootNode, EFbxImportMode::Skeleton);
	// import mesh
	ImportMeshes(pRootNode);
	//ImportAllNodes(pRootNode, EFbxImportMode::Mesh);
	// import animation
	//ImportAllNodes(pRootNode, EFbxImportMode::Animation);

	//*
	//GETTING ANIMAION DATA
	char pTimeString[256];

	auto timeMode = gSettings.GetTimeMode();
	printf("Time Mode : %s\n", FbxGetTimeModeName(timeMode));
	double fps = FrameRateToDouble(timeMode);
	FbxTimeSpan pTimeSpan;
	gSettings.GetTimelineDefaultTimeSpan(pTimeSpan);
	FbxTime     lStart, lEnd;
	lStart = pTimeSpan.GetStart();
	lEnd = pTimeSpan.GetStop();
	printf("Timeline default timespan: \n");
	printf("     Start: %s\n", lStart.GetTimeString(pTimeString, FbxUShort(256)));
	printf("     Stop : %s\n", lEnd.GetTimeString(pTimeString, FbxUShort(256)));

	// Get animation information
	// Now only supports one take
	for (int i = 0; i < _pFbxScene->GetSrcObjectCount<FbxAnimStack>(); ++i)
	{
		auto lAnimStack = _pFbxScene->GetSrcObject<FbxAnimStack>(i);

		auto stackName = lAnimStack->GetName();
		printf("Animation Stack %d Name: %s\n", i, stackName);
		auto start = lAnimStack->GetLocalTimeSpan().GetStart();
		auto stop = lAnimStack->GetLocalTimeSpan().GetStop();
		auto animationLength = stop.GetFrameCount(timeMode) - start.GetFrameCount(timeMode);// TODO: do we really need +1?;
		printf("     Start: %d\n", start.GetFrameCount(timeMode));
		printf("     Stop : %d\n", stop.GetFrameCount(timeMode));
		// For Each layer
		int numLayers = lAnimStack->GetMemberCount<FbxAnimLayer>();
		for (int j = 0; j < numLayers; ++j)
		{
			FbxAnimLayer* pAnimLayer = lAnimStack->GetMember<FbxAnimLayer>(j);

			auto layerName = pAnimLayer->GetName();
			printf("Animation Stack Name: %s\n", layerName);

			// Start with root node
			FbxNode* pNodeItr = _pFbxScene->GetRootNode();
			// Breadth first inline recursion
			std::queue<FbxNode*> nodes;
			while (pNodeItr != NULL)
			{
				FbxAnimCurve* lAnimCurve = pNodeItr->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
				// For each node, extract all channels
				for (int i = 0; i < 3; i++)
				{
					// Extract Translation Curve
					auto lAnimCurve = pNodeItr->LclTranslation.GetCurve(pAnimLayer, AnimComponents[i]);
					if (lAnimCurve)
					{
						printf("        %c%c\n", AnimChannels[0], AnimComponents[i]);
						//ProcessCurve(lAnimCurve);
					}

					// Extract Rotation Curve
					lAnimCurve = pNodeItr->LclRotation.GetCurve(pAnimLayer, AnimComponents[i]);
					if (lAnimCurve)
					{
						printf("        %c%c\n", AnimChannels[0], AnimComponents[i]);
						//ProcessCurve(lAnimCurve);
					}

					// Extract Scaling Curve
					lAnimCurve = pNodeItr->LclScaling.GetCurve(pAnimLayer, AnimComponents[i]);
					if (lAnimCurve)
					{
						printf("        %c%c\n", AnimChannels[0], AnimComponents[i]);
						//ProcessCurve(lAnimCurve);
					}
				}

				for (int i = 0; i < pNodeItr->GetChildCount(false); ++i)
				{
					nodes.push(pNodeItr->GetChild(i));
				}

				if (nodes.size() > 0)
				{
					pNodeItr = nodes.front();
					nodes.pop();
				}
				else
				{
					pNodeItr = NULL;
				}
			}
		}
	}
	//*/
	return true;
}

// Skeleton
void CVS_FbxImporter::ImportSkeletons(FbxNode* _pNode)
{
	// Skip RootNode by default
	// If we don't skip, RootNode will be parsed as a bone, NG
	for (int i = 0, e = _pNode->GetChildCount(); i < e; i++)
	{
		_processSkeletonNodeRecursive(_pNode->GetChild(i));
	}

	for (auto i : m_skeletons)
	{
		i->Init();
	}
}

bool CVS_FbxImporter::_processSkeletonNodeRecursive(FbxNode* _pNode)
{
	auto pNodeAttribute = _pNode->GetNodeAttribute();
	if (pNodeAttribute)
	{
		// Blender specifies that Null and Skeleton are both parsed as bones
		if (pNodeAttribute->GetAttributeType() != FbxNodeAttribute::eNull &&
			pNodeAttribute->GetAttributeType() != FbxNodeAttribute::eSkeleton)
		{
			// Process children
			for (int i = 0, e = _pNode->GetChildCount(); i < e; ++i)
			{
				_processSkeletonNodeRecursive(_pNode->GetChild(i));
			}
			return false;
		}
	}
	auto pBone = new CVS_Bone;

	// todo: pack em in a struct?
	auto offset = getNodeOffset(_pNode);
	pBone->m_translation = offset.vecLclTranslation;
	pBone->m_rotation = offset.vecLclRotation;
	pBone->m_scaling = offset.vecLclScaling;
	pBone->m_geoTranslation = offset.vecGeoTranslation;
	pBone->m_geoRotation = offset.vecGeoRotation;
	pBone->m_geoScaling = offset.vecGeoScaling;

	if (InitBoneFromFbxNode(pBone, _pNode))
	{
		// Add bone to all bone list
		m_allBones[_pNode] = pBone;
		// Resolving the bone tree relies on correctly traversing the scene nodes
		// Try to find its parent
		auto pParentFbxNode = _pNode->GetParent();
		if (pParentFbxNode)
		{
			CVS_Bone* pParentBone = static_cast<CVS_Bone*>(m_allBones[pParentFbxNode]);
			if (!pParentBone)
			{
				// A whole new tree, so we create a new skeleton
				auto pSkeleton = new CVS_Skeleton;
				pSkeleton->m_pRootBone = pBone;
				pSkeleton->m_name = pBone->m_name;
				m_skeletons.push_back(pSkeleton);
				m_pResourcePool->push_back(pSkeleton);
			}
			else
			{
				// Create Link between parent and our new bone
				pParentBone->m_children.push_back(pBone);
				pBone->m_pParent = pParentBone;
			}
		}
	}

	// Process children
	for (int i = 0, e = _pNode->GetChildCount(); i < e; ++i)
	{
		_processSkeletonNodeRecursive(_pNode->GetChild(i));
	}
	return true;
}

bool CVS_FbxImporter::InitBoneFromFbxNode(CVS_Bone* _pBone, FbxNode* _pNode)
{
	// Check what kind of bone this is
	// We assume Empty nodes and nodes containing FbxNulls and FbxSkeletons
	// *Are all bones*
	_pBone->m_name = _pNode->GetName();
	_pBone->m_type = CVS_InternalResource::eType::Bone;


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
											_pBone->m_color = cvec3(boneColor.mRed, boneColor.mGreen, boneColor.mBlue);
											_pBone->m_size = pSkeleton->Size.Get();
											_pBone->m_limbLength = pSkeleton->LimbLength.Get();

		}
		}
	}
	return true;
}

// Meshes
void CVS_FbxImporter::ImportMeshes(FbxNode* _pNode)
{
	// Skip RootNode by default
	for (int i = 0, e = _pNode->GetChildCount(); i < e; i++)
	{
		_processMeshNodeRecursive(_pNode->GetChild(i));
	}

	for (auto i : m_meshes)
	{
		i->Init();
	}
}

bool CVS_FbxImporter::_processMeshNodeRecursive(FbxNode* _pNode)
{
	auto pNodeAttribute = _pNode->GetNodeAttribute();
	if (pNodeAttribute)
	{
		if (pNodeAttribute->GetAttributeType() != FbxNodeAttribute::eMesh)
		{
			// Process children
			for (int i = 0, e = _pNode->GetChildCount(); i < e; ++i)
			{
				_processMeshNodeRecursive(_pNode->GetChild(i));
			}
			return false;
		}
		auto pMesh = new CVS_Mesh;
		// Get the mesh offset
		auto offset = getNodeOffset(_pNode);
		pMesh->m_offset = offset.matLclTranslation * offset.matLclRotation * offset.matLclScaling * offset.matGlobalTransform;
		if (InitMeshFromFbxNode(pMesh, _pNode))
		{
#pragma region
			auto pFbxMesh = _pNode->GetMesh();
			int skinCount = pFbxMesh->GetDeformerCount(FbxDeformer::eSkin);
			if (skinCount)
			{
				for (int skinIndex = 0; skinIndex != skinCount; ++skinIndex)
				{
					auto pSkin = (FbxSkin *)pFbxMesh->GetDeformer(skinIndex, FbxDeformer::eSkin);
					if (skinCount > 1)
					{
						printf("CVS_Mesh::initFromFbxNode: Mesh contains more than 1 skin.\n");
						assert(0);
					}

					// Allocate memory for vertex bone data
					pMesh->m_vertexBones.reserve(pMesh->m_vertices.size());
					// for each skin, find the correct skeleton
					// TODO: might be buggy when there's nore than 1 skeleton per mesh
					CVS_Skeleton* pSkeleton = nullptr;
					int clusterCount = (pSkin)->GetClusterCount();
					for (int clusterIndex = 0; clusterIndex != clusterCount; ++clusterIndex)
					{
						auto pCluster = ((FbxSkin *)pFbxMesh->GetDeformer(skinIndex, FbxDeformer::eSkin))->GetCluster(clusterIndex);

						_PrintClusterInfo(pCluster, clusterIndex);

						// For each cluster, we check against the bone map to make sure
						// everything comes from the same skeleton tree
						auto pSkeletonNode = pCluster->GetLink();

						CVS_Bone* pBone = nullptr;
						pBone = static_cast<CVS_Bone*>(m_allBones[pSkeletonNode]);
						if (!pBone)
						{
							printf("CVS_Mesh::initFromFbxNode: Current deformer's bone link not found in bonemap\n");
							assert(0);
						}

						// Check if the parent is the same skeleton we currently have
						// If no skeleton currently in place, use it directly
						auto pParent = pBone->getSkeleton();
						if (pSkeleton)
						{
							if (pSkeleton != pParent)
							{
								printf("CVS_Mesh::initFromFbxNode: Current mesh seems to have 2 rigs affecting it as the same time. Check your mesh.\n");
								assert(0);
							}
						}
						else
						{
							pSkeleton = pParent;
						}

						// Inject data into vertices
						int* lIndices = pCluster->GetControlPointIndices();
						double* lWeights = pCluster->GetControlPointWeights();

						for (int j = 0, e = pCluster->GetControlPointIndicesCount(); j < e; j++)
						{
							int i = lIndices[j];
							float f = lWeights[j];
							pMesh->m_vertexBones[i].AddBoneData(pBone->m_index, f);
						}

						FbxAMatrix transformMatrix;
						FbxAMatrix transformLinkMatrix;
						FbxAMatrix globalBindposeInverseMatrix;

						pCluster->GetTransformMatrix(transformMatrix); // The transformation of the mesh at binding time
						pCluster->GetTransformLinkMatrix(transformLinkMatrix); // The transformation of the cluster(joint) at binding time from joint space to world space
						globalBindposeInverseMatrix = transformLinkMatrix.Inverse();

						auto pMat = cmat4();
						auto pMatLink = cmat4();
						auto pBindPoseInverse = cmat4();
						FbxAMatrixToMat4(&transformMatrix, pMat);
						FbxAMatrixToMat4(&transformLinkMatrix, pMatLink);
						FbxAMatrixToMat4(&globalBindposeInverseMatrix, pBindPoseInverse);

						pBone->m_offset *= pBindPoseInverse * pMat;
					}
					if (pMesh->m_pSkeleton)
					{
						printf("CVS_Mesh::initFromFbxNode: Current mesh seems to have 2 skins.\n");
						assert(0);
					}
					else
					{
						pMesh->m_pSkeleton = pSkeleton;
					}
				}

			}
			else
			{
				// TODO: fix shader support for static mesh

				// If there's no bone at all, we make a dummy
				// to pass to shaders
				/*
				if (!pMesh->m_pSkeleton)
				{
				auto pDummyBone = new CVS_Bone;
				pDummyBone->m_name = "DummyBone";
				pDummyBone->m_color = cvec3(1.0f, 1.0f, 1.0f);
				pDummyBone->m_index = MAX_BONES - 1;
				this->m_pSkeleton = pDummyBone;
				// 			for (int i = 0, e = m_vertexBones.size(); i < e; i++)
				// 			{
				// 				m_vertexBones[i].AddBoneData(0, 1.0f);
				// 			}
				}
				//*/
			}
#pragma endregion Add Bones
			m_meshes.push_back(pMesh);
			printf("Init mesh %s\n", pMesh->m_name.c_str());
			m_pResourcePool->push_back(pMesh);
		}
	}

	// Process children
	for (int i = 0, e = _pNode->GetChildCount(); i < e; ++i)
	{
		_processMeshNodeRecursive(_pNode->GetChild(i));
	}
	return true;
}

bool CVS_FbxImporter::InitMeshFromFbxNode(CVS_Mesh* _pMesh, FbxNode* _pNode)
{
	auto pInMesh = _pNode->GetMesh();
	if (!pInMesh)
	{
		printf("Mesh::initFromFbxNode: Not a valid mesh node. %s\n", _pNode->GetName());
		return 1;
	}
	else
	{
		printf("Mesh::initFromFbxNode: Mesh processed: %s\n", _pNode->GetName());
	}

	_pMesh->m_name = _pNode->GetName();

	// Count the polygon count of each material
	auto lPolyCount = pInMesh->GetPolygonCount();

#pragma region
	FbxLayerElementArrayTemplate<int>* lMaterialIndices = NULL;
	auto lMaterialMappingMode = FbxGeometryElement::eNone;
	if (pInMesh->GetElementMaterial())
	{
		lMaterialIndices = &pInMesh->GetElementMaterial()->GetIndexArray();
		lMaterialMappingMode = pInMesh->GetElementMaterial()->GetMappingMode();
		if (lMaterialIndices && lMaterialMappingMode == FbxGeometryElement::eByPolygon)
		{
			FBX_ASSERT(lMaterialIndices->GetCount() == lPolyCount);
			if (lMaterialIndices->GetCount() == lPolyCount)
			{
				// Count the faces of each material
				for (int i = 0; i < lPolyCount; ++i)
				{
					const int lMaterialIndex = lMaterialIndices->GetAt(i);
					if (_pMesh->m_subMeshes.size() < lMaterialIndex + 1)
					{
						_pMesh->m_subMeshes.resize(lMaterialIndex + 1);
					}
					if (_pMesh->m_subMeshes[lMaterialIndex] == NULL)
					{
						_pMesh->m_subMeshes[lMaterialIndex] = new CVS_SubMesh;
					}
					_pMesh->m_subMeshes[lMaterialIndex]->m_triangleCount += 1;
				}

				// Make sure we have no "holes" (NULL) in the pDstMesh->m_subMeshes table. _pMesh can happen
				// if, in the loop above, we resized the pDstMesh->m_subMeshes by more than one slot.
				for (int i = 0; i < _pMesh->m_subMeshes.size(); i++)
				{
					if (_pMesh->m_subMeshes[i] == NULL)
						_pMesh->m_subMeshes[i] = new CVS_SubMesh;
				}

				// Record the offset (how many vertex)
				const int lMaterialCount = _pMesh->m_subMeshes.size();
				int lOffset = 0;
				for (int lIndex = 0; lIndex < lMaterialCount; ++lIndex)
				{
					_pMesh->m_subMeshes[lIndex]->m_indexOffset = lOffset;
					lOffset += _pMesh->m_subMeshes[lIndex]->m_triangleCount * 3;
					// _pMesh will be used as counter in the following procedures, reset to zero
					_pMesh->m_subMeshes[lIndex]->m_triangleCount = 0;
				}
				FBX_ASSERT(lOffset == lPolyCount * 3);
			}
		}
	}

	// All faces will use the same material.
	if (_pMesh->m_subMeshes.size() == 0)
	{
		_pMesh->m_subMeshes.resize(1);
		_pMesh->m_subMeshes[0] = new CVS_SubMesh;
	}
#pragma endregion Create SubMeshes Based on Materials

#pragma region
	// Congregate all the data of a mesh to be cached in VBOs.
	// If normal or UV is by polygon vertex, record all vertex attributes by polygon vertex.
	bool hasNormal = pInMesh->GetElementNormalCount() > 0;
	bool hasUV = pInMesh->GetElementUVCount() > 0;
	bool bAllByControlPoint = true;
	auto lNormalMappingMode = FbxGeometryElement::eNone;
	auto lUVMappingMode = FbxGeometryElement::eNone;
	if (hasNormal)
	{
		lNormalMappingMode = pInMesh->GetElementNormal(0)->GetMappingMode();
		if (lNormalMappingMode == FbxGeometryElement::eNone)
		{
			hasNormal = false;
		}
		// TODO: Really should be hasNormal && lNormalMappingMode != FbxGeometryElement::eByControlPoint
		// Here, but the API tell us the geometry layout the exact opposite.
		// If it contains split normals, it tells you it's by control point, and vice versa
		// DON'T TRUST ANYONE
		if (hasNormal && lNormalMappingMode == FbxGeometryElement::eByControlPoint)
		{
			bAllByControlPoint = false;
		}
	}
	if (hasUV)
	{
		lUVMappingMode = pInMesh->GetElementUV(0)->GetMappingMode();
		if (lUVMappingMode == FbxGeometryElement::eNone)
		{
			hasUV = false;
		}
		if (hasUV && lUVMappingMode == FbxGeometryElement::eByControlPoint)
		{
			bAllByControlPoint = false;
		}
	}

	// Allocate the array memory, by control point or by polygon vertex.
	int lPolygonVertexCount = pInMesh->GetControlPointsCount();
	if (!bAllByControlPoint)
	{
		lPolygonVertexCount = lPolyCount * TRIANGLE_VERTEX_COUNT;
	}

	_pMesh->m_vertices.resize(lPolygonVertexCount);
	_pMesh->m_indices.resize(lPolyCount * TRIANGLE_VERTEX_COUNT);
	_pMesh->m_vertexBones.resize(lPolygonVertexCount);

	FbxStringList lUVNames;
	pInMesh->GetUVSetNames(lUVNames);
	const char * lUVName = NULL;
	if (hasUV && lUVNames.GetCount())
	{
		lUVName = lUVNames[0];
	}

	// Populate the array with vertex attribute, if by control point.
	const FbxVector4 * lControlPoints = pInMesh->GetControlPoints();
	glm::vec3 lCurrentVertex;
	glm::vec3 lCurrentNormal;
	glm::vec2 lCurrentUV;
	if (bAllByControlPoint)
	{
		const FbxGeometryElementNormal * lNormalElement = NULL;
		const FbxGeometryElementUV * lUVElement = NULL;
		if (hasNormal)
		{
			lNormalElement = pInMesh->GetElementNormal(0);
		}
		if (hasUV)
		{
			lUVElement = pInMesh->GetElementUV(0);
		}
		for (int lIndex = 0; lIndex < lPolygonVertexCount; ++lIndex)
		{
			// Save the vertex position.
			lCurrentVertex = glm::vec3(
				lControlPoints[lIndex][0],
				lControlPoints[lIndex][1],
				lControlPoints[lIndex][2]);

			_pMesh->m_vertices[lIndex].position = lCurrentVertex;

			// Save the normal.
			if (hasNormal)
			{
				int lNormalIndex = lIndex;
				if (lNormalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
				{
					lNormalIndex = lNormalElement->GetIndexArray().GetAt(lIndex);
				}
				lCurrentNormal = glm::vec3(
					lNormalElement->GetDirectArray().GetAt(lNormalIndex)[0],
					lNormalElement->GetDirectArray().GetAt(lNormalIndex)[1],
					lNormalElement->GetDirectArray().GetAt(lNormalIndex)[2]);
				_pMesh->m_vertices[lIndex].normal = lCurrentNormal;
			}

			// Save the UV.
			if (hasUV)
			{
				int lUVIndex = lIndex;
				if (lUVElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
				{
					lUVIndex = lUVElement->GetIndexArray().GetAt(lIndex);
				}
				lCurrentUV = glm::vec2(
					lUVElement->GetDirectArray().GetAt(lUVIndex)[0],
					lUVElement->GetDirectArray().GetAt(lUVIndex)[1]);
				_pMesh->m_vertices[lIndex].uv = lCurrentUV;

			}
		}

	}
	int lVertexCount = 0;
	for (int polyIndex = 0; polyIndex < lPolyCount; ++polyIndex)
	{
		// The material for current face.
		int lMaterialIndex = 0;
		if (lMaterialIndices && lMaterialMappingMode == FbxGeometryElement::eByPolygon)
		{
			lMaterialIndex = lMaterialIndices->GetAt(polyIndex);
		}

		// Where should I save the vertex attribute index, according to the material
		const int lIndexOffset = _pMesh->m_subMeshes[lMaterialIndex]->m_indexOffset +
			_pMesh->m_subMeshes[lMaterialIndex]->m_triangleCount * 3;
		for (int j = 0; j < TRIANGLE_VERTEX_COUNT; ++j)
		{
			const int lControlPointIndex = pInMesh->GetPolygonVertex(polyIndex, j);

			if (bAllByControlPoint)
			{
				_pMesh->m_indices[lIndexOffset + j] = static_cast<unsigned int>(lControlPointIndex);
			}
			// Populate the array with vertex attribute, if by polygon vertex.
			else
			{
				_pMesh->m_indices[lIndexOffset + j] = static_cast<unsigned int>(lVertexCount);

				lCurrentVertex = glm::vec3(
					lControlPoints[lControlPointIndex].mData[0],
					lControlPoints[lControlPointIndex].mData[1],
					lControlPoints[lControlPointIndex].mData[2]);
				_pMesh->m_vertices[lVertexCount].position = lCurrentVertex;

				if (hasNormal)
				{
					FbxVector4 lNormal;
					pInMesh->GetPolygonVertexNormal(polyIndex, j, lNormal);
					lCurrentNormal = glm::vec3(
						lNormal[0],
						lNormal[1],
						lNormal[2]);
					_pMesh->m_vertices[lVertexCount].normal = lCurrentNormal;
				}

				if (hasUV)
				{
					bool lUnmappedUV;
					FbxVector2 lUV;
					pInMesh->GetPolygonVertexUV(polyIndex, j, lUVName, lUV, lUnmappedUV);
					lCurrentUV = glm::vec2(
						lUV[0],
						lUV[1]);
					_pMesh->m_vertices[lVertexCount].uv = lCurrentUV;
				}
			}
			++lVertexCount;
		}
		_pMesh->m_subMeshes[lMaterialIndex]->m_triangleCount += 1;
	}
#pragma endregion Inject Data: vertices, normal, UV
	return true;
}

void CVS_FbxImporter::_PrintClusterInfo(FbxCluster* _pCluster, int _index)
{
	static const char* lClusterModes[] = { "Normalize", "Additive", "Total1" };

	printf("    Cluster %d\n", _index);

	printf("    Mode: %s\n", lClusterModes[_pCluster->GetLinkMode()]);

	if (_pCluster->GetLink() != NULL)
	{
		printf("        Name: %s\n", (char *)_pCluster->GetLink()->GetName());
	}

	FbxString lString1 = "        Link Indices: ";
	FbxString lString2 = "        Weight Values: ";

	int k, lIndexCount = _pCluster->GetControlPointIndicesCount();
	int* lIndices = _pCluster->GetControlPointIndices();
	double* lWeights = _pCluster->GetControlPointWeights();

	for (k = 0; k < lIndexCount; k++)
	{
		lString1 += lIndices[k];
		lString2 += (float)lWeights[k];

		if (k < lIndexCount - 1)
		{
			lString1 += ", ";
			lString2 += ", ";
		}
	}

	lString1 += "\n";
	lString2 += "\n";

	FBXSDK_printf(lString1);
	//FBXSDK_printf(lString2);

	printf("\n");

	FbxAMatrix lMatrix;

	lMatrix = _pCluster->GetTransformMatrix(lMatrix);
	printf("        Transform Translation     : %f, %f, %f\n", lMatrix.GetT()[0], lMatrix.GetT()[1], lMatrix.GetT()[2]);
	printf("        Transform Rotation        : %f, %f, %f\n", lMatrix.GetR()[0], lMatrix.GetR()[1], lMatrix.GetR()[2]);
	printf("        Transform Scaling         : %f, %f, %f\n", lMatrix.GetS()[0], lMatrix.GetS()[1], lMatrix.GetS()[2]);

	lMatrix = _pCluster->GetTransformLinkMatrix(lMatrix);
	printf("        Transform Link Translation: %f, %f, %f\n", lMatrix.GetT()[0], lMatrix.GetT()[1], lMatrix.GetT()[2]);
	printf("        Transform Link Rotation   : %f, %f, %f\n", lMatrix.GetR()[0], lMatrix.GetR()[1], lMatrix.GetR()[2]);
	printf("        Transform Link Scaling    : %f, %f, %f\n", lMatrix.GetS()[0], lMatrix.GetS()[1], lMatrix.GetS()[2]);

	if (_pCluster->GetAssociateModel() != NULL)
	{
		lMatrix = _pCluster->GetTransformAssociateModelMatrix(lMatrix);
		printf("        Associate Model            : %s\n", (char *)_pCluster->GetAssociateModel()->GetName());
		printf("        Associate Model Translation: %f, %f, %f\n", lMatrix.GetT()[0], lMatrix.GetT()[1], lMatrix.GetT()[2]);
		printf("        Associate Model Rotation   : %f, %f, %f\n", lMatrix.GetR()[0], lMatrix.GetR()[1], lMatrix.GetR()[2]);
		printf("        Associate Model Scaling    : %f, %f, %f\n", lMatrix.GetS()[0], lMatrix.GetS()[1], lMatrix.GetS()[2]);
	}

	printf("\n");
}

// Animations
void CVS_FbxImporter::ImportAnimations(FbxNode* _pNode, EFbxImportMode _parseMode)
{

}

void CVS_FbxImporter::_DisplayCurveKeys(FbxAnimCurve* pCurve)
{
	static const char* interpolation[] = { "?", "constant", "linear", "cubic" };
	static const char* constantMode[] = { "?", "Standard", "Next" };
	static const char* cubicMode[] = { "?", "Auto", "Auto break", "Tcb", "User", "Break", "User break" };
	static const char* tangentWVMode[] = { "?", "None", "Right", "Next left" };

	FbxTime   lKeyTime;
	float   lKeyValue;
	char    lTimeString[256];
	FbxString lOutputString;
	int     lCount;

	int lKeyCount = pCurve->KeyGetCount();

	for (lCount = 0; lCount < lKeyCount; lCount++)
	{
		lKeyValue = static_cast<float>(pCurve->KeyGetValue(lCount));
		lKeyTime = pCurve->KeyGetTime(lCount);

		lOutputString = "            Key Time: ";
		lOutputString += lKeyTime.GetTimeString(lTimeString, FbxUShort(256));
		lOutputString += ".... Key Value: ";
		lOutputString += lKeyValue;
		lOutputString += " [ ";
		lOutputString += interpolation[InterpolationFlagToIndex(pCurve->KeyGetInterpolation(lCount))];
		if ((pCurve->KeyGetInterpolation(lCount)&FbxAnimCurveDef::eInterpolationConstant) == FbxAnimCurveDef::eInterpolationConstant)
		{
			lOutputString += " | ";
			lOutputString += constantMode[ConstantmodeFlagToIndex(pCurve->KeyGetConstantMode(lCount))];
		}
		else if ((pCurve->KeyGetInterpolation(lCount)&FbxAnimCurveDef::eInterpolationCubic) == FbxAnimCurveDef::eInterpolationCubic)
		{
			lOutputString += " | ";
			lOutputString += cubicMode[TangentmodeFlagToIndex(pCurve->KeyGetTangentMode(lCount))];
			lOutputString += " | ";
			lOutputString += tangentWVMode[TangentweightFlagToIndex(pCurve->KeyGet(lCount).GetTangentWeightMode())];
			lOutputString += " | ";
			lOutputString += tangentWVMode[TangentVelocityFlagToIndex(pCurve->KeyGet(lCount).GetTangentVelocityMode())];
		}
		lOutputString += " ]";
		lOutputString += "\n";
		FBXSDK_printf(lOutputString);
	}
}

int CVS_FbxImporter::TangentVelocityFlagToIndex(int flags)
{
	if ((flags & FbxAnimCurveDef::eVelocityNone) == FbxAnimCurveDef::eVelocityNone) return 1;
	if ((flags & FbxAnimCurveDef::eVelocityRight) == FbxAnimCurveDef::eVelocityRight) return 2;
	if ((flags & FbxAnimCurveDef::eVelocityNextLeft) == FbxAnimCurveDef::eVelocityNextLeft) return 3;
	return 0;
}

int CVS_FbxImporter::TangentweightFlagToIndex(int flags)
{
	if ((flags & FbxAnimCurveDef::eWeightedNone) == FbxAnimCurveDef::eWeightedNone) return 1;
	if ((flags & FbxAnimCurveDef::eWeightedRight) == FbxAnimCurveDef::eWeightedRight) return 2;
	if ((flags & FbxAnimCurveDef::eWeightedNextLeft) == FbxAnimCurveDef::eWeightedNextLeft) return 3;
	return 0;
}

int CVS_FbxImporter::TangentmodeFlagToIndex(int flags)
{
	if ((flags & FbxAnimCurveDef::eTangentAuto) == FbxAnimCurveDef::eTangentAuto) return 1;
	if ((flags & FbxAnimCurveDef::eTangentAutoBreak) == FbxAnimCurveDef::eTangentAutoBreak) return 2;
	if ((flags & FbxAnimCurveDef::eTangentTCB) == FbxAnimCurveDef::eTangentTCB) return 3;
	if ((flags & FbxAnimCurveDef::eTangentUser) == FbxAnimCurveDef::eTangentUser) return 4;
	if ((flags & FbxAnimCurveDef::eTangentGenericBreak) == FbxAnimCurveDef::eTangentGenericBreak) return 5;
	if ((flags & FbxAnimCurveDef::eTangentBreak) == FbxAnimCurveDef::eTangentBreak) return 6;
	return 0;
}

int CVS_FbxImporter::ConstantmodeFlagToIndex(int flags)
{
	if ((flags & FbxAnimCurveDef::eConstantStandard) == FbxAnimCurveDef::eConstantStandard) return 1;
	if ((flags & FbxAnimCurveDef::eConstantNext) == FbxAnimCurveDef::eConstantNext) return 2;
	return 0;
}

int CVS_FbxImporter::InterpolationFlagToIndex(int flags)
{
	if ((flags & FbxAnimCurveDef::eInterpolationConstant) == FbxAnimCurveDef::eInterpolationConstant) return 1;
	if ((flags & FbxAnimCurveDef::eInterpolationLinear) == FbxAnimCurveDef::eInterpolationLinear) return 2;
	if ((flags & FbxAnimCurveDef::eInterpolationCubic) == FbxAnimCurveDef::eInterpolationCubic) return 3;
	return 0;
}

double CVS_FbxImporter::FrameRateToDouble(FbxTime::EMode fp, double customFPSVal /*= -1.0*/)
{
	switch (fp)
	{
	case FbxTime::EMode::eDefaultMode:
		return 1.0;

	case FbxTime::EMode::eFrames120:
		return 120.0;

	case FbxTime::EMode::eFrames100:
		return 100.0;

	case FbxTime::EMode::eFrames60:
		return 60.0;

	case FbxTime::EMode::eFrames50:
		return 50.0;

	case FbxTime::EMode::eFrames48:
		return 48.0;

	case FbxTime::EMode::eFrames30:
	case FbxTime::EMode::eFrames30Drop:
		return 30.0;

	case FbxTime::EMode::eNTSCDropFrame:
	case FbxTime::EMode::eNTSCFullFrame:
		return 29.9700262;

	case FbxTime::EMode::ePAL:
		return 25.0;

	case FbxTime::EMode::eFrames24:
		return 24.0;

	case FbxTime::EMode::eFrames1000:
		return 1000.0;

	case FbxTime::EMode::eFilmFullFrame:
		return 23.976;

	case FbxTime::EMode::eCustom:
		return customFPSVal;

	case FbxTime::EMode::eModesCount:
		break;
	}

	assert(0);
	return -1.0f; // this is to silence compiler warnings
}
